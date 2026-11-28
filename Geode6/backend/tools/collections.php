<?php
require_once __DIR__ . '/../incl/lib/WebSecurity.php';
WebSecurity::startSession();
chdir(__DIR__);
require '../incl/lib/connection.php';
require_once '../incl/lib/RoleManager.php';
require_once '../incl/lib/Collections.php';
require_once '../incl/lib/generatePass.php';
$roles = new RoleManager($db); $error = ''; $message = '';
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    WebSecurity::requireCSRF();
    try {
        if (isset($_POST['login'])) {
            $user = $_POST['user'] ?? ''; $pass = $_POST['password'] ?? '';
            if (!is_string($user) || !is_string($pass) || strlen($user)>100 || strlen($pass)>1024 || GeneratePass::isValidUsrname($user,$pass) != 1)
                throw new DomainException('Wrong GDPS username or password.',403);
            $q=$db->prepare('SELECT accountID FROM accounts WHERE userName=?'); $q->execute([$user]); $id=(int)$q->fetchColumn();
            if (empty($roles->access($id)['canCreateLists'])) throw new DomainException('Use an active personal GDPS account.',403);
            session_regenerate_id(true); $_SESSION['collectionsAccountID']=$id;
        } elseif (isset($_POST['logout'])) {
            $_SESSION['collectionsAccountID'] = 0;
        } else {
            $actor=(int)($_SESSION['collectionsAccountID'] ?? $_SESSION['copyAccountID'] ?? $_SESSION['accountID'] ?? $_SESSION['adminAccountID'] ?? 0);
            $result=(new Collections($db,$roles))->save($actor,$_POST);
            $_SESSION['collectionMessage']=ucfirst($result['kind']) . ' saved: ' . $result['name'] . ' (ID ' . $result['id'] . '). Open ' . ($result['kind']==='list' ? 'Lists' : 'Gauntlets') . ' in your GDPS to play it.';
            header('Location: collections.php'); exit;
        }
    } catch (DomainException $e) { $error=$e->getMessage(); }
    catch (Throwable $e) { error_log('Collections: '.$e->getMessage()); $error='Could not save the collection. Try again shortly.'; }
}
$actor=(int)($_SESSION['collectionsAccountID'] ?? $_SESSION['copyAccountID'] ?? $_SESSION['accountID'] ?? $_SESSION['adminAccountID'] ?? 0);
$access=$roles->access($actor);
if (!empty($_SESSION['collectionMessage'])) { $message=$_SESSION['collectionMessage']; unset($_SESSION['collectionMessage']); }
function esc($s) { return htmlspecialchars((string)$s,ENT_QUOTES,'UTF-8'); }
$lists=[]; $gauntlets=[];
if ($access['canCreateLists']) {
    $q=$db->prepare('SELECT listID,listName,listlevels FROM lists WHERE accountID=? ORDER BY listID DESC LIMIT 100'); $q->execute([$actor]); $lists=$q->fetchAll(PDO::FETCH_ASSOC);
    if ($access['canManageGauntlets']) $gauntlets=$db->query('SELECT g.*,n.name FROM gauntlets g LEFT JOIN gdps_gauntlet_names n ON g.ID=n.gauntletID ORDER BY ID')->fetchAll(PDO::FETCH_ASSOC);
}
?>
<!doctype html><html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1"><title>Lists &amp; Gauntlets · Damian GDPS</title>
<style>body{font:16px system-ui;background:#0c1220;color:#edf3ff;margin:0}main{max-width:850px;margin:40px auto;padding:0 20px}a{color:#83baff}section{background:#141e31;border:1px solid #2a3953;border-radius:16px;padding:24px;margin:20px 0}h1,h2{margin:0 0 16px}label{display:block;margin:16px 0 6px}input,textarea,select,button{font:inherit;box-sizing:border-box}input:not([type=checkbox]),textarea,select{width:100%;padding:12px;border:1px solid #42516a;background:#0c1424;color:inherit;border-radius:8px}textarea{min-height:90px}button{padding:10px 22px;border:0;border-radius:8px;background:#a5ed50;color:#111d04;margin-top:18px;cursor:pointer}small{color:#b0bfd4}.notice{padding:16px;border-radius:8px;background:#1c4431}.error{background:#512337}table{width:100%;border-collapse:collapse}td,th{text-align:left;padding:10px;border-bottom:1px solid #2a3953;overflow-wrap:anywhere}</style>
<main><a href="index.php">← GDPS tools</a><h1>Lists &amp; Gauntlets</h1><p>Choose levels from Damian GDPS and put them in your own order.</p>
<?php if($error): ?><p class="notice error"><?=esc($error)?></p><?php endif ?>
<?php if($message): ?><p class="notice"><?=esc($message)?></p><?php endif ?>
<?php if(!$access['canCreateLists']): ?>
<section><h2>Sign in to your GDPS</h2><form method="post"><?=WebSecurity::csrfField()?><label>Username<input name="user" autocomplete="username" required maxlength="100"></label><label>Password<input type="password" name="password" autocomplete="current-password" required></label><button name="login" value="1">Sign in</button></form></section>
<?php else: ?>
<section><h2>Create or edit a list</h2><form method="post"><?=WebSecurity::csrfField()?><input type="hidden" name="kind" value="list"><label>List<select name="id"><option value="0">Create a new list</option><?php foreach($lists as $list): ?><option value="<?=esc($list['listID'])?>"><?=esc($list['listName'])?> (#<?=esc($list['listID'])?>)</option><?php endforeach ?></select></label><label>Name<input name="name" required maxlength="80"></label><label>Description<textarea name="description" maxlength="1000"></textarea></label><label>GDPS level IDs, in order<input name="levels" required placeholder="60, 59, 58, 61, 64"></label><small>Use 1–100 different public GDPS levels. Existing list ratings stay unchanged when edited.</small><br><button>Save list</button></form></section>
<?php if($access['canManageGauntlets']): ?>
<section><h2>Create or update a gauntlet</h2><form method="post"><?=WebSecurity::csrfField()?><input type="hidden" name="kind" value="gauntlet"><label>Name<input name="name" required maxlength="80"></label><label>Gauntlet theme<select name="id"><?php for($i=1;$i<=62;$i++): ?><option value="<?=$i?>"><?=$i?> — <?=esc((new mainLib())->getGauntletName($i))?></option><?php endfor ?></select></label><label>Five GDPS level IDs, in order<input name="levels" required placeholder="60, 59, 58, 61, 64"></label><label><input type="checkbox" name="replace" value="1"> Replace the existing gauntlet in this theme</label><small>The updated mod shows your custom name. Other clients use the theme's standard name.</small><br><button>Save gauntlet</button></form>
<?php if($gauntlets): ?><h3>Current gauntlets</h3><table><tr><th>Theme</th><th>Name</th><th>Levels</th></tr><?php foreach($gauntlets as $g): ?><tr><td><?=esc($g['ID'])?></td><td><?=esc($g['name'] ?? (new mainLib())->getGauntletName($g['ID']))?></td><td><?=esc(implode(', ',array_map(fn($i)=>$g['level'.$i],range(1,5))))?></td></tr><?php endforeach ?></table><?php endif ?></section>
<?php endif ?>
<section><h2>Your lists</h2><?php if(!$lists): ?><p>Your first list will appear here.</p><?php else: ?><table><tr><th>ID</th><th>Name</th><th>Levels</th></tr><?php foreach($lists as $l): ?><tr><td><?=esc($l['listID'])?></td><td><?=esc($l['listName'])?></td><td><?=esc($l['listlevels'])?></td></tr><?php endforeach ?></table><?php endif ?><form method="post"><?=WebSecurity::csrfField()?><button name="logout" value="1">Sign out of collections</button></form></section>
<?php endif ?></main></html>
