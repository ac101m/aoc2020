#include "docopt/docopt.h"
#include "stringutil.hpp"
#include "loadlines.hpp"
#include "intutil.hpp"

#include <string>
#include <vector>
#include <map>
#include <iostream>


using namespace std;


static const char USAGE[] =
R"(Docking decoder emulator v1.1

Supported decoder chip versions:
 - 1 (version with value masking)
 - 2 (version with address masking)

Usage:
  a.out [options] <path>
  a.out (-h | --help)

Options:
  -h --help   Print this help message.
  -v --version <version>  Version of the decoder chip to emulate. [default: 1]
)";


enum Opcode {
  NOP,
  MASK,
  MEM
};


enum Version {
  V_01,
  V_02
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
  unsigned largest_floating_bit_count = 0;

  uint64_t mask_set = 0;
  uint64_t mask_clear = 0;

  uint64_t address_mask = 0;

  map<uint64_t, uint64_t> mem;

public:
  Executor (unsigned const address_space_size)
  {
    address_mask = ((uint64_t)0x01 << address_space_size) - 1;
  }

  uint64_t Read(uint64_t const address) const
  {
    if (mem.find(address) != mem.end())
    {
      return mem.at(address);
    }

    return 0;
  }

  void Write(uint64_t const value, uint64_t address, uint64_t floating_mask = 0)
  {
    address &= address_mask;
    floating_mask &= address_mask;

    if (floating_mask != 0)
    {
      uint64_t const new_floating_mask = floating_mask & (floating_mask - 1);
      uint64_t const floating_lsb_mask = new_floating_mask ^ floating_mask;

      uint64_t const addr_clear = address & ~floating_lsb_mask;
      uint64_t const addr_set = address | floating_lsb_mask;

      Write(value, addr_clear, new_floating_mask);
      Write(value, addr_set, new_floating_mask);
    }
    else
    {
      mem[address] = value;
    }
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

  void Execute(Instruction const instr, Version const version = V_01)
  {
    switch (instr.opcode)
    {
      case MASK:
        mask_set = instr.mask_set;
        mask_clear = instr.mask_clear;
        break;

      case MEM:
        switch (version)
        {
          case V_01:
            Write((instr.mem_value | mask_set) & ~mask_clear, instr.mem_address);
            break;

          case V_02:
            Write(instr.mem_value, instr.mem_address | mask_set, ~(mask_clear | mask_set));
            break;

          default:
            cout << "Invalid version code." << endl;
            exit(1);
        }
        break;

      case NOP:
        cerr << "Unexepcted NOP in program." << endl;
        exit(1);

      default:
        cerr << "Unrecognised opcode in program." << endl;
        exit(1);
    }
  }

  void Execute(Program const program, Version const version = V_01)
  {
    for (auto const& instruction : program)
    {
      Execute(instruction, version);
    }
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
  string const version = args["--version"].asString();

  Program const program = LoadProgram(path);
  Executor executor(36);

  if (version == "1")
  {
    executor.Execute(program, V_01);
  }
  else if (version == "2")
  {
    executor.Execute(program, V_02);
  }
  else
  {
    cerr << version << " is not a valid decoder chip version!" << endl;
    exit(1);
  }

  cout << executor.MemoryChecksum() << endl;

  return 0;
}
