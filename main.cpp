#include <fstream>
#include <iostream>
#include <sstream>

#include "cppoutput.h"
#include "parser.h"
#include "testout.h"

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
    still:unique XXX;
    sub:[shared YYY];
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
  {
    std::fstream f("../CoreData/testout.h", std::fstream::out);
    WriteCppCode(f, p);
    f.close();
  }

  auto y = std::make_shared<SubC::scope::YYY>();
  y->still = std::make_unique<SubC::scope::XXX>();
  y->still->aul = true;
  y->still->bill = 42;
  y->still->kaul = 4.2f;
  y->sub.emplace_back(new SubC::scope::YYY);
  y->sub.emplace_back(new SubC::scope::YYY);
  y->sub.emplace_back(new SubC::scope::YYY);
  y->sub.push_back(y->sub.front());
  y->still->baul = std::vector<float>(20, 1.2f);

  std::string data;
  SubC::scope::YYY_io io;
  {
    std::stringstream s;
    io.WriteYYY(s, *y);

    data = s.str();
  }

  auto yy = std::make_shared<SubC::scope::YYY>();
  {
    std::stringstream s(data);
    io.ReadYYY(s, *yy);
  }

  return 0;
}
