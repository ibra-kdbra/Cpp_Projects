#ifndef INPUTPARSER_H
#define INPUTPARSER_H

#include <string>
#include <vector>

class InputParser{
    public:
        InputParser (int &argc, char **argv);
        const std::string& getStr(const std::string &option);
        const int getInt(const std::string &option);
        const float getFloat(const std::string &option);
        bool exists(const std::string &option);
    private:
        std::vector <std::string> tokens;
};

#endif
