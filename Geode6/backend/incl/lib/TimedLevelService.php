<?php
/** Shared selection for the Daily card, its download, and staff actions. */
final class TimedLevelService {
    public static function current(PDO $db, int $type, ?int $now = null): ?array {
        if ($type < 0 || $type > 2) throw new DomainException('Invalid timed level type.', 400);
        $q = $db->prepare('SELECT feaID, levelID, timestamp FROM dailyfeatures WHERE timestamp <= ? AND type = ? ORDER BY timestamp DESC, feaID DESC LIMIT 1');
        $q->execute([$now ?? time(), $type]);
        return $q->fetch(PDO::FETCH_ASSOC) ?: null;
    }
    public static function state(array $row, int $type, ?int $now = null): array {
        $now = $now ?? time();
        $end = strtotime($type === 1 ? 'next monday 00:00:00' : 'tomorrow 00:00:00', $now);
        return ['ok' => true, 'levelID' => (int)$row['levelID'], 'type' => $type,
            'dailyID' => (int)$row['feaID'] + ($type === 1 ? 100001 : ($type === 2 ? 200001 : 0)),
            'timeLeft' => max(1, $end - $now)];
    }
    public static function set(PDO $db, $roles, int $actor, int $levelID, int $type): array {
        if (!in_array($type, [0, 1], true)) throw new DomainException('Invalid timed level type.', 400);
        $access = $roles->access($actor);
        if (empty($access[$type === 0 ? 'canSetDaily' : 'canSetWeekly']))
            throw new DomainException('This account cannot set that timed level.', 403);
        if ($levelID < 1) throw new DomainException('Invalid level ID.', 400);
        $db->beginTransaction();
        try {
            $q = $db->prepare('SELECT levelID, unlisted, unlisted2, levelString FROM levels WHERE levelID = ? FOR UPDATE');
            $q->execute([$levelID]); $level = $q->fetch(PDO::FETCH_ASSOC);
            if (!$level) throw new DomainException('Level not found.', 404);
            if ((int)$level['unlisted'] || (int)$level['unlisted2'])
                throw new DomainException('Choose a public level for the Daily or Weekly.', 400);
            if ($level['levelString'] === '' && !is_file(__DIR__ . '/../../data/levels/' . $levelID))
                throw new DomainException('This level has no downloadable data.', 400);
            $now = time(); $row = self::current($db, $type, $now);
            // Re-selecting the current level is harmless. An older Daily can become current again.
            if (!$row || (int)$row['levelID'] !== $levelID) {
                $db->prepare('INSERT INTO dailyfeatures (levelID, timestamp, type) VALUES (?, ?, ?)')->execute([$levelID, $now, $type]);
                $row = ['feaID' => (int)$db->lastInsertId(), 'levelID' => $levelID, 'timestamp' => $now];
                $db->prepare("INSERT INTO modactions (type, value, value2, value3, value4, timestamp, account) VALUES ('5', '1', ?, ?, ?, ?, ?)")
                    ->execute([$now, $levelID, $type, $now, $actor]);
            }
            $db->commit();
            return self::state($row, $type, $now);
        } catch (Throwable $e) {
            if ($db->inTransaction()) $db->rollBack();
            throw $e;
        }
    }
}
