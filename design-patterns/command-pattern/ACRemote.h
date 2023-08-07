#ifndef COMMAND_PATTERN_ACREMOTE_H
#define COMMAND_PATTERN_ACREMOTE_H


#include "AirConditioner.h"
#include "TurnOnCommand.h"
#include "RaiseTempCommand.h"

class ACRemote {
public:
    ACRemote(std::shared_ptr<AirConditioner> ac): on(false),
    turnOn(std::make_unique<TurnOnCommand>(ac)),
    raiseTemp(std::make_unique<RaiseTempCommand>(ac)) {}

    void pressOnOff();

    void pressTempUp();

    void pressTempDown();


private:
    bool on;
    std::unique_ptr<TurnOnCommand> turnOn;
    std::unique_ptr<RaiseTempCommand> raiseTemp;
};


#endif //COMMAND_PATTERN_ACREMOTE_H
