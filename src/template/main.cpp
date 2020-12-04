#include "docopt/docopt.h"


using namespace std;


static const char USAGE[] =
R"([TODO - DESCRIPTION]

Usage:
  a.out [options] <path>
  a.out (-h | --help)

Options:
  -h --help   Print this help message.
)";


int main(int argc, char **argv)
{
  auto args = docopt::docopt(USAGE, {argv + 1, argv + argc}, true);

  string const path = args["<path>"].asString();

  // TODO - Solve the problem!

  return 0;
}
