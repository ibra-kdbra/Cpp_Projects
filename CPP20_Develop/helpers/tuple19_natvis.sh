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
    echo -n '  <Type Name="tuple19::Tuple&lt;'
    asterisks $i
    echo '&gt;">'
}

function viewer()
{
    echo -n "((tuple19::details::TupleEntry&lt;$[$1-1],\$T$1&gt;*)&amp;indexed)->v"
}


function displayStrings()
{
    echo -n "    <DisplayString IncludeView=\"noparens\">"
    echo -n "{$(viewer 1)}"
    num=$1
    for j in $(seq $[$num-1])
    do
        echo -n ", {" $(viewer $[$j + 1]) "}"
    done
    echo "</DisplayString>"
    echo "    <DisplayString ExcludeView=\"noparens\">({*this,view(noparens)})</DisplayString>"
}

function expansion()
{
    num=$1
    echo "    <Expand HideRawView=\"true\">"
    for j in $(seq $num)
    do
        echo -n "      <Item Name=\"[$[j-1]]\">"
        viewer $j
        echo "</Item>"
    done
    echo "    </Expand>"
}

echo "$INTRO"

for i in $(seq 32)
do
  typeVariadicHeader $i
  displayStrings $i
  expansion $i
  echo "$TYPE_END"
done

echo "$OUTRO"
