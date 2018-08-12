#include <fstream>
#include <iostream>
#include <sstream>

#include "cppoutput.h"
#include "parser.h"

using namespace std;

void usage(const std::string &msg, const std::string &program)
{
  cerr << msg << endl << endl;
  cerr << "usage:" << endl;
  cerr << "  " << program << " <input.cor> <ouput.h>" << endl;
}

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    usage("missing commad line argument(s).", argv[0]);
    return 1;
  }

  std::ifstream t(argv[1]);
  if (!t)
  {
    usage(string("can not open input '") + argv[1] + "'.", argv[0]);
    return 2;
  }

  std::string source((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

  Package p;
  try
  {
    if (!Parser(source, p).parse())
    {
      usage(string("parsing faild for '") + argv[1] + "'.", argv[0]);
      return 3;
    }
  }
  catch (const ParserError &pe)
  {
    cerr << argv[0] << ":" << pe._state.line << ":" << pe._state.column << ": " << pe.what() << endl;
    return 3;
  }

  std::ofstream o(argv[2]);
  if (!o)
  {
    usage(string("can not open output '") + argv[2] + "'.", argv[0]);
    return 4;
  }

  WriteCppCode(o, p);

  return 0;
}
