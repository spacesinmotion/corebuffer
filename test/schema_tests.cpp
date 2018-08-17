#define CATCH_CONFIG_FAST_COMPILE
#include "catch2/catch.hpp"

#include <sstream>
#include "schema.h"

using namespace CoreBuffer;

TEST_CASE("Schema output test", "[output]")
{
  SECTION("compare operations")
  {
    CHECK(EnumEntry() == EnumEntry());
    CHECK_FALSE(EnumEntry() != EnumEntry());

    CHECK(Enum() == Enum());
    CHECK_FALSE(Enum() != Enum());

    CHECK(Member() == Member());
    CHECK_FALSE(Member() != Member());

    CHECK(Table() == Table());
    CHECK_FALSE(Table() != Table());

    CHECK(Package() == Package());
    CHECK_FALSE(Package() != Package());

    CHECK(Type() == Type());
    CHECK_FALSE(Type() != Type());
  }

  SECTION("reading whats written")
  {
    Package p("Scope", "0.1", "P1");

    Enum e1;
    e1.entries.emplace_back("x", 42);
    e1.entries.emplace_back("half", 21);
    p.types.emplace_back("E1", e1);

    Table t1;
    t1.member.emplace_back("e", "E1");
    t1.member.emplace_back("x", "std::uint32_t");
    p.types.emplace_back("T1", t1);

    p.types.emplace_back("base_type", BaseType(true));

    Union u;
    u.tables.emplace_back("X1");
    u.tables.emplace_back("X2");
    p.types.emplace_back("U1", u);

    Package cIn;
    CHECK_FALSE(p == cIn);

    std::stringstream sOut;
    Package_io().WritePackage(sOut, p);

    const auto buffer = sOut.str();
    std::stringstream sIn(buffer);

    Package_io().ReadPackage(sIn, cIn);

    CHECK(p == cIn);
    CHECK_FALSE(p != cIn);
  }

  SECTION("Reading fails with wrong data")
  {
    Package r;

    std::istringstream s1("FALSE");
    CHECK_FALSE(Package_io().ReadPackage(s1, r));

    std::istringstream s2("COREfails");
    CHECK_FALSE(Package_io().ReadPackage(s2, r));
  }
}
