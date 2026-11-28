<?php
final class Collections {
    private PDO $db;
    private $roles;
    public function __construct(PDO $db, $roles) { $this->db = $db; $this->roles = $roles; }
    public static function name($value): string {
        if (!is_string($value)) throw new DomainException('Enter a collection name.', 400);
        $value = trim($value);
        if ($value === '' || strlen($value) > 80 || !preg_match('//u', $value) || preg_match('/[\x00-\x1f\x7f:|#]/u', $value))
            throw new DomainException('Use a name up to 80 bytes, without : | # or control characters.', 400);
        return $value;
    }
    public static function ids($value, int $max, ?int $exact = null): array {
        if (!is_string($value) || strlen($value) > 2000) throw new DomainException('Enter comma-separated GDPS level IDs.', 400);
        $parts = preg_split('/\s*,\s*/', trim($value));
        if (!$parts || count($parts) > $max || ($exact !== null && count($parts) !== $exact))
            throw new DomainException($exact ? "A gauntlet needs exactly $exact levels." : "Use 1–$max levels.", 400);
        $ids = [];
        foreach ($parts as $part) {
            if (!preg_match('/^[1-9][0-9]{0,9}$/D', $part) || (int)$part > 2147483647)
                throw new DomainException('Every level ID must be a positive whole number.', 400);
            $ids[] = (int)$part;
        }
        if (count(array_unique($ids)) !== count($ids)) throw new DomainException('A collection cannot repeat a level.', 400);
        return $ids;
    }
    private function publicLevels(array $ids): void {
        $marks = implode(',', array_fill(0, count($ids), '?'));
        $q = $this->db->prepare("SELECT levelID FROM levels WHERE levelID IN ($marks) AND unlisted = 0 AND unlisted2 = 0 FOR UPDATE");
        $q->execute($ids); $found = array_map('intval', $q->fetchAll(PDO::FETCH_COLUMN));
        $missing = array_values(array_diff($ids, $found));
        if ($missing) throw new DomainException('These GDPS levels are missing or private: ' . implode(', ', $missing), 400);
    }
    public function save(int $actor, array $input): array {
        $access = $this->roles->access($actor);
        $kind = $input['kind'] ?? '';
        if (!in_array($kind, ['list','gauntlet'], true)) throw new DomainException('Choose a list or gauntlet.', 400);
        if (empty($access['canCreateLists']) || ($kind === 'gauntlet' && empty($access['canManageGauntlets'])))
            throw new DomainException('This account cannot manage this collection.', 403);
        $name = self::name($input['name'] ?? '');
        $ids = self::ids($input['levels'] ?? '', $kind === 'gauntlet' ? 5 : 100, $kind === 'gauntlet' ? 5 : null);
        $id = $input['id'] ?? '0';
        if (!is_string($id) || !preg_match('/^(0|[1-9][0-9]{0,9})$/D', $id) || (int)$id > 2147483647)
            throw new DomainException('Invalid collection ID.', 400);
        $id = (int)$id;
        if ($kind === 'gauntlet' && ($id < 1 || $id > 62)) throw new DomainException('Choose a gauntlet theme from 1 to 62.', 400);
        $desc = $input['description'] ?? '';
        if (!is_string($desc) || strlen($desc) > 1000 || !preg_match('//u', $desc)) throw new DomainException('Description is too long or invalid.', 400);
        $this->db->beginTransaction();
        try {
            $this->publicLevels($ids);
            $now = time();
            if ($kind === 'list') {
                if ($id) {
                    $q = $this->db->prepare('SELECT accountID FROM lists WHERE listID = ? FOR UPDATE'); $q->execute([$id]);
                    $owner = $q->fetchColumn();
                    if ($owner === false) throw new DomainException('List not found.', 404);
                    if ((int)$owner !== $actor) throw new DomainException('You can only edit your own lists.', 403);
                    $this->db->prepare('UPDATE lists SET listName=?, listDesc=?, listlevels=?, listVersion=listVersion+1, updateDate=? WHERE listID=?')
                        ->execute([$name,base64_encode($desc),implode(',',$ids),$now,$id]);
                } else {
                    $this->db->prepare('INSERT INTO lists (listName,listDesc,listlevels,accountID,starDifficulty,uploadDate,updateDate) VALUES (?,?,?,?, -1,?,?)')
                        ->execute([$name,base64_encode($desc),implode(',',$ids),$actor,$now,$now]);
                    $id = (int)$this->db->lastInsertId();
                }
            } else {
                $q = $this->db->prepare('SELECT ID FROM gauntlets WHERE ID = ? FOR UPDATE'); $q->execute([$id]);
                if ($q->fetchColumn() !== false && ($input['replace'] ?? '') !== '1')
                    throw new DomainException('That theme already has a gauntlet. Check Replace to update it.', 409);
                $this->db->prepare('INSERT INTO gauntlets (ID,level1,level2,level3,level4,level5) VALUES (?,?,?,?,?,?) ON DUPLICATE KEY UPDATE level1=VALUES(level1),level2=VALUES(level2),level3=VALUES(level3),level4=VALUES(level4),level5=VALUES(level5)')
                    ->execute(array_merge([$id],$ids));
                $this->db->prepare('INSERT INTO gdps_gauntlet_names (gauntletID,name,accountID,updatedAt) VALUES (?,?,?,?) ON DUPLICATE KEY UPDATE name=VALUES(name),accountID=VALUES(accountID),updatedAt=VALUES(updatedAt)')
                    ->execute([$id,$name,$actor,$now]);
            }
            $this->db->commit();
            return ['kind'=>$kind, 'id'=>$id, 'name'=>$name, 'levels'=>$ids];
        } catch (Throwable $e) {
            if ($this->db->inTransaction()) $this->db->rollBack();
            throw $e;
        }
    }
}
