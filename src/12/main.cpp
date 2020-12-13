#include "docopt/docopt.h"
#include "loadlines.hpp"

#include <vector>
#include <string>
#include <sstream>


using namespace std;


static const char USAGE[] =
R"([TODO - DESCRIPTION]

modes:
 - ship (Relative to ships current position)
 - waypoint (Relative to waypoint)

Usage:
  a.out [options] <path>
  a.out (-h | --help)

Options:
  -h --help   Print this help message.
  -m --mode <mode>  Mode to use the tool in. [default: ship]
)";


struct Vec2 {
  int x = 0;
  int y = 0;

  Vec2(int const x, int const y) :
    x(x), y(y)
  {}

  Vec2 operator*(int const multiplier) const
  {
    return Vec2(x * multiplier, y * multiplier);
  }

  Vec2 operator+(Vec2 const& other) const
  {
    return Vec2(x + other.x, y + other.y);
  }

  Vec2 operator-(Vec2 const& other) const
  {
    return Vec2(x - other.x, y - other.y);
  }
};


typedef enum {
  NORTH = 0,
  EAST = 1,
  SOUTH = 2,
  WEST = 3
} Direction;


Vec2 DirectionToVec2(Direction const dir)
{
  switch (dir)
  {
    case NORTH:
      return Vec2(0, 1);

    case SOUTH:
      return Vec2(0, -1);

    case EAST:
      return Vec2(1, 0);

    case WEST:
      return Vec2(-1, 0);

    default:
      cerr << "Invalid direction enum " << dir << endl;
      exit(1);
  }
}


Vec2 RotateVec2(Vec2 vec, int angle)
{
  if ((angle = angle % 360) < 0)
  {
    angle += 360;
  }

  while (angle != 0)
  {
    vec = Vec2(vec.y, 0 - vec.x);
    angle -= 90;
  }

  return vec;
}


struct Instruction {
  char opcode;
  int operand;

  Instruction(string const line)
  {
    stringstream(line) >> opcode >> operand;
  }
};


struct Ferry {
  Vec2 heading;
  Vec2 position;
  Vec2 waypoint;

  Ferry(Direction const heading, Vec2 const position, Vec2 const waypoint) :
    heading(DirectionToVec2(heading)), position(position), waypoint(waypoint)
  {}

  void UpdateShipRelative(Instruction const instruction)
  {
    int const operand = instruction.operand;

    switch (instruction.opcode)
    {
      case 'N':
        position.y += operand;
        break;

      case 'S':
        position.y -= operand;
        break;

      case 'E':
        position.x += operand;
        break;

      case 'W':
        position.x -= operand;
        break;

      case 'L':
        heading = RotateVec2(heading, 0 - operand);
        break;

      case 'R':
        heading = RotateVec2(heading, operand);
        break;

      case 'F':
        position = position + (waypoint * operand);
        break;

      default:
        cerr << "Invalid operation '" << instruction.opcode << '\'' << endl;
        exit(1);
    }
  }

  void UpdateWaypointRelative(Instruction const instruction)
  {
    int const operand = instruction.operand;

    switch (instruction.opcode)
    {
      case 'N':
        waypoint.y += operand;
        break;

      case 'S':
        waypoint.y -= operand;
        break;

      case 'E':
        waypoint.x += operand;
        break;

      case 'W':
        waypoint.x -= operand;
        break;

      case 'L':
        waypoint = RotateVec2(waypoint, 0 - operand);
        break;

      case 'R':
        waypoint = RotateVec2(waypoint, operand);
        break;

      case 'F':
        position = position + (waypoint * operand);
        break;

      default:
        cerr << "Invalid operation '" << instruction.opcode << '\'' << endl;
        exit(1);
    }
  }
};


int main(int argc, char **argv)
{
  auto args = docopt::docopt(USAGE, {argv + 1, argv + argc}, true);

  string const path = args["<path>"].asString();
  string const mode = args["--mode"].asString();

  Ferry ferry(EAST, Vec2(0, 0), Vec2(10, 1));

  if (mode == "ship")
  {
    for (auto const line : LoadLinesFromFile(path))
    {
      ferry.UpdateShipRelative(Instruction(line));
    }
  }
  else if (mode == "waypoint")
  {
    for (auto const line : LoadLinesFromFile(path))
    {
      ferry.UpdateWaypointRelative(Instruction(line));
    }
  }
  else
  {
    cerr << mode << " is not a valid mode!" << endl;
    exit(1);
  }

  cout << abs(ferry.position.x) + abs(ferry.position.y) << endl;

  return 0;
}
