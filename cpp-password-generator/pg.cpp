#include <iostream>
#include <cstdlib>
#include <ctime>

using std::isdigit;
using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::stoi;

bool isNumber(string& str)
{
	if(str.empty()) return false;
	for(char const &c: str){
		if(isdigit(c)==0)
			return false;
	}
	return true;
}

int main(){
	srand(time(0));

	char ascii_uppercase[27] = {"ABCDEFGHIJKLMONPQRSTUVWXYZ"};
	char digits[11] = {"1234567890"};
	char symbs[32] = {"!\"#$&\\'()*+,-./:;<=>?@[]^_`{}|~"};
	string charsUsed = "abcdefghijklmnopqrstuvwxyz";
	string length = "";
	while(!isNumber(length)){
		cout << "Password length: ";
		cin >> length;
	}
	string underline;
	string numbers;
	string symbols;
	cout << "Uppercase? [y/n] ";
	cin >> underline;
	cout << "Numbers? [y/n] ";
	cin >> numbers;
	cout << "Symbols? [y/n] ";
	cin >> symbols;

	if(underline == "y"){charsUsed += ascii_uppercase;}
	if(numbers == "y"){charsUsed += digits;}
	if(symbols == "y"){charsUsed += symbs;}

	int len = stoi(length);
	int charsAvaliable = charsUsed.length();
	for(int i = 0;i < len;i++)
	{
		cout << charsUsed[rand() % charsAvaliable];
	}
	cout << endl;
}
