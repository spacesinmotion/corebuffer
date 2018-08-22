#define CATCH_CONFIG_FAST_COMPILE
#include "catch2/catch.hpp"

#include "enumtypes.h"

#include <sstream>

using namespace Scope;

TEST_CASE("EnumTypes test", "[output, enum]")
{
  SECTION("defined values")
  {
    CHECK(0 == int(EnumTypes::alpha));
    CHECK(4 == int(EnumTypes::beta));
    CHECK(5 == int(EnumTypes::gamma));
    CHECK(42 == int(EnumTypes::delta));
  }

  SECTION("defined value list")
  {
    auto definedValues = EnumTypesValues();

    REQUIRE(definedValues.size() == 4);

    CHECK(EnumTypes::alpha == definedValues[0]);
    CHECK(EnumTypes::beta == definedValues[1]);
    CHECK(EnumTypes::gamma == definedValues[2]);
    CHECK(EnumTypes::delta == definedValues[3]);
  }

  SECTION("defined value names")
  {
    CHECK(ValueName(EnumTypes::alpha) == std::string("alpha"));
    CHECK(ValueName(EnumTypes::beta) == std::string("beta"));
    CHECK(ValueName(EnumTypes::gamma) == std::string("gamma"));
    CHECK(ValueName(EnumTypes::delta) == std::string("delta"));
  }

  SECTION("defined value names - error") { CHECK(ValueName(static_cast<EnumTypes>(12)) == std::string("<error>")); }

  SECTION("default values")
  {
    Dummy d;
    CHECK(EnumTypes::alpha == d.en1);
    CHECK(EnumTypes::delta == d.en2);
  }

  SECTION("Special data size enums")
  {
    SECTION("enums have minimum sizes")
    {
      CHECK(sizeof(EnumTypes) == sizeof(std::int8_t));
      CHECK(sizeof(Enum16) == sizeof(std::int16_t));
      CHECK(sizeof(Enum32) == sizeof(std::int32_t));
      CHECK(sizeof(Enum64) == sizeof(std::int64_t));
      CHECK(sizeof(EnumMinus8) == sizeof(std::int8_t));
    }

    SECTION("defined value list")
    {
      CHECK(Enum16::a == Enum16Values()[0]);
      CHECK(Enum32::a == Enum32Values()[0]);
      CHECK(Enum64::a == Enum64Values()[0]);
      CHECK(EnumMinus8::a == EnumMinus8Values()[0]);
    }

    SECTION("defined value names")
    {
      CHECK(ValueName(Enum16::a) == std::string("a"));
      CHECK(ValueName(Enum32::a) == std::string("a"));
      CHECK(ValueName(Enum64::a) == std::string("a"));
      CHECK(ValueName(EnumMinus8::a) == std::string("a"));
    }

    SECTION("defined value names - error")
    {
      CHECK(ValueName(static_cast<Enum16>(42)) == std::string("<error>"));
      CHECK(ValueName(static_cast<Enum32>(42)) == std::string("<error>"));
      CHECK(ValueName(static_cast<Enum64>(42)) == std::string("<error>"));
      CHECK(ValueName(static_cast<EnumMinus8>(42)) == std::string("<error>"));
    }
  }

  SECTION("reading whats written")
  {
    Dummy dOut;
    dOut.en1 = EnumTypes::beta;
    dOut.en2 = EnumTypes::gamma;
    dOut.en3.emplace_back(EnumTypes::beta);
    dOut.en3.emplace_back(EnumTypes::gamma);

    std::stringstream sOut;
    Dummy_io().WriteDummy(sOut, dOut);

    const auto buffer = sOut.str();
    std::stringstream sIn(buffer);

    Dummy dIn;
    Dummy_io().ReadDummy(sIn, dIn);

    CHECK(EnumTypes::beta == dIn.en1);
    CHECK(EnumTypes::gamma == dIn.en2);

    REQUIRE(dIn.en3.size() == 2);
    CHECK(dIn.en3[0] == EnumTypes::beta);
    CHECK(dIn.en3[1] == EnumTypes::gamma);
  }

  SECTION("Compare operations")
  {
    Dummy d1;
    Dummy d2;

    CHECK(d1 == d2);
    CHECK_FALSE(d1 != d2);

    d1.en1 = EnumTypes::beta;
    d2.en1 = EnumTypes::gamma;

    CHECK_FALSE(d1 == d2);
    CHECK(d1 != d2);
  }

  SECTION("Reading fails with wrong data")
  {
    Dummy r;

    std::stringstream s1("FALSE");
    CHECK_FALSE(Dummy_io().ReadDummy(s1, r));

    std::stringstream s2("COREfails");
    CHECK_FALSE(Dummy_io().ReadDummy(s2, r));
  }

  SECTION("vector algorithm enum")
  {
    Dummy p;
    p.en3.emplace_back(EnumTypes::alpha);
    p.en3.emplace_back(EnumTypes::delta);
    p.en3.emplace_back(EnumTypes::beta);

    SECTION("generate")
    {
      int x = 1;
      p.generate_en3([&x]() { return EnumTypesValues()[x++]; });
      CHECK(p.en3[0] == EnumTypes::beta);
      CHECK(p.en3[1] == EnumTypes::gamma);
      CHECK(p.en3[2] == EnumTypes::delta);
    }

    SECTION("fill")
    {
      p.fill_en3(EnumTypes::gamma);

      CHECK(p.en3[0] == EnumTypes::gamma);
      CHECK(p.en3[1] == EnumTypes::gamma);
      CHECK(p.en3[2] == EnumTypes::gamma);
    }

    SECTION("remove")
    {
      auto e = p.remove_en3(EnumTypes::delta);

      CHECK(e == p.en3.end() - 1);
      CHECK(3 == p.en3.size());
      CHECK(p.en3[0] == EnumTypes::alpha);
      CHECK(p.en3[1] == EnumTypes::beta);
    }

    SECTION("remove_if")
    {
      auto e = p.remove_en3_if([](EnumTypes i) { return i == EnumTypes::delta; });

      CHECK(e == p.en3.end() - 1);
      CHECK(3 == p.en3.size());
      CHECK(p.en3[0] == EnumTypes::alpha);
      CHECK(p.en3[1] == EnumTypes::beta);
    }

    SECTION("erase")
    {
      p.erase_en3(EnumTypes::delta);

      CHECK(2 == p.en3.size());
      CHECK(p.en3[0] == EnumTypes::alpha);
      CHECK(p.en3[1] == EnumTypes::beta);
    }

    SECTION("erase_if")
    {
      p.erase_en3_if([](EnumTypes i) { return i == EnumTypes::delta; });

      CHECK(2 == p.en3.size());
      CHECK(p.en3[0] == EnumTypes::alpha);
      CHECK(p.en3[1] == EnumTypes::beta);
    }

    SECTION("reverse")
    {
      p.reverse_en3();

      REQUIRE(3 == p.en3.size());
      CHECK(p.en3[0] == EnumTypes::beta);
      CHECK(p.en3[1] == EnumTypes::delta);
      CHECK(p.en3[2] == EnumTypes::alpha);
    }

    SECTION("rotate")
    {
      p.rotate_en3(p.en3.begin() + 1);

      REQUIRE(3 == p.en3.size());
      CHECK(p.en3[0] == EnumTypes::delta);
      CHECK(p.en3[1] == EnumTypes::beta);
      CHECK(p.en3[2] == EnumTypes::alpha);
    }

    SECTION("sort")
    {
      p.sort_en3();

      REQUIRE(3 == p.en3.size());
      CHECK(p.en3[0] == EnumTypes::alpha);
      CHECK(p.en3[1] == EnumTypes::beta);
      CHECK(p.en3[2] == EnumTypes::delta);
    }

    SECTION("sort with comparator")
    {
      p.sort_en3([](EnumTypes l, EnumTypes r) { return l > r; });

      REQUIRE(3 == p.en3.size());
      CHECK(p.en3[0] == EnumTypes::delta);
      CHECK(p.en3[1] == EnumTypes::beta);
      CHECK(p.en3[2] == EnumTypes::alpha);
    }

    SECTION("any of")
    {
      CHECK(p.any_of_en3([](EnumTypes x) { return x == EnumTypes::delta; }));
      CHECK_FALSE(p.any_of_en3([](EnumTypes x) { return x == EnumTypes::gamma; }));
    }

    SECTION("all of")
    {
      CHECK_FALSE(p.all_of_en3([](EnumTypes x) { return x == EnumTypes::delta; }));
      CHECK(p.any_of_en3([](EnumTypes x) { return int(x) >= 0; }));
    }

    SECTION("none of")
    {
      CHECK(p.none_of_en3([](EnumTypes x) { return x == EnumTypes::gamma; }));
      CHECK_FALSE(p.none_of_en3([](EnumTypes x) { return x == EnumTypes::delta; }));
    }

    SECTION("any of is")
    {
      CHECK(p.any_of_en3_is(EnumTypes::delta));
      CHECK_FALSE(p.any_of_en3_is(EnumTypes::gamma));
    }

    SECTION("all of are")
    {
      CHECK_FALSE(p.all_of_en3_are(EnumTypes::delta));
      p.fill_en3(EnumTypes::gamma);
      CHECK(p.all_of_en3_are(EnumTypes::gamma));
    }

    SECTION("none of value")
    {
      CHECK_FALSE(p.none_of_en3_is(EnumTypes::delta));
      CHECK(p.none_of_en3_is(EnumTypes::gamma));
    }

    SECTION("for each")
    {
      std::vector<EnumTypes> x;
      p.for_each_en3([&x](EnumTypes i) { x.emplace_back(i); });

      CHECK(x[0] == EnumTypes::alpha);
      CHECK(x[1] == EnumTypes::delta);
      CHECK(x[2] == EnumTypes::beta);
    }

    SECTION("find")
    {
      CHECK(p.find_in_en3(EnumTypes::delta) == p.en3.begin() + 1);
      CHECK(p.find_in_en3(EnumTypes::gamma) == p.en3.end());
    }

    SECTION("find if")
    {
      CHECK(p.find_in_en3_if([](EnumTypes x) { return x == EnumTypes::delta; }) == p.en3.begin() + 1);
      CHECK(p.find_in_en3_if([](EnumTypes x) { return x == EnumTypes::gamma; }) == p.en3.end());
    }

    SECTION("count")
    {
      CHECK(p.count_in_en3(EnumTypes::delta) == 1);
      CHECK(p.count_in_en3(EnumTypes::gamma) == 0);
    }

    SECTION("count if")
    {
      CHECK(p.count_in_en3_if([](EnumTypes x) { return x == EnumTypes::delta; }) == 1);
      CHECK(p.count_in_en3_if([](EnumTypes x) { return x == EnumTypes::gamma; }) == 0);
    }
  }
}
