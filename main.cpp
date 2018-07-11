#include <iostream>

#include "cppoutput.h"
#include "parser.h"

using namespace std;

int main()
{
  const auto test = R"(

  package SubC.scope;
  version "0.1.203";
  root_type YYY;

  table XXX {
    bill:int = 1;
    aul:bool = false;
    kaul:float;
    baul:[float];
  }

  table YYY {
    still:weak XXX;
    sub:[YYY];
  }

  enum Access { Private, Public = 3, Protected }
)";

  Package p;
  const auto parsed = Parser(test, p).parse();
  if (parsed)
  {
    std::cout << "    path: " << p.path << std::endl;
    std::cout << " version: " << p.version << std::endl;

    for (const auto &e : p.enums)
    {
      std::cout << ". enum " << e.name << std::endl;
      for (const auto &v : e.entries)
        std::cout << ".. " << v.first << " = " << v.second << std::endl;
    }
    for (const auto &t : p.tables)
    {
      std::cout << ". table " << t.name << std::endl;
      for (const auto &m : t.member)
      {
        std::cout << ".. " << m.name << ":" << (m.isVector ? "[" : "") << m.type << (m.isVector ? "]" : "");
        if (!m.defaultValue.empty())
          std::cout << " = " << m.defaultValue;

        std::cout << std::endl;
      }
    }
  }

  cout << (parsed ? "yes" : "no") << endl;

  if (parsed)
    WriteCppCode(cout, p);

  return 0;
}
