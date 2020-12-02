#include "loadlines.hpp"
#include "stringutil.hpp"

#include "docopt/docopt.h"

#include <iostream>


static const char USAGE[] =
R"(Check for correct passwords.
modes:
  1 - counts - "1-3 a" means "one to three 'a' characters"
  2 - positions - "1-3 a" means "either position one or position three must be 'a'"

Usage:
  a.out [options] <path>
  a.out (-h | --help)

Options:
  -h --help           Print this help message.
  -m --mode <mode>    The mode to run checks in [default: 1].
)";


std::string GetRule(std::string const line)
{
  char const delimiter = ':';
  return trim(line.substr(0, line.find(delimiter)));
}


std::string GetPassword(std::string const line)
{
  char const delimiter = ':';
  return trim(line.substr(line.find(delimiter) + 1, line.size()));
}


char GetRuleChar(std::string const rule)
{
  return rule[rule.find_last_of(' ') + 1];
}


std::string GetRuleRange(std::string const rule)
{
  return trim(rule.substr(0, rule.find_last_of(' ')));
}


int GetRuleMin(std::string const rule)
{
  std::string const range = GetRuleRange(rule);
  std::string const min_str = trim(range.substr(0, range.find_first_of('-')));
  return StringToInt(min_str);
}


int GetRuleMax(std::string const rule)
{
  std::string const range = GetRuleRange(rule);
  std::string const min_str = trim(range.substr(range.find_first_of('-') + 1, range.size()));
  return StringToInt(min_str);
}


bool CheckPasswordCount(std::string const rule, std::string const password)
{
  char const ch = GetRuleChar(rule);
  int const max = GetRuleMax(rule);
  int const min = GetRuleMin(rule);

  int count = 0;

  for (char const c : password)
  {
    if (c == ch)
    {
      count++;
    }
  }

  return (count >= min && count <= max);
}


bool CheckPasswordPosition(std::string const rule, std::string const password)
{
  char const ch = GetRuleChar(rule);
  int const pos1 = GetRuleMax(rule) - 1;
  int const pos2 = GetRuleMin(rule) - 1;

  return ((password.at(pos1) == ch) != (password.at(pos2) == ch));
}


bool CheckLine(std::string const line, int const mode)
{
  std::string rule = GetRule(line);
  std::string password = GetPassword(line);

  if (mode == 1)
  {
    return CheckPasswordCount(rule, password);
  }
  else if (mode == 2)
  {
    return CheckPasswordPosition(rule, password);
  }
  else
  {
    std::cerr << mode << " is not a recognised mode!" << std::endl;
    exit(1);
  }
}


int main(int argc, char **argv)
{
  std::map<std::string, docopt::value> args =
    docopt::docopt(USAGE, {argv + 1, argv + argc}, true);

  int const mode = args["--mode"].asLong();
  std::vector<std::string> const lines = LoadLinesFromFile(args["<path>"].asString());

  int count = 0;

  for (auto const line : lines)
  {
    if (CheckLine(line, mode))
    {
      count++;
    }
  }

  std::cout << count << "\n";

  return 0;
}
