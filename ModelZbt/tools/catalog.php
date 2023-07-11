<?php
/*
 * tools/catalog.php
 *
 * Copyright (C) 2019 bzt (bztsrc@gitlab)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * @brief Tool to generate Model 3D homepage and model catalog
 * https://gitlab.com/bztsrc/model3d
 *
 */

/* helper function to convert models and get its properties */
function convertmodel($line, $in, $dir) {
    $path = pathinfo($line[0]);
    $m3d = $dir."/models/".$path['filename'].".m3d";
    $png = escapeshellarg($dir."/models/".$path['filename'].".png");
    $cmd = "";
    if(!file_exists($in."/".$line[0])) { echo("File not found '".$in."/".$line[0]."'\n"); return; }
    if(!file_exists($m3d) || filemtime($in."/".$line[0]) > @filemtime($m3d)) {
        if($path['extension'] == "m3d") {
            copy($in."/".$line[0], $m3d);
        } else
            $cmd = "../m3dconv/m3dconv -i".$line[1]." ".
            "-n ".escapeshellarg($line[2])." -l ".escapeshellarg($line[3])." -a ".escapeshellarg($line[4])." ".
            escapeshellarg($in."/".$line[0])." ".escapeshellarg($m3d)." && ";
    }
    $p = popen($cmd."../m3dview/m3dview ".escapeshellarg($m3d)." ".$png, "r");
    $n = trim(fgets($p));
    $l = trim(fgets($p));
    $d = explode(",",trim(fgets($p)));
    pclose($p);
    $t = [$line[5]];
    for($i=6;$i<count($line);$i++) $t = array_merge($t, explode(",", $line[$i]));
    for($i=0;$i<count($t);$i++) $t[$i] = trim(mb_strtolower($t[$i]));
    if($d[0] < 1024 && !in_array("lowpoly", $t)) $t[] = "lowpoly";
    if($d[0] > 65535 && !in_array("highres", $t)) $t[] = "highres";
    if($d[12]) $d[0] = $d[2] = $d[6] = 0;
    return ["n"=>$n,"p"=>$path['filename'],"s"=>filesize($dir."/models/".$path['filename'].".m3d"),
        "l"=>$l==","?"Public Domain":$l,"f"=>$d[0],"u"=>$d[1],"o"=>$d[2],"h"=>$d[3],"U"=>$d[4],"O"=>$d[5],"v"=>$d[6],
        "m"=>$d[7],"t"=>$d[8],"b"=>$d[9],"a"=>$d[10],"L"=>$d[11],"V"=>$d[12],"c"=>$t];
}
function safename($n)
{
    if(mb_substr($n,1,1) == " ") $n = mb_substr($n, 2);
    return htmlspecialchars(mb_strtolower($n));
}
function nicename($n)
{
    if(mb_substr($n,1,1) == " ") { $p = mb_substr($n, 0, 2); $n = mb_substr($n, 2); }
    else { $p = ""; }
    return htmlspecialchars($p.mb_strtoupper(mb_substr($n, 0, 1)).mb_substr($n, 1));
}

/* check arguments */
if(empty($_SERVER['argv'][2])) {
    die("Model 3D Homepage and Model Catalog Generator\n\nphp catalog.php <source csv> <output dir>\n\n".
        "CSV: file,convopts,name,license,author,category,tags\n");
}

/* parse input csv and convert models */
$models = []; $cat = [];
if(($f = fopen($_SERVER['argv'][1], "r")) !== FALSE) {
    for($n = 0; fgets($f) !== FALSE && !feof($f); $n++);
    fseek($f, 0);
    $l = 0;
    @mkdir($_SERVER['argv'][2]."/models");
    mb_internal_encoding("UTF-8");
    while(($line = fgetcsv($f)) !== FALSE && count($line) > 5) {
        echo("\r".round(++$l*100/$n, 2)."% -i".$line[1]." ".$line[0]."                    ");
        if($line[0][0]=='#') continue;
        $data = convertmodel($line, dirname($_SERVER['argv'][1]), $_SERVER['argv'][2]);
        $models[] = $data;
        $cat[$data["c"][0]] = 1;
    }
    fclose($f);
    echo("\r                                                                             ");
    echo("\rDone. ".count($models)." models in ".count($cat)." categories.\n");
}
if(empty($models)) die();
usort($models, function($a, $b){ return strnatcasecmp($a["n"], $b["n"]); });
$cat = array_keys($cat);

