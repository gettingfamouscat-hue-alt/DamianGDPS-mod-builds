<?php
/**
 * GET/update versus match state.
 * POST accountID, gjp2, matchID
 * Optional: percent, death=1
 */
chdir(dirname(__FILE__));
require_once "_schema.php";

[$db, $accountID] = versusAuth();
$matchID = isset($_POST["matchID"]) ? intval($_POST["matchID"]) : 0;
if ($matchID <= 0) {
	versusJson(["ok" => false, "error" => "match"], 400);
}

$row = versusMatchRow($db, $matchID);
if (!$row) {
	versusJson(["ok" => false, "error" => "missing"], 404);
}
if (intval($row["player1"]) !== $accountID && intval($row["player2"]) !== $accountID) {
	versusJson(["ok" => false, "error" => "auth"], 403);
}

$amP1 = intval($row["player1"]) === $accountID;
$now = time();

if ($row["status"] === "active") {
	$sets = ["updatedAt = :t"];
	$params = [":t" => $now, ":id" => $matchID];

    try { [$progressSets, $progressParams] = versusProgressUpdate($amP1, $_POST); }
    catch (DomainException $e) { versusJson(['ok' => false, 'error' => 'progress'], 400); }
    $sets = array_merge($sets, $progressSets); $params += $progressParams;

	$death = !empty($_POST["death"]) && intval($_POST["death"]) === 1;
	$finishWinner = 0;

	if ($death) {
		$livesCol = $amP1 ? "p1Lives" : "p2Lives";
		$curLives = $amP1 ? intval($row["p1Lives"]) : intval($row["p2Lives"]);
		$newLives = max(0, $curLives - 1);
		$sets[] = "$livesCol = :lives";
		$params[":lives"] = $newLives;
		if ($newLives <= 0) {
			// Opponent wins when you hit 0 lives
			$finishWinner = $amP1 ? intval($row["player2"]) : intval($row["player1"]);
		}
	} elseif (isset($_POST["percent"]) && floatval($_POST["percent"]) >= 100.0) {
		// First to finish the level wins
		$finishWinner = $accountID;
	}

	if ($finishWinner > 0) {
		$sets[] = "status = 'finished'";
		$sets[] = "winner = :winner";
		$params[":winner"] = $finishWinner;
	}

	$sql = "UPDATE versus_matches SET " . implode(", ", $sets) . " WHERE matchID = :id AND status = 'active'";
	$db->prepare($sql)->execute($params);
	$row = versusMatchRow($db, $matchID);
}

versusJson(versusPublicMatch($row, $accountID));
