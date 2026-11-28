<?php
if (PHP_SAPI !== 'cli' || count($argv) !== 3) exit('Usage: php test_collections.php BACKEND_ROOT PRIVATE_DATABASE_CREDENTIALS_JSON');
$c=json_decode(file_get_contents($argv[2]),true,512,JSON_THROW_ON_ERROR);
$db=new PDO('mysql:host='.$c['host'].';port='.$c['port'].';dbname='.$c['database'].';charset=utf8mb4',$c['administrative_user'],$c['administrative_password'],[PDO::ATTR_ERRMODE=>PDO::ERRMODE_EXCEPTION]); unset($c);
require_once $argv[1].'/incl/lib/Collections.php';
foreach (['levels','lists','gauntlets','gdps_gauntlet_names'] as $table) {
    $ddl=$db->query("SHOW CREATE TABLE `$table`")->fetch(PDO::FETCH_NUM)[1];
    $db->exec(preg_replace('/^CREATE TABLE /','CREATE TEMPORARY TABLE ',$ddl));
}
// Minimal level fixtures: only collection validation fields are read.
$columns=$db->query('SHOW COLUMNS FROM levels')->fetchAll(PDO::FETCH_ASSOC);
$names=[]; $values=[];
foreach ($columns as $col) {
    if (str_contains($col['Extra'],'auto_increment') && $col['Field'] !== 'levelID') continue;
    $names[]='`'.$col['Field'].'`';
    $values[]=$col['Default'] ?? ($col['Null']==='YES' ? null : (preg_match('/int|decimal|float|double/i',$col['Type']) ? 0 : ''));
}
$q=$db->prepare('INSERT INTO levels ('.implode(',',$names).') VALUES ('.implode(',',array_fill(0,count($names),'?')).')');
for($id=101;$id<=106;$id++) { $row=$values; foreach($names as $i=>$name) { if($name==='`levelID`') $row[$i]=$id; if($name==='`unlisted`' && $id===106) $row[$i]=1; } $q->execute($row); }
$roles=new class { public function access($id) { return ['canCreateLists'=>in_array($id,[1,2],true),'canManageGauntlets'=>$id===1]; } };
$service=new Collections($db,$roles);
function expect($ok,$why) { if(!$ok) throw new RuntimeException($why); }
function denied($fn,$code) { try {$fn();} catch(DomainException $e) {expect($e->getCode()===$code,'Wrong rejection: '.$e->getMessage());return;} throw new RuntimeException('Invalid collection accepted'); }
$list=['kind'=>'list','id'=>'0','name'=>'My route','levels'=>'101, 102, 103','description'=>'A <fun> route'];
$saved=$service->save(2,$list);
$row=$db->query('SELECT * FROM lists')->fetch(PDO::FETCH_ASSOC);
expect((int)$row['accountID']===2 && $row['listlevels']==='101,102,103' && base64_decode($row['listDesc'])==='A <fun> route','List lost owner/order/description');
$edit=$list; $edit['id']=(string)$saved['id']; $edit['levels']='103,102,101';
denied(fn()=>$service->save(1,$edit),403);
$service->save(2,$edit);
expect($db->query('SELECT listlevels FROM lists')->fetchColumn()==='103,102,101','List edit failed');
$gauntlet=['kind'=>'gauntlet','id'=>'1','name'=>'Damian route','levels'=>'101,102,103,104,105'];
denied(fn()=>$service->save(2,$gauntlet),403);
$service->save(1,$gauntlet);
expect($db->query('SELECT name FROM gdps_gauntlet_names')->fetchColumn()==='Damian route','Custom gauntlet name not saved');
denied(fn()=>$service->save(1,$gauntlet),409);
$gauntlet['replace']='1'; $gauntlet['levels']='105,104,103,102,101'; $service->save(1,$gauntlet);
expect((int)$db->query('SELECT level1 FROM gauntlets')->fetchColumn()===105,'Explicit replacement did not preserve order');
denied(fn()=>$service->save(0,$list),403);
foreach(['101,106','101,9999','101,101','1 OR 1=1','','-1','2147483648'] as $bad) { $v=$list;$v['levels']=$bad;denied(fn()=>$service->save(2,$v),400); }
foreach(['bad:name','bad|name','bad#name',str_repeat('x',81)] as $bad) { $v=$list;$v['name']=$bad;denied(fn()=>$service->save(2,$v),400); }
$v=$gauntlet;$v['levels']='101,102';denied(fn()=>$service->save(1,$v),400);
$v=$gauntlet;$v['id']='63';denied(fn()=>$service->save(1,$v),400);
expect(!$db->inTransaction(),'Transaction leaked');
echo "PASS: list create/edit/ownership, description encoding, gauntlet order/custom names/admin permission, explicit replacement, missing/private/repeated/invalid IDs, safe names. No real collections changed.\n";
