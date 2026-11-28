<?php
/**
 * Ensure versus tables exist (idempotent).
 */
function versusEnsureSchema(PDO $db): void {
    // Tables are installed once by _updates/2026-11-20-versus.sql.
    // The application account intentionally has no CREATE/DROP privileges.
}

function versusAuth(): array {
	include dirname(__FILE__) . "/../incl/lib/connection.php";
	require_once dirname(__FILE__) . "/../incl/lib/generatePass.php";
	require_once dirname(__FILE__) . "/../incl/lib/exploitPatch.php";

	$accountID = isset($_POST["accountID"]) ? intval($_POST["accountID"]) : 0;
	if ($accountID <= 0) {
		versusJson(["ok" => false, "error" => "auth"], 401);
	}

	$pass = 0;
	if (!empty($_POST["gjp2"])) {
		$pass = GeneratePass::isGJP2Valid($accountID, ExploitPatch::remove($_POST["gjp2"]));
	} elseif (!empty($_POST["password"])) {
		$pass = GeneratePass::isValid($accountID, $_POST["password"]);
	}
	if ($pass != 1) {
		versusJson(["ok" => false, "error" => "auth"], 401);
	}

	$q = $db->prepare("SELECT userName FROM accounts WHERE accountID = :id LIMIT 1");
	$q->execute([":id" => $accountID]);
	$userName = $q->fetchColumn();
	if (!$userName) {
		versusJson(["ok" => false, "error" => "auth"], 401);
	}

	versusEnsureSchema($db);
	return [$db, $accountID, (string)$userName];
}

function versusJson(array $data, int $code = 200): void {
	http_response_code($code);
	header("Content-Type: application/json; charset=utf-8");
	header("Cache-Control: no-store");
	echo json_encode($data, JSON_UNESCAPED_UNICODE);
	exit;
}

function versusPickLevel(PDO $db): int {
	$pick = static function (bool $rated) use ($db): int {
		$sql = "SELECT levelID, (levelString IS NOT NULL AND levelString <> '') AS hasData
			FROM levels WHERE levelLength <> 5 AND unlisted = 0 AND unlisted2 = 0 AND isDeleted = 0";
		if ($rated) $sql .= " AND starStars > 0";
		$sql .= " ORDER BY RAND()";
		foreach ($db->query($sql) as $row) {
			$id = (int)$row['levelID'];
			if ($id > 0 && ((int)$row['hasData'] === 1 || is_file(__DIR__ . "/../data/levels/$id")))
				return $id;
		}
		return 0;
	};
	return $pick(true) ?: $pick(false);
}

function versusMatchRow(PDO $db, int $matchID): ?array {
	$q = $db->prepare("SELECT * FROM versus_matches WHERE matchID = :id LIMIT 1");
	$q->execute([":id" => $matchID]);
	$row = $q->fetch(PDO::FETCH_ASSOC);
	return $row ?: null;
}

// Store the high-water mark separately from the live attempt, which returns to
// zero after death. GREATEST also protects against delayed progress packets.
function versusProgressUpdate(bool $amP1, array $input): array {
    $values = [];
    foreach (['percent', 'best'] as $field) {
        if (!isset($input[$field])) continue;
        $raw = $input[$field];
        if (!is_string($raw) || !is_numeric($raw) || !is_finite((float)$raw))
            throw new DomainException('Invalid progress.');
        $values[$field] = max(0.0, min(100.0, (float)$raw));
    }
    if (!$values) return [[], []];
    $prefix = $amP1 ? 'p1' : 'p2';
    $sets = ["{$prefix}Best = GREATEST({$prefix}Best, :best)"];
    $params = [':best' => max($values['percent'] ?? 0.0, $values['best'] ?? 0.0)];
    if (isset($values['percent'])) {
        $sets[] = "{$prefix}Percent = :pct"; $params[':pct'] = $values['percent'];
    }
    return [$sets, $params];
}

function versusPublicMatch(array $row, int $me): array {
	$amP1 = intval($row["player1"]) === $me;
	$oppID = $amP1 ? intval($row["player2"]) : intval($row["player1"]);
	$oppName = $amP1 ? $row["p2Name"] : $row["p1Name"];
	$myLives = $amP1 ? intval($row["p1Lives"]) : intval($row["p2Lives"]);
	$oppLives = $amP1 ? intval($row["p2Lives"]) : intval($row["p1Lives"]);
	$myPct = $amP1 ? floatval($row["p1Percent"]) : floatval($row["p2Percent"]);
	$oppPct = $amP1 ? floatval($row["p2Percent"]) : floatval($row["p1Percent"]);
    $myBest = max($myPct, (float)($row[$amP1 ? 'p1Best' : 'p2Best'] ?? 0));
    $oppBest = max($oppPct, (float)($row[$amP1 ? 'p2Best' : 'p1Best'] ?? 0));
	// Cube and colors are the public icon settings already shown on GD profiles.
	$db = $GLOBALS['gdpsDatabase'];
	$iconQuery = $db->prepare("SELECT icon, color1, color2 FROM users WHERE extID = ? LIMIT 1");
	$iconFor = static function(int $accountID) use ($iconQuery): array {
		$iconQuery->execute([(string)$accountID]);
		$user = $iconQuery->fetch(PDO::FETCH_ASSOC) ?: [];
		return ["icon" => max(1, (int)($user["icon"] ?? 1)),
			"color1" => (int)($user["color1"] ?? 0), "color2" => (int)($user["color2"] ?? 3)];
	};
	$myIcon = $iconFor($me); $theirIcon = $iconFor($oppID);
	$winner = intval($row["winner"]);
	$youWin = ($winner > 0 && $winner === $me);
	$youLose = ($winner > 0 && $winner !== $me);

	return [
		"ok" => true,
		"status" => $row["status"],
		"matchID" => intval($row["matchID"]),
		"levelID" => intval($row["levelID"]),
		"you" => [
			"accountID" => $me,
			"lives" => $myLives,
			"percent" => $myPct,
            "best" => $myBest,
			"icon" => $myIcon,
		],
		"opponent" => [
			"accountID" => $oppID,
			"userName" => $oppName,
			"lives" => $oppLives,
			"percent" => $oppPct,
            "best" => $oppBest,
			"icon" => $theirIcon,
		],
		"winner" => $winner,
		"youWin" => $youWin,
		"youLose" => $youLose,
	];
}
