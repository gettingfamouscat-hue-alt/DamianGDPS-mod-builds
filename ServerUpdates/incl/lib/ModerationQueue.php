<?php
// Native GD 2.2091 queue/report protocol. Unimplemented automated flags stay empty.
final class ModerationQueue {
    public function __construct(private PDO $db, private $roles) {}
    private static function number($v, int $min, int $max): int {
        if (!is_scalar($v) || !preg_match('/^-?[0-9]{1,10}$/D',(string)$v) || (int)$v<$min || (int)$v>$max)
            throw new DomainException('Invalid number.',400);
        return (int)$v;
    }
    private function target(int $type,int $id,int $special,int $actor): int {
        switch ($type) {
            case 0: $sql='SELECT accountID FROM accounts WHERE accountID=?'; $args=[$id]; break;
            case 1: $sql='SELECT extID FROM levels WHERE levelID=?'; $args=[$id]; break;
            case 2: $sql='SELECT accountID FROM lists WHERE listID=?'; $args=[$id]; break;
            case 3: $sql='SELECT u.extID FROM comments c JOIN users u ON u.userID=c.userID WHERE c.commentID=? AND c.levelID=?'; $args=[$id,$special]; break;
            case 4: $sql='SELECT u.extID FROM acccomments c JOIN users u ON u.userID=c.userID WHERE c.commentID=? AND u.extID=?'; $args=[$id,$special]; break;
            case 5:
                if (!$actor) throw new DomainException('Sign in to report a private message.',403);
                $sql='SELECT accID FROM messages WHERE messageID=? AND (accID=? OR toAccountID=?)'; $args=[$id,$actor,$actor]; break;
            case 6:
                if (!$actor) throw new DomainException('Sign in to report a friend request.',403);
                $sql='SELECT accountID FROM friendreqs WHERE ID=? AND (accountID=? OR toAccountID=?)'; $args=[$id,$actor,$actor]; break;
            default: throw new DomainException('Unsupported content type.',400);
        }
        $q=$this->db->prepare($sql); $q->execute($args); $owner=(int)$q->fetchColumn();
        $q=$this->db->prepare('SELECT accountID FROM accounts WHERE accountID=?'); $q->execute([$owner]);
        if ($owner<1 || !$q->fetchColumn()) throw new DomainException('Content not found.',404);
        return $owner;
    }
    public function report(int $actor,string $ip,array $input): void {
        if ($actor && empty($this->roles->access($actor)['canCreateLists'])) throw new DomainException('Inactive account.',403);
        $type=self::number($input['type']??null,0,6);
        $id=self::number($input['targetID']??null,1,2147483647);
        $special=self::number($input['special']??0,-2147483647,2147483647);
        $raw=$input['reasons']??null;
        if (!is_string($raw) || !preg_match('/^[0-7](,[0-7]){0,7}$/D',$raw)) throw new DomainException('Invalid reasons.',400);
        $reasons=array_unique(array_map('intval',explode(',',$raw)));
        foreach ($reasons as $reason) {
            if ((in_array($reason,[4,5],true) && $type!==0) || ($reason===6 && !in_array($type,[1,2],true)))
                throw new DomainException('Reason does not apply to this content.',400);
        }
        $target=$this->target($type,$id,$special,$actor);
        $key=hash('sha256',$actor ? 'account:'.$actor : 'ip:'.$ip);
        $q=$this->db->prepare('SELECT COUNT(*) FROM content_reports WHERE reporterKey=? AND createdAt>?'); $q->execute([$key,time()-3600]);
        if ((int)$q->fetchColumn()>=100) throw new DomainException('Too many reports. Try later.',429);
        $this->db->beginTransaction();
        try {
            $q=$this->db->prepare('INSERT INTO content_reports (targetAccountID,reporterAccountID,reporterKey,kind,itemID,special,reason,createdAt) VALUES (?,?,?,?,?,?,?,?) ON DUPLICATE KEY UPDATE reportID=reportID');
            foreach ($reasons as $reason) $q->execute([$target,$actor,$key,$type,$id,$special,$reason,time()]);
            $this->db->commit();
        } catch (Throwable $e) { if ($this->db->inTransaction()) $this->db->rollBack(); throw $e; }
    }
    public function queue(int $actor,array $input): string {
        $access=$this->roles->access($actor);
        if (empty($access['canSuggest']) && empty($access['canRate'])) throw new DomainException('Moderator access required.',403);
        $page=self::number($input['page']??0,0,100000);
        $sort=self::number($input['sort']??0,0,1);
        $mode=self::number($input['mode']??0,0,2); $offset=$page*20;
        if ($mode===2) return '#0:'.$offset.':20';
        // Legacy level reports are retained, with no invented date or flag score.
        $events="SELECT targetAccountID,reporterAccountID,CAST(reporterKey AS BINARY) reporterKey,kind,itemID,reason,createdAt,reportID FROM content_reports WHERE status=0
            UNION ALL SELECT l.extID,0,CAST(SHA2(CONCAT('ip:',r.hostname),256) AS BINARY),1,r.levelID,0,0,r.ID FROM reports r JOIN levels l ON l.levelID=r.levelID";
        $from="FROM ($events) e JOIN accounts a ON a.accountID=e.targetAccountID";
        $total=(int)$this->db->query("SELECT COUNT(DISTINCT a.accountID) $from")->fetchColumn();
        $order=$sort===1 ? 'newest DESC,sequence DESC' : 'reportCount DESC,newest DESC';
        $rows=$this->db->query("SELECT a.accountID,a.userName,
            COUNT(DISTINCT CASE WHEN kind<>0 THEN CONCAT(kind,':',itemID) END) items,
            COUNT(DISTINCT CASE WHEN reporterAccountID=0 THEN reporterKey END) anonymous,
            COUNT(DISTINCT CASE WHEN reporterAccountID>0 THEN reporterAccountID END) registered,
            COUNT(DISTINCT CASE WHEN kind=0 THEN reporterKey END) direct,
            COUNT(*) reportCount,MAX(createdAt) newest,MAX(reportID) sequence
            $from GROUP BY a.accountID,a.userName ORDER BY $order,a.accountID ASC LIMIT 20 OFFSET $offset")->fetchAll(PDO::FETCH_ASSOC);
        $reason=$this->db->prepare("SELECT reason $from WHERE a.accountID=? GROUP BY reason ORDER BY COUNT(*) DESC,reason ASC LIMIT 1");
        $wire=[];
        foreach ($rows as $row) {
            $reason->execute([$row['accountID']]);
            $age=$row['newest'] ? max(0,(int)floor((time()-(int)$row['newest'])/60)).'m' : '';
            $name=strtr(base64_encode($row['userName']),'+/','-_');
            $wire[]=implode(':',[$row['accountID'],$name,$row['items'],0,$row['anonymous'],(int)$reason->fetchColumn(),0,$age,$row['direct'],0,'','',0,0,0,0,0,0,0,$row['registered']]);
        }
        return implode('|',$wire).'#'.$total.':'.$offset.':20';
    }
}
