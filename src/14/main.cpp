#include "docopt/docopt.h"
#include "stringutil.hpp"
#include "loadlines.hpp"

#include <string>
#include <vector>
#include <map>
#include <iostream>


using namespace std;


static const char USAGE[] =
R"(Docking decoder emulator v1.0

Usage:
  a.out [options] <path>
  a.out (-h | --help)

Options:
  -h --help   Print this help message.
)";


enum Opcode {
  NOP,
  MASK,
  MEM
};


struct Instruction {
public:
  Opcode opcode = NOP;

  uint64_t mem_address = 0;
  uint64_t mem_value = 0;

  uint64_t mask_set = 0;
  uint64_t mask_clear = 0;


private:
  void MaskInstruction(string const line)
  {
    vector<string> const tokens = Split(line, '=');

    string const lhs = trim(tokens.at(0));
    string const rhs = trim(tokens.at(1));

    opcode = MASK;
    mask_set = 0;
    mask_clear = 0;

    for (char const c : rhs)
    {
      mask_set = mask_set << 1;
      mask_clear = mask_clear << 1;

      switch (c)
      {
        case 'X':
          break;

        case '0':
          mask_clear |= 0x01;
          break;

        case '1':
          mask_set |= 0x01;
          break;

        default:
          cerr << "Unexepcted mask character '" << c << "'." << endl;
          exit(1);
      }
    }
  }

  void MemInstruction(string const line)
  {
    vector<string> const tokens = Split(line, '=');

    string const lhs = trim(tokens.at(0));
    string const rhs = trim(tokens.at(1));

    opcode = MEM;

    stringstream(Split(lhs, '[').at(1)) >> mem_address;
    stringstream(rhs) >> mem_value;
  }


public:
  Instruction(string const line)
  {
    if (line.find("mask") == 0)
    {
      MaskInstruction(line);
    }
    else if (line.find("mem") == 0)
    {
      MemInstruction(line);
    }
    else
    {
      cout << "Unrecognised instruction '" << line << '\'' << endl;
      exit(1);
    }
  }
};


typedef vector<Instruction> Program;


class Executor
{
private:
  uint64_t mask_set = 0;
  uint64_t mask_clear = 0;

  map<uint64_t, uint64_t> mem;

public:
  void Execute(Instruction const instr)
  {
    switch (instr.opcode)
    {
      case MASK:
        mask_set = instr.mask_set;
        mask_clear = instr.mask_clear;
        break;

      case MEM:
        Write((instr.mem_value | mask_set) & ~mask_clear, instr.mem_address);
        break;

      case NOP:
        cerr << "Unexepcted NOP in program." << endl;
        exit(1);

      default:
        cerr << "Unrecognised opcode in program." << endl;
        exit(1);
    }
  }

  void Execute(Program const program)
  {
    for (auto const& instruction : program)
    {
      Execute(instruction);
    }
  }

  uint64_t Read(uint64_t const address) const
  {
    if (mem.find(address) != mem.end())
    {
      return mem.at(address);
    }

    return 0;
  }

  void Write(uint64_t const value, uint64_t const address)
  {
    mem[address] = value;
  }

  uint64_t MemoryChecksum() const
  {
    uint64_t sum = 0;

    for (auto const item : mem)
    {
      sum += item.second;
    }

    return sum;
  }
};


Program LoadProgram(string const path)
{
  Program program;

  for (string const line : LoadLinesFromFile(path))
  {
    program.push_back(Instruction(line));
  }

  return program;
}


int main(int argc, char **argv)
{
  auto args = docopt::docopt(USAGE, {argv + 1, argv + argc}, true);

  string const path = args["<path>"].asString();

  Program const program = LoadProgram(path);

  Executor executor;
  executor.Execute(program);

  cout << executor.MemoryChecksum() << endl;

  return 0;
}
