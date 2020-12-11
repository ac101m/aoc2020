#ifndef LOADLINES_INCLUDED
#define LOADLINES_INCLUDED

#include <vector>
#include <string>
#include <fstream>
#include <iostream>


inline std::vector<std::string> LoadLinesFromStream(std::istream& is)
{
  std::string line;
  std::vector<std::string> lines;

  while (std::getline(is, line))
  {
    lines.push_back(line);
  }

  return lines;
}


inline std::vector<std::string> LoadLinesFromFile(std::string const path)
{
  std::ifstream ifs(path);

  if (!ifs)
  {
    std::cerr << "No such file '" << path << "'" << std::endl;
    exit(1);
  }

  return LoadLinesFromStream(ifs);
}


#endif // LOADLINES_INCLUDED