/* generate index.html */
$f = fopen($_SERVER['argv'][2]."/index.html", "w");
fwrite($f, '<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <meta name="description" content="Model 3D, file format, triangle mesh, Assimp, Blender, SDK, C, C++">
    <meta name="author" content="bzt">
    <title>Model 3D</title>
    <link href="logo.png" rel="shortcut icon">
    <link href="bootstrap.min.css" rel="stylesheet">
    <style>
body { padding-top: 64px; }
.card { height: 100%; }
.headerbg {background: #333 url(images/headerbg.png) center;background-size: cover;}
.headerbg p {text-shadow: 2px 2px 2px #000;}
.headerbg h1 {text-shadow: 3px 3px 5px #000;}
.headerbg div {padding-bottom:5px;}
.headerbg img { height: 30px; margin-right: 20px;}
#content, #results { padding-bottom: 25px; }
#menuchk:checked ~ .navbar-collapse { display:block; }
.nav-link { cursor:pointer; }
.nav-link:hover {  border-color: #888; }
.nav-tabs { border-bottom: 1px solid #888; }
.nav-tabs li { position: relative; bottom: -1px; }
.nav-tabs li label { margin-bottom: 0px !important; cursor: inherit; }
.tabcol { border-bottom: 1px solid #888; }
.webdemo img { margin: 10px; }
#results .row { padding: 10px; border: 1px solid #dee2e6; border-radius: .25rem; margin: 10px; }
#results .row div > img { width: 320px; height: 240px; cursor:pointer; background:url(images/bg.png) 50% 0px no-repeat; }
#results .row small { cursor:pointer; }
#results .row a { margin-top: 5px; width: 320px; }
#results .row a img { margin-right:10px; }
#results .row th, .filename { font-size:80% }
#results .row td { text-align: right; }
#results .row .card-text { display: none; }
#results .card { width: 170px; padding:5px; margin:5px; display:inline-block; text-align:center; }
#results .card img { width:160px; height:120px; cursor:pointer; }
#results .card .col-lg-5 { max-width:100%!important;padding:0px!important; }
#results .card a, #results .card .col-lg-7, #results .hidden { display:none; }
');
for($i = 0; $i < count($cat); $i++)
    fwrite($f, '#tab'.$i.':checked ~ ul li[rel="tab'.$i.'"]'.($i == count($cat)-1 ? '' : ",\n"));
fwrite($f, ' { border-color: #888 #888 #fff; cursor: default; }
input[name="tab"], div[rel^="tab"] { display: none; }
    </style>
  </head>
  <body>
    <nav class="navbar navbar-expand-lg navbar-dark bg-dark fixed-top">
      <div class="container">
        <a class="navbar-brand" href="/model3d/"><img src="images/model3dhdr.png" height="30" alt="Model 3D"> Model 3D</a>
        <input type="checkbox" id="menuchk" style="display:none;"><label for="menuchk" class="navbar-toggler">
          <span class="navbar-toggler-icon"></span>
        </label>
        <div class="collapse navbar-collapse" id="navbarResponsive">
          <ul class="navbar-nav ml-auto">
            <li class="nav-item">
              <a class="nav-link" href="/model3d/">Home</a>
            </li>
            <li class="nav-item">
              <a class="nav-link" href="#features" onclick="changedivs(0);">Features</a>
            </li>
            <li class="nav-item">
              <a class="nav-link" href="#sdk" onclick="changedivs(0);">Usage</a>
            </li>
            <li class="nav-item">
              <a class="nav-link" href="#models" onclick="changedivs(1);">Models</a>
            </li>
            <li class="nav-item">
              <a class="nav-link" href="https://gitlab.com/bztsrc/model3d/issues">Issues</a>
            </li>
          </ul>
        </div>
      </div>
    </nav>
    <div style="display:none;"><img src="images/headerbg.png" alt=""></div>
    <div class="container" id="content" style="display:block;">
      <header class="jumbotron my-4 headerbg" id="home">
        <h1 class="display-3 text-white">Welcome to Model 3D!</h1>
        <p class="lead text-white">Model 3D is an application and engine neutral, universal 3D model format to store CAD models, meshes, skeletal animations and voxel images. It comes with a dependency-free, single header ANSI C/C++ SDK.</p>
        <div class="row text-center">
        <div class="col-lg-6"><a href="https://gitlab.com/bztsrc/model3d/blob/master/docs/m3d_format.md" class="btn btn-primary btn-lg btn-block" target="new"><img src="images/download.png" alt="Specification">Specification</a></div>
        <div class="col-lg-6"><a href="https://gitlab.com/bztsrc/model3d/" class="btn btn-primary btn-lg btn-block" target="new"><img src="images/gitlab.png" alt="GitLab">Browse the Source</a></div>
        </div>
      </header>
      <div class="card text-white bg-secondary my-4" id="features">
        <div class="card-body">
          <p class="text-white text-center m-0">The file format and the SDK is Open Source and free as in free beer! Licensed under <a href="https://gitlab.com/bztsrc/model3d/blob/master/LICENSE" class="text-white" target="new"><b>MIT license</b></a>.</p>
        </div>
      </div>

      <div class="row text-center">
        <div class="col-lg-3 col-md-6 mb-4">
          <div class="card">
            <h4 class="card-header">Compact</h4>
            <div class="card-body">
              <small class="card-text">The format has the best data density of all the available 3D model formats (including <a href="https://gitlab.com/bztsrc/model3d/blob/master/docs/CAD.md" target="new">CAD</a>), easy on network traffic. Parsing is painless and lightning fast.</small>
            </div>
            <div class="card-footer">
              <a href="https://gitlab.com/bztsrc/model3d/blob/master/docs/performance.md" class="btn btn-primary" target="new">Find Out More!</a>
            </div>
          </div>
        </div>

        <div class="col-lg-3 col-md-6 mb-4">
          <div class="card">
            <h4 class="card-header">Animations</h4>
            <div class="card-body">
              <small class="card-text">It can store multiple skeleton-based animations in a model file (so called actions). The SDK makes it easy to get the animation-pose for each.</small>
            </div>
            <div class="card-footer">
              <a href="https://gitlab.com/bztsrc/model3d/blob/master/docs/usage.md" class="btn btn-primary" target="new">Find Out More!</a>
            </div>
          </div>
        </div>

        <div class="col-lg-3 col-md-6 mb-4">
          <div class="card">
            <h4 class="card-header">Procedural</h4>
            <div class="card-body">
              <small class="card-text">Model 3D is capable of storing procedural textures and faces. Any scripting language can be used, the SDK ships <a href="https://gitlab.com/bztsrc/model3d/blob/master/m3d_lua.h" target="new">Lua binding</a> as a Proof of Concept.</small>
            </div>
            <div class="card-footer">
              <a href="https://gitlab.com/bztsrc/model3d/blob/master/docs/procedural.md" class="btn btn-primary" target="new">Find Out More!</a>
            </div>
          </div>
        </div>

        <div class="col-lg-3 col-md-6 mb-4">
          <div class="card">
            <h4 class="card-header">Integration</h4>
            <div class="card-body">
              <small class="card-text">Extremely easy to integrate into any project and any workflow. A command line tool can convert from any format, and a <a href="https://blender.org" target="new">Blender</a> plugin is provided to export M3D models.</small>
            </div>
            <div class="card-footer">
              <a href="https://gitlab.com/bztsrc/model3d/tree/master/blender/" class="btn btn-primary" target="new">Find Out More!</a>
            </div>
          </div>
        </div>
      </div>
    <hr>
      <div class="row">
        <div class="col-lg-12">
          <h2>Embeddable on webpages</h2>
          <p>With the <a href="https://gitlab.com/bztsrc/model3d/tree/master/webgl-js/" target="new">WebGL polyfill</a>, you can include Model 3D files in webpages just like normal 2D images! Hint: drag the images below!</p>
        </div>
      </div>
      <div class="row text-center">
        <div class="col-lg-4 webdemo">
          <img src="models/cube_with_vertexcolors.m3d" width="300" height="300" alt="demo"><br><small><samp>&lt;img src="cube.m3d" width="300" height="300"&gt;</samp></small>
        </div>
        <div class="col-lg-4 webdemo">
          <img src="models/suzanne.m3d" width="300" height="300" alt="demo"><br><small><samp>&lt;img src="suzanne.m3d" width="300" height="300"&gt;</samp></small>
        </div>
        <div class="col-lg-4 webdemo">
          <img src="models/voxelimg.m3d" width="300" height="300" alt="demo"><br><small><samp>&lt;img src="voxelimg.m3d" width="300" height="300"&gt;</samp></small>
        </div>
      </div>
    <hr id="sdk">
      <div class="row">
        <div class="col-lg-12">
          <h2>Easy to use Software Development Kit</h2>
          <ul>
            <li>The M3D SDK is an stb-style, dependency-free <a href="https://gitlab.com/bztsrc/model3d/blob/master/m3d.h" target="new">single header</a> file written in ANSI C89, licensed under MIT.</li>
            <li>If you prefer, the SDK can provide a C++11 wrapper class around the C API, or you can write your own.</li>
            <li>You can configure the SDK using defines to add <a href="https://gitlab.com/bztsrc/model3d/blob/master/docs/a3d_format.md" target="new">ASCII format</a> support or model exporting functionality.</li>
            <li>You don\'t have to worry about the textures, the SDK will decompress PNG textures for you on its own (no library needed).</li>
            <li>Unlike other 3D model SDKs, the in-memory format is simple, your code that interfaces with it can be extremely simple.</li>
            <li>Follows the K.I.S.S. principle, there\'re only <a href="https://gitlab.com/bztsrc/model3d/blob/master/docs/API.md" target="new">5 functions in the API</a>.</li>
            <li>The <a href="https://gitlab.com/bztsrc/model3d/blob/master/docs/usage.md" target="new">API manual</a> provides you with detailed description and SDK usage examples.</li>
            <li>You can integrate the <a href="https://gitlab.com/bztsrc/model3d/tree/master/m3dconv" target="new">m3dconv</a> utility into your build environment to convert foreign models in compilation time.</li>
            <li>There\'s a <a href="https://gitlab.com/bztsrc/model3d/tree/master/m3dview" target="new">simple model viewer</a>, which demonstrates how to display animated models using the M3D SDK.</li>
          </ul>
        </div>
      </div>
    <hr>
      <div class="row">
        <div class="col-lg-12">
          <h2>Convert Anything into Model 3D</h2>
          <ul>
            <li>The <a href="https://gitlab.com/bztsrc/model3d/tree/master/blender/" target="new">Blender plugin</a> makes it possible to directly save models into Model 3D format.</li>
<!--
            <li>A similar <a href="https://gitlab.com/bztsrc/model3d/tree/master/maya/" target="new">Maya plugin</a> exists for AutoDesk Maya.</li>
            <li>As well as a <a href="https://gitlab.com/bztsrc/model3d/tree/master/lw3d/" target="new">LW3D plugin</a> for Lightwave 3D.</li>
-->
            <li>With the <a href="https://gitlab.com/bztsrc/model3d/tree/master/m3dconv" target="new">m3dconv</a> command line tool you can convert almost any existing model into Model 3D easily:</li>
            <li>Fully supports <a href="https://gitlab.com/bztsrc/model3d/blob/master/docs/obj_spec.md" target="new">Wavefront OBJ</a> (including negative indices, Bezier curves / surfaces, B-spline and NURBS), and a <a href="https://webstore.ansi.org/Standards/ISO/ISO10303212016" target="new">STEP</a> (ISO-10303-21-4 / ISO-10303-24-2) file importer is on the way.</li>
            <li>Simple text files with skeletal animations using <a href="https://gitlab.com/bztsrc/model3d/blob/master/docs/ms3d.md" target="new">Milkshape 3D</a> (this is a very simple format, similar to OBJ, widely supported and damn easy to create programatically).</li>
            <li>Voxel images, like <a href="http://www.patrickmin.com/binvox/binvox.html" target="new">BINVOX</a> files, <a href="https://minecraft.gamepedia.com/Schematic_file_format" target="new">Minecraft Schematic</a> files, <a href="https://github.com/ephtracy/voxel-model/blob/master/MagicaVoxel-file-format-vox.txt" target="new">Magicavoxel VOX</a> files and <a href="https://getqubicle.com/qubicle/documentation/docs/file/qb" target="new">Qubicle QB</a> files can be converted too efficiently.</li>
            <li>FBX support via the <a href="https://github.com/bqqbarbhg/ufbx" target="new">uFBX</a> library (included).</li>
            <li>Other supported formats via <a href="http://assimp.org" target="new">Assimp</a> (not included, dinamically linked): GLTF, GLB, 3DS, 3DS MAX, X, ASE, BLEND, COLLADA (DAE), B3D, MS3D, MD5, MESH, STL, PLY and many many more...</li>
            <li>When running into problems with loading a specific model, you can use the web based <a href="validator">Model 3D file validator</a>, or run <samp>m3dconv -D</samp> natively on your local computer.</li>
          </ul>
        </div>
      </div>
    <hr>
      <div class="row">
        <div class="col-lg-12">
          <h2>Projects using Model 3D</h2>
          <ul>
            <li><a href="https://www.raylib.com/" target="new">raylib</a> a multiplatform, simple and easy-to-use programming library to enjoy videogames programming.</li>
            <li><a href="https://gitlab.com/bztsrc/mtsedit" target="new">mtsedit</a> a Minetest Schematic editor, which can import Model 3D voxel files.</li>
            <li><a href="https://tirnanog.codeberg.page" target="new">TirNanoG</a> an Action and Adventure RPG creation suite, which can generate 2D sprite sheets from Model 3D files.</li>
            <li><a href="https://machengine.org/" target="new">Mach</a> game engine and graphics toolkit for the future.</li>
          </ul>
          <small>(Open an <a href="https://gitlab.com/bztsrc/model3d/issues" target="new">issue</a> if you want your project to be listed here.)</small>
        </div>
      </div>
    </div>

    <div class="container" id="models" style="display:block;">
      <div id="modelsearch" style="display:none;margin-bottom:10px;">
        <div class="row">
          <div class="col-lg-12">
            <h2>Search Model 3D Samples</h2>
          </div>
        </div>
        <div class="row">
          <div class="col-lg-2 tabcol">
          <input class="form-control" id="srch" type="text" placeholder="Search..." onkeyup="dosearch();">
            <input type="hidden" id="last" value="">
            <input type="hidden" id="tag" value="">
          </div>
          <div class="col-lg-10" style="padding-left:0px;">
');
for($i = 0; $i < count($cat); $i++)
    fwrite($f, '<input type="radio" name="tab" id="tab'.$i.'" onchange="dosearch(this.value);" value="'.safename($cat[$i]).'"'.(!$i?" checked":"").'>'."\n");
fwrite($f, '            <ul class="nav nav-tabs">');
for($i = 0; $i < count($cat); $i++)
    fwrite($f, '              <li class="nav-link" rel="tab'.$i.'"><label for="tab'.$i.'">'.nicename($cat[$i]).'</label></li>'."\n");
fwrite($f, '            </ul>
          </div>
        </div>
      </div>
      <div id="modelhdr" class="row" style="display:block;">
        <hr>
        <div class="col-lg-12">
          <h2>Model 3D Samples</h2>
        </div>
      </div>
      <div id="results">
');
for($i = 0; $i < count($models); $i++) {
    $s = $models[$i]["s"]; $u = " bytes";
    if($s < 8) continue;
    if($s > 1023) { $s = floor(($s + 1023)/1024); $u = " Kib"; }
    fwrite($f, '      <div class="row">
        <div class="col-lg-5 text-center"><img src="models/'.$models[$i]["p"].'.png" alt="" onclick="togglediv(this);"><div class="card-text">'.htmlspecialchars($models[$i]["n"]).'</div>
        <a href="models/'.$models[$i]["p"].'.m3d" class="btn btn-primary" target="new"><img src="images/download.png" alt="Download">Download</a></div>
        <div class="col-lg-7">
          <h5>'.htmlspecialchars($models[$i]["n"]).'</h5>
          <p>'.htmlspecialchars($models[$i]["l"]).'</p>
          <table class="table table-sm table-striped">
            <tr><th>File</th><td class="filename">'.htmlspecialchars($models[$i]["p"]).'.m3d ('.number_format($s,0,""," ").$u.')</td></tr>
            <tr><th>Polygons'.($models[$i]["u"]?' <span class="badge badge-dark">UV</span>':'').($models[$i]["o"]?' <span class="badge badge-dark">Normals</span>':'').'</th><td>'.number_format($models[$i]["f"],0,""," ").'</td></tr>
            <tr><th>Shapes'.($models[$i]["U"]?' <span class="badge badge-dark">UV</span>':'').($models[$i]["O"]?' <span class="badge badge-dark">Normals</span>':'').'</th><td>'.number_format($models[$i]["h"],0,""," ").'</td></tr>
            <tr><th>Vertices</th><td>'.number_format($models[$i]["v"],0,""," ").'</td></tr>
            <tr><th>Voxels</th><td>'.number_format($models[$i]["V"],0,""," ").'</td></tr>
            <tr><th>Materials</th><td><span class="badge badge-'.($models[$i]["m"]>0?'success">Yes':'danger">No').'</span></td></tr>
            <tr><th>Textures inlined</th><td><span class="badge badge-'.($models[$i]["t"]>0?'success">Yes':'danger">No').'</span></td></tr>
            <tr><th>Rigged (bones + skin)</th><td><span class="badge badge-'.($models[$i]["b"]>0?'success">Yes':'danger">No').'</span></td></tr>
            <tr><th>Animated</th><td><span class="badge badge-'.($models[$i]["a"]>0?'success">Yes':'danger">No').'</span></td></tr>
          </table>
');
    foreach($models[$i]["c"] as $c)
        fwrite($f, '          <small class="badge badge-pill badge-secondary" onclick="dosearch(this.innerText);">'.safename($c).'</small>'."\n");
    fwrite($f, '        </div>
      </div>
');
}
fwrite($f, '      </div>
    </div>
    <footer class="py-5 bg-dark">
      <div class="container">
        <p class="text-center text-white">Copyright &copy; bzt (bztsrc@gitlab) 2019.</p>
      </div>
    </footer>
    <script>
    function changedivs(isModels) {
      document.getElementById("content").style.display = isModels? "none" : "block";
      document.getElementById("models").style.display = isModels? "block" : "none";
      if(isModels) window.scrollTo(0,0);
    }
    function togglediv(div) {
      var divs = document.querySelectorAll("#results .row");
      var i, pdiv = div != undefined ? div.parentNode.parentNode : undefined;
      var pclose = pdiv != undefined ? pdiv.className == "row" : 0;
      for(i = 0; i < divs.length; i++) divs[i].className = "card";
      if(!pclose && pdiv != undefined) pdiv.className = "row";
    }
    function dosearch(newtag) {
        var srch = document.getElementById("srch"), last = document.getElementById("last"), tag = document.getElementById("tag");
        var i, j, fnd, divs = document.getElementById("results").childNodes, srchval;
        if(newtag != undefined && newtag != null) { tag.value = newtag; last.value = srch.value + "a"; }
        else if(tag.value == "") tag.value = document.getElementById("tab0").value;
        if(last.value != srch.value) {
            last.value = srch.value;
            srchval = srch.value.toLowerCase();
            for(i = 0; i < divs.length; i++) {
                if(divs[i].tagName != "DIV") continue;
                var tags = divs[i].getElementsByTagName("div")[2].getElementsByTagName("small");
                var name = divs[i].getElementsByTagName("h5")[0].innerText.toLowerCase();
                fnd = 0;
                for(j = 0; j < tags.length; j++)
                    if(tags[j].innerText == tag.value) { fnd = 1; break; }
                if(fnd && srchval != "" && !name.includes(srchval)) fnd = 0;
                divs[i].className = fnd ? "card" : "hidden";
            }
        }
    }
    /* replace model ist with searchable cards if we have javascript */
    document.addEventListener("DOMContentLoaded", function(e) {
        changedivs(document.location.href.split("#")[1] == "models");
        document.getElementById("modelhdr").style.display = "none";
        document.getElementById("modelsearch").style.display = "block";
        togglediv();
        dosearch(document.getElementById("tab0").value);
    });
    </script>
    <script src="m3d.min.js"></script>
  </body>
</html>
');
fclose($f);

