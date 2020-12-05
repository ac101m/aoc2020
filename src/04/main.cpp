#include "loadlines.hpp"
#include "stringutil.hpp"

#include "docopt/docopt.h"

#include <map>
#include <string>
#include <iostream>
#include <set>


using namespace std;


static const char USAGE[] =
R"(I swear officer, I'm not hacking! I'm just
"fixing" your system for you!

Usage:
  a.out [options] <path>
  a.out (-h | --help)

Options:
  -h --help             Print this help message.
  -d --detailed-check   Perform a detailed check of passport values.
)";


set<string> const required_fields = {"byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid"};
set<string> const valid_eyecolours = {"amb", "blu", "brn", "gry", "grn", "hzl", "oth"};
string const num_chars = "0123456789";
string const hex_chars = "0123456789abcdef";


typedef map<string, string> Passport;


vector<Passport> PassportList(std::string const path)
{
  vector<Passport> passports(1);
  vector<string> lines = LoadLinesFromFile(path);

  for (auto const line : lines)
  {
    if (line == "")
    {
      passports.push_back(Passport());
    }
    else
    {
      for (auto const token : Split(line, ' '))
      {
        passports.back()[Split(token, ':').at(0)] = Split(token, ':').at(1);
      }
    }
  }

  return passports;
}


bool HasRequiredFields(Passport const passport)
{
  if (passport.size() < required_fields.size())
  {
    return false;
  }

  for (auto const field : required_fields)
  {
    if (passport.find(field) == passport.end())
    {
      return false;
    }
  }

  return true;
}


bool IntegerValueBetween(
  Passport const passport,
  string const field,
  int const min,
  int const max)
{
  int const i = StringToInt(passport.at(field));
  return i >= min && i <= max && passport.at(field).size() == 4;
}


bool HeightValid(Passport const passport)
{
  int height;
  string units;

  stringstream(passport.at("hgt")) >> height >> units;

  if (units == "cm")
  {
    return height >= 150 && height <= 193;
  }
  else if (units == "in")
  {
    return height >= 59 && height <= 76;
  }

  return false;
}


bool HairColourValid(Passport const passport)
{
  string const colour_str = passport.at("hcl");

  if (colour_str[0] != '#' || colour_str.size() != 7)
  {
    return false;
  }

  for (unsigned i = 1; i < colour_str.size(); i++)
  {
    if (hex_chars.find(colour_str[i]) == string::npos)
    {
      return false;
    }
  }

  return true;
}


bool EyeColourValid(Passport const passport)
{
  return valid_eyecolours.find(passport.at("ecl")) != valid_eyecolours.end();
}


bool PassportIDValid(Passport const passport)
{
  string const id_str = passport.at("pid");

  if (id_str.size() != 9)
  {
    return false;
  }

  for (char const c : id_str)
  {
    if (num_chars.find(c) == string::npos)
    {
      return false;
    }
  }

  return true;
}


bool IsValid(Passport const passport, bool const do_detailed_check)
{
  if (do_detailed_check)
  {
    return HasRequiredFields(passport) &&
           IntegerValueBetween(passport, "byr", 1920, 2002) &&
           IntegerValueBetween(passport, "iyr", 2010, 2020) &&
           IntegerValueBetween(passport, "eyr", 2020, 2030) &&
           HeightValid(passport) &&
           HairColourValid(passport) &&
           EyeColourValid(passport) &&
           PassportIDValid(passport);
  }
  else
  {
    return HasRequiredFields(passport);
  }
}


int main(int argc, char **argv)
{
  auto args = docopt::docopt(USAGE, {argv + 1, argv + argc}, true);

  string const path = args["<path>"].asString();
  bool const do_detailed_check = args["--detailed-check"].asBool();

  int count = 0;

  for (Passport const passport : PassportList(path))
  {
    if (IsValid(passport, do_detailed_check))
    {
      cout << passport.at("eyr") << endl;
      count++;
    }
  }

  cout << count << endl;

  return 0;
}
