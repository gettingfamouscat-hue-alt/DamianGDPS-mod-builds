<?php
if (PHP_SAPI!=='cli' || count($argv)!==3) exit('Usage: php test_moderation.php BACKEND_ROOT PRIVATE_DATABASE_CREDENTIALS_JSON');
$c=json_decode(file_get_contents($argv[2]),true,512,JSON_THROW_ON_ERROR);
$db=new PDO('mysql:host='.$c['host'].';port='.$c['port'].';dbname='.$c['database'].';charset=utf8mb4',$c['administrative_user'],$c['administrative_password'],[PDO::ATTR_ERRMODE=>PDO::ERRMODE_EXCEPTION]);unset($c);
require $argv[1].'/incl/lib/ModerationQueue.php';
$db->exec(str_replace('CREATE TABLE IF NOT EXISTS','CREATE TEMPORARY TABLE',file_get_contents($argv[1].'/migrations/content_reports.sql')));
foreach ([
    'accounts'=>'accountID INT PRIMARY KEY,userName VARCHAR(100)',
    'levels'=>'levelID INT PRIMARY KEY,extID INT',
    'lists'=>'listID INT PRIMARY KEY,accountID INT',
    'users'=>'userID INT PRIMARY KEY,extID INT',
    'comments'=>'commentID INT PRIMARY KEY,levelID INT,userID INT',
    'acccomments'=>'commentID INT PRIMARY KEY,userID INT',
    'messages'=>'messageID INT PRIMARY KEY,accID INT,toAccountID INT',
    'friendreqs'=>'ID INT PRIMARY KEY,accountID INT,toAccountID INT',
    'reports'=>'ID INT PRIMARY KEY,levelID INT,hostname VARCHAR(255)',
] as $table=>$columns) $db->exec("CREATE TEMPORARY TABLE $table ($columns)");
$db->exec("INSERT INTO accounts VALUES (1,'Moderator'),(2,'Other staff'),(3,'Player'),(4,'Reported:|name')");
$db->exec('INSERT INTO levels VALUES (60,4),(61,4)');
$db->exec('INSERT INTO lists VALUES (10,4)');
$db->exec('INSERT INTO users VALUES (50,4)');
$db->exec('INSERT INTO comments VALUES (11,60,50)');
$db->exec('INSERT INTO acccomments VALUES (12,50)');
$db->exec('INSERT INTO messages VALUES (13,4,3)');
$db->exec('INSERT INTO friendreqs VALUES (14,4,3)');
$db->exec("INSERT INTO reports VALUES (1,61,'192.0.2.2')");
$roles=new class {function access($id) {return ['canCreateLists'=>in_array($id,[1,2,3],true),'canRate'=>$id===1,'canSuggest'=>$id===2];}};
$service=new ModerationQueue($db,$roles);
function expect($ok,$why) {if (!$ok) throw new RuntimeException($why);}
function denied($fn,$code) {try {$fn();}catch(DomainException $e){expect($e->getCode()===$code,'Wrong rejection: '.$e->getMessage());return;}throw new RuntimeException('Invalid moderation request accepted');}
$input=['type'=>'1','targetID'=>'60','reasons'=>'2,6'];
$service->report(3,'192.0.2.1',$input);$service->report(3,'192.0.2.1',$input);
expect((int)$db->query('SELECT COUNT(*) FROM content_reports')->fetchColumn()===2,'Duplicate report inserted');
$service->report(0,'192.0.2.2',$input);
$service->report(3,'192.0.2.1',['type'=>'0','targetID'=>'4','reasons'=>'4,5']);
$wire=$service->queue(1,[]); [$entries,$paging]=explode('#',$wire);
$f=explode(':',$entries);
expect(count($f)===20 && (int)$f[0]===4 && base64_decode(strtr($f[1],'-_','+/'))==='Reported:|name','Unsafe username or malformed native entry');
expect((int)$f[2]===2 && (int)$f[4]===1 && (int)$f[8]===1 && (int)$f[19]===1,'Items, direct reports, or unique reporters incorrect');
expect($paging==='1:0:20' && !str_contains($wire,'192.0.2.'),'Invalid paging or IP exposed');
expect($service->queue(2,['mode'=>'2'])==='#0:0:20','Unimplemented flags fabricated');
foreach ([0,3] as $actor) denied(fn()=>$service->queue($actor,[]),403);
foreach ([['page'=>'-1'],['page'=>'1 OR 1=1'],['mode'=>3],['sort'=>2]] as $bad) denied(fn()=>$service->queue(1,$bad),400);
foreach ([['type'=>'1','targetID'=>'60','reasons'=>'4'],['type'=>'1','targetID'=>'60','reasons'=>'9'],['type'=>[],'targetID'=>'60','reasons'=>'0']] as $bad) denied(fn()=>$service->report(3,'ip',$bad),400);
foreach ([['type'=>'3','targetID'=>'11','special'=>'61','reasons'=>'0'],['type'=>'4','targetID'=>'12','special'=>'3','reasons'=>'0']] as $bad) denied(fn()=>$service->report(3,'ip',$bad),404);
foreach ([5=>13,6=>14] as $type=>$id) {
    $r=['type'=>(string)$type,'targetID'=>(string)$id,'reasons'=>'1'];
    denied(fn()=>$service->report(0,'ip',$r),403);denied(fn()=>$service->report(2,'ip',$r),404);
    $service->report(3,'ip',$r);
}
foreach ([[2,10,0],[3,11,60],[4,12,4]] as [$type,$id,$special]) $service->report(3,'ip',['type'=>(string)$type,'targetID'=>(string)$id,'special'=>(string)$special,'reasons'=>'0']);
// Pagination and New sorting on a full second page.
for ($id=100;$id<121;$id++) {$db->exec("INSERT INTO accounts VALUES ($id,'Page$id')");$service->report(3,'ip',['type'=>'0','targetID'=>(string)$id,'reasons'=>'0']);}
$next=$service->queue(1,['page'=>'1','sort'=>'1']);expect(str_ends_with($next,'#22:20:20') && substr_count(explode('#',$next)[0],'|')===1,'Pagination lost or repeated rows');
$db->exec('DELETE FROM content_reports');$db->exec('DELETE FROM reports');
expect($service->queue(1,[])==='#0:0:20','Empty queue is a failure response');
expect(!$db->inTransaction(),'Transaction leaked');
echo "PASS: native 20-field queue, empty state, legacy reports, unique counts, role checks, safe usernames, paging, report ownership/reasons, deduplication and private-message access. No real reports changed.\n";
