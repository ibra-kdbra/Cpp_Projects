<?php
/* tool to analyze and recursively dump STEP files. Also reading STEP files is a real pain in the ass. */

function step_dump($id, $l = 0, $p = "", $pi = -1)
{
    global $entities;
    global $children;
    global $parents;
    global $needed;
    global $cmd;
    global $maxlevel;
    if($l >= 254) {echo("php max level reached\n"); return; }
    if($l >= $maxlevel) return;
    if($cmd == 1) {
        $n = trim(explode("(", $e[0]=="("?substr($e,1):$e)[0]);
        if($p && $p != $n) $parents[$p."\t".$n] = 1;
        $p = $n;
    } else if($cmd == 2) {
        if($pi != -1) $needed[$id] = $needed[$pi];
    } else echo(str_repeat("| ",$l)."#".$id." ".$entities[$id]."\n");
    foreach($children[$id] as $d)
        step_dump($d, $l+1, $p, $id);
}
function step_needed($id)
{
    global $entities;
    global $children;
    global $needed;
    $needed[$id] = 1;
    foreach($children as $i=>$e)
        if(in_array($id, $e)) {
            step_needed($i);
            foreach($e as $d)
                if(substr($entities[$d],0,5)=="PLANE" || substr($entities[$d],0,19)=="CYLINDRICAL_SURFACE" ||
                    substr($entities[$d],0,17)=="SPHERICAL_SURFACE" || substr($entities[$d],0,15)=="CONICAL_SURFACE" ||
                    substr($entities[$d],0,4)=="AXIS" || substr($entities[$d],0,10)=="(GEOMETRIC") {
                        $needed[$d] = 1;
                        step_dump($d, 0, "");
                }
        }
}
if(!isset($_SERVER['argv'][2])) die(
    "php stepanal.php <step file> <entity id | entity type> [maxlevel]\n".
    "php stepanal.php -p <step file1> [step file2] [step file3...]\n".
    "php stepanal.php -e <step file> <entity id>\n");

if($_SERVER['argv'][1] == "-p") {
    $files = array_slice($_SERVER['argv'], 2);
    $root = "CLOSED_SHELL";
    $cmd = 1;
    $maxlevel = 128;
} else if($_SERVER['argv'][1] == "-e") {
    $files[] = $_SERVER['argv'][2];
    $root = $_SERVER['argv'][3];
    $cmd = 2;
    $maxlevel = 128;
} else {
    $files[] = $_SERVER['argv'][1];
    $root = $_SERVER['argv'][2];
    $cmd = 0;
    $maxlevel = @intval($_SERVER['argv'][3]) ? intval($_SERVER['argv'][3]) : 128;
}

$parents = [];
foreach($files as $fn) {
    $entities = []; $children = []; $needed = [];
    $f=fopen($fn, "r");
    if($f) {
        $line = "";
        while(!feof($f) && trim($line) != "DATA;") {
            $line = fgets($f);
            if($cmd == 2) echo($line);
        }
        $data = explode(";",preg_replace("|[\ \t]+#|", "#", preg_replace("|\([\ \t]+|", "(", preg_replace("|/\*.*?\*/|U","",
            str_replace("\r","", str_replace("\n","", fread($f, filesize($fn))))))));
        fclose($f);
    }
    foreach($data as $d)
        if(@trim($d)[0] == "#") {
            $l = explode("=", $d);
            $e = @trim(substr($d, strlen($l[0])+1));
            $entities[intval(substr(trim($l[0]),1))] = $e;
            $needed[intval(substr(trim($l[0]),1))] = 1;
            $a = explode(",", str_replace("(", ",", str_replace(")", ",", $e)));
            $children[intval(substr(trim($l[0]),1))] = [];
            foreach($a as $d)
                if(@trim($d)[0] == "#")
                    $children[intval(substr(trim($l[0]),1))][] = intval(substr(trim($d),1));
        }
    if($cmd == 2) {
        foreach(["ADVANCED_BREP_SHAPE_REPRESENTATION", "MANIFOLD_SURFACE_SHAPE_REPRESENTATION",
            "FACETED_BREP_SHAPE_REPRESENTATION"] as $r)
            foreach($entities as $i=>$e)
                if(substr($e, 0, strlen($r)) == $r) {
                    $needed[$i] = 0;
                    step_dump($i, 0, "");
                }
    }
    if(intval($root)."" == $root) {
        $needed[intval($root)] = 1;
        step_dump(intval($root), 0, "");
        if($cmd == 2) step_needed(intval($root));
    } else
        foreach($entities as $i=>$e)
            if(substr($e, 0, strlen($root)) == strtoupper($root)) {
                $needed[$i] = 1;
                step_dump($i, 0, "");
                if($cmd == 2) step_needed($i);
            }
}
if($cmd == 1) {
    ksort($parents);
    foreach($parents as $k=>$v) {
        $l = explode("\t",$k);
        echo(sprintf("%32s %s",$l[0],$l[1])."\n");
    }
}
if($cmd == 2) {
    foreach($entities as $i=>$e)
        if($needed[$i]) {
            foreach($children[$i] as $d)
                if($d != $i && !$needed[$d]) $e = str_replace("(,", "(", str_replace(",)", ")", preg_replace("|[,]+|", ",",
                    preg_replace("|#".$d."([^0-9])|", "$1", $e))));
            echo("#".$i."=".$e.";\n");
        }
    echo("ENDSEC;\nEND_ISO-10303-21;\n");
}
