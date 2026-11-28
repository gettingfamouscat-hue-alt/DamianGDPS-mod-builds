<?php
if (PHP_SAPI !== 'cli' || count($argv) !== 3) exit('Usage: php test_timed_levels.php BACKEND_ROOT PRIVATE_DATABASE_CREDENTIALS_JSON');
$c = json_decode(file_get_contents($argv[2]), true, 512, JSON_THROW_ON_ERROR);
$db = new PDO('mysql:host=' . $c['host'] . ';port=' . $c['port'] . ';dbname=' . $c['database'] . ';charset=utf8mb4', $c['administrative_user'], $c['administrative_password'], [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);
unset($c);
require_once $argv[1] . '/incl/lib/TimedLevelService.php';
require_once $argv[1] . '/incl/lib/StaffRating.php';
// Connection-local fixtures shadow production tables. Player data is never written.
$db->exec("CREATE TEMPORARY TABLE levels (levelID INT PRIMARY KEY, unlisted INT DEFAULT 0, unlisted2 INT DEFAULT 0, levelString TEXT, starStars INT DEFAULT 7) ENGINE=InnoDB");
$db->exec('CREATE TEMPORARY TABLE dailyfeatures (feaID INT PRIMARY KEY AUTO_INCREMENT, levelID INT, timestamp INT, type INT) ENGINE=InnoDB');
$db->exec('CREATE TEMPORARY TABLE modactions (type VARCHAR(10), value VARCHAR(50), value2 INT, value3 INT, value4 INT, timestamp INT, account INT) ENGINE=InnoDB');
$db->exec("INSERT INTO levels (levelID, levelString, unlisted, unlisted2) VALUES (11,'data',0,0),(12,'data',0,0),(13,'data',1,0),(14,'data',0,1),(2147483646,'',0,0)");
$roles = new class {
    public function access(int $actor): array { return ['canSetDaily' => $actor === 1, 'canSetWeekly' => $actor === 2]; }
};
function expect(bool $ok, string $why): void { if (!$ok) throw new RuntimeException($why); }
function denied(callable $fn, int $code): void {
    try { $fn(); } catch (DomainException $e) { expect($e->getCode() === $code, 'Wrong rejection: ' . $e->getMessage()); return; }
    throw new RuntimeException('Invalid request was accepted');
}
$rating = new StaffRating($db, $roles, null);
$a = $rating->handle(1, ['action' => 'daily', 'levelID' => '11']);
expect($a['levelID'] === 11 && $a['dailyID'] > 0 && $a['timeLeft'] > 0, 'Missing Daily state');
expect((int)TimedLevelService::current($db, 0)['levelID'] === 11, 'New Daily is not active immediately');
$again = $rating->handle(1, ['action' => 'daily', 'levelID' => '11']);
expect($a['dailyID'] === $again['dailyID'], 'Same Daily created duplicate');
$rating->handle(1, ['action' => 'daily', 'levelID' => '12']);
$back = $rating->handle(1, ['action' => 'daily', 'levelID' => '11']);
expect($back['dailyID'] > $a['dailyID'] && (int)TimedLevelService::current($db, 0)['levelID'] === 11, 'Previous Daily cannot be reactivated');
// Explicit equal-time records exercise the ordering even if the clock ticks during the test.
$stamp = time();
$q = $db->prepare('INSERT INTO dailyfeatures (levelID,timestamp,type) VALUES (?,?,0)');
$q->execute([11,$stamp]); $q->execute([12,$stamp]);
expect((int)TimedLevelService::current($db,0,$stamp)['levelID'] === 12, 'Same-second selection is unstable');
$q->execute([11,$stamp+3600]);
expect((int)TimedLevelService::current($db,0,$stamp)['levelID'] === 12, 'Future entry activated early');
$w = $rating->handle(2, ['action'=>'weekly','levelID'=>'11']);
expect($w['dailyID'] === (int)TimedLevelService::current($db,1)['feaID']+100001, 'Weekly ID mismatch');
expect((int)TimedLevelService::current($db,0)['levelID'] === 12, 'Weekly changed Daily');
denied(fn()=>$rating->handle(0,['action'=>'daily','levelID'=>'11']),403);
denied(fn()=>$rating->handle(1,['action'=>'weekly','levelID'=>'11']),403);
denied(fn()=>$rating->handle(1,['action'=>'daily','levelID'=>'0']),400);
denied(fn()=>$rating->handle(1,['action'=>'daily','levelID'=>'999999']),404);
foreach (['13','14','2147483646','1 OR 1=1'] as $bad) denied(fn()=>$rating->handle(1,['action'=>'daily','levelID'=>$bad]),400);
expect((int)$db->query('SELECT MIN(starStars) FROM levels')->fetchColumn() === 7, 'Daily changed rating');
expect(! $db->inTransaction(), 'Transaction leaked');
expect(TimedLevelService::current($db,2) === null, 'Absent Event state is not empty');
echo "PASS: immediate Daily, same-second ordering, repeat selection, reactivation, separate Weekly, future dates, permission checks, public/data validation, rating preservation. No real levels changed.\n";
