<?php
class Commands {
	public static function ownCommand($comment, $command, $accountID, $targetExtID){
		require_once "../lib/mainLib.php";
		$gs = new mainLib();
		$commandInComment = strtolower("!".$command);
		$commandInPerms = ucfirst(strtolower($command));
		$commandlength = strlen($commandInComment);
		if(substr($comment,0,$commandlength) == $commandInComment AND (($gs->checkPermission($accountID, "command".$commandInPerms."All") OR ($targetExtID == $accountID AND $gs->checkPermission($accountID, "command".$commandInPerms."Own"))))){
			return true;
		}
		return false;
	}
	public static function doCommands($accountID, $comment, $levelID) {
		if(!is_numeric($accountID)) return false;
		if($levelID < 0) return self::doListCommands($accountID, $comment, $levelID);
		include dirname(__FILE__)."/../lib/connection.php";
		require_once "../lib/exploitPatch.php";
		require_once "../lib/mainLib.php";
		$gs = new mainLib();
		$commentarray = explode(' ', $comment);
		$uploadDate = time();
		//LEVELINFO
		$query2 = $db->prepare("SELECT extID FROM levels WHERE levelID = :id");
		$query2->execute([':id' => $levelID]);
		$targetExtID = $query2->fetchColumn();
		//ADMIN COMMANDS
		// !rate — Elder Mod only (Rate Button Submit). Normal mods get converted to a send/suggest.
		if(substr($comment,0,5) == '!rate'){
			$starStars = isset($commentarray[2]) ? $commentarray[2] : "0";
			if(!ctype_digit($starStars) || strlen($starStars) > 10 || intval($starStars) > 2147483647) return true;
			$starStars = intval($starStars);
			$starCoins = isset($commentarray[3]) ? $commentarray[3] : "";
			$starFeatured = isset($commentarray[4]) ? $commentarray[4] : "";
			$diffName = isset($commentarray[1]) ? $commentarray[1] : "na";
			$diffArray = $gs->getDiffFromName($diffName);
			$starDifficulty = $diffArray[0];
			$starDemon = $diffArray[1];
			$starAuto = $diffArray[2];
			$starDemonDiff = isset($diffArray[3]) ? $diffArray[3] : 0;
			$feat = ($starFeatured !== "" && $starFeatured !== null) ? intval($starFeatured) : 0;

			if($gs->isElderMod($accountID) AND $gs->checkPermission($accountID, "commandRate")){
				$query = $db->prepare("UPDATE levels SET starStars=:starStars, starDifficulty=:starDifficulty, starDemon=:starDemon, starDemonDiff=:starDemonDiff, starAuto=:starAuto, rateDate=:timestamp WHERE levelID=:levelID");
				$query->execute([':starStars' => $starStars, ':starDifficulty' => $starDifficulty, ':starDemon' => $starDemon, ':starDemonDiff' => $starDemonDiff, ':starAuto' => $starAuto, ':timestamp' => $uploadDate, ':levelID' => $levelID]);
				$query = $db->prepare("INSERT INTO modactions (type, value, value2, value3, timestamp, account) VALUES ('1', :value, :value2, :levelID, :timestamp, :id)");
				$query->execute([':value' => $diffName, ':timestamp' => $uploadDate, ':id' => $accountID, ':value2' => $starStars, ':levelID' => $levelID]);
				$gs->featureLevel($accountID, $levelID, $feat);
				if($starCoins !== ""){
					$query = $db->prepare("INSERT INTO modactions (type, value, value3, timestamp, account) VALUES ('3', :value, :levelID, :timestamp, :id)");
					$query->execute([':value' => $starCoins, ':timestamp' => $uploadDate, ':id' => $accountID, ':levelID' => $levelID]);
					$query = $db->prepare("UPDATE levels SET starCoins=:starCoins WHERE levelID=:levelID");
					$query->execute([':starCoins' => $starCoins, ':levelID' => $levelID]);
				}
				return true;
			}
			// Normal mod: Rate Button Submit becomes a send to the Sent tab
			if($gs->checkPermission($accountID, "actionSuggestRating")){
				$gs->suggestLevel($accountID, $levelID, $starDifficulty, $starStars, $feat, $starAuto, $starDemon);
				return true;
			}
			return true; // consume command, no public comment leak
		}
		// !send / !unsend — Normal + Elder mods (Rate Button "Send Only")
		if(substr($comment,0,5) == '!send' AND $gs->checkPermission($accountID, "actionSuggestRating")){
			// !send {diff} {stars} {feature} {verifiedCoins}
			$diffName = isset($commentarray[1]) ? $commentarray[1] : "na";
			$starStars = isset($commentarray[2]) ? $commentarray[2] : "0";
			if(!ctype_digit($starStars) || strlen($starStars) > 10 || intval($starStars) > 2147483647) return true;
			$starStars = intval($starStars);
			$feat = isset($commentarray[3]) ? intval($commentarray[3]) : 0;
			$diffArray = $gs->getDiffFromName($diffName);
			$coins = isset($commentarray[4]) && $commentarray[4] === "1" ? 1 : 0;
			$gs->suggestLevel($accountID, $levelID, $diffArray[0], $starStars, $feat, $diffArray[2], $diffArray[1], $diffArray[3], $coins);
			return true;
		}
		if(substr($comment,0,7) == '!unsend' AND $gs->checkPermission($accountID, "actionSuggestRating")){
			$query = $db->prepare("DELETE FROM suggest WHERE suggestBy = :acc AND suggestLevelId = :levelID");
			$query->execute([':acc' => $accountID, ':levelID' => $levelID]);
			return true;
		}
		if(substr($comment,0,7) == '!unrate' AND $gs->isElderMod($accountID) AND $gs->checkPermission($accountID, "commandRate")){
			$query = $db->prepare("UPDATE levels SET starStars=0, starDifficulty=0, starDemon=0, starDemonDiff=0, starAuto=0, starFeatured=0, starEpic=0, starCoins=0, rateDate=:timestamp WHERE levelID=:levelID");
			$query->execute([':timestamp' => $uploadDate, ':levelID' => $levelID]);
			return true;
		}
		// !demon easy|medium|hard|insane|extreme|impossible — Elder Mod only
		if(substr($comment,0,6) == '!demon' AND $gs->isElderMod($accountID) AND ($gs->checkPermission($accountID, "actionRateDemon") OR $gs->checkPermission($accountID, "commandRate"))){
			$tier = isset($commentarray[1]) ? strtolower($commentarray[1]) : "hard";
			$diffName = (strpos($tier, "demon") !== false || $tier === "impossible") ? (($tier === "impossible") ? "impossible-demon" : $tier) : ($tier . "-demon");
			$diffArray = $gs->getDiffFromName($diffName);
			if(empty($diffArray[1])){
				// bare names already covered by getDiffFromName; fallback hard
				$diffArray = $gs->getDiffFromName("hard-demon");
			}
			$starDemonDiff = isset($diffArray[3]) ? $diffArray[3] : 0;
			$query = $db->prepare("UPDATE levels SET starDemon = 1, starDemonDiff = :diff, starDifficulty = 50, rateDate = :timestamp WHERE levelID = :levelID");
			$query->execute([':diff' => $starDemonDiff, ':timestamp' => $uploadDate, ':levelID' => $levelID]);
			$query = $db->prepare("INSERT INTO modactions (type, value, value3, timestamp, account) VALUES ('10', :value, :levelID, :timestamp, :id)");
			$query->execute([':value' => $diffName, ':timestamp' => $uploadDate, ':id' => $accountID, ':levelID' => $levelID]);
			return true;
		}
		if(substr($comment,0,8) == '!feature' AND $gs->isElderMod($accountID) AND $gs->checkPermission($accountID, "commandFeature")){
			$gs->featureLevel($accountID, $levelID, 1);
			return true;
		}
		if(substr($comment,0,5) == '!epic' AND $gs->isElderMod($accountID) AND $gs->checkPermission($accountID, "commandEpic")){
			// Optional tier: !epic [1|2|3] → Epic/Legendary/Mythic (default Epic)
			$epicTier = isset($commentarray[1]) && $commentarray[1] !== "" ? intval($commentarray[1]) : 1;
			if($epicTier < 1) $epicTier = 1;
			if($epicTier > 3) $epicTier = 3;
			$gs->featureLevel($accountID, $levelID, $epicTier + 1); // 2/3/4 in featureLevel terms
			return true;
		}
		if(substr($comment,0,7) == '!unepic' AND $gs->isElderMod($accountID) AND $gs->checkPermission($accountID, "commandUnepic")){
			$query = $db->prepare("UPDATE levels SET starEpic='0' WHERE levelID=:levelID");
			$query->execute([':levelID' => $levelID]);
			$query = $db->prepare("INSERT INTO modactions (type, value, value3, timestamp, account) VALUES ('4', :value, :levelID, :timestamp, :id)");
			$query->execute([':value' => "0", ':timestamp' => $uploadDate, ':id' => $accountID, ':levelID' => $levelID]);
			return true;
		}
		if(substr($comment,0,12) == '!verifycoins' AND $gs->isElderMod($accountID) AND $gs->checkPermission($accountID, "commandVerifycoins")){
			$query = $db->prepare("UPDATE levels SET starCoins='1' WHERE levelID = :levelID");
			$query->execute([':levelID' => $levelID]);
			$query = $db->prepare("INSERT INTO modactions (type, value, value3, timestamp, account) VALUES ('2', :value, :levelID, :timestamp, :id)");
			$query->execute([':value' => "1", ':timestamp' => $uploadDate, ':id' => $accountID, ':levelID' => $levelID]);
			return true;
		}
		if (preg_match('/^!(daily|weekly)(?:\s|$)/', $comment, $timed)) {
			require_once __DIR__ . '/TimedLevelService.php';
			require_once __DIR__ . '/RoleManager.php';
			try {
				TimedLevelService::set($db, new RoleManager($db), (int)$accountID, (int)$levelID, $timed[1] === 'daily' ? 0 : 1);
				return true;
			} catch (DomainException $e) { return false; }
		}
		if(substr($comment,0,6) == '!delet' AND $gs->isElderMod($accountID) AND $gs->checkPermission($accountID, "commandDelete")){
			if(!is_numeric($levelID)){
				return false;
			}
			$query = $db->prepare("DELETE from levels WHERE levelID=:levelID LIMIT 1");
			$query->execute([':levelID' => $levelID]);
			$query = $db->prepare("INSERT INTO modactions (type, value, value3, timestamp, account) VALUES ('6', :value, :levelID, :timestamp, :id)");
			$query->execute([':value' => "1", ':timestamp' => $uploadDate, ':id' => $accountID, ':levelID' => $levelID]);
			if(file_exists(dirname(__FILE__)."../../data/levels/$levelID")){
				rename(dirname(__FILE__)."../../data/levels/$levelID",dirname(__FILE__)."../../data/levels/deleted/$levelID");
			}
			return true;
		}
		if(substr($comment,0,7) == '!setacc' AND $gs->checkPermission($accountID, "commandSetacc")){
			$query = $db->prepare("SELECT accountID FROM accounts WHERE userName = :userName OR accountID = :userName LIMIT 1");
			$query->execute([':userName' => $commentarray[1]]);
			if($query->rowCount() == 0){
				return false;
			}
			$targetAcc = $query->fetchColumn();
			//var_dump($result);
			$query = $db->prepare("SELECT userID FROM users WHERE extID = :extID LIMIT 1");
			$query->execute([':extID' => $targetAcc]);
			$userID = $query->fetchColumn();
			$query = $db->prepare("UPDATE levels SET extID=:extID, userID=:userID, userName=:userName WHERE levelID=:levelID");
			$query->execute([':extID' => $targetAcc, ':userID' => $userID, ':userName' => $commentarray[1], ':levelID' => $levelID]);
			$query = $db->prepare("INSERT INTO modactions (type, value, value3, timestamp, account) VALUES ('7', :value, :levelID, :timestamp, :id)");
			$query->execute([':value' => $commentarray[1], ':timestamp' => $uploadDate, ':id' => $accountID, ':levelID' => $levelID]);
			return true;
		}

		
	//NON-ADMIN COMMANDS
		if(self::ownCommand($comment, "rename", $accountID, $targetExtID)){
			$name = ExploitPatch::remove(str_replace("!rename ", "", $comment));
			$query = $db->prepare("UPDATE levels SET levelName=:levelName WHERE levelID=:levelID");
			$query->execute([':levelID' => $levelID, ':levelName' => $name]);
			$query = $db->prepare("INSERT INTO modactions (type, value, timestamp, account, value3) VALUES ('8', :value, :timestamp, :id, :levelID)");
			$query->execute([':value' => $name, ':timestamp' => $uploadDate, ':id' => $accountID, ':levelID' => $levelID]);
			return true;
		}
		if(self::ownCommand($comment, "pass", $accountID, $targetExtID)){
			$pass = ExploitPatch::remove(str_replace("!pass ", "", $comment));
			if(is_numeric($pass)){
				$pass = sprintf("%06d", $pass);
				if($pass == "000000"){
					$pass = "";
				}
				$pass = "1".$pass;
				$query = $db->prepare("UPDATE levels SET password=:password WHERE levelID=:levelID");
				$query->execute([':levelID' => $levelID, ':password' => $pass]);
				$query = $db->prepare("INSERT INTO modactions (type, value, timestamp, account, value3) VALUES ('9', :value, :timestamp, :id, :levelID)");
				$query->execute([':value' => $pass, ':timestamp' => $uploadDate, ':id' => $accountID, ':levelID' => $levelID]);
				return true;
			}
		}
		if(self::ownCommand($comment, "song", $accountID, $targetExtID)){
			$song = ExploitPatch::remove(str_replace("!song ", "", $comment));
			if(is_numeric($song)){
				$query = $db->prepare("UPDATE levels SET songID=:song WHERE levelID=:levelID");
				$query->execute([':levelID' => $levelID, ':song' => $song]);
				$query = $db->prepare("INSERT INTO modactions (type, value, timestamp, account, value3) VALUES ('16', :value, :timestamp, :id, :levelID)");
				$query->execute([':value' => $song, ':timestamp' => $uploadDate, ':id' => $accountID, ':levelID' => $levelID]);
				return true;
			}
		}
		if(self::ownCommand($comment, "description", $accountID, $targetExtID)){
			$desc = base64_encode(ExploitPatch::remove(str_replace("!description ", "", $comment)));
			$query = $db->prepare("UPDATE levels SET levelDesc=:desc WHERE levelID=:levelID");
			$query->execute([':levelID' => $levelID, ':desc' => $desc]);
			$query = $db->prepare("INSERT INTO modactions (type, value, timestamp, account, value3) VALUES ('13', :value, :timestamp, :id, :levelID)");
			$query->execute([':value' => $desc, ':timestamp' => $uploadDate, ':id' => $accountID, ':levelID' => $levelID]);
			return true;
		}
		if(self::ownCommand($comment, "public", $accountID, $targetExtID)){
			$query = $db->prepare("UPDATE levels SET unlisted='0' WHERE levelID=:levelID");
			$query->execute([':levelID' => $levelID]);
			$query = $db->prepare("INSERT INTO modactions (type, value, value3, timestamp, account) VALUES ('12', :value, :levelID, :timestamp, :id)");
			$query->execute([':value' => "0", ':timestamp' => $uploadDate, ':id' => $accountID, ':levelID' => $levelID]);
			return true;
		}
		if(self::ownCommand($comment, "unlist", $accountID, $targetExtID)){
			$query = $db->prepare("UPDATE levels SET unlisted='1' WHERE levelID=:levelID");
			$query->execute([':levelID' => $levelID]);
			$query = $db->prepare("INSERT INTO modactions (type, value, value3, timestamp, account) VALUES ('12', :value, :levelID, :timestamp, :id)");
			$query->execute([':value' => "1", ':timestamp' => $uploadDate, ':id' => $accountID, ':levelID' => $levelID]);
			return true;
		}
		if(self::ownCommand($comment, "sharecp", $accountID, $targetExtID)){
			$query = $db->prepare("SELECT userID FROM users WHERE userName = :userName ORDER BY isRegistered DESC LIMIT 1");
			$query->execute([':userName' => $commentarray[1]]);
			$targetAcc = $query->fetchColumn();
			//var_dump($result);
			$query = $db->prepare("INSERT INTO cpshares (levelID, userID) VALUES (:levelID, :userID)");
			$query->execute([':userID' => $targetAcc, ':levelID' => $levelID]);
			$query = $db->prepare("UPDATE levels SET isCPShared='1' WHERE levelID=:levelID");
			$query->execute([':levelID' => $levelID]);
			$query = $db->prepare("INSERT INTO modactions (type, value, value3, timestamp, account) VALUES ('11', :value, :levelID, :timestamp, :id)");
			$query->execute([':value' => $commentarray[1], ':timestamp' => $uploadDate, ':id' => $accountID, ':levelID' => $levelID]);
			return true;
		}
		if(self::ownCommand($comment, "ldm", $accountID, $targetExtID)){
			$query = $db->prepare("UPDATE levels SET isLDM='1' WHERE levelID=:levelID");
			$query->execute([':levelID' => $levelID]);
			$query = $db->prepare("INSERT INTO modactions (type, value, value3, timestamp, account) VALUES ('14', :value, :levelID, :timestamp, :id)");
			$query->execute([':value' => "1", ':timestamp' => $uploadDate, ':id' => $accountID, ':levelID' => $levelID]);
			return true;
		}
		if(self::ownCommand($comment, "unldm", $accountID, $targetExtID)){
			$query = $db->prepare("UPDATE levels SET isLDM='0' WHERE levelID=:levelID");
			$query->execute([':levelID' => $levelID]);
			$query = $db->prepare("INSERT INTO modactions (type, value, value3, timestamp, account) VALUES ('14', :value, :levelID, :timestamp, :id)");
			$query->execute([':value' => "0", ':timestamp' => $uploadDate, ':id' => $accountID, ':levelID' => $levelID]);
			return true;
		}
		return false;
	}
	public static function doListCommands($accountID, $command, $listID) {
		if(substr($command,0,1) != '!') return false;
		$listID = $listID * -1;
		include dirname(__FILE__)."/../lib/connection.php";
		require_once "../lib/exploitPatch.php";
		require_once "../lib/mainLib.php";
		$gs = new mainLib();
		$carray = explode(' ', $command);
		switch($carray[0]) {
			case '!r':
			case '!rate':
				$getList = $db->prepare('SELECT * FROM lists WHERE listID = :listID');
				$getList->execute([':listID' => $listID]);
				$getList = $getList->fetch();
				$reward = ExploitPatch::number($carray[1]);
				$diff = ExploitPatch::charclean($carray[2]);
				$featured = is_numeric($carray[3]) ? ExploitPatch::number($carray[3]) : ExploitPatch::number($carray[4]);
				$count = is_numeric($carray[3]) ? ExploitPatch::number($carray[4]) : ExploitPatch::number($carray[5]);
				if(empty($count)) {
					$levelsCount = $getList['listlevels'];
					$count = count(explode(',', $levelsCount));
				}
				if(!is_numeric($diff)) {
					$diff = strtolower($diff);
					if(isset($carray[3]) AND strtolower($carray[3]) == "demon") {
						$diffList = ['easy' => 1, 'medium' => 2, 'hard' => 3, 'insane' => 4, 'extreme' => 5];
						$diff = 5+$diffList[$diff];
					} else {
						$diffList = ['na' => -1, 'auto' => 0, 'easy' => 1, 'normal' => 2, 'hard' => 3, 'harder' => 4, 'demon' => 5];
						$diff = $diffList[$diff];
					}
				}
				if(!isset($diff)) $diff = $getList['starDifficulty'];
				if($gs->checkPermission($accountID, "commandRate")) {
					$query = $db->prepare("UPDATE lists SET starStars = :reward, starDifficulty = :diff, starFeatured = :feat, countForReward = :count WHERE listID = :listID");
					$query->execute([':listID' => $listID, ':reward' => $reward, ':diff' => $diff, ':feat' => $featured, ':count' => $count]);
					$query = $db->prepare("INSERT INTO modactions (type, value, value2, value3, timestamp, account) VALUES ('30', :value, :value2, :listID, :timestamp, :id)");
					$query->execute([':value' => $reward, ':value2' => $diff, ':timestamp' => time(), ':id' => $accountID, ':listID' => $listID]);
				} elseif($gs->checkPermission($accountID, "actionSuggestRating")) {
					$query = $db->prepare("INSERT INTO suggest (suggestBy, suggestLevelId, suggestDifficulty, suggestStars, suggestFeatured, timestamp) VALUES (:accID, :listID, :diff, :reward, :feat, :time)");
					$query->execute([':listID' => $listID*-1, ':reward' => $reward, ':diff' => $diff, ':accID' => $accountID, ':feat' => $featured, ':time' => time()]);
					$query = $db->prepare("INSERT INTO modactions (type, value, value2, value3, timestamp, account) VALUES ('31', :value, :value2, :listID, :timestamp, :id)");
					$query->execute([':value' => $reward, ':value2' => $diff, ':timestamp' => time(), ':id' => $accountID, ':listID' => $listID]);
				} else return false;
				break;
			case '!f':
			case '!feature':
				if(!$gs->checkPermission($accountID, "commandFeature")) return false;
				if(!isset($carray[1])) $carray[1] = 1;
				$query = $db->prepare("UPDATE lists SET starFeatured = :feat WHERE listID=:listID");
				$query->execute([':listID' => $listID, ':feat' => $carray[1]]);
				$query = $db->prepare("INSERT INTO modactions (type, value, value3, timestamp, account) VALUES ('32', :value, :listID, :timestamp, :id)");
				$query->execute([':value' => $carray[1], ':timestamp' => time(), ':id' => $accountID, ':listID' => $listID]);
				break;
			case '!un':
			case '!unlist':
				$accCheck = $gs->getListOwner($listID);
				if(!$gs->checkPermission($accountID, "commandUnlistAll") AND $accountID != $accCheck) return false;
				if(!isset($carray[1])) $carray[1] = 1;
				$query = $db->prepare("UPDATE lists SET unlisted = :unlisted WHERE listID=:listID");
				$query->execute([':listID' => $listID, ':unlisted' => $carray[1]]);
				$query = $db->prepare("INSERT INTO modactions (type, value, value3, timestamp, account) VALUES ('33', :value, :listID, :timestamp, :id)");
				$query->execute([':value' => $carray[1], ':timestamp' => time(), ':id' => $accountID, ':listID' => $listID]);
				break;
			case '!d':
			case '!delete':
				if(!$gs->checkPermission($accountID, "commandDelete")) return false;
				$query = $db->prepare("DELETE FROM lists WHERE listID = :listID");
				$query->execute([':listID' => $listID]);
				$query = $db->prepare("INSERT INTO modactions (type, value, value3, timestamp, account) VALUES ('34', 0, :listID, :timestamp, :id)");
				$query->execute([':timestamp' => time(), ':id' => $accountID, ':listID' => $listID]);
				break;
			case '!acc':
			case '!setacc':
				if(!$gs->checkPermission($accountID, "commandSetacc")) return false;
				if(is_numeric($carray[1])) $acc = ExploitPatch::number($carray[1]);
				else $acc = $gs->getAccountIDFromName(ExploitPatch::charclean($carray[1]));
				if(empty($acc)) return false;
				$query = $db->prepare("UPDATE lists SET accountID = :accID WHERE listID=:listID");
				$query->execute([':listID' => $listID, ':accID' => $acc]);
				$query = $db->prepare("INSERT INTO modactions (type, value, value3, timestamp, account) VALUES ('35', :value, :listID, :timestamp, :id)");
				$query->execute([':value' => $acc, ':timestamp' => time(), ':id' => $accountID, ':listID' => $listID]);
				break;
			case '!re':
			case '!rename':
				$accCheck = $gs->getListOwner($listID);
				if(!$gs->checkPermission($accountID, "commandRenameAll") AND $accountID != $accCheck) return false;
				$carray[0] = '';
				$oldName = $gs->getListName($listID);
				$name = trim(ExploitPatch::charclean(implode(' ', $carray)));
				$query = $db->prepare("UPDATE lists SET listName = :name WHERE listID = :listID");
				$query->execute([':listID' => $listID, ':name' => $name]);
				$query = $db->prepare("INSERT INTO modactions (type, value, value2, value3, timestamp, account) VALUES ('36', :value, :value2, :listID, :timestamp, :id)");
				$query->execute([':value' => $name, ':value2' => $oldName, ':timestamp' => time(), ':id' => $accountID, ':listID' => $listID]);
				break;
			case '!desc':
			case '!description':
				$accCheck = $gs->getListOwner($listID);
				if(!$gs->checkPermission($accountID, "commandDescriptionAll") AND $accountID != $accCheck) return false;
				$carray[0] = '';
				$name = base64_encode(trim(ExploitPatch::charclean(implode(' ', $carray))));
				$query = $db->prepare("UPDATE lists SET listDesc = :name WHERE listID = :listID");
				$query->execute([':listID' => $listID, ':name' => $name]);
				$query = $db->prepare("INSERT INTO modactions (type, value, value3, timestamp, account) VALUES ('37', :value, :listID, :timestamp, :id)");
				$query->execute([':value' => $name, ':timestamp' => time(), ':id' => $accountID, ':listID' => $listID]);
				break;
		}
		return true;
	}
	public static function doProfileCommands($accountID, $command){
		include dirname(__FILE__)."/../lib/connection.php";
		require_once "../lib/exploitPatch.php";
		require_once "../lib/mainLib.php";
				$gs = new mainLib();
		if(substr($command, 0, 8) == '!discord'){
			if(substr($command, 9, 6) == "accept"){
				$query = $db->prepare("UPDATE accounts SET discordID = discordLinkReq, discordLinkReq = '0' WHERE accountID = :accountID AND discordLinkReq <> 0");
				$query->execute([':accountID' => $accountID]);
				$query = $db->prepare("SELECT discordID, userName FROM accounts WHERE accountID = :accountID");
				$query->execute([':accountID' => $accountID]);
				$account = $query->fetch();
				$gs->sendDiscordPM($account["discordID"], "Your link request to " . $account["userName"] . " has been accepted!");
				return true;
			}
			if(substr($command, 9, 4) == "deny"){
				$query = $db->prepare("SELECT discordLinkReq, userName FROM accounts WHERE accountID = :accountID");
				$query->execute([':accountID' => $accountID]);
				$account = $query->fetch();
				$gs->sendDiscordPM($account["discordLinkReq"], "Your link request to " . $account["userName"] . " has been denied!");
				$query = $db->prepare("UPDATE accounts SET discordLinkReq = '0' WHERE accountID = :accountID");
				$query->execute([':accountID' => $accountID]);
				return true;
			}
			if(substr($command, 9, 6) == "unlink"){
				$query = $db->prepare("SELECT discordID, userName FROM accounts WHERE accountID = :accountID");
				$query->execute([':accountID' => $accountID]);
				$account = $query->fetch();
				$gs->sendDiscordPM($account["discordID"], "Your Discord account has been unlinked from " . $account["userName"] . "!");
				$query = $db->prepare("UPDATE accounts SET discordID = '0' WHERE accountID = :accountID");
				$query->execute([':accountID' => $accountID]);
				return true;
			}
		}
		return false;
	}
}
?>
