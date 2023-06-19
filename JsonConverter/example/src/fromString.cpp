#include <JsonPP.h>

#include <iostream>

int main(int args, char** argv) 
{
	std::cout << "\n";
	std::cout << "##### Example - from String #####\n";
	std::cout << "ccd::Json::Object from String\n";
	std::cout << "#################################\n";

	// example JSON string
	constexpr auto JSONstring=R"({"hello":"world","answer":42,"pi":3.14159,"valid":true,"empty":null,"colors":["red","green","blue"],"user":{"name":"max","age":20,"size":1.90,"hobbies":["reading","rockmusic"]}})";

	// parse String and return ccd::Json::Object
	auto jOb = ccd::Json::objectFromString(JSONstring);

	std::cout << "Initial String:\n";
	std::cout << "==>\n";
	std::cout << JSONstring;
	std::cout << "\n<==\n\n";

	std::cout << "Resulting Object:\n";
	std::cout << "==>\n";
	std::cout << jOb;
	std::cout << "\n<==\n\n";

	return 0;
}
