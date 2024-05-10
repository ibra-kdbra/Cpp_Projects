#include <fstream>
#include <string>
#include <vector>

using std::string;
using std::vector;

/*!
  Removes space/tabs/new line characters from start of string.
*/
inline string ltrim(const string& str)
{
   return str.substr(str.find_first_not_of(" \t\n"));
}

/*!
  Removes space/tabs/new line characters from end of string.
*/
inline string rtrim(const string& str)
{
   return str.substr(0, str.find_last_not_of(" \t\n") + 1);
}

/*!
  Removes space/tabs/new line characters from start and end of string.
*/
inline string trim(const string& str)
{
   return ltrim(rtrim(str));
}

/*!
  Check if a string starts with another string.
*/
inline bool start_with(const string& str, const string& str2)
{
   return str.find(str2) == 0;
}

/*!
  Check if a string ends with another string.
*/
inline bool end_with(const string& str, const string& str2)
{
   if (str.length() < str2.length())
      return false;
   return 0 == str.compare(str.length() - str2.length(), str2.length(), str2);
}

/*!
  Split a string by a given delimiter, a vector of words is returned.
*/
inline vector<string> split(const string& str, const string& delim)
{
   vector<string> res{};
   res.reserve(8);

   size_t begin = 0, end = 0;
   while (true)
   {
      end = str.find(delim, begin);
      if (end == string::npos)
      {
         res.emplace_back(str.substr(begin));
         break;
      }
      res.emplace_back(str.substr(begin, end - begin));
      begin = end + delim.length();
   }
   return res;
}

inline string read_file(const string& path)
{
   string        res{};
   std::ifstream file{ path };
   for (string tmp; std::getline(file, tmp);)
   {
      res += tmp + '\n';
   }
   return res;
}
