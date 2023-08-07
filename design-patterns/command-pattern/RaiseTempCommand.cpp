#include <iostream>
#include "RaiseTempCommand.h"
#include "ACNotAvailableException.h"


void RaiseTempCommand::execute() {
    if(auto ac = this->receiver.lock()){
        ac->raiseTemp();
    } else {
        throw(ACNoteAvailableException{});
    }
}

void RaiseTempCommand::undo() {
    if(auto ac = this->receiver.lock()){
        ac->lowerTemp();
    } else {
        throw(ACNoteAvailableException{});
    }
}