#include "docopt/docopt.h"
#include "loadlines.hpp"
#include "stringutil.hpp"

#include <vector>
#include <string>
#include <map>
#include <set>
#include <iostream>


using namespace std;


static const char USAGE[] =
R"(Bag rule demystifier v1.0

modes:
 - options (number of ways you can include a bag of the given bag type)
 - count (number of bags you must carry within the given bag type)

Usage:
  a.out [options] <path>
  a.out (-h | --help)

Options:
  -h --help         Print this help message.
  -t --type <name>  Type of bag we are interested in. [default: shiny gold]
  -m --mode <name>  Mode to use. [default: options]
)";


typedef pair<string, int> BagSpec;
typedef map<string, vector<BagSpec>> RuleMap;


BagSpec NewBagSpec(string const str)
{
  stringstream ss(trim(str));

  int count;
  string adjective, colour;
  ss >> count >> adjective >> colour;

  string const name = adjective + " " + colour;

  return make_pair(name, count);
}


RuleMap LoadRuleMap(string const path)
{
  RuleMap rules;

  string const delimiter = " bags contain ";

  for (auto const line : LoadLinesFromFile(path))
  {
    string const bag_name = trim(line.substr(0, line.find(delimiter)));
    string const bag_contains = trim(line.substr(line.find(delimiter) + delimiter.size(), line.size()));

    rules.insert(make_pair(bag_name, vector<BagSpec>()));

    for (auto spec_str : Split(bag_contains, ','))
    {
      if (trim(spec_str) != "no other bags.")
      {
        rules.at(bag_name).push_back(NewBagSpec(trim(spec_str)));
      }
    }
  }

  return rules;
}


set<string> ContainableBags(
  RuleMap const& rules,
  string const bag_type,
  map<string, set<string>>& containables_map)
{
  if (containables_map.find(bag_type) != containables_map.end())
  {
    return containables_map.at(bag_type);
  }

  set<string> containable;

  for (auto const bag_spec : rules.at(bag_type))
  {
    string const interior_bag_type = bag_spec.first;

    containable.insert(interior_bag_type);

    for (auto const containable_name : ContainableBags(rules, interior_bag_type, containables_map))
    {
      containable.insert(containable_name);
    }
  }

  containables_map.insert(make_pair(bag_type, containable));

  return containable;
}


set<string> ContainableBags(
  RuleMap const& rules,
  string const bag_type)
{
  map<string, set<string>> containables_map;
  return ContainableBags(rules, bag_type, containables_map);
}


int GetBagOptionCount(RuleMap const& rules, string const bag_type)
{
  int count = 0;

  map<string, set<string>> containables_map;

  for (auto const item : rules)
  {
    set<string> const containable = ContainableBags(rules, item.first, containables_map);

    if (containable.find(bag_type) != containable.end())
    {
      count++;
    }
  }

  return count;
}


uint64_t GetBagCount(
  RuleMap const& rules,
  string const bag_type,
  map<string, uint64_t>& counts_cache)
{
  if (counts_cache.find(bag_type) != counts_cache.end())
  {
    return counts_cache.at(bag_type);
  }

  uint64_t count = 0;

  for (BagSpec const spec : rules.at(bag_type))
  {
    string const interior_bag_name = spec.first;
    int const interior_bag_count = spec.second;

    count += interior_bag_count;
    count += interior_bag_count * GetBagCount(rules, interior_bag_name, counts_cache);
  }

  counts_cache.insert(make_pair(bag_type, count));

  return count;
}


uint64_t GetBagCount(
  RuleMap const& rules,
  string const bag_type)
{
  map<string, uint64_t> counts_cache;
  return GetBagCount(rules, bag_type, counts_cache);
}


int main(int argc, char **argv)
{
  auto args = docopt::docopt(USAGE, {argv + 1, argv + argc}, true);

  string const path = args["<path>"].asString();
  string const mode = args["--mode"].asString();
  string const bag_type = args["--type"].asString();

  RuleMap const rules = LoadRuleMap(path);

  if (mode == "options")
  {
    cout << GetBagOptionCount(rules, bag_type) << endl;
  }
  else if (mode == "count")
  {
    cout << GetBagCount(rules, bag_type) << endl;
  }
  else
  {
    cerr << "No such mode '" << mode << "'" << endl;
    return 1;
  }

  return 0;
}
