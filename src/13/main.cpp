#include "docopt/docopt.h"
#include "loadlines.hpp"
#include "stringutil.hpp"

#include <vector>
#include <string>
#include <sstream>
#include <iostream>


using namespace std;


static const char USAGE[] =
R"(Bus schedule decombobulator v1.0

Usage:
  a.out [options] <path>
  a.out (-h | --help)

Options:
  -h --help   Print this help message.
)";


int main(int argc, char **argv)
{
  auto args = docopt::docopt(USAGE, {argv + 1, argv + argc}, true);

  string const path = args["<path>"].asString();
  vector<string> const lines = LoadLinesFromFile(path);

  if (lines.size() < 2)
  {
    cerr << "Invalid input, not enough lines!" << endl;
    exit(1);
  }

  string const& timestamp_str = lines.at(0);
  string const& schedule_str = lines.at(1);

  int timestamp;
  vector<int> bus_ids;

  stringstream(timestamp_str) >> timestamp;

  for (string token : Split(schedule_str, ','))
  {
    token = trim(token);

    if (token != "x")
    {
      bus_ids.push_back(0);
      stringstream(token) >> bus_ids.back();
    }
  }

  int best_bus_id;
  int best_wait_duration = 16777216;

  for (int const id : bus_ids)
  {
    int const& frequency = id;

    int wait_duration = 0;

    if (timestamp % frequency != 0)
    {
      wait_duration = frequency - (timestamp % frequency);
    }

    if (wait_duration < best_wait_duration)
    {
      best_wait_duration = wait_duration;
      best_bus_id = id;
    }
  }

  cout << best_wait_duration * best_bus_id << endl;

  return 0;
}
