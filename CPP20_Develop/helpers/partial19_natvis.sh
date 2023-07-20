#!/bin/bash

_IFS=$IFS
IFS=''

read -d '' INTRO << 'INTRO'
<?xml version='1.0' encoding='utf-8'?>
<AutoVisualizer xmlns="http://schemas.microsoft.com/vstudio/debugger/natvis/2010">
  <Type Name="partial19::Bitset&lt;*&gt;">
    <DisplayString>{{{$T1} in {Len}x{BitsPerT} ({m,x})}}</DisplayString>
    <Expand>
      <IndexListItems>
         <Size>$T1</Size>
         <ValueNode>(m[$i / BitsPerT] &gt;&gt; ($i % BitsPerT)) &amp; 1,d</ValueNode>
      </IndexListItems>
    </Expand>
  </Type>
INTRO

read -d '' OUTRO << 'OUTRO'
</AutoVisualizer>
OUTRO

read -d '' TYPE_BEGIN << 'TYPE_BEGIN'
    <Intrinsic Name="dataEmpty" Expression="m_pointer == 0" />
    <Intrinsic Name="dataPtr" Expression="m_pointer"  />

    <Intrinsic Name="has" ReturnType="bool" Expression="(bool)((m_bits.m[idx / m_bits.BitsPerT] &gt;&gt; (idx % m_bits.BitsPerT)) &amp; 1)">
      <Parameter Type="int" Name="idx" />
    </Intrinsic>

    <Intrinsic Name="alignOffset" Expression="0 == offset * Align ? offset : (((offset - 1) / Align) + 1) * Align">
      <Parameter Type="int" Name="offset" />
      <Parameter Type="int" Name="Align" />
    </Intrinsic>

    <DisplayString Condition="!dataEmpty()" >{{Count = {count} }</DisplayString>
    <DisplayString Condition="dataEmpty()" >empty</DisplayString>

    <Expand HideRawView="true">
      <CustomListItems MaxItemsPerView="5000" ExcludeView="Test">
        <Variable Name="offset" InitialValue="0" />
        <Variable Name="i" InitialValue="0" />

        <Loop>
          <Break Condition="dataEmpty()" />
          <Break Condition="i == count" />

          <If Condition="has(i)">
            <Exec>offset = alignOffset(offset, alignof_ts[i])</Exec>

TYPE_BEGIN

read -d '' TYPE_MIDDLE << 'TYPE_MIDDLE'
            <Exec>offset = offset + sizeof_ts[i]</Exec>
          </If>

          <If Condition="!has(i)" >
TYPE_MIDDLE

read -d '' TYPE_END << 'TYPE_END'
          </If>

          <Exec>i = i + 1</Exec>
        </Loop>
      </CustomListItems>
    </Expand>
  </Type>
TYPE_END


IFS=$_IFS

function asterisks() {
    echo -n "*"
    for i in $(seq $1)
    do
        echo -n ",*"
    done
}

function typeVariadicHeader() {
    echo -n '  <Type Name="partial19::Partial&lt;'
    asterisks $i
    echo '&gt;">'
}

function typeHasVariadicItem() {
    for i in $(seq 0 $i)
    do
        echo -n "          "
        echo -n "<Item Condition=\"i == $i\">"
        echo -n "*(\$T$[i+1] *)(dataPtr() + offset)"
        echo  "</Item>"
    done
}
function typeNotHasVariadicItem() {
    for i in $(seq 0 $i)
    do
        echo -n "          "
        echo -n "<Item Name=\"[{i}] NA\" Condition=\"i == $i\">"
        echo -n "(\$T$[i+1] *)(nullptr),na"
        echo  "</Item>"
    done
}

echo "$INTRO"

for i in $(seq 0 15)
do
  typeVariadicHeader $i
  echo "$TYPE_BEGIN"
  typeHasVariadicItem $i
  echo "$TYPE_MIDDLE"
  typeNotHasVariadicItem $i
  echo "$TYPE_END"
done

echo "$OUTRO"
