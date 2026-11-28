<?php
if (PHP_SAPI !== 'cli' || count($argv) !== 3) exit('Usage: php test_staff_rating.php BACKEND_ROOT PRIVATE_DATABASE_CREDENTIALS_JSON');
chdir($argv[1] . '/incl/lib');
$c = json_decode(file_get_contents($argv[2]), true, 512, JSON_THROW_ON_ERROR);
$db = new PDO('mysql:host=' . $c['host'] . ';port=' . $c['port'] . ';dbname=' . $c['database'] . ';charset=utf8mb4', $c['administrative_user'], $c['administrative_password'], [PDO::ATTR_ERRMODE=>PDO::ERRMODE_EXCEPTION]);
unset($c);
require_once 'mainLib.php';
require_once 'StaffRating.php';
// Temporary tables shadow real tables on this connection. No player data changes.
$db->exec('CREATE TEMPORARY TABLE levels (levelID INT PRIMARY KEY, starStars INT DEFAULT 0, starDifficulty INT DEFAULT 0, starDemon INT DEFAULT 0, starAuto INT DEFAULT 0, starDemonDiff INT DEFAULT 0, starCoins INT DEFAULT 0, starFeatured INT DEFAULT 0, starEpic INT DEFAULT 0, featuredOrder INT DEFAULT 0, rateDate INT DEFAULT 0) ENGINE=InnoDB');
$db->exec('CREATE TEMPORARY TABLE suggest (suggestBy INT, suggestLevelId INT, suggestDifficulty INT, suggestStars INT, suggestFeatured INT, suggestAuto INT, suggestDemon INT, suggestDemonDiff INT, suggestCoins INT, timestamp INT) ENGINE=InnoDB');
$db->exec('CREATE TEMPORARY TABLE modactions (type VARCHAR(20), value VARCHAR(100), value2 VARCHAR(100), value3 INT, timestamp INT, account INT) ENGINE=InnoDB');
$db->exec('INSERT INTO levels (levelID) VALUES (1),(2),(3)');
$roles = new class {
    public function access($id) { return ['canSuggest'=>in_array($id,[1,2,3],true), 'canRate'=>$id===1 || $id===3, 'canSetDemon'=>$id===1, 'canFeature'=>$id===1, 'canSetFire'=>$id===1]; }
};
$rating = new StaffRating($db, $roles, new mainLib());
function expect($condition,$message) { if (!$condition) throw new RuntimeException($message); }
function denied($callback,$code) { try { $callback(); } catch (DomainException $e) { expect($e->getCode()===$code,'Wrong denial'); return; } throw new RuntimeException('Request unexpectedly allowed'); }
$star = ['action'=>'stars','levelID'=>'1','difficulty'=>'na','stars'=>'2147483647','coins'=>'1'];
denied(fn()=>$rating->handle(0,$star),403);
denied(fn()=>$rating->handle(2,$star),403);
denied(fn()=>$rating->handle(2,['action'=>'featured','levelID'=>'1','order'=>'500','tier'=>'4']),403);
$rating->handle(1,['action'=>'featured','levelID'=>'1','order'=>'24990','tier'=>'4']);
$r = $rating->handle(1,$star);
expect($r['stars']===2147483647 && $r['order']===24990 && $r['tier']===4,'Star rating reset Featured');
expect((int)$db->query('SELECT starDifficulty FROM levels WHERE levelID=1')->fetchColumn()===0,'N/A face changed');
$rating->handle(1,['action'=>'featured','levelID'=>'1','order'=>'50','tier'=>'2']);
expect((int)$db->query('SELECT starStars FROM levels WHERE levelID=1')->fetchColumn()===2147483647,'Featured changed stars');
denied(fn()=>$rating->handle(2,['action'=>'demon','levelID'=>'1','difficulty'=>'easy-demon']),403);
denied(fn()=>$rating->handle(3,['action'=>'demon','levelID'=>'1','difficulty'=>'easy-demon']),403);
$r=$rating->handle(1,['action'=>'demon','levelID'=>'1','difficulty'=>'easy-demon']);
expect($r['subtype']===3 && $r['demon']===1 && $r['stars']===2147483647 && $r['coins']===1 && $r['order']===50 && $r['tier']===2,'Demon subtype changed unrelated awards/fire/order');
$rating->handle(1,['action'=>'featured','levelID'=>'2','order'=>'500','tier'=>'1']);
$order=$db->query('SELECT levelID FROM levels WHERE starFeatured=1 ORDER BY '.FeaturedOrder::order($db).' DESC')->fetchAll(PDO::FETCH_COLUMN);
expect(array_map('intval',$order)===[2,1],'Featured ordering not highest first');
$rating->handle(2,['action'=>'send','levelID'=>'3','difficulty'=>'grandpa-demon','stars'=>'16','coins'=>'1','tier'=>'4']);
$s=$db->query('SELECT * FROM suggest')->fetch(PDO::FETCH_ASSOC);
expect((int)$s['suggestDemonDiff']===13 && (int)$s['suggestCoins']===1 && (int)$s['suggestFeatured']===4,'Suggestion lost subtype/coins/fire');
expect((int)$db->query('SELECT starStars FROM levels WHERE levelID=3')->fetchColumn()===0,'Suggestion rated public level');
foreach (['2147483648','-1','1.5','1 OR 1=1'] as $bad) { $invalid=$star; $invalid['stars']=$bad; denied(fn()=>$rating->handle(1,$invalid),400); }
$rating->handle(1,['action'=>'featured','levelID'=>'1','order'=>'0','tier'=>'0']);
$r=$rating->handle(1,['action'=>'read','levelID'=>'1']);
expect($r['order']===0 && $r['tier']===0 && $r['stars']===2147483647,'Unfeature changed stars');
echo "PASS: permission boundaries including admin-only demon controls, N/A maximum stars, independent Featured/fire, descending order, complete suggestion details, invalid inputs, unfeature. No real levels changed.\n";
