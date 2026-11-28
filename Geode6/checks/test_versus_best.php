<?php
if (PHP_SAPI !== 'cli' || count($argv) !== 3) exit('Usage: php test_staff_rating.php BACKEND_ROOT PRIVATE_DATABASE_CREDENTIALS_JSON');
chdir($argv[1] . '/incl/lib');
$c = json_decode(file_get_contents($argv[2]), true, 512, JSON_THROW_ON_ERROR);
$db = new PDO('mysql:host=' . $c['host'] . ';port=' . $c['port'] . ';dbname=' . $c['database'] . ';charset=utf8mb4', $c['administrative_user'], $c['administrative_password'], [PDO::ATTR_ERRMODE=>PDO::ERRMODE_EXCEPTION]);
unset($c);
require_once 'mainLib.php';
require_once 'StaffRating.php';

require_once '../../versus/_schema.php';
$GLOBALS['gdpsDatabase']=$db;
$db->exec("CREATE TEMPORARY TABLE versus_matches (matchID INT PRIMARY KEY, player1 INT DEFAULT 1, player2 INT DEFAULT 2, levelID INT DEFAULT 61, p1Name VARCHAR(30) DEFAULT 'Me', p2Name VARCHAR(30) DEFAULT 'Opponent', p1Lives INT DEFAULT 3, p2Lives INT DEFAULT 3, p1Percent DOUBLE DEFAULT 0, p2Percent DOUBLE DEFAULT 0, p1Best DOUBLE DEFAULT 0, p2Best DOUBLE DEFAULT 0, status VARCHAR(16) DEFAULT 'active', winner INT DEFAULT 0) ENGINE=InnoDB");
$db->exec('CREATE TEMPORARY TABLE users (extID VARCHAR(20), icon INT, color1 INT, color2 INT)');
$db->exec('INSERT INTO versus_matches (matchID) VALUES (1),(2)');
function check($ok,$message) { if (!$ok) throw new RuntimeException($message); }
function updateProgress($db,$p1,$values) {
    [$sets,$params]=versusProgressUpdate($p1,$values);
    if (!$sets) return;
    $params[':id']=1;
    $db->prepare('UPDATE versus_matches SET '.implode(', ',$sets)." WHERE matchID=:id AND status='active'")->execute($params);
}
updateProgress($db,true,['percent'=>'76.4']);
updateProgress($db,true,['percent'=>'0']);
updateProgress($db,true,['percent'=>'45']);
updateProgress($db,true,['percent'=>'5','best'=>'81.2']);
updateProgress($db,false,['percent'=>'44.5','best'=>'44.5']);
updateProgress($db,false,['percent'=>'0']);
$row=versusMatchRow($db,1);
check(abs($row['p1Best']-81.2)<.001 && abs($row['p2Best']-44.5)<.001,'Best reset after death or delayed packet');
$p1=versusPublicMatch($row,1);$p2=versusPublicMatch($row,2);
check(abs($p1['opponent']['best']-44.5)<.001 && abs($p2['opponent']['best']-81.2)<.001,'Opponent perspective is wrong');
check($p1['opponent']['percent']===0.0 && $p1['opponent']['lives']===3,'Best changed live progress/lives');
$fresh=versusPublicMatch(versusMatchRow($db,2),1);
check($fresh['you']['best']===0.0 && $fresh['opponent']['best']===0.0,'Best leaked into another match');
foreach (['INF','nan','1e999',[], 'not a number'] as $bad) {
    try { versusProgressUpdate(true,['best'=>$bad]); throw new RuntimeException('Invalid progress accepted'); }
    catch (DomainException $expected) {}
}
$db->exec("UPDATE versus_matches SET status='finished' WHERE matchID=1");
updateProgress($db,true,['percent'=>'100','best'=>'100']);
check(abs(versusMatchRow($db,1)['p1Best']-81.2)<.001,'Finished match was changed');
echo "PASS: match best survives retries and delayed packets, reports correct opponent perspective, resets for a new match, rejects invalid progress, and preserves finished matches. No real matches changed.\n";
