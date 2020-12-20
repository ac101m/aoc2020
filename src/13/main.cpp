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

modes:
 schedule - (schedule a bus ride)
 challenge - (solve the bus companies challenge)

Usage:
  a.out [options] <path>
  a.out (-h | --help)

Options:
  -h --help   Print this help message.
  -m --mode <mode>  The mode to run the tool in. [default: schedule]
)";


struct Bus
{
  uint64_t id;
  uint64_t position;

  Bus(uint64_t const id, uint64_t const position) :
    id(id), position(position)
  {}

  bool operator<(Bus const& other) const
  {
    return id > other.id;
  }
};


uint64_t WaitDuration(uint64_t const timestamp, Bus const bus)
{
  uint64_t const frequency = bus.id;

  uint64_t wait_duration = 0;

  if (timestamp % frequency != 0)
  {
    wait_duration = frequency - (timestamp % frequency);
  }

  return wait_duration;
}


pair<uint64_t, uint64_t> BestBus(int const timestamp, vector<Bus> const& busses)
{
  uint64_t best_bus_id;
  uint64_t best_wait_duration = -1;

  for (Bus const bus : busses)
  {
    uint64_t const wait_duration = WaitDuration(timestamp, bus);

    if (wait_duration < best_wait_duration || best_wait_duration < 0)
    {
      best_wait_duration = wait_duration;
      best_bus_id = bus.id;
    }
  }

  return make_pair(best_bus_id, best_wait_duration);
}


bool TimestampIsAligned(uint64_t const timestamp, vector<Bus> const busses)
{
  for (Bus const bus : busses)
  {
    if (WaitDuration(timestamp + bus.position, bus) != 0)
    {
      return false;
    }
  }

  return true;
}

uint64_t FindAlignedTimestampFast(vector<Bus> busses)
{
  sort(busses.begin(), busses.end());

  uint64_t timestamp = 0;
  uint64_t i = 0;

  uint64_t step = 1;

  for (Bus const bus : busses)
  {
    while (WaitDuration(timestamp + bus.position, bus) != 0)
    {
      timestamp += step;
    }

    step *= bus.id;
  }

  return timestamp;
}


int main(int argc, char **argv)
{
  auto args = docopt::docopt(USAGE, {argv + 1, argv + argc}, true);

  string const path = args["<path>"].asString();
  string const mode = args["--mode"].asString();

  vector<string> const lines = LoadLinesFromFile(path);

  if (lines.size() < 2)
  {
    cerr << "Invalid input, not enough lines!" << endl;
    exit(1);
  }

  string const& timestamp_str = lines.at(0);
  string const& schedule_str = lines.at(1);
  vector<string> const schedule_tokens = Split(schedule_str, ',');

  uint64_t timestamp;
  vector<Bus> busses;

  stringstream(timestamp_str) >> timestamp;

  for (int i = 0; i < schedule_tokens.size(); i++)
  {
    string const token = trim(schedule_tokens.at(i));

    if (token != "x")
    {
      int bus_id;
      stringstream(token) >> bus_id;
      busses.push_back(Bus(bus_id, i));
    }
  }

  if (mode == "schedule")
  {
    auto const best_bus = BestBus(timestamp, busses);
    cout << "Bus ID: " << best_bus.first << endl;
    cout << "Minimum wait: " << best_bus.second << endl;
    cout << "Checksum: " << best_bus.first * best_bus.second << endl;
  }
  else if (mode == "challenge")
  {
    cout << "Timestamp: " << FindAlignedTimestampFast(busses) << endl;
  }
  else
  {
    cerr << mode << " is not a valid mode!" << endl;
    exit(1);
  }

  return 0;
}
