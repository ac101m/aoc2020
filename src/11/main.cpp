#include "docopt/docopt.h"
#include "loadlines.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <memory>


using namespace std;


static const char USAGE[] =
R"(Seat selection simulator.

modes:
 - adjacent (Adjacent seats are counted)
 - visible (Visible seats are counted)

Usage:
  a.out [options] <path>
  a.out (-h | --help)

Options:
  -h --help         Print this help message.
  -m --mode <mode>  Select mode. [default: adjacent]
)";


class SeatLocation {
public:
  bool present = false;
  bool occupied = false;
  int adjacent_count = 0;

  SeatLocation()
  {}

  SeatLocation(char const c) : present(c == 'L')
  {
    if (c != 'L' && c != '.')
    {
      cerr << "Unrecognised seat specification " << c << endl;
      exit(1);
    }
  }
};


typedef vector<vector<SeatLocation>> SeatMap;


void UpdateAdjacency(int const delta, SeatMap& target, int const i, int const j)
{
  int const height = target.size();
  int const width = target.at(0).size();

  for (int k = max(i - 1, 0); k < min(i + 2, height); k++)
  {
    for (int l = max(j - 1, 0); l < min(j + 2, width); l++)
    {
      if (k != i || l != j)
      {
        target.at(k).at(l).adjacent_count += delta;
      }
    }
  }
}


int NextSeat(SeatMap const& source, SeatMap& target, int const i, int const j)
{
  SeatLocation const& source_seat = source.at(i).at(j);
  SeatLocation& target_seat = target.at(i).at(j);

  target_seat.present = source_seat.present;
  target_seat.occupied = source_seat.occupied;
  target_seat.adjacent_count += source_seat.adjacent_count;

  if (!source_seat.occupied && source_seat.adjacent_count == 0)
  {
    target_seat.occupied = true;
    UpdateAdjacency(1, target, i, j);
    return true;
  }

  if (source_seat.occupied && source_seat.adjacent_count >= 4)
  {
    target_seat.occupied = false;
    UpdateAdjacency(-1, target, i, j);
    return true;
  }

  return false;
}


int CountOccupied(SeatMap const& map)
{
  int occupied = 0;

  for (int i = 0; i < map.size(); i++)
  {
    for (int j = 0; j < map.at(i).size(); j++)
    {
      if (map.at(i).at(j).occupied)
      {
        occupied++;
      }
    }
  }

  return occupied;
}


int TickAdjacent(SeatMap& source, SeatMap& target)
{
  int updates = 0;

  for (int i = 0; i < source.size(); i++)
  {
    for (int j = 0; j < source.at(i).size(); j++)
    {
      if (source.at(i).at(j).present)
      {
        if (NextSeat(source, target, i, j))
        {
          updates++;
        }
      }

      source.at(i).at(j).adjacent_count = 0;
    }
  }

  return updates;
}


bool CheckDirection(
  SeatMap const& source,
  int i, int j,
  int const di, int const dj)
{
  int const height = source.size();
  int const width = source.at(0).size();

  bool collision = false;

  i += di;
  j += dj;

  while (i >= 0 && i < height && j >= 0 && j < width)
  {
    if (source.at(i).at(j).present)
    {
      return source.at(i).at(j).occupied;
    }

    i += di;
    j += dj;
  }

  return collision;
}


int VisibleOccupiedSeats(SeatMap const& source, int const i, int const j)
{
  int count = 0;

  for (int di = -1; di <= 1; di++)
  {
    for (int dj = -1; dj <= 1; dj++)
    {
      if (di != 0 || dj != 0)
      {
        if (CheckDirection(source, i, j, di, dj))
        {
          count++;
        }
      }
    }
  }

  return count;
}


bool NextSeatVisible(SeatMap const& source, SeatMap& target, int const i, int const j)
{
  SeatLocation const& source_seat = source.at(i).at(j);
  SeatLocation& target_seat = target.at(i).at(j);

  target_seat.present = source_seat.present;
  target_seat.occupied = source_seat.occupied;

  int const visible_count = VisibleOccupiedSeats(source, i, j);

  if (source_seat.occupied)
  {
    if (visible_count >= 5)
    {
      target_seat.occupied = false;
    }
  }
  else
  {
    if (visible_count == 0)
    {
      target_seat.occupied = true;
    }
  }

  return source_seat.occupied != target_seat.occupied;
}


int TickVisible(SeatMap const& source, SeatMap& target)
{
  int updates = 0;

  for (int i = 0; i < source.size(); i++)
  {
    for (int j = 0; j < source.at(i).size(); j++)
    {
      if (source.at(i).at(j).present)
      {
        if (NextSeatVisible(source, target, i, j))
        {
          updates++;
        }
      }
    }
  }

  return updates;
}


void Print(SeatMap const& map)
{
  for (int i = 0; i < map.size(); i++)
  {
    for (int j = 0; j < map.at(i).size(); j++)
    {
      if (!map.at(i).at(j).present)
      {
        cout << '.';
      }
      else if (map.at(i).at(j).occupied)
      {
        cout << '#';
      }
      else
      {
        cout << 'L';
      }
    }
    cout << endl;
  }
}


int main(int argc, char **argv)
{
  auto args = docopt::docopt(USAGE, {argv + 1, argv + argc}, true);

  string const path = args["<path>"].asString();
  string const mode = args["--mode"].asString();

  auto seat_map_1 = shared_ptr<SeatMap>(new SeatMap());
  auto seat_map_2 = shared_ptr<SeatMap>(new SeatMap());

  for (auto const line : LoadLinesFromFile(path))
  {
    seat_map_1->push_back(vector<SeatLocation>());

    for (auto const c : line)
    {
      vector<SeatLocation>& seat_row = seat_map_1->back();
      seat_row.push_back(SeatLocation(c));
    }
  }

  *seat_map_2 = *seat_map_1;

  if (mode == "adjacent")
  {
    while (TickAdjacent(*seat_map_1, *seat_map_2) > 0)
    {
      auto const tmp = seat_map_1;
      seat_map_1 = seat_map_2;
      seat_map_2 = tmp;
    }
  }
  else if (mode == "visible")
  {
    while (TickVisible(*seat_map_1, *seat_map_2) > 0)
    {
      auto const tmp = seat_map_1;
      seat_map_1 = seat_map_2;
      seat_map_2 = tmp;
    }
  }
  else
  {
    cerr << mode << " is not a valid mode!" << endl;
    exit(1);
  }

  cout << "Result:" << endl;
  Print(*seat_map_2);

  cout << endl << "Occupied seats: ";
  cout << CountOccupied(*seat_map_2) << endl;

  return 0;
}
