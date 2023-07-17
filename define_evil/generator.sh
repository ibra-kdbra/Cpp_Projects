#!/bin/bash
set -euo pipefail
IFS=$'\n\t'

max_num=110
prefix="EVIL_GEN"

printf "// auto-generated from generator.sh, DO NOT EDIT!\n"
printf "// dealing with close to or more then $max_num elements may not work\n"
printf "// this limit can be increased in generator.sh\n\n"

printf "/* MIT License
 *
 * Copyright (c) 2021 ibra-kdbra
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the \"Software\"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */\n\n"

printf "#if !defined(EVIL) || defined(_${prefix}_H)\n"
printf "#error this file should only be included by define_EVIL.h\n"
printf "#endif\n"
printf "#define _${prefix}_H\n\n"

printf "#define EVIL_GEN_MAX_NUM $max_num\n\n"

printf "// EVIL_INC_ and EVIL_DEC_\n"
printf "// usage: cat EVIL_INC_ or EVIL_DEC_ to any small number to get n+1 or n-1\n"
for (( i=0; i<$max_num; i++ ))
do
	printf "#define EVIL_INC_$i $[i + 1]\n"
done
printf "\n"
for (( i=0; i<$max_num; i++ ))
do
	printf "#define EVIL_DEC_$[i + 1] $i\n"
done
printf "\n"

printf "// enables the usage of EVIL_EQ on numbers\n"
for (( i=0; i<$max_num; i++ ))
do
	printf "#define EVIL_ENABLE_EQ_${i}_${i}\n"
done
printf "\n"

printf "// EVIL_REMOVE_COMMAS\n"
printf "#define _${prefix}_REMOVE_COMMAS(...) _${prefix}_REMOVE_COMMAS_A(__VA_ARGS__"
for (( i=0; i<$max_num; i++ ))
do
    printf ","
done
printf ")\n"
printf "#define _${prefix}_REMOVE_COMMAS_A("
for (( i=0; i<$max_num; i++ ))
do
    printf "_$i, "
done
printf "...) "
for (( i=0; i<$max_num; i++ ))
do
    printf "_$i "
done
printf "\n\n"

printf "// EVIL_COUNT\n"
printf "#define _${prefix}_COUNT_NUMBERS "
for (( i=$max_num; i>=2; i-- ))
do
    printf "$i, "
done
printf "1\n"
printf "#define _${prefix}_COUNT("
for (( i=0; i<$max_num; i++ ))
do
    printf "_$i, "
done
printf "n, ...) n\n\n"

printf "// EVIL_MAP\n"
printf "#define _${prefix}_MAP_0(closure, order, index, dir, empty)\n"
printf "#define _${prefix}_MAP_1(closure, order, index, dir, item) EVIL_CLOSURE_INVOKE(closure, item, index)\n"
for (( i=2; i<=$max_num; i++ ))
do
    printf "#define _${prefix}_MAP_$i(closure, order, index, dir, item, ...) order(EVIL_CLOSURE_INVOKE(closure, item, index), _${prefix}_MAP_$[i - 1](closure, order, EVIL_EXPAND_CAT(dir, index), dir, __VA_ARGS__))\n"
done
printf "\n"
