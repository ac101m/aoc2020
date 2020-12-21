#include "docopt/docopt.h"
#include "loadlines.hpp"
#include "stringutil.hpp"

#include <vector>
#include <string>
#include <sstream>


using namespace std;


static const char USAGE[] =
R"(Memory game chea- I mean "assistance" system.

Usage:
  a.out [options] <path>
  a.out (-h | --help)

Options:
  -h --help         Print this help message.
  -t --turn <turn>  Which turn are we interested in knowing the value for. [default: 2020]
)";


int main(int argc, char **argv)
{
  auto args = docopt::docopt(USAGE, {argv + 1, argv + argc}, true);

  string const path = args["<path>"].asString();
  int const target_turn = args["--turn"].asLong();

  vector<int> sequence, starting_numbers;
  map<int, vector<int>> occurrence_map;

  vector<string> const lines = LoadLinesFromFile(path);

  for (string token : Split(lines.at(0), ','))
  {
    starting_numbers.push_back(0);
    stringstream(token) >> starting_numbers.back();
  }

  for (int turn = 0; turn < target_turn; turn++)
  {
    int const previous_number = sequence.size() > 0 ? sequence.back() : 0;
    int next_number = 0;

    if (turn < starting_numbers.size())
    {
      next_number = starting_numbers.at(turn);
    }
    else
    {
      if (occurrence_map.find(previous_number) != occurrence_map.end())
      {
        vector<int> const& occurrences = occurrence_map.at(previous_number);

        if (occurrences.size() > 1)
        {
          next_number = occurrences.back() - occurrences.at(occurrences.size() - 2);
        }
      }
    }

    occurrence_map[next_number].push_back(turn);
    sequence.push_back(next_number);
  }

  cout << sequence.back() << endl;

  return 0;
}
