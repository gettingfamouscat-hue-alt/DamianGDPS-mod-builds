<?php
chdir(dirname(__FILE__));
include "../lib/connection.php";
require_once "../lib/generateHash.php";
$gauntletstring = "";
$string = "";
$query = $db->prepare("SELECT g.ID,g.level1,g.level2,g.level3,g.level4,g.level5,n.name FROM gauntlets g LEFT JOIN gdps_gauntlet_names n ON n.gauntletID=g.ID WHERE g.level5 != '0' ORDER BY g.ID ASC");
$query->execute();
$result = $query->fetchAll();
foreach($result as &$gauntlet){
	$lvls = $gauntlet["level1"].",".$gauntlet["level2"].",".$gauntlet["level3"].",".$gauntlet["level4"].",".$gauntlet["level5"];
	$name = $gauntlet['name'] ?? '';
	$gauntletstring .= "1:".$gauntlet["ID"].($name !== '' ? ':2:'.$name : '').":3:".$lvls."|";
	$string .= $gauntlet["ID"].$lvls;
}
$gauntletstring = substr($gauntletstring, 0, -1);
echo $gauntletstring;
echo "#".GenerateHash::genSolo2($string);
?>
