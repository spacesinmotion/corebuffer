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
  }

  SECTION("reading whats written")
  {
    Enum e1("E1");
    e1.entries.emplace_back("x", 42);
    e1.entries.emplace_back("half", 21);

    Table t1("T1");
    t1.member.emplace_back("e", "E1");
    t1.member.emplace_back("x", "std::uint32_t");

    Package p("Scope", "0.1", "T1");
    p.tables.emplace_back(t1);
    p.baseTypes.emplace_back("base_type");
    p.enums.emplace_back(e1);

    Package cIn;
    CHECK_FALSE(p == cIn);

    std::stringstream sOut;
    Package_io().WritePackage(sOut, p);

    const auto buffer = sOut.str();
    std::stringstream sIn(buffer);

    Package_io().ReadPackage(sIn, cIn);

    CHECK(p == cIn);
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
