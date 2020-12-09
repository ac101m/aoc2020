#include "docopt/docopt.h"
#include "loadlines.hpp"

#include <vector>
#include <string>
#include <sstream>
#include <iostream>


using namespace std;


static const char USAGE[] =
R"(Entertainment system "interfacing" tool

Usage:
  a.out [options] <path>
  a.out (-h | --help)

Options:
  -h --help                 Print this help message.
  -w --window-size <count>  Count of values in value window. [default: 25]
)";


template<class T>
bool VectorContains(vector<T> const v, T const target)
{
  for (auto const element : v)
  {
    if (element == target)
    {
      return true;
    }
  }

  return false;
}


bool WindowContainsSumPair(vector<uint64_t> const window, uint64_t const target)
{
  for (uint64_t const i : window)
  {
    uint64_t const required = target - i;

    if (required != i && VectorContains(window, required))
    {
      return true;
    }
  }

  return false;
}


uint64_t FindErrorNumber(
  vector<uint64_t> const sequence,
  unsigned const preamble)
{
  vector<uint64_t> window(preamble);
  unsigned window_ptr = 0;

  for (unsigned i = 0; i < preamble; i++)
  {
    window.at(i) = sequence.at(i);
  }

  for (unsigned i = preamble; i < sequence.size(); i++)
  {
    uint64_t const value = sequence.at(i);

    if (!WindowContainsSumPair(window, value))
    {
      return value;
    }

    window.at(window_ptr) = value;
    window_ptr = (window_ptr + 1) % preamble;
  }

  return 0;
}


vector<uint64_t> FindContiguousSum(vector<uint64_t> const sequence, uint64_t const target)
{
  unsigned back = 0;
  unsigned front = 1;

  uint64_t current_sum = sequence.at(front) + sequence.at(back);

  while (current_sum != target)
  {
    if (current_sum < target)
    {
      current_sum += sequence.at(++front);
    }
    else if (current_sum > target)
    {
      current_sum -= sequence.at(back++);
    }
  }

  vector<uint64_t> contiguous_sum((front - back) + 1);

  for (unsigned i = 0; i < contiguous_sum.size(); i++)
  {
    contiguous_sum.at(i) = sequence.at(back + i);
  }

  return contiguous_sum;
}


int main(int argc, char **argv)
{
  auto args = docopt::docopt(USAGE, {argv + 1, argv + argc}, true);

  string const path = args["<path>"].asString();
  unsigned const window_size = args["--window-size"].asLong();

  vector<uint64_t> sequence;

  for (auto const line : LoadLinesFromFile(path))
  {
    stringstream ss(line);
    uint64_t value;
    ss >> value;
    sequence.push_back(value);
  }

  uint64_t const invalid_number = FindErrorNumber(sequence, window_size);

  vector<uint64_t> contiguous_sum = FindContiguousSum(sequence, invalid_number);
  sort(contiguous_sum.begin(), contiguous_sum.end());

  cout << "Error number: " << invalid_number << endl;
  cout << "Contiguous sum signature: " << contiguous_sum.back() + contiguous_sum.front() << endl;

  return 0;
}
