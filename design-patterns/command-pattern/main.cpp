#include <iostream>

#include "AirConditioner.h"
#include "ACRemote.h"

int main() {
    auto ac = std::make_shared<AirConditioner>();
    ACRemote remote{ac};

    std::cout << "---press on/off---" << std::endl;
    remote.pressOnOff();
    std::cout << "---press on/off---" << std::endl;
    remote.pressOnOff();
    std::cout << "---press temp up---" << std::endl;
    remote.pressTempUp();
    std::cout << "---press on/off---" << std::endl;
    remote.pressOnOff();
    std::cout << "---press temp down---" << std::endl;
    remote.pressTempDown();
    std::cout << "---press temp down---" << std::endl;
    remote.pressTempDown();
    std::cout << "---press temp down---" << std::endl;
    remote.pressTempDown();
    std::cout << "---press on/off---" << std::endl;
    remote.pressOnOff();
    std::cout << "---press temp down---" << std::endl;
    remote.pressTempDown();
    std::cout << "---press on/off---" << std::endl;
    remote.pressOnOff();
    std::cout << "---press temp up---" << std::endl;
    remote.pressTempUp();

    return 0;
}
