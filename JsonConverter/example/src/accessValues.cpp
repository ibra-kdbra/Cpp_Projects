#include <iostream>
#include <JsonPP.h>

int main(int args, char** argv)
{
	auto jOb = ccd::Json::Object {
			{"hello", "world"},
			{"answer", 42},
			{"pi", 3.14159},
			{"valid", true},
			{"empty", nullptr},
			{"colors", {"red", "green", "blue"}},
			{"user", ccd::Json::Object{
				{"name","max"},
				{"age",20},
				{"size",1.90},
				{"hobbies", { "reading", "rock music" }}}}};

	std::cout << "\n";
	std::cout << "##### Example - access Values #####\n";
	std::cout << "Access ccd::Json::Object values\n";
	std::cout << "###################################\n";
	
	// If using operator<< of an iostream
	// there is no need to cast the ccd::Json
	// into explicit type 
	std::cout << "iostream << value:\n";
	std::cout << "==>\n";
	std::cout << "PI: " << jOb["pi"];
	std::cout << "\n<==\n\n";

	// Read element and write to variable
	// If writing to variable use access method like
	// .toString() or a explicit cast like static_cast<int>
	auto userName = jOb["user"]["name"].toString();
	auto userAge = static_cast<int>(jOb["user"]["age"]);
	std::cout << "copy to variable:\n";
	std::cout << "==>\n";
	std::cout << "Name/Age: " << userName << "/" << userAge;
	std::cout << "\n<==\n\n";

	// Access array elements in a for loop
	std::cout << "Array elements in a loop:\n";
	std::cout << "==>\n";
	for ( const auto& color : jOb["colors"].toArray()) {
		std::cout << "color: " << color << "\n";
	}
	std::cout << "<==\n\n";
	
	return 0;
}
