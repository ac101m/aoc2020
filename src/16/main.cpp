#include "docopt/docopt.h"
#include "stringutil.hpp"

#include <map>
#include <vector>
#include <iostream>
#include <fstream>


using namespace std;


static const char USAGE[] =
R"(Ticket translator

Usage:
  a.out [options] <path>
  a.out (-h | --help)

Options:
  -h --help   Print this help message.
)";


struct Range
{
  int min = 0;
  int max = 0;

  Range(int const min, int const max) :
    min(min), max(max)
  {}

  Range(string const token)
  {
    char c;
    stringstream(token) >> min >> c >> max;
  }

  bool Contains(int const n) const
  {
    return n <= max && n >= min;
  }
};


bool RangesContain(vector<Range> const ranges, int const value)
{
  for (Range const range : ranges)
  {
    if (range.Contains(value))
    {
      return true;
    }
  }

  return false;
}


struct Ticket
{
  vector<int> field_values;

  Ticket(string const line)
  {
    for (string const token : Split(line, ','))
    {
      field_values.push_back(0);
      stringstream(trim(token)) >> field_values.back();
    }
  }

  int InvalidFieldChecksum(map<string, vector<Range>> const field_constraints) const
  {
    int checksum = 0;

    for (int const field_value : field_values)
    {
      bool valid = false;

      for (auto const [name, ranges] : field_constraints)
      {
        if (RangesContain(ranges, field_value))
        {
          valid = true;
          break;
        }
      }

      if (!valid)
      {
        checksum += field_value;
      }
    }

    return checksum;
  }
};


map<string, vector<Range>> LoadFieldConstraints(istream& is)
{
  string line;

  map<string, vector<Range>> field_constraints;

  while (getline(is, line) && line != "")
  {
    vector<string> tokens = Split(line, ':');

    string const name = trim(tokens.at(0));
    vector<string> const range_tokens = Split(trim(tokens.at(1)), ' ');

    field_constraints[name].push_back(Range(trim(range_tokens.at(0))));
    field_constraints[name].push_back(Range(trim(range_tokens.at(2))));
  }

  return field_constraints;
}


Ticket LoadMyTicket(istream& is)
{
  string line;

  getline(is, line);

  if (line != "your ticket:")
  {
    cerr << "Malformed input, expected 'your ticket:'." << endl;
    exit(1);
  }

  getline(is, line);

  Ticket ticket(line);

  getline(is, line);

  if (line != "")
  {
    cerr << "Malformed input, expected empty line." << endl;
    exit(1);
  }

  return ticket;
}


vector<Ticket> LoadNearbyTickets(istream& is)
{
  string line;

  getline(is, line);

  if (line != "nearby tickets:")
  {
    cerr << "Malformed input, expected 'nearby tickets:'." << endl;
    exit(1);
  }

  vector<Ticket> tickets;

  while (getline(is, line))
  {
    tickets.push_back(Ticket(line));
  }

  return tickets;
}


int main(int argc, char **argv)
{
  auto args = docopt::docopt(USAGE, {argv + 1, argv + argc}, true);

  string const path = args["<path>"].asString();

  ifstream ifs(path);

  if (!ifs)
  {
    std::cerr << "No such file '" << path << "'" << std::endl;
    exit(1);
  }

  auto const field_constraints = LoadFieldConstraints(ifs);
  auto const my_ticket = LoadMyTicket(ifs);
  auto const nearby_tickets = LoadNearbyTickets(ifs);

  ifs.close();

  vector<Ticket> valid_tickets;
  int checksum = 0;

  for (Ticket const ticket : nearby_tickets)
  {
    int invalid_field_checksum = ticket.InvalidFieldChecksum(field_constraints);

    if (invalid_field_checksum == 0)
    {
      valid_tickets.push_back(ticket);
    }

    checksum += invalid_field_checksum;
  }

  cout << "Total tickets: " << nearby_tickets.size() << endl;
  cout << "Valid tickets: " << valid_tickets.size() << endl;
  cout << "Invalid field checksum: " << checksum << endl;

  return 0;
}
