#include <iostream>
#include <thread>
#include "Singleton.h"

void f1() {
    Singleton::instance().value += 2;
}

void f2() {
    Singleton::instance().value += 3;
}

int main() {
    std::thread t1(f1);
    std::thread t2(f2);
    t1.join();
    t2.join();

    std::cout << Singleton::instance().get() << std::endl;

    return 0;
}
