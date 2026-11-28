<?php
chdir(__DIR__);
header('Content-Type: text/plain; charset=utf-8');
if (($_SERVER['REQUEST_METHOD']??'')!=='POST' || ($_POST['secret']??'')!=='Wmfd2893gb7') exit('-1');
require '../lib/connection.php';
require_once '../lib/GJPCheck.php';
require_once '../lib/RoleManager.php';
require_once '../lib/ModerationQueue.php';
$actor=(int)GJPCheck::getAccountIDOrDie();
try { echo (new ModerationQueue($db,new RoleManager($db)))->queue($actor,$_POST); }
catch (DomainException $e) { echo '-1'; }
catch (Throwable $e) { error_log('Moderation queue failed (code '.$e->getCode().')'); http_response_code(503); echo '-1'; }
