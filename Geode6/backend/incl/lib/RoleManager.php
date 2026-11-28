<?php
require_once __DIR__ . '/mainLib.php';

class RoleManager {
    private $db;
    private $owner;
    private $roles;
    private $gs;

    public function __construct(PDO $db) {
        include __DIR__ . '/../../config/roles.php';
        $this->db = $db;
        $this->owner = (int)$gdpsOwnerAccountID;
        $this->roles = $gdpsManagedRoleIDs;
        $this->gs = new mainLib();
    }

    public function access(int $accountID): array {
        $q = $this->db->prepare('SELECT isAdmin, isActive FROM accounts WHERE accountID = ?');
        $q->execute([$accountID]);
        $account = $q->fetch(PDO::FETCH_ASSOC);
        $active = $account && (int)$account['isActive'] === 1 && !$this->gs->isGuestAccount($accountID);
        $rate = $active && $this->gs->isElderMod($accountID) && $this->gs->checkPermission($accountID, 'actionRateStars');
        $send = $active && ($rate || $this->gs->checkPermission($accountID, 'actionSuggestRating'));
        return [
            'accountID' => $accountID,
            'canRate' => (bool)$rate,
            'canSetDemon' => (bool)($rate && (int)$account['isAdmin'] === 1),
            'canSuggest' => (bool)$send,
            'canFeature' => (bool)($rate && $this->gs->checkPermission($accountID, 'commandFeature')),
            'canSetFire' => (bool)($rate && $this->gs->checkPermission($accountID, 'commandEpic')),
            'canSetDaily' => (bool)($active && $this->gs->isElderMod($accountID) && $this->gs->checkPermission($accountID, 'commandDaily')),
            'canSetWeekly' => (bool)($active && $this->gs->isElderMod($accountID) && $this->gs->checkPermission($accountID, 'commandWeekly')),
            'canManageRoles' => (bool)($active && $accountID === $this->owner && (int)$account['isAdmin'] === 1),
            'accessLevel' => $rate ? 2 : ($send ? 1 : 0),
        ];
    }

    public function assign(int $actor, int $target, string $role): array {
        if (!$this->access($actor)['canManageRoles']) throw new DomainException('Only the server owner can manage roles.', 403);
        if ($target <= 0 || $target === $this->owner) throw new DomainException('The server owner role cannot be changed here.', 403);
        if ($role !== 'player' && !isset($this->roles[$role])) throw new DomainException('Invalid role.', 400);
        $ownsTransaction = !$this->db->inTransaction();
        if ($ownsTransaction) $this->db->beginTransaction();
        try {
            $q = $this->db->prepare('SELECT accountID, userName, isActive FROM accounts WHERE accountID = ? FOR UPDATE');
            $q->execute([$target]);
            $account = $q->fetch(PDO::FETCH_ASSOC);
            if (!$account) throw new DomainException('Player account not found.', 404);
            if ((int)$account['isActive'] !== 1 || $this->gs->isGuestAccount($target))
                throw new DomainException('Staff roles require an active, personal account.', 400);
            $ids = array_values($this->roles);
            $placeholders = implode(',', array_fill(0, count($ids), '?'));
            $this->db->prepare("DELETE FROM roleassign WHERE accountID = ? AND roleID IN ($placeholders)")
                ->execute(array_merge([$target], $ids));
            $this->db->prepare('UPDATE accounts SET isAdmin = ? WHERE accountID = ?')
                ->execute([$role === 'admin' ? 1 : 0, $target]);
            if ($role !== 'player') {
                $this->db->prepare('INSERT INTO roleassign (roleID, accountID) VALUES (?, ?)')
                    ->execute([(int)$this->roles[$role], $target]);
            }
            // Audit event 50: target account, new role, actor. Same transaction.
            $this->db->prepare('INSERT INTO actions (type, value, value2, account, timestamp) VALUES (50, ?, ?, ?, ?)')
                ->execute([(string)$target, $role, $actor, time()]);
            if ($ownsTransaction) $this->db->commit();
            return ['ok' => true, 'targetAccountID' => $target, 'userName' => $account['userName'], 'role' => $role];
        } catch (Throwable $e) {
            if ($ownsTransaction && $this->db->inTransaction()) $this->db->rollBack();
            throw $e;
        }
    }
}
