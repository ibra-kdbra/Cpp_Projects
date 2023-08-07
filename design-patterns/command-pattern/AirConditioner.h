#ifndef COMMAND_PATTERN_AIRCONDITIONER_H
#define COMMAND_PATTERN_AIRCONDITIONER_H


class AirConditioner {
public:
    AirConditioner():on(false), temp((this->HIGH_TEMP + this->LOW_TEMP) / 2) {}

    void turnOn();

    void turnOff();

    void raiseTemp();

    void lowerTemp();

private:

    const int HIGH_TEMP = 35;
    const int LOW_TEMP = 16;

    bool on;
    int temp;
};


#endif //COMMAND_PATTERN_AIRCONDITIONER_H
