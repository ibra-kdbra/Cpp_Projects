#ifndef COMMAND_PATTERN_RAISETEMPCOMMAND_H
#define COMMAND_PATTERN_RAISETEMPCOMMAND_H


#include "ICommand.h"
#include "AirConditioner.h"

class RaiseTempCommand : public ICommand<AirConditioner> {
public:

    RaiseTempCommand(std::shared_ptr<AirConditioner> ac) : ICommand(ac) {}

    void execute();
    void undo();
};


#endif //COMMAND_PATTERN_RAISETEMPCOMMAND_H
