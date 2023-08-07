#ifndef COMMAND_PATTERN_TURNONCOMMAND_H
#define COMMAND_PATTERN_TURNONCOMMAND_H

#include <memory>
#include "ICommand.h"
#include "AirConditioner.h"

class TurnOnCommand : public ICommand<AirConditioner> {
public:

    TurnOnCommand(std::shared_ptr<AirConditioner> rec) : ICommand(rec) {}

    void execute();
    void undo();
};


#endif //COMMAND_PATTERN_TURNONCOMMAND_H
