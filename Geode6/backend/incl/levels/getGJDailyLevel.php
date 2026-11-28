<?php
chdir(__DIR__);
require_once '../lib/connection.php';
require_once '../lib/TimedLevelService.php';
$type = $_POST['type'] ?? $_POST['weekly'] ?? '0';
if (!is_scalar($type) || !in_array((string)$type, ['0', '1', '2'], true)) exit('-1');
$type = (int)$type;
$row = TimedLevelService::current($db, $type);
if (!$row) exit('-1');
$state = TimedLevelService::state($row, $type);
echo $state['dailyID'] . '|' . $state['timeLeft'];
