#include "docopt/docopt.h"
#include "loadlines.hpp"

#include <string>
#include <sstream>
#include <iostream>


using namespace std;


static const char USAGE[] =
R"(Gamboy bootloop fixarroo-majig

modes:
 - execute (run the program)
 - repair (fix the program)

Usage:
  a.out [options] <path>
  a.out (-h | --help)

Options:
  -h --help         Print this help message.
  -m --mode <mode>  Select fix or run. [default: run]
)";


typedef enum {
  NOP,
  JMP,
  ACC
} Opcode;


typedef pair<Opcode, int> Instruction;


Opcode GetOpcode(string const str)
{
  if (str == "nop")
  {
    return NOP;
  }
  else if (str == "jmp")
  {
    return JMP;
  }
  else if (str == "acc")
  {
    return ACC;
  }
  else
  {
    cerr << "Invalid opcode \'" << str << '\'' << endl;
    exit(1);
  }
}


Instruction GetInstruction(string const line)
{
  string opstr;
  char plus_minus;
  int value;

  stringstream ss(line);

  ss >> opstr >> plus_minus >> value;

  if (plus_minus != '-' && plus_minus != '+')
  {
    cerr << "Error, expected +/-, found '" << plus_minus << "'" << endl;
    exit(1);
  }

  if (plus_minus == '-')
  {
    value = ~value + 1;
  }

  return Instruction(GetOpcode(opstr), value);
}


pair<int, bool> Execute(vector<Instruction> const program)
{
  vector<bool> executed(program.size(), false);

  int program_counter = 0;
  int accumulator = 0;
  bool graceful_exit = false;

  while (program_counter < program.size() && executed.at(program_counter) == false)
  {
    executed.at(program_counter) = true;

    Instruction const instr = program.at(program_counter);

    Opcode const opcode = instr.first;
    int const argument = instr.second;

    switch (opcode)
    {
      case NOP:
        program_counter++;
        break;

      case JMP:
        program_counter += argument;
        break;

      case ACC:
        accumulator += argument;
        program_counter++;
        break;

      default:
        cerr << "Unrecognised opcode" << endl;
        exit(1);
    }
  }

  return make_pair(accumulator, program_counter == program.size());
}


int main(int argc, char **argv)
{
  auto args = docopt::docopt(USAGE, {argv + 1, argv + argc}, true);

  string const path = args["<path>"].asString();
  string const mode = args["--mode"].asString();

  vector<Instruction> program;

  for (string const line : LoadLinesFromFile(path))
  {
    program.push_back(GetInstruction(line));
  }

  if (mode == "run")
  {
    cout << Execute(program).first << endl;
  }
  else if (mode == "fix")
  {
    for (unsigned i = 0; i < program.size(); i++)
    {
      Instruction const instr = program.at(i);

      Opcode const opcode = instr.first;
      int const argument = instr.second;

      if (opcode == NOP || opcode == JMP)
      {
        vector<Instruction> mutated_program = program;

        mutated_program.at(i).first = (opcode == NOP) ? JMP : NOP;

        pair<int, bool> result = Execute(mutated_program);

        if (result.second)
        {
          cout << result.first << endl;
        }
      }
    }
  }
  else
  {
    cerr << mode << " is not a valid mode!" << endl;
    exit(1);
  }

  return 0;
}
