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


int Min(std::vector<int> const numbers)
{
  int min = numbers.at(0);

  for (int const i : numbers)
  {
    if (i < min)
    {
      min = i;
    }
  }

  return min;
}


std::vector<int> FindNumbersWhichAddTo(
  int const target,                 // Sum that we want to reach
  int const count,                  // How many numbers are we looking for?
  std::vector<int> const numbers,   // Array of numbers
  int const min,                    // Minimum value in numbers array
  int const i = 0)                  // Dimensions of loop "1 - up" from this one
{
  if (count > 1)
  {
    for (int j = i; j < numbers.size(); j++)
    {
      int const number = numbers.at(j);

      int const required = target - number;

      if (required >= (count - 1) * min)
      {
        std::vector<int> answer = FindNumbersWhichAddTo(required, count - 1, numbers, j, min);

        if (answer.size() != 0)
        {
          answer.push_back(number);
          return answer;
        }
      }
    }
  }
  else
  {
    for (int j = i; j < numbers.size(); j++)
    {
      int const number = numbers.at(j);

      if (number == target)
      {
        return {number};
      }
    }
  }

  // Oh no!
  return {};
}


std::vector<int> FindNumbersWhichAddTo(
  int const target,                 // Sum that we want to reach
  int const count,                  // How many numbers are we looking for?
  std::vector<int> const numbers)   // Array of numbers
{
  return FindNumbersWhichAddTo(target, count, numbers, Min(numbers));
}


int main(int argc, char **argv)
{
  std::map<std::string, docopt::value> args =
    docopt::docopt(USAGE, {argv + 1, argv + argc}, true);

  std::vector<int> numbers = LoadIntsFromFile(args["<path>"].asString());
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
    std::cout << i << std::endl;
    product *= i;
  }

  std::cout << "product: " << product << std::endl;

  return 0;
}
