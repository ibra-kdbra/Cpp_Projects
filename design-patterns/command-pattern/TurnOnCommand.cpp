#include "TurnOnCommand.h"
#include "ACNotAvailableException.h"

void TurnOnCommand::execute() {
    if(auto ac = this->receiver.lock()){
        ac->turnOn();
    } else {
        throw(ACNoteAvailableException{});
    }
}

void TurnOnCommand::undo() {
    if(auto ac = this->receiver.lock()){
        ac->turnOff();
    } else {
        throw(ACNoteAvailableException{});
    }
}