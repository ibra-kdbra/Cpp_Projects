#include"collatz.hpp"
#include<cstdlib>
#include<ctime>
#include<cmath>

namespace cltz {
    //the first element in the list will be the strongest digit and the last will be the weakest digit

    bigNumber& bigNumber::genNumber(unsigned int n) {
        srand(time(0));
        number_.clear();

        for(int i = 0; i < n; ++i) {
            number_.push_back('0' + rand()%10);
        }

        return *this;

    } 

    bigNumber& bigNumber::setNumber(const std::string& s) {
        number_.clear();

        /* if(n == 0) { */
        /*     number_.push_front('0'); */
        /*     return *this; */
        /* } */

        /* while(n > 0) { */
        /*     int tmp = n%10; */
        /*     n /= 10; */
        /*     number_.push_front('0' + tmp); */
        /* } */

        for(int i = 0; i < s.size(); ++i) {
            push_back(s[i]);
        }

        return *this;

    } 
    
    std::ostream& operator<<(std::ostream& out, bigNumber& n) {

        for(auto it= n.begin(); it != n.end(); ++it) {
            out << (*it); 
        }

        return out;

    }


    bigNumber& bigNumber::operator*(long long other){
      int remained=0;
      // iterating throuht the list from the end to the begging
      for (auto it = --(this->end()); it != --(this->begin()); it--){
        int temp =((*it - '0')*other) + remained%10;
        remained /= 10;
        while ( temp > 9){
          temp -= 10;
          remained++;
        }
        *it = '0' + temp;
      }
      //the list is already filled up to the beggining
      while (remained > 0){
        number_.push_front(char(remained%10 + '0'));
        remained /=10;
      }

      return *this;
    }

    bool bigNumber::operator==(long long other){
      int equals=0,number_of_digits=int(log10(other)+1);
      if (this->size() != number_of_digits) return false;
      for(auto it= --(this->end()); it != --(this->begin()); it--){
        if (*it == (other%10 + '0')){
          equals++;
        } 
        other /= 10;
      }
      if (equals == this->size())return true;
      else return false;
    }

    bool bigNumber::operator!=(long long other){return !(this->operator==(other));}
    

    bigNumber& bigNumber::operator+(long long n) {

        //positioning iterator at the smallest digit
        auto it = end();
        it--;

        while(n > 0) {

            //extracting smallest digit
            int tmp = n%10;
            n /= 10;

            if(it != begin()) {
                //adding the number and moving it over if it overflows
                *it += tmp;

                if(*it > '9') {
                    *it -= 10;
                    n += 1;
                }

                //moving iterator to the next higher digit
                --it;
            }
            else {
                //if it reaches the begining of the list
                *it += tmp;
                if(*it > '9') {
                    *it -= 10;
                    number_.push_front('1');
                }
                else {
                    if(n > 0) number_.push_front('0');
                }

                it = begin();
            }

        }

        return *this;
    }

    
    bigNumber& bigNumber::operator/(long long n) {

       auto it = begin(); 

       int tmp = (*it) - '0';
       while(it != end()) {

           if(tmp / n == 0) {
               //removing unecessery digits or setting to zero
                if(it == begin()) {
                    pop_front();
                    it = begin();
                } 
                else {
                    *it = '0';
                    ++it;
                }

                tmp = tmp*10 + *it - '0';
                
           }
           else {
               *it = tmp/n + '0';
               tmp = tmp%n;
               ++it;
               tmp = tmp*10 + *it - '0';

           }
       }

       return *this;
    }


}
  void cltz::collatz_conjecture(bigNumber& number){

    bigNumber repetition;
    repetition.setNumber("1");

    while(number != 1){
        std::cout << repetition << " number: " << number << std::endl;
      if (number.is_even()) {
        number = number/2;
      }else if(number.is_odd()){
        number = number*3 + 1;
      }
      repetition = repetition + 1 ;
    }
    std::cout << repetition << " number: " << number << std::endl;
  }
