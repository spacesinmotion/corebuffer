#include <fstream>
#include <iostream>
#include <sstream>

#include "cppoutput.h"
#include "parser.h"

using namespace std;

int main(int argc, char *argv[])
{
  if (argc < 3)
    return 1;

  std::ifstream t(argv[1]);
  if (!t)
    return 2;

  std::string source((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

  Package p;
  if (!Parser(source, p).parse())
    return 3;

  std::ofstream o(argv[2]);
  if (!o)
    return 4;

  WriteCppCode(o, p);

  return 0;
}
