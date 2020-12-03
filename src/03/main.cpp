#include "loadlines.hpp"

#include "docopt/docopt.h"

#include <iostream>


static const char USAGE[] =
R"(Toboggan tree collision avoidance system v1.0.

Usage:
  a.out [options] <path>
  a.out (-h | --help)

Options:
  -h --help       Print this help message.
  -x --dx <int>   Downward steps per iteration [default: 3].
  -y --dy <int>   Rightward steps per iteration [default: 1].
)";


int main(int argc, char **argv)
{
  auto args = docopt::docopt(USAGE, {argv + 1, argv + argc}, true);

  std::string const path = args["<path>"].asString();
  std::vector<std::string> const tree_map = LoadLinesFromFile(path);

  int const dx = args["--dx"].asLong();
  int const dy = args["--dy"].asLong();

  int x = 0, y = 0, ouch = 0;

  while (y < tree_map.size())
  {
    if (tree_map.at(y)[x % tree_map.at(y).size()] == '#')
    {
      ouch++;
    }

    x += dx;
    y += dy;
  }

  std::cout << ouch << std::endl;

  return 0;
}
