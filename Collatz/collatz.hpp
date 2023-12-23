#pragma once
#include<list>
#include<iostream>
#include<string>

namespace cltz {

    class bigNumber {
        public:
            bigNumber() = default;
            bigNumber(const bigNumber& other): number_{other.number_} {}
            bigNumber(bigNumber&& other): number_{std::move(other.number_)} {}
            bigNumber& operator=(const bigNumber& other) {
                number_ = other.number_;
                return *this;
            }
            bigNumber& operator=(bigNumber&& other) {
                number_ = std::move(other.number_);
                return *this;
            }
            //all of the methods return a reference to bigNumber so they can be chained

            //method to generate random number with number of digits as argument (for ginormous numbers, NECESSERY!) 
            //if a number already exists delete it
            bigNumber& genNumber(unsigned int);
            //method to push back a digit one number at a time (to increase the number one digit at a time)
            bigNumber& push_back(const char& c) {
                number_.push_back(c);
                return *this;
            }

            //method to pop last digit (not neccessary) 
            bigNumber& pop_back() {
                number_.pop_back();
                return *this;
            }
            //method to pop first digit
            bigNumber& pop_front() {
                number_.pop_front();
                return *this;
            }
            //method to set number_ from a value (for smaller numbers, not necessary, will make testing a lot easier, will require for loops and division) 
            //if a number already exists delete it
            bigNumber& setNumber(const std::string&);

            //operator to add a number (NECCESARY!) 
            bigNumber& operator+(long long);
            //operator to substract a number (implement last because it might not be necessasry for this project)
            bigNumber& operator-(long long);
            //operator to multiply (NECCESARY!)
            bigNumber& operator*(long long);
            //operator to divide (NECCESARY!)
            bigNumber& operator/(long long);
            //operator to check equality
            bool operator==(long long);
            //operator tp check inequality
            bool operator!=(long long);

            //getter for size
            long long size() const { return number_.size(); }
            //begin() method
            std::list<char>::iterator begin() { return number_.begin(); }
            //end() method
            std::list<char>::iterator end() { return number_.end(); }

            //check if even/odd
            bool is_even() {
                return (*(--end()))%2 == 0;
            }
            bool is_odd() {
                return !is_even();
            }

        private:
            std::list<char> number_;

    };

    //function declaration for collatz-conjecture (takes out bigNumber as input, spits out on screen number of steps it takes to reach zero)
    //possibly print number at each step
    //possibly draw graph of the process (there is a library like in python for that)
    void collatz_conjecture(bigNumber&);

    //print bigNumber
    std::ostream& operator<<(std::ostream&, bigNumber&);
}
