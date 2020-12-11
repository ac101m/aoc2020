#include "docopt/docopt.h"
#include "loadints.hpp"

#include <vector>
#include <string>
#include <iostream>


using namespace std;


static const char USAGE[] =
R"(Adapter demystifier v1.0

modes:
 - checksum (compute checksum of adapter differences)
 - enumerate (enumerate possible ways of connecting adapaters)

Usage:
  a.out [options] <path>
  a.out (-h | --help)

Options:
  -h --help         Print this help message.
  -m --mode <mode>  Which mode to use the tool in. [default: checksum]
)";


int GetJoltChecksum(vector<int> const jolt_ratings)
{
  int previous_jolt_rating = 0;
  vector<int> difference_size_counts(3);

  for (auto const jolt_rating : jolt_ratings)
  {
    int const diff = jolt_rating - previous_jolt_rating;
    difference_size_counts.at(diff - 1)++;
    previous_jolt_rating = jolt_rating;
  }

  return difference_size_counts.at(0) * difference_size_counts.at(2);
}


uint64_t CountConnectionPossibilities(vector<int> const jolt_ratings)
{
  vector<uint64_t> connection_possibilities(jolt_ratings.size(), 0);

  for (int i = 0; i < jolt_ratings.size(); i++)
  {
    connection_possibilities.at(i) = jolt_ratings.at(i) <= 3 ? 1 : 0;

    for (int j = i - 1; j >= 0 && jolt_ratings.at(i) - jolt_ratings.at(j) <= 3; j--)
    {
      connection_possibilities.at(i) += connection_possibilities.at(j);
    }
  }

  return connection_possibilities.back();
}


int main(int argc, char **argv)
{
  auto args = docopt::docopt(USAGE, {argv + 1, argv + argc}, true);

  string const path = args["<path>"].asString();
  string const mode = args["--mode"].asString();

  vector<int> jolt_ratings = LoadIntsFromFile(path);
  sort(jolt_ratings.begin(), jolt_ratings.end());
  jolt_ratings.push_back(jolt_ratings.back() + 3);

  if (mode == "checksum")
  {
    cout << GetJoltChecksum(jolt_ratings) << endl;
  }
  else if (mode == "enumerate")
  {
    cout << CountConnectionPossibilities(jolt_ratings) << endl;
  }
  else
  {
    cerr << mode << " is not a valid mode!" << endl;
    exit(1);
  }

  return 0;
}
