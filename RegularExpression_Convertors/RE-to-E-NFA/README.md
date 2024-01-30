# RE-to-E-NFA

A c++ terminal based application to convert regular expression to epsilon non-deterministic finite automata.

Algorithm:

1. Create a menu for getting regular expressions input as choice.

2. To draw NFA for a, a/b ,ab ,a* create a routine for each regular expression.

3. For converting from regular expression to NFA, certain transitions had been made based on choice of input at the runtime.

4. Each of the NFA will be displayed in sequential order.

To execute the program:-

g++ -o outputfilename program name

./outputfilename

(a|b)*abb
