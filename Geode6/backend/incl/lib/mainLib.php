<?php
include_once __DIR__ . "/ip_in_range.php";
class mainLib {
	public function getAudioTrack($id) {
		$songs = ["Stereo Madness by ForeverBound",
			"Back on Track by DJVI",
			"Polargeist by Step",
			"Dry Out by DJVI",
			"Base after Base by DJVI",
			"Can't Let Go by DJVI",
			"Jumper by Waterflame",
			"Time Machine by Waterflame",
			"Cycles by DJVI",
			"xStep by DJVI",
			"Clutterfunk by Waterflame",
			"Theory of Everything by DJ Nate",
			"Electroman Adventures by Waterflame",
			"Club Step by DJ Nate",
			"Electrodynamix by DJ Nate",
			"Hexagon Force by Waterflame",
			"Blast Processing by Waterflame",
			"Theory of Everything 2 by DJ Nate",
			"Geometrical Dominator by Waterflame",
			"Deadlocked by F-777",
			"Fingerbang by MDK",
			"The Seven Seas by F-777",
			"Viking Arena by F-777",
			"Airborne Robots by F-777",
			"Secret by RobTopGames",
			"Payload by Dex Arson",
			"Beast Mode by Dex Arson",
			"Machina by Dex Arson",
			"Years by Dex Arson",
			"Frontlines by Dex Arson",
			"Space Pirates by Waterflame",
			"Striker by Waterflame",
			"Embers by Dex Arson",
			"Round 1 by Dex Arson",
			"Monster Dance Off by F-777"];
		if($id < 0 || $id >= count($songs))
			return "Unknown by DJVI";
		return $songs[$id];
	}
	public function getDifficulty($diff,$auto,$demon) {
		if($auto != 0){
			return "Auto";
		}else if($demon != 0){
			return "Demon";
		}else{
			switch($diff){
				case 0:
					return "N/A";
					break;
				case 10:
					return "Easy";
					break;
				case 20:
					return "Normal";
					break;
				case 30:
					return "Hard";
					break;
				case 40:
					return "Harder";
					break;
				case 50:
					return "Insane";
					break;
				default:
					return "Unknown";
					break;
			}
		}
	}
	public function getDiffFromStars($stars) {
		$auto = 0;
		$demon = 0;
		switch($stars){
			case 1:
				$diffname = "Auto";
				$diff = 50;
				$auto = 1;
				break;
			case 2:
				$diffname = "Easy";
				$diff = 10;
				break;
			case 3:
				$diffname = "Normal";
				$diff = 20;
				break;
			case 4:
			case 5:
				$diffname = "Hard";
				$diff = 30;
				break;
			case 6:
			case 7:
				$diffname = "Harder";
				$diff = 40;
				break;
			case 8:
			case 9:
				$diffname = "Insane";
				$diff = 50;
				break;
			case 10:
				$diffname = "Demon";
				$diff = 50;
				$demon = 1;
				break;
			default:
				$diffname = "N/A: " . $stars;
				$diff = 0;
				$demon = 0;
				break;
		}
		return array('diff' => $diff, 'auto' => $auto, 'demon' => $demon, 'name' => $diffname);
	}
	public function getLength($length) {
		switch($length){
			case 0:
				return "Tiny";
				break;
			case 1:
				return "Short";
				break;
			case 2:
				return "Medium";
				break;
			case 3:
				return "Long";
				break;
			case 4:
				return "XL";
				break;
			case 5:
				return "Platformer";
				break;
			default:
				return "Unknown";
				break;
		}
	}
	public function getGameVersion($version) {
		if($version > 17){
			return $version / 10;
		}elseif($version == 11){
			return "1.8";
		}elseif($version == 10){
			return "1.7";
		}else{
			$version--;
			return "1.$version";
		}
	}
	public function getDemonDiff($dmn) {
		switch($dmn){
			case 3:
				return "Easy";
				break;
			case 4:
				return "Medium";
				break;
			case 5:
				return "Insane";
				break;
			case 6:
				return "Extreme";
				break;
			case 7:
				return "Impossible";
				break;
			default:
				return "Hard";
				break;
		}
	}
	public function getDiffFromName($name) {
		$name = strtolower(str_replace(["_", " "], "-", trim($name)));
		$starAuto = 0;
		$starDemon = 0;
		$starDemonDiff = 0; // Hard Demon default when demon
		switch ($name) {
			default:
				$starDifficulty = 0;
				break;
			/*
				Very Easy (GDPS custom):
				  starDifficulty=10 (Easy bucket), starDemon=0, starAuto=0, starDemonDiff=1
				Vanilla ignores demonDiff when not demon; Rate Button overlays the custom face.
				demonDiff=1 does not collide with demon subtypes (0/3/4/5/6/7).
			*/
			case "very-easy":
			case "veryeasy":
				$starDifficulty = 10;
				$starDemonDiff = 1;
				break;
			case "easy":
				$starDifficulty = 10;
				break;
			case "normal":
				$starDifficulty = 20;
				break;
			case "hard":
				$starDifficulty = 30;
				break;
			case "harder":
				$starDifficulty = 40;
				break;
			case "insane":
				$starDifficulty = 50;
				break;
			case "auto":
				$starDifficulty = 50;
				$starAuto = 1;
				break;
			case "demon":
			case "hard-demon":
			case "harddemon":
				$starDifficulty = 50;
				$starDemon = 1;
				$starDemonDiff = 0;
				break;
			case "easy-demon":
			case "easydemon":
				$starDifficulty = 50;
				$starDemon = 1;
				$starDemonDiff = 3;
				break;
			case "medium-demon":
			case "mediumdemon":
				$starDifficulty = 50;
				$starDemon = 1;
				$starDemonDiff = 4;
				break;
			case "insane-demon":
			case "insanemon":
			case "insanedemon":
				$starDifficulty = 50;
				$starDemon = 1;
				$starDemonDiff = 5;
				break;
			case "extreme-demon":
			case "extremedemon":
				$starDifficulty = 50;
				$starDemon = 1;
				$starDemonDiff = 6;
				break;
			case "supreme-demon":
			case "ultimate-demon":
			case "legendary-demon":
			case "mythical-demon":
			case "silent-demon":
			case "grandpa-demon":
				$grandpaDiffs = ["supreme-demon" => 8, "ultimate-demon" => 9,
					"legendary-demon" => 10, "mythical-demon" => 11,
					"silent-demon" => 12, "grandpa-demon" => 13];
				$starDifficulty = 50;
				$starDemon = 1;
				$starDemonDiff = $grandpaDiffs[$name];
				break;
			case "impossible-demon":
			case "impossibledemon":
			case "impossible":
				$starDifficulty = 50;
				$starDemon = 1;
				$starDemonDiff = 6;
				break;
		}
		return array($starDifficulty, $starDemon, $starAuto, $starDemonDiff);
	}
	public function getGauntletName($id){
		$gauntlets = ["Unknown", "Fire", "Ice", "Poison", "Shadow", "Lava", "Bonus", "Chaos", "Demon", "Time", "Crystal", "Magic", "Spike", "Monster", "Doom", "Death", 'Forest', 'Rune', 'Force', 'Spooky', 'Dragon', 'Water', 'Haunted', 'Acid', 'Witch', 'Power', 'Potion', 'Snake', 'Toxic', 'Halloween', 'Treasure', 'Ghost', 'Spider', 'Gem', 'Inferno', 'Portal', 'Strange', 'Fantasy', 'Christmas', 'Surprise', 'Mystery', 'Cursed', 'Cyborg', 'Castle', 'Grave', 'Temple', 'World', 'Galaxy', 'Universe', 'Discord', 'Split', 'NCS', 'NCS II', 'Space', 'Cosmos', 'Random', 'Chance', 'Future', 'Utopia', 'Cinema', 'Love', 'Duality', 'Paradox'];
		if($id < 0 || $id >= count($gauntlets))
			return $gauntlets[0];
		return $gauntlets[$id];
	}

