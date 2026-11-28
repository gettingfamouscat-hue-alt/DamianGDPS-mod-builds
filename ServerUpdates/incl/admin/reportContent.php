<?php
chdir(__DIR__);
header('Content-Type: text/plain; charset=utf-8');
if (($_SERVER['REQUEST_METHOD']??'')!=='POST' || ($_POST['secret']??'')!=='Wmfd2893gb7') exit('-1');
require '../lib/connection.php';
require_once '../lib/GJPCheck.php';
require_once '../lib/RoleManager.php';
require_once '../lib/ModerationQueue.php';
$actor=0;
if (!empty($_POST['accountID']) || isset($_POST['gjp']) || isset($_POST['gjp2'])) $actor=(int)GJPCheck::getAccountIDOrDie();
try { (new ModerationQueue($db,new RoleManager($db)))->report($actor,(new mainLib())->getIP(),$_POST); echo '1'; }
catch (DomainException $e) { echo '-1'; }
catch (Throwable $e) { error_log('Content report failed (code '.$e->getCode().')'); http_response_code(503); echo '-1'; }
