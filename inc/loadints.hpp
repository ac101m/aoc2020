#ifndef LOADINTS_INCLUDED
#define LOADINTS_INCLUDED

#include <vector>
#include <string>
#include <fstream>


inline std::vector<int> LoadIntsFromStream(std::istream& is)
{
  int my_int;
  std::vector<int> ints;

  while (is >> my_int)
  {
    ints.push_back(my_int);
  }

  return ints;
}


inline std::vector<int> LoadIntsFromFile(std::string const path)
{
  std::ifstream ifs(path);
  return LoadIntsFromStream(ifs);
}


#endif // LOADINTS_INCLUDED