	function makeTime($delta)
	{
		if ($delta < 31536000)
		{
			if ($delta < 2628000)
			{
				if ($delta < 604800)
				{
					if ($delta < 86400)
					{
						if ($delta < 3600)
						{
							if ($delta < 60)
							{
								return $delta." second".($delta == 1 ? "" : "s");
							}
							else
							{
                        					$rounded = floor($delta / 60);
								return $rounded." minute".($rounded == 1 ? "" : "s");
							}
						}
						else
						{
							$rounded = floor($delta / 3600);
							return $rounded." hour".($rounded == 1 ? "" : "s");
						}
					}
					else
					{
						$rounded = floor($delta / 86400);
						return $rounded." day".($rounded == 1 ? "" : "s");
					}
				}
				else
				{
					$rounded = floor($delta / 604800);
					return $rounded." week".($rounded == 1 ? "" : "s");
				}
			}
			else
			{
				$rounded = floor($delta / 2628000); 
				return $rounded." month".($rounded == 1 ? "" : "s");
			}
		}
		else
		{
			$rounded = floor($delta / 31536000);
			return $rounded." year".($rounded == 1 ? "" : "s");
		}
	}
	public function getIDFromPost(){
		include __DIR__ . "/../../config/security.php";
		include_once __DIR__ . "/exploitPatch.php";
		include_once __DIR__ . "/GJPCheck.php";

		if(!empty($_POST["udid"]) AND $_POST['gameVersion'] < 20 AND $unregisteredSubmissions) 
		{
			$id = ExploitPatch::remove($_POST["udid"]);
			if(is_numeric($id)) exit("-1");
		}
		elseif(!empty($_POST["accountID"]) AND $_POST["accountID"]!="0")
		{
			$id = GJPCheck::getAccountIDOrDie();
		}
		else
		{
			exit("-1");
		}
		return $id;
	}
	/** Shared Guest account â€” can play levels, cannot comment / leaderboard / upload */
	public function ensureGuestAccount(){
		include __DIR__ . "/../../config/gdps.php";
		include __DIR__ . "/connection.php";
		require_once __DIR__ . "/generatePass.php";

		$query = $db->prepare("SELECT accountID FROM accounts WHERE userName = :u LIMIT 1");
		$query->execute([':u' => $gdpsGuestUser]);
		$accountID = intval($query->fetchColumn());
		if($accountID <= 0){
			$hashpass = GeneratePass::hashPassword($gdpsGuestPass);
			$gjp2 = GeneratePass::GJP2hash($gdpsGuestPass);
			require_once __DIR__ . "/ReservedIDs.php";
			$accountID = ReservedIDs::insert($db, 'accounts',
				"INSERT INTO accounts (userName, password, email, registerDate, isActive, gjp2) VALUES (:userName, :password, :email, :time, 1, :gjp)", [
				':userName' => $gdpsGuestUser,
				':password' => $hashpass,
				':email' => "guest@localhost",
				':time' => time(),
				':gjp' => $gjp2
			]);
		}
		// Ensure users row exists and is leaderboard-banned
		$userID = $this->getUserID($accountID, $gdpsGuestUser);
		$db->prepare("UPDATE users SET isBanned = 1, userName = :u WHERE userID = :id")
			->execute([':u' => $gdpsGuestUser, ':id' => $userID]);
		return $accountID;
	}
	public function isGuestAccount($accountID){
		if(!is_numeric($accountID) || intval($accountID) <= 0) return true; // no account = guest
		include __DIR__ . "/../../config/gdps.php";
		include __DIR__ . "/connection.php";
		$guestID = intval($gdpsGuestAccountID);
		if($guestID <= 0){
			$query = $db->prepare("SELECT accountID FROM accounts WHERE userName = :u LIMIT 1");
			$query->execute([':u' => $gdpsGuestUser]);
			$guestID = intval($query->fetchColumn());
		}
		if($guestID > 0 && intval($accountID) === $guestID) return true;
		$query = $db->prepare("SELECT userName FROM accounts WHERE accountID = :id LIMIT 1");
		$query->execute([':id' => $accountID]);
		$name = $query->fetchColumn();
		return strcasecmp((string)$name, (string)$gdpsGuestUser) === 0;
	}
	public function getUserID($extID, $userName = "Undefined") {
		include __DIR__ . "/connection.php";
		if(is_numeric($extID)){
			$register = 1;
		}else{
			$register = 0;
		}
		$query = $db->prepare("SELECT userID FROM users WHERE extID LIKE BINARY :id");
		$query->execute([':id' => $extID]);
		if ($query->rowCount() > 0) {
			$userID = $query->fetchColumn();
		} else {
			require_once __DIR__ . "/ReservedIDs.php";
			$userID = ReservedIDs::insert($db, 'users',
				"INSERT INTO users (isRegistered, extID, userName, lastPlayed) VALUES (:register, :id, :userName, :uploadDate)",
				[':id' => $extID, ':register' => $register, ':userName' => $userName, ':uploadDate' => time()]);
		}
		return $userID;
	}
	public function getAccountName($accountID) {
		if(!is_numeric($accountID)) return false;

		include __DIR__ . "/connection.php";
		$query = $db->prepare("SELECT userName FROM accounts WHERE accountID = :id");
		$query->execute([':id' => $accountID]);
		if ($query->rowCount() > 0) {
			$userName = $query->fetchColumn();
		} else {
			$userName = false;
		}
		return $userName;
	}
	public function getUserName($userID) {
		include __DIR__ . "/connection.php";
		$query = $db->prepare("SELECT userName FROM users WHERE userID = :id");
		$query->execute([':id' => $userID]);
		if ($query->rowCount() > 0) {
			$userName = $query->fetchColumn();
		} else {
			$userName = false;
		}
		return $userName;
	}
	public function getAccountIDFromName($userName) {
		include __DIR__ . "/connection.php";
		$query = $db->prepare("SELECT accountID FROM accounts WHERE userName LIKE :usr");
		$query->execute([':usr' => $userName]);
		if ($query->rowCount() > 0) {
			$accountID = $query->fetchColumn();
		} else {
			$accountID = 0;
		}
		return $accountID;
	}
	public function getExtID($userID) {
		include __DIR__ . "/connection.php";
		$query = $db->prepare("SELECT extID FROM users WHERE userID = :id");
		$query->execute([':id' => $userID]);
		if ($query->rowCount() > 0) {
			return $query->fetchColumn();
		}else{
			return 0;
		}
	}
	public function getUserString($userdata) {
		include __DIR__ . "/connection.php";
		/*$query = $db->prepare("SELECT userName, extID FROM users WHERE userID = :id");
		$query->execute([':id' => $userID]);
		$userdata = $query->fetch();*/
		$extID = is_numeric($userdata['extID']) ? $userdata['extID'] : 0;
		return "${userdata['userID']}:${userdata['userName']}:${extID}";
	}
	public function getSongString($song){
		include __DIR__ . "/connection.php";
		/*$query3=$db->prepare("SELECT ID,name,authorID,authorName,size,isDisabled,download FROM songs WHERE ID = :songid LIMIT 1");
		$query3->execute([':songid' => $songID]);*/
		if($song['ID'] == 0 || empty($song['ID'])){
			return false;
		}
		//$song = $query3->fetch();
		if($song["isDisabled"] == 1){
			return false;
		}
		$dl = $song["download"];
		if(strpos($dl, ':') !== false){
			$dl = urlencode($dl);
		}
		return "1~|~".$song["ID"]."~|~2~|~".str_replace("#", "", $song["name"])."~|~3~|~".$song["authorID"]."~|~4~|~".$song["authorName"]."~|~5~|~".$song["size"]."~|~6~|~~|~10~|~".$dl."~|~7~|~~|~8~|~1";
	}
	public function sendDiscordPM($receiver, $message){
		include __DIR__ . "/../../config/discord.php";
		if(!$discordEnabled){
			return false;
		}
		//findind the channel id
		$data = array("recipient_id" => $receiver);                                                                    
		$data_string = json_encode($data);
		$url = "https://discord.com/api/v8/users/@me/channels";
		//echo $url;
		$crl = curl_init($url);
		$headr = array();
		$headr['User-Agent'] = 'GMDprivateServer (https://github.com/Cvolton/GMDprivateServer, 1.0)';
		curl_setopt($crl, CURLOPT_CUSTOMREQUEST, "POST");                                                                     
		curl_setopt($crl, CURLOPT_POSTFIELDS, $data_string);
		$headr[] = 'Content-type: application/json';
		$headr[] = 'Authorization: Bot '.$bottoken;
		curl_setopt($crl, CURLOPT_HTTPHEADER,$headr);
		curl_setopt($crl, CURLOPT_RETURNTRANSFER, 1);
		curl_setopt($crl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
		$response = curl_exec($crl);
		curl_close($crl);
		$responseDecode = json_decode($response, true);
		$channelID = $responseDecode["id"];
		//sending the msg
		$data = array("content" => $message);                                                                    
		$data_string = json_encode($data);
		$url = "https://discord.com/api/v8/channels/".$channelID."/messages";
		//echo $url;
		$crl = curl_init($url);
		$headr = array();
		$headr['User-Agent'] = 'GMDprivateServer (https://github.com/Cvolton/GMDprivateServer, 1.0)';
		curl_setopt($crl, CURLOPT_CUSTOMREQUEST, "POST");                                                                     
		curl_setopt($crl, CURLOPT_POSTFIELDS, $data_string);
		$headr[] = 'Content-type: application/json';
		$headr[] = 'Authorization: Bot '.$bottoken;
		curl_setopt($crl, CURLOPT_HTTPHEADER,$headr);
		curl_setopt($crl, CURLOPT_RETURNTRANSFER, 1);
		curl_setopt($crl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
		$response = curl_exec($crl);
		curl_close($crl);
		return $response;
	}
	public function getDiscordAcc($discordID){
		include __DIR__ . "/../../config/discord.php";
		///getting discord acc info
		$url = "https://discord.com/api/v8/users/".$discordID;
		$crl = curl_init($url);
		$headr = array();
		$headr['User-Agent'] = 'GMDprivateServer (https://github.com/Cvolton/GMDprivateServer, 1.0)';
		$headr[] = 'Content-type: application/json';
		$headr[] = 'Authorization: Bot '.$bottoken;
		curl_setopt($crl, CURLOPT_HTTPHEADER,$headr);
		curl_setopt($crl, CURLOPT_RETURNTRANSFER, 1);
		curl_setopt($crl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
		$response = curl_exec($crl);
		curl_close($crl);
		$userinfo = json_decode($response, true);
		//var_dump($userinfo);
		return $userinfo["username"] . "#" . $userinfo["discriminator"];
	}
	public function randomString($length = 6) {
		$randomString = openssl_random_pseudo_bytes($length);
		if($randomString == false){
			$characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
			$charactersLength = strlen($characters);
			$randomString = '';
			for ($i = 0; $i < $length; $i++) {
				$randomString .= $characters[rand(0, $charactersLength - 1)];
			}
			return $randomString;
		}
		$randomString = bin2hex($randomString);
		return $randomString;
	}
	public function getAccountsWithPermission($permission){
		include __DIR__ . "/connection.php";
		$query = $db->prepare("SELECT roleID FROM roles WHERE $permission = 1 ORDER BY priority DESC");
		$query->execute();
		$result = $query->fetchAll();
		$accountlist = array();
		foreach($result as &$role){
			$query = $db->prepare("SELECT accountID FROM roleassign WHERE roleID = :roleID");
			$query->execute([':roleID' => $role["roleID"]]);
			$accounts = $query->fetchAll();
			foreach($accounts as &$user){
				$accountlist[] = $user["accountID"];
			}
		}
		return $accountlist;
	}
	/** Elder Mod = badge >= 2, or can rate stars, or account isAdmin */
	public function isElderMod($accountID) {
		if (!is_numeric($accountID)) return false;
		include __DIR__ . "/connection.php";
		$query = $db->prepare("SELECT isAdmin FROM accounts WHERE accountID = :accountID");
		$query->execute([':accountID' => $accountID]);
		if (intval($query->fetchColumn()) >= 1) return true;
		if ($this->getMaxValuePermission($accountID, "modBadgeLevel") >= 2) return true;
		if ($this->checkPermission($accountID, "actionRateStars")) return true;
		return false;
	}
	/** Regular mod who can only send/suggest (not rate) */
	public function canSuggestOnly($accountID) {
		if ($this->isElderMod($accountID)) return false;
		return $this->checkPermission($accountID, "actionSuggestRating");
	}
	public function checkPermission($accountID, $permission){
		if(!is_numeric($accountID)) return false;

		include __DIR__ . "/connection.php";
		//isAdmin check
		$query = $db->prepare("SELECT isAdmin FROM accounts WHERE accountID = :accountID");
		$query->execute([':accountID' => $accountID]);
		$isAdmin = $query->fetchColumn();
		if(intval($isAdmin) >= 1){
			return 1;
		}
		
		$query = $db->prepare("SELECT roleID FROM roleassign WHERE accountID = :accountID");
		$query->execute([':accountID' => $accountID]);
		$roleIDarray = $query->fetchAll();
		$roleIDlist = "";
		foreach($roleIDarray as &$roleIDobject){
			$roleIDlist .= $roleIDobject["roleID"] . ",";
		}
		$roleIDlist = substr($roleIDlist, 0, -1);
		if($roleIDlist != ""){
			$query = $db->prepare("SELECT $permission FROM roles WHERE roleID IN ($roleIDlist) ORDER BY priority DESC");
			$query->execute();
			$roles = $query->fetchAll();
			foreach($roles as &$role){
				if($role[$permission] == 1){
					return true;
				}
				if($role[$permission] == 2){
					return false;
				}
			}
		}
		$query = $db->prepare("SELECT $permission FROM roles WHERE isDefault = 1");
		$query->execute();
		$permState = $query->fetchColumn();
		if($permState == 1){
			return true;
		}
		if($permState == 2){
			return false;
		}
		return false;
	}
	public function isCloudFlareIP($ip) {
    	$cf_ips = array(
	        '173.245.48.0/20',
			'103.21.244.0/22',
			'103.22.200.0/22',
			'103.31.4.0/22',
			'141.101.64.0/18',
			'108.162.192.0/18',
			'190.93.240.0/20',
			'188.114.96.0/20',
			'197.234.240.0/22',
			'198.41.128.0/17',
			'162.158.0.0/15',
			'104.16.0.0/13',
			'104.24.0.0/14',
			'172.64.0.0/13',
			'131.0.72.0/22'
	    );
	    foreach ($cf_ips as $cf_ip) {
	        if (ipInRange::ipv4_in_range($ip, $cf_ip)) {
	            return true;
	        }
	    }
	    return false;
	}
	public function getIP(){
        $remote = $_SERVER['REMOTE_ADDR'] ?? '127.0.0.1';
        if (!filter_var($remote, FILTER_VALIDATE_IP)) return '127.0.0.1';
        $forwarded = $_SERVER['HTTP_CF_CONNECTING_IP'] ?? '';
        if (filter_var($remote, FILTER_VALIDATE_IP, FILTER_FLAG_IPV4) && $this->isCloudFlareIP($remote)
            && filter_var($forwarded, FILTER_VALIDATE_IP)) return $forwarded;
        include __DIR__ . '/../../config/security.php';
        if (in_array($remote, $trustedProxyAddresses ?? [], true)) {
            $chain = explode(',', $_SERVER['HTTP_X_FORWARDED_FOR'] ?? '');
            // Walk from the trusted proxy backwards, ignoring untrusted prefixes.
            for ($i = count($chain) - 1; $i >= 0; --$i) {
                $candidate = trim($chain[$i]);
                if (!filter_var($candidate, FILTER_VALIDATE_IP)) return $remote;
                if (!in_array($candidate, $trustedProxyAddresses, true)) return $candidate;
            }
        }
        return $remote;
    }

	public function checkModIPPermission($permission){
		include __DIR__ . "/connection.php";
		$ip = $this->getIP();
		$query=$db->prepare("SELECT modipCategory FROM modips WHERE IP = :ip");
		$query->execute([':ip' => $ip]);
		$categoryID = $query->fetchColumn();
		
		$query=$db->prepare("SELECT $permission FROM modipperms WHERE categoryID = :id");
		$query->execute([':id' => $categoryID]);
		$permState = $query->fetchColumn();
		
		if($permState == 1){
			return true;
		}
		if($permState == 2){
			return false;
		}
		return false;
	}
	public function getFriends($accountID){
		if(!is_numeric($accountID)) return false;

		include __DIR__ . "/connection.php";
		$friendsarray = array();
		$query = "SELECT person1,person2 FROM friendships WHERE person1 = :accountID OR person2 = :accountID"; //selecting friendships
		$query = $db->prepare($query);
		$query->execute([':accountID' => $accountID]);
		$result = $query->fetchAll();//getting friends
		if($query->rowCount() == 0){
			return array();
		}
		else
		{//oh so you actually have some friends kden
			foreach ($result as &$friendship) {
				$person = $friendship["person1"];
				if($friendship["person1"] == $accountID){
					$person = $friendship["person2"];
				}
				$friendsarray[] = $person;
			}
		}
		return $friendsarray;
	}
	public function isFriends($accountID, $targetAccountID) {
		if(!is_numeric($accountID) || !is_numeric($targetAccountID)) return false;

		include __DIR__ . "/connection.php";
		$query = $db->prepare("SELECT count(*) FROM friendships WHERE person1 = :accountID AND person2 = :targetAccountID OR person1 = :targetAccountID AND person2 = :accountID");
		$query->execute([':accountID' => $accountID, ':targetAccountID' => $targetAccountID]);
		return $query->fetchColumn() > 0;
	}
	public function getMaxValuePermission($accountID, $permission){
		if(!is_numeric($accountID)) return false;

		include __DIR__ . "/connection.php";
		$maxvalue = 0;
		$query = $db->prepare("SELECT roleID FROM roleassign WHERE accountID = :accountID");
		$query->execute([':accountID' => $accountID]);
		$roleIDarray = $query->fetchAll();
		$roleIDlist = "";
		foreach($roleIDarray as &$roleIDobject){
			$roleIDlist .= $roleIDobject["roleID"] . ",";
		}
		$roleIDlist = substr($roleIDlist, 0, -1);
		if($roleIDlist != ""){
			$query = $db->prepare("SELECT $permission FROM roles WHERE roleID IN ($roleIDlist) ORDER BY priority DESC");
			$query->execute();
			$roles = $query->fetchAll();
			foreach($roles as &$role){ 
				if($role[$permission] > $maxvalue){
					$maxvalue = $role[$permission];
				}
			}
		}
		return $maxvalue;
	}
	public function getAccountCommentColor($accountID){
		if(!is_numeric($accountID)) return false;

		include __DIR__ . "/connection.php";
		$query = $db->prepare("SELECT roleID FROM roleassign WHERE accountID = :accountID");
		$query->execute([':accountID' => $accountID]);
		$roleIDarray = $query->fetchAll();
		$roleIDlist = "";
		foreach($roleIDarray as &$roleIDobject){
			$roleIDlist .= $roleIDobject["roleID"] . ",";
		}
		$roleIDlist = substr($roleIDlist, 0, -1);
		if($roleIDlist != ""){
			$query = $db->prepare("SELECT commentColor FROM roles WHERE roleID IN ($roleIDlist) ORDER BY priority DESC");
			$query->execute();
			$roles = $query->fetchAll();
			foreach($roles as &$role){
				if($role["commentColor"] != "000,000,000"){
					return $role["commentColor"];
				}
			}
		}
		$query = $db->prepare("SELECT commentColor FROM roles WHERE isDefault = 1");
		$query->execute();
		if($query->rowCount() > 0)
			return $query->fetchColumn();
		return "255,255,255";
	}
	public function rateLevel($accountID, $levelID, $stars, $difficulty, $auto, $demon){
		if(!is_numeric($accountID)) return false;

		include __DIR__ . "/connection.php";
		//lets assume the perms check is done properly before
		$query = "UPDATE levels SET starDemon=:demon, starAuto=:auto, starDifficulty=:diff, starStars=:stars, rateDate=:now WHERE levelID=:levelID";
		$query = $db->prepare($query);	
		$query->execute([':demon' => $demon, ':auto' => $auto, ':diff' => $difficulty, ':stars' => $stars, ':levelID'=>$levelID, ':now' => time()]);
		
		$query = $db->prepare("INSERT INTO modactions (type, value, value2, value3, timestamp, account) VALUES ('1', :value, :value2, :levelID, :timestamp, :id)");
		$query->execute([':value' => $this->getDiffFromStars($stars)["name"], ':timestamp' => time(), ':id' => $accountID, ':value2' => $stars, ':levelID' => $levelID]);
		
		
	}
	/**
	 * Normalize fire tiers for client output / genSolo2.
	 * Correct DB: starFeatured 0|1, starEpic 0=none 1=epic 2=legendary 3=mythic.
	 * Legacy Rate Button: tiers 2-4 were written into starFeatured alone.
	 */
	public function normalizeFireTiers($starFeatured, $starEpic) {
		$starFeatured = intval($starFeatured);
		$starEpic = intval($starEpic);
		if ($starFeatured >= 2 && $starFeatured <= 4) {
			$starEpic = $starFeatured - 1;
			$starFeatured = 1;
		} else {
			$starFeatured = ($starFeatured != 0) ? 1 : 0;
		}
		return ['featured' => $starFeatured, 'epic' => $starEpic];
	}
	public function featureLevel($accountID, $levelID, $state) {
		if(!is_numeric($accountID)) return false;
		// Client/Rate Button state: 0=None, 1=Featured, 2=Epic, 3=Legendary, 4=Mythic
		// DB storage: starFeatured is 0/1; starEpic is 0/1/2/3 (key :42:)
		$state = intval($state);
		switch($state) {
			case 0:
				$feature = 0;
				$epic = 0;
				break;
			case 1:
				$feature = 1;
				$epic = 0;
				break;
			case 2: // Epic
				$feature = 1;
				$epic = 1;
				break;
			case 3: // Legendary
				$feature = 1;
				$epic = 2;
				break;
			case 4: // Mythic
				$feature = 1;
				$epic = 3;
				break;
			default:
				$feature = 0;
				$epic = 0;
				$state = 0;
				break;
		}
		include __DIR__ . "/connection.php";
		$query = "UPDATE levels SET starFeatured=:feature, starEpic=:epic, rateDate=:now WHERE levelID=:levelID";
		$query = $db->prepare($query);	
		$query->execute([':feature' => $feature, ':epic' => $epic, ':levelID' => $levelID, ':now' => time()]);
		$query = $db->prepare("INSERT INTO modactions (type, value, value3, timestamp, account) VALUES ('2', :value, :levelID, :timestamp, :id)");
		$query->execute([':value' => $state, ':timestamp' => time(), ':id' => $accountID, ':levelID' => $levelID]);
	}
	public function verifyCoinsLevel($accountID, $levelID, $coins){
		if(!is_numeric($accountID)) return false;

		include __DIR__ . "/connection.php";
		$query = "UPDATE levels SET starCoins=:coins WHERE levelID=:levelID";
		$query = $db->prepare($query);	
		$query->execute([':coins' => $coins, ':levelID'=>$levelID]);
		
		$query = $db->prepare("INSERT INTO modactions (type, value, value3, timestamp, account) VALUES ('3', :value, :levelID, :timestamp, :id)");
		$query->execute([':value' => $coins, ':timestamp' => time(), ':id' => $accountID, ':levelID' => $levelID]);
	}
	public function songReupload($url){
		require __DIR__ . "/../../incl/lib/connection.php";
		require_once __DIR__ . "/../../incl/lib/exploitPatch.php";
		$song = str_replace("www.dropbox.com","dl.dropboxusercontent.com",$url);
		if (filter_var($song, FILTER_VALIDATE_URL) == TRUE && substr($song, 0, 4) == "http") {
			$song = str_replace(["?dl=0","?dl=1"],"",$song);
			$song = trim($song);
			$query = $db->prepare("SELECT count(*) FROM songs WHERE download = :download");
			$query->execute([':download' => $song]);	
			$count = $query->fetchColumn();
			if($count != 0){
				return "-3";
			}
			$name = ExploitPatch::remove(urldecode(str_replace([".mp3",".webm",".mp4",".wav"], "", basename($song))));
			$author = "Reupload";
			$info = $this->getFileInfo($song);
			$size = $info['size'];
			if(substr($info['type'], 0, 6) != "audio/")
				return "-4";
			$size = round($size / 1024 / 1024, 2);
			$hash = "";
			$query = $db->prepare("INSERT INTO songs (name, authorID, authorName, size, download, hash)
			VALUES (:name, '9', :author, :size, :download, :hash)");
			$query->execute([':name' => $name, ':download' => $song, ':author' => $author, ':size' => $size, ':hash' => $hash]);
			return $db->lastInsertId();
		}else{
			return "-2";
		}
	}
	public function getFileInfo($url){
        require_once __DIR__ . '/SafeRemote.php';
        return SafeRemote::fileInfo($url);
    }

	public function suggestLevel($accountID, $levelID, $difficulty, $stars, $feat, $auto, $demon, $demonDiff = 0, $coins = 0){
		if(!is_numeric($accountID) || !is_numeric($levelID)) return false;
		
		include __DIR__ . "/connection.php";
		// Re-send allowed: replace this mod's prior row (bump timestamp â†’ Sent tab floats up).
		// Do not block when the level was already suggested.
		$del = $db->prepare("DELETE FROM suggest WHERE suggestBy = :account AND suggestLevelId = :level");
		$del->execute([':account' => $accountID, ':level' => $levelID]);
		$query = $db->prepare("INSERT INTO suggest (suggestBy, suggestLevelId, suggestDifficulty, suggestStars, suggestFeatured, suggestAuto, suggestDemon, suggestDemonDiff, suggestCoins, timestamp) VALUES (:account, :level, :diff, :stars, :feat, :auto, :demon, :demonDiff, :coins, :timestamp)");
		$query->execute([':account' => $accountID, ':level' => $levelID, ':diff' => $difficulty, ':stars' => $stars, ':feat' => $feat, ':auto' => $auto, ':demon' => $demon, ':demonDiff' => $demonDiff, ':coins' => $coins ? 1 : 0, ':timestamp' => time()]);
		return true;
	}
	public function getListOwner($listID) {
		if(!is_numeric($listID)) return false;
		include __DIR__ . "/connection.php";
		$query = $db->prepare('SELECT accountID FROM lists WHERE listID = :id');
		$query->execute([':id' => $listID]);
		return $query->fetchColumn();
	}
	public function getListLevels($listID) {
		if(!is_numeric($listID)) return false;
		include __DIR__ . "/connection.php";
		$query = $db->prepare('SELECT listlevels FROM lists WHERE listID = :id');
		$query->execute([':id' => $listID]);
		return $query->fetchColumn();
	}
	public function getListDiffName($diff) {
		if($diff == -1) return 'N/A';
		$diffs = ['Auto', 'Easy', 'Normal', 'Hard', 'Harder', 'Extreme', 'Easy Demon', 'Medium Demon', 'Hard Demon', 'Insane Demon', 'Extreme Demon'];
		return $diffs[$diff];
	}
	public function getListName($listID) {
		if(!is_numeric($listID)) return false;
		include __DIR__ . "/connection.php";
		$query = $db->prepare('SELECT listName FROM lists WHERE listID = :id');
		$query->execute([':id' => $listID]);
		return $query->fetchColumn();
	}
}
