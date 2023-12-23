#include<iostream>
#include<cstring>
#include"collatz.hpp"


int main(int argc, char* argv[]) {

    cltz::bigNumber n;

    if(argc != 3){
        std::cout << "Invalid arguments\nCompile example: ./file_name type number\n(type can be \"g\" (generate random number with a specified number of digits) or \"s\" (set specific number)" << std::endl;
        return 1;
    }
    else if(strcmp(argv[1], "s") == 0)
        n.setNumber(argv[2]);
    else if(strcmp(argv[1], "g") == 0)
        n.genNumber(std::stoi(argv[2]));
    else {
        std::cout << "Invalid parameter\nCompile example: ./file_name type number\n(type can be \"g\" (generate random number with a specified number of digits) or \"s\" (set specific number)" << std::endl;
        return 1;
    }
    cltz::collatz_conjecture(n);
    return 0;
}
