#include "loadints.hpp"
#include "timescope.hpp"

#include "docopt/docopt.h"

#include <iostream>
#include <vector>
#include <set>


static const char USAGE[] =
R"(Find the product of N numbers which add up to M in a file
containing a list of numbers.

Usage:
  a.out [options] <path>
  a.out (-h | --help)

Options:
  -h --help           Print this help message.
  -t --target <int>   The target value that we are aiming for [default: 2020].
  -c --count <int>    The number of numbers we need to sum [default: 2].
)";



std::vector<int> FindNumbersWhichAddTo(
  unsigned const target,            // Sum that we want to reach
  unsigned const count,             // How many numbers are we looking for?
  std::vector<int> const numbers)   // List of numbers
{
  if (count > 1)
  {
    for (int const i : numbers)
    {
      int const required = target - i;

      std::vector<int> answer = FindNumbersWhichAddTo(required, count - 1, numbers);

      if (answer.size() != 0)
      {
        answer.push_back(i);
        return answer;
      }
    }
  }
  else
  {
    for (int const i : numbers)
    {
      if (i ==  target)
      {
        return {i};
      }
    }
  }

  // Oh no!
  return std::vector<int>();
}


int main(int argc, char **argv)
{
  std::map<std::string, docopt::value> args =
    docopt::docopt(USAGE, {argv + 1, argv + argc}, true);

  std::vector<int> const numbers = LoadIntsFromFile(args["<path>"].asString());
  int const target = args["--target"].asLong();
  int const count = args["--count"].asLong();

  std::vector<int> answers;

  {
    TimeScope t("Computation duration");
    answers = FindNumbersWhichAddTo(target, count, numbers);
  }

  if (answers.size() != count)
  {
    std::cerr << "Oh no! We didn't find the numbers!" << std::endl;
    return 1;
  }

  int64_t product = 1;

  for (int const i : answers)
  {
    product *= i;
  }

  std::cout << "product: " << product << std::endl;

  return 0;
}
