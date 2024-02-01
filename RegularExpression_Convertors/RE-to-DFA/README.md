# RegexToDFAConverter

C++ application that directly converts regular expressions into Deterministic Finite Automata

## Installation

Make sure you have installed `git`, `make` and `cmake`.

To install the application follow these commands:

```sh
#make new directory for binary
mkdir build
cd build

#pass the path to the CMakeLists.txt to cmake, in this case the parent directory of build
cmake ..

#run make
make

#the binary RegexToDFAConverter should now be present in the build folder and you can run it like this
./RegexToDFAConverter
```

## Using the application

After running the binary, the application expects you to type on the first line of the standard input a regular expression following these restrictions:

- the alphabet consists of only uppercase and lowercase english letters
- you can still have the empty letter lambda (or epsilon), unsing the `$` character
- you can have parentheses
- the symbol for union is `|`
- the symbol for catenation is `.` (and can be omitted, being considered implicit)
- the symbol for Klenee star is `*`
- you cannot use spaces
- the order of operator priorities is this: `| < . < *`

The application will print to the standard output a DFA equivalent to this regular expression in the following format:

- on the first line, the number of states
- on the second line, the ids of the states (represented as integers)
- on the third line, the number of different letters used
- on the fourth line, the used letters (separated by spaces)
- on the fifth line, the id of the starting state of the DFA
- on the sixth line, the number of final states of the DFA
- on the seventh line, the ids of the final states of the DFA
- on the eighth line, the number of transitions in the DFA
- on each of the next lines, there will be a transition represented in the format `state_from letter state_to`

## Example

For this regular expression:

```sh
(a|$)a.bb*  
```

The application will output:

```sh
4
4 3 1 2 
2
b a 
1
1
4 
5
1 a 2
2 a 3
2 b 4
3 b 4
4 b 4
```

**Observation:** Since the `.` operator is optional this expression is equivalent to the previous one:

```sh
(a|$)abb*
```
