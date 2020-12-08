#include "docopt/docopt.h"
#include "loadlines.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <set>


using namespace std;


static const char USAGE[] =
R"(Let's do some customs declarations! yay!

modes:
 - any (anyone in a group)
 - all (everyone in a group)

Usage:
  a.out [options] <path>
  a.out (-h | --help)

Options:
  -h --help         Print this help message.
  -m --mode <mode>  Which mode should this run in. [default: any]
)";


int CountAnswersAny(vector<string> const answers)
{
  set<char> seen_chars;
  int unique_chars = 0;

  for (auto const answer : answers)
  {
    for (char const c : answer)
    {
      if (seen_chars.find(c) == seen_chars.end())
      {
        unique_chars++;
        seen_chars.insert(c);
      }
    }
  }

  return unique_chars;
}


int CountAnswersAll(vector<string> const answers)
{
  int count = 0;

  for (char const c : answers.at(0))
  {
    int increment = 1;

    for (unsigned i = 1; i < answers.size(); i++)
    {
      string const answer = answers.at(i);

      if (answer.find_first_of(c) == string::npos)
      {
        increment = 0;
        break;
      }
    }

    count += increment;
  }

  return count;
}


int EnumerateAnswers(vector<string> const answers, string const mode)
{
  if (mode == "any")
  {
    return CountAnswersAny(answers);
  }
  else if (mode == "all")
  {
    return CountAnswersAll(answers);
  }
  else
  {
    cerr << mode << " is not a valid mode!" << endl;
    exit(1);
  }
}


int main(int argc, char **argv)
{
  auto args = docopt::docopt(USAGE, {argv + 1, argv + argc}, true);

  string const path = args["<path>"].asString();
  string const mode = args["--mode"].asString();

  int sum = 0;
  vector<string> group;

  for (auto const line : LoadLinesFromFile(path))
  {
    if (line != "")
    {
      group.push_back(line);
    }
    else
    {
      sum += EnumerateAnswers(group, mode);
      group.clear();
    }
  }

  sum += EnumerateAnswers(group, mode);

  cout << sum << endl;

  return 0;
}
