<?php
final class FeaturedOrder {
    public static function available(PDO $db): bool {
        static $cache = [];
        $key = spl_object_id($db);
        if (!array_key_exists($key, $cache)) {
            $q = $db->query("SHOW COLUMNS FROM levels LIKE 'featuredOrder'");
            $cache[$key] = (bool)$q->fetch();
        }
        return $cache[$key];
    }
    public static function install(PDO $db): void {
        if (!self::available($db)) {
            try { $db->exec('ALTER TABLE levels ADD COLUMN featuredOrder INT NOT NULL DEFAULT 0'); }
            catch (PDOException $e) {
                // A concurrent authenticated request may have installed it first.
                if (($e->errorInfo[1] ?? 0) !== 1060) throw $e;
            }
        }
    }
    public static function order(PDO $db): string {
        return self::available($db)
            ? 'CASE WHEN featuredOrder > 0 THEN featuredOrder ELSE 1 END ASC, rateDate DESC, levels.levelID'
            : 'rateDate DESC, levels.levelID';
    }
}
