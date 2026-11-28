<?php
require_once __DIR__ . '/FeaturedOrder.php';
final class StaffRating {
    private PDO $db;
    private $roles;
    private $gs;
    public function __construct(PDO $db, $roles, $gs) {
        $this->db = $db; $this->roles = $roles; $this->gs = $gs;
    }
    public static function number($value, int $max = 2147483647): int {
        if (!is_string($value) || !preg_match('/^(0|[1-9][0-9]{0,9})$/D', $value)
            || (int)$value > $max) throw new DomainException('Invalid whole number.', 400);
        return (int)$value;
    }
    public function handle(int $actor, array $input): array {
        $access = $this->roles->access($actor);
        $action = $input['action'] ?? '';
        if (!is_string($action) || !in_array($action, ['read', 'stars', 'featured', 'send'], true))
            throw new DomainException('Unknown rating action.', 400);
        if (empty($access['canSuggest']) || ($action === 'stars' && empty($access['canRate']))
            || ($action === 'featured' && empty($access['canFeature'])))
            throw new DomainException('This account cannot perform that action.', 403);
        $id = self::number($input['levelID'] ?? '');
        if (!$id) throw new DomainException('Invalid level ID.', 400);
        if ($action === 'featured') FeaturedOrder::install($this->db);
        $this->db->beginTransaction();
        try {
            $q = $this->db->prepare('SELECT * FROM levels WHERE levelID = ? FOR UPDATE');
            $q->execute([$id]); $level = $q->fetch(PDO::FETCH_ASSOC);
            if (!$level) throw new DomainException('Level not found.', 404);
            if ($action === 'featured') {
                $rank = self::number($input['order'] ?? '');
                $tier = self::number($input['tier'] ?? '', 4);
                if (($rank === 0) !== ($tier === 0))
                    throw new DomainException('Choose a fire tier for a featured level, or set order to zero to remove it.', 400);
                if ($tier > 1 && empty($access['canSetFire']))
                    throw new DomainException('This account cannot set Epic or higher fire.', 403);
                $this->db->prepare('UPDATE levels SET featuredOrder = ?, starFeatured = ?, starEpic = ?, rateDate = ? WHERE levelID = ?')
                    ->execute([$rank, $rank ? 1 : 0, max(0, $tier - 1), time(), $id]);
                $this->audit($actor, $id, '2', $rank, $tier);
            } elseif ($action === 'stars' || $action === 'send') {
                $stars = self::number($input['stars'] ?? '');
                $coins = self::number($input['coins'] ?? '', 1);
                $name = $input['difficulty'] ?? '';
                $allowed = ['na', 'auto', 'easy', 'very-easy', 'normal', 'hard', 'harder', 'insane',
                    'easy-demon', 'medium-demon', 'hard-demon', 'insane-demon', 'extreme-demon',
                    'supreme-demon', 'ultimate-demon', 'legendary-demon', 'mythical-demon', 'silent-demon', 'grandpa-demon'];
                if (!is_string($name) || !in_array($name, $allowed, true))
                    throw new DomainException('Invalid difficulty.', 400);
                [$diff, $demon, $auto, $subtype] = $this->gs->getDiffFromName($name);
                if ($action === 'stars') {
                    // Rating a face/award never changes its Featured position or fire.
                    $this->db->prepare('UPDATE levels SET starStars = ?, starDifficulty = ?, starDemon = ?, starAuto = ?, starDemonDiff = ?, starCoins = ?, rateDate = ? WHERE levelID = ?')
                        ->execute([$stars, $diff, $demon, $auto, $subtype, $coins, time(), $id]);
                    $this->audit($actor, $id, '1', $name, $stars);
                } else {
                    $tier = self::number($input['tier'] ?? '', 4);
                    $this->db->prepare('DELETE FROM suggest WHERE suggestBy = ? AND suggestLevelId = ?')->execute([$actor, $id]);
                    $this->db->prepare('INSERT INTO suggest (suggestBy, suggestLevelId, suggestDifficulty, suggestStars, suggestFeatured, suggestAuto, suggestDemon, suggestDemonDiff, suggestCoins, timestamp) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)')
                        ->execute([$actor, $id, $diff, $stars, $tier, $auto, $demon, $subtype, $coins, time()]);
                }
            }
            $q->execute([$id]); $level = $q->fetch(PDO::FETCH_ASSOC);
            $this->db->commit();
            return ['ok' => true, 'levelID' => $id, 'stars' => (int)$level['starStars'],
                'coins' => (int)$level['starCoins'],
                'difficulty' => (int)$level['starDifficulty'], 'demon' => (int)$level['starDemon'],
                'auto' => (int)$level['starAuto'], 'subtype' => (int)$level['starDemonDiff'],
                'order' => ((int)$level['starFeatured'] || (int)$level['starEpic']) ? max(1, (int)($level['featuredOrder'] ?? 0)) : 0,
                'tier' => (int)$level['starEpic'] ? (int)$level['starEpic'] + 1 : ((int)$level['starFeatured'] ? 1 : 0)];
        } catch (Throwable $e) {
            if ($this->db->inTransaction()) $this->db->rollBack();
            throw $e;
        }
    }
    private function audit(int $actor, int $level, string $type, $value, $extra): void {
        $this->db->prepare('INSERT INTO modactions (type, value, value2, value3, timestamp, account) VALUES (?, ?, ?, ?, ?, ?)')
            ->execute([$type, $value, $extra, $level, time(), $actor]);
    }
}
