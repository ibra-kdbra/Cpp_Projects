# Collatz-Conjecture

The Collatz conjecture in mathematics asks whether repeating two simple arithmetic operations will eventually transform every positive integer into one.

It is a simple sequence:

* if the number is even divide it by two
* if the number is odd multiply it by three and add 1

Compile with g++ main.cpp collatz.cpp -o collatz-conjecture

If you want to input a specific number execute with ./collatz-conjecture s <your_number_here>

If you want to generate a random number with specific length execute with ./collatx-conjecture g <number_of_digits>
