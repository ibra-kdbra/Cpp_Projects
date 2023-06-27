# pipes
Pipes terminal screen saver made in C  

## Config
Config can be found in `config.h`  
`OPTBOLD`: Are the characters bold  
`OPTTIME`: Time between frames  
`OPTCHANCE`: Chance for the pipe to change direction

## Compiling
Run `make`

## Installing
Run `make install` to install  
Run `make uninstall` to uninstall

## Usage
Run `pipes`

## Problems
At the start a few thousand ticks could be pre-computed to make it always look like it has been running  
There could be overlap detection creating + shapes where things cross  
Cmdline args
