#include <algorithm>
#include "InputParser.h"

InputParser::InputParser(int &argc, char **argv){
    for (int i=1; i < argc; ++i)
        this->tokens.push_back(std::string(argv[i]));
}

const std::string& InputParser::getStr(const std::string &option){
    std::vector<std::string>::const_iterator itr;
    itr = std::find(this->tokens.begin(), this->tokens.end(), option);
    if (itr != this->tokens.end() && ++itr != this->tokens.end()){
        return *itr;
    }
    static const std::string empty_string("");
    return empty_string;
}
const int InputParser::getInt(const std::string &option){
    const std::string &s = this->getStr(option);
    return std::stoi(s);
}
const float InputParser::getFloat(const std::string &option){
    const std::string &s = this->getStr(option);
    return std::stof(s);
}

bool InputParser::exists(const std::string &option){
    return std::find(this->tokens.begin(), this->tokens.end(), option) != this->tokens.end();
}
