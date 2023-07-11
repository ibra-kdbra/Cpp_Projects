Model 3D Tools
==============

Catalog Generator
-----------------

The `catalog.php` is a small script I use to convert models in batch and generate the homepage and the [model catalog](https://bztsrc.github.io/model3d/).

STEP Analyzer
-------------

The other, `stepanal.php` is a tool to analyze and dump STEP files.
```
php stepanal.php <stepfile> <entity id | entity name> [maxlevel]
```
Will dump the hierarchy sub-tree in STEP files from the given entity.
```
php stepanal.php -p <step file1> [step file2] [step file3...]
```
Will read in all STEP files given in the arguments and will print parent - child relations for entity names.
```
php stepanal.php -e <step file> <entity id>
```
Will extract one sub-tree from the STEP file which contains entity id. You might want to redirect the output to a new STEP file.
