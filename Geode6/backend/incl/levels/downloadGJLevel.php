<?php
chdir(dirname(__FILE__));
include "../lib/connection.php";
require "../lib/XORCipher.php";
require_once "../lib/exploitPatch.php";
require_once "../lib/mainLib.php";
$gs = new mainLib();
require "../lib/generateHash.php";
require "../lib/GJPCheck.php";
if(empty($_POST["gameVersion"])){
	$gameVersion = 1;
}else{
	$gameVersion = ExploitPatch::remove($_POST["gameVersion"]);
}
if(empty($_POST["levelID"])){
	exit("-1");
}
$extras = !empty($_POST["extras"]) && $_POST["extras"];
$inc = !empty($_POST["inc"]) && $_POST["inc"];
$ip = $gs->getIP();
$levelID = ExploitPatch::remove($_POST["levelID"]);
$binaryVersion = !empty($_POST["binaryVersion"]) ? ExploitPatch::remove($_POST["binaryVersion"]) : 0;
$feaID = 0;
if(!is_numeric($levelID)){
	echo -1;
}else{
	$daily = 0;
	if (in_array((int)$levelID, [-1, -2, -3], true)) {
		require_once '../lib/TimedLevelService.php';
		$type = -(int)$levelID - 1;
		$row = TimedLevelService::current($db, $type);
		if (!$row) exit('-1');
		$state = TimedLevelService::state($row, $type);
		$levelID = $state['levelID'];
		$feaID = $state['dailyID'];
		$daily = 1;
	}
	//downloading the level
	if($daily == 1)
		$query=$db->prepare("SELECT levels.*, users.userName, users.extID FROM levels LEFT JOIN users ON levels.userID = users.userID WHERE levelID = :levelID");
	else
		$query=$db->prepare("SELECT * FROM levels WHERE levelID = :levelID");

	$query->execute([':levelID' => $levelID]);
	$lvls = $query->rowCount();
	if($lvls!=0){
		$result = $query->fetch();

		//Verifying friends only unlisted
		if($result["unlisted2"] != 0){
			$accountID = GJPCheck::getAccountIDOrDie();
			if(! ($result["extID"] == $accountID || $gs->isFriends($accountID, $result["extID"])) ) exit("-1");
		}

		//adding the download
		$query6 = $db->prepare("SELECT count(*) FROM actions_downloads WHERE levelID=:levelID AND ip=INET6_ATON(:ip)");
		$query6->execute([':levelID' => $levelID, ':ip' => $ip]);
		if($inc && $query6->fetchColumn() < 2){
			$query2=$db->prepare("UPDATE levels SET downloads = downloads + 1 WHERE levelID = :levelID");
			$query2->execute([':levelID' => $levelID]);
			$query6 = $db->prepare("INSERT INTO actions_downloads (levelID, ip) VALUES 
														(:levelID,INET6_ATON(:ip))");
			$query6->execute([':levelID' => $levelID, ':ip' => $ip]);
		}
		//getting the days since uploaded... or outputting the date in Y-M-D format at least for now...
		$uploadDate = date("d-m-Y G-i", $result["uploadDate"]);
		$updateDate = date("d-m-Y G-i", $result["updateDate"]);
		//password xor
		$pass = (string)$result["password"];
		$desc = $result["levelDesc"];
		// Preserve the creator's Copy setting for every player. IP-based free-copy
		// privileges also affected unrelated users behind the same NAT.
		if (!ctype_digit($pass)) $pass = '0';
		if ((int)$pass >= 1000 && (int)$pass <= 999999) $pass = (string)(1000000 + (int)$pass);
		$xorPass = $pass;
		if($gameVersion > 19){
			if($pass != 0) $xorPass = base64_encode(XORCipher::cipher($pass,26364));
		}else{
			$desc = ExploitPatch::remove(base64_decode($desc));
		}
		//submitting data
		if(file_exists("../../data/levels/$levelID")){
			$levelstring = file_get_contents("../../data/levels/$levelID");
		}else{
			$levelstring = $result["levelString"];
		}
		if($gameVersion > 18){
			if(substr($levelstring,0,3) == 'kS1'){
				$levelstring = base64_encode(gzcompress($levelstring));
				$levelstring = str_replace("/","_",$levelstring);
				$levelstring = str_replace("+","-",$levelstring);
			}
		}
		// Normalize legacy Rate Button tiers; keep :19: and genSolo2 featured as 0/1
		$fire = $gs->normalizeFireTiers($result["starFeatured"], $result["starEpic"]);
		$response = "1:".$result["levelID"].":2:".ExploitPatch::levelNameWire($result["levelName"]).":3:".$desc.":4:".$levelstring.":5:".$result["levelVersion"].":6:".$result["userID"].":8:10:9:".$result["starDifficulty"].":10:".$result["downloads"].":11:1:12:".$result["audioTrack"].":13:".$result["gameVersion"].":14:".$result["likes"].":17:".$result["starDemon"].":43:".$result["starDemonDiff"].":25:".$result["starAuto"].":18:".$result["starStars"].":19:".$fire["featured"].":42:".$fire["epic"].":45:".$result["objects"].":15:".$result["levelLength"].":30:".$result["original"].":31:".$result['twoPlayer'].":28:".$uploadDate. ":29:".$updateDate. ":35:".$result["songID"].":36:".$result["extraString"].":37:".$result["coins"].":38:".$result["starCoins"].":39:".$result["requestedStars"].":46:".$result["wt"].":47:".$result["wt2"].":48:".$result["settingsString"].":40:".$result["isLDM"].":27:$xorPass:52:".$result["songIDs"].":53:".$result["sfxIDs"].":57:".$result['ts'];
		if($daily == 1) $response .= ":41:".$feaID;
		if($extras) $response .= ":26:" . $result["levelInfo"];
		//2.02 stuff
		$response .= "#" . GenerateHash::genSolo($levelstring) . "#";
		//2.1 stuff — featured in hash must match key :19: (0 or 1)
		$somestring = $result["userID"].",".$result["starStars"].",".$result["starDemon"].",".$result["levelID"].",".$result["starCoins"].",".$fire["featured"].",".$pass.",".$feaID;
		$response .= GenerateHash::genSolo2($somestring);
		if($daily == 1){
			$response .= "#" . $gs->getUserString($result);
		}elseif($binaryVersion == 30){
			/*
				This was only part of the response for a brief time prior to GD 2.1's relase.
				This binary version corresponds to the original release of Geometry Dash World.
				It is currently unknown if it's required, so it is left in for now.
			*/
			$response .= "#" . $somestring;
		}
		echo $response;
	}else{
		echo -1;
	}
}
?>
