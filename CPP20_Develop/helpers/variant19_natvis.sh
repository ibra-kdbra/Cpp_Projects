#!/bin/bash

_IFS=$IFS
IFS=''

read -d '' INTRO << 'INTRO'
<?xml version='1.0' encoding='utf-8'?>
<AutoVisualizer xmlns="http://schemas.microsoft.com/vstudio/debugger/natvis/2010">
INTRO

read -d '' OUTRO << 'OUTRO'
</AutoVisualizer>
OUTRO

read -d '' TYPE_BEGIN << 'TYPE_BEGIN'
  <Intrinsic Name=\"which\" Expression=\"(int)indexed.which\"/>
TYPE_BEGIN

read -d '' TYPE_END << 'TYPE_END'
  </Type>
TYPE_END

IFS=$_IFS

function asterisks() {
    echo -n "*"
    for k in $(seq $[$1-1])
    do
        echo -n ",*"
    done
}

function typeVariadicHeader() {
    echo -n '  <Type Name="variant19::Variant&lt;'
    asterisks $i
    echo '&gt;">'
}

function viewer()
{
    echo -n "*(\$T$1 *)&amp;(indexed.storage)"
}

function displayStrings()
{
    num=$1
    for j in $(seq $num)
    do
        echo -n "    <DisplayString Condition=\"which() == $[j-1]\" >"
        echo -n "{" $(viewer $j) "}"
        echo "    </DisplayString>"
    done
}

function expansion()
{
    num=$1
    echo "    <Expand HideRawView=\"true\">"
    echo "    <Item Name=\"which\">which()</Item>"
    for j in $(seq $num)
    do
        echo -n "      <Item Name=\"[value]\" Condition=\"which() == $[j-1]\" >"
        viewer $j
        echo "</Item>"
    done
    echo "    </Expand>"
}

echo "$INTRO"

for i in $(seq 32)
do
  typeVariadicHeader $i
  echo "$TYPE_BEGIN"
  displayStrings $i
  expansion $i
  echo "$TYPE_END"
done

echo "$OUTRO"
