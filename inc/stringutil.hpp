#ifndef STRINGUTIL_INCLUDED
#define STRINGUTIL_INCLUDED

#include <string>
#include <sstream>


std::string const WHITESPACE = " \n\r\t\f\v";


std::string ltrim(std::string const str)
{
  size_t start = str.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : str.substr(start);
}


std::string rtrim(std::string const str)
{
  size_t end = str.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}


std::string trim(std::string const str)
{
  return rtrim(ltrim(str));
}


int StringToInt(std::string const str)
{
  std::stringstream ss(str);
  int i;
  ss >> i;
  return i;
}


#endif // STRINGUTIL_INCLUDED
