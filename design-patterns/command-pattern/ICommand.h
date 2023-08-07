#ifndef COMMAND_PATTERN_ICOMMAND_H
#define COMMAND_PATTERN_ICOMMAND_H

template<class Receiver>
class ICommand {
public:
    ICommand(std::shared_ptr<Receiver> rec): receiver(rec) {}

    virtual void execute() = 0;
    virtual void undo() = 0;

protected:
    std::weak_ptr<Receiver> receiver;
};

#endif //COMMAND_PATTERN_ICOMMAND_H
