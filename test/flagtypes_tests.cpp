
#define CATCH_CONFIG_FAST_COMPILE
#include "catch2/catch.hpp"
#include "flagtypes.h"

#include <sstream>

using namespace FlagScope;

TEST_CASE("FlagTypes test", "[output, flag]")
{
  SECTION("defined values")
  {
    CHECK(1 == int(Flags::alpha));
    CHECK(2 == int(Flags::beta));
    CHECK(4 == int(Flags::gamma));
    CHECK(8 == int(Flags::delta));
  }

  SECTION("defined value list")
  {
    const auto &definedValues = FlagsValues();

    REQUIRE(definedValues.size() == 4);

    CHECK(Flags::alpha == definedValues[0]);
    CHECK(Flags::beta == definedValues[1]);
    CHECK(Flags::gamma == definedValues[2]);
    CHECK(Flags::delta == definedValues[3]);
  }

  SECTION("defined value names")
  {
    CHECK(ValueName(Flags::alpha) == std::string("alpha"));
    CHECK(ValueName(Flags::beta) == std::string("beta"));
    CHECK(ValueName(Flags::gamma) == std::string("gamma"));
    CHECK(ValueName(Flags::delta) == std::string("delta"));
  }

  SECTION("Flag | operations")
  {
    auto flag = (Flags::alpha | Flags::beta);
    CHECK(((1 | 2) == flag.value));
    CHECK(((1 | 2) == (Flags(Flags::alpha) | Flags(Flags::beta)).value));

    flag |= Flags::gamma;
    CHECK((1 | 2 | 4) == flag.value);
  }

  SECTION("Flag & operations")
  {
    auto flag = (Flags::alpha & Flags::beta);
    CHECK(((1 & 2) == flag.value));
    CHECK((Flags::alpha == ((Flags::alpha | Flags::beta) & Flags::alpha)));
    CHECK(((1 | 2) == (Flags(Flags::alpha) | Flags(Flags::beta)).value));

    flag = (Flags::alpha | Flags::beta);
    flag &= Flags::beta;
    CHECK(Flags::beta == flag);
  }

  SECTION("Flag ~ operations")
  {
    CHECK(int(0xff & (~1)) == static_cast<unsigned char>((~Flags::alpha).value));
    CHECK(int(0xff & (~1)) == static_cast<unsigned char>((~Flags(Flags::alpha)).value));
  }

  SECTION("Flag ^ operations")
  {
    CHECK((1 ^ 2) == (Flags::alpha ^ Flags::beta).value);
    CHECK((1 | 2) == (Flags::alpha ^ Flags::beta).value);
    CHECK((1 | 2) == (Flags(Flags::alpha) ^ Flags(Flags::beta)).value);

    auto flag = Flags(Flags::alpha);
    flag ^= Flags::beta;
    CHECK((1 ^ 2) == flag.value);
  }

  SECTION("Flag bitset access")
  {
    CHECK((Flags::alpha | Flags::beta)[Flags::beta]);
    CHECK((Flags::alpha | Flags::beta)[Flags::alpha]);
    CHECK_FALSE((Flags::alpha | Flags::beta)[Flags::gamma]);

    const auto a = (Flags::alpha | Flags::beta);
    CHECK(a[Flags::alpha]);
    CHECK_FALSE(a[Flags(Flags::gamma)]);
  }

  SECTION("Flag compare")
  {
    CHECK(Flags(Flags::gamma) < Flags::delta);
    CHECK_FALSE(Flags(Flags::gamma) < Flags::alpha);

    CHECK_FALSE(Flags(Flags::gamma) > Flags::delta);
    CHECK(Flags(Flags::gamma) > Flags::alpha);

    CHECK(Flags(Flags::gamma) <= Flags::delta);
    CHECK_FALSE(Flags(Flags::gamma) <= Flags::alpha);

    CHECK_FALSE(Flags(Flags::gamma) >= Flags::delta);
    CHECK(Flags(Flags::gamma) >= Flags::alpha);
  }

  SECTION("defined value names - error") { CHECK(ValueName(Flags::value_t(12)) == std::string("<error>")); }

  SECTION("default values")
  {
    Dummy d;
    CHECK(Flags::_none == d.en1);
    CHECK(Flags::gamma == d.en2);
  }

  SECTION("Compare operations")
  {
    Dummy d1;
    Dummy d2;

    CHECK(d1 == d2);
    CHECK_FALSE(d1 != d2);

    d1.en1 = Flags::beta;
    d2.en1 = Flags::gamma;

    CHECK_FALSE(d1 == d2);
    CHECK(d1 != d2);
  }

  SECTION("reading whats written")
  {
    Dummy dOut;
    dOut.en1 = Flags::beta;
    dOut.en2 = Flags::gamma | Flags::delta;
    dOut.en3.emplace_back(Flags::beta | Flags::delta);
    dOut.en3.emplace_back(Flags::gamma);

    std::stringstream sOut;
    Dummy_io().WriteDummy(sOut, dOut);

    const auto buffer = sOut.str();
    std::stringstream sIn(buffer);

    Dummy dIn;
    Dummy_io().ReadDummy(sIn, dIn);

    CHECK(Flags::beta == dIn.en1);
    CHECK((Flags::gamma | Flags::delta) == dIn.en2);

    REQUIRE(dIn.en3.size() == 2);
    CHECK(dIn.en3[0] == (Flags::beta | Flags::delta));
    CHECK(dIn.en3[1] == Flags::gamma);
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
    p.en3.emplace_back(Flags::alpha);
    p.en3.emplace_back(Flags::delta);
    p.en3.emplace_back(Flags::beta);

    SECTION("generate")
    {
      int x = 1;
      p.generate_en3([&x]() { return FlagsValues()[x++]; });
      CHECK(p.en3[0] == Flags::beta);
      CHECK(p.en3[1] == Flags::gamma);
      CHECK(p.en3[2] == Flags::delta);
    }

    SECTION("fill")
    {
      p.fill_en3(Flags::gamma);

      CHECK(p.en3[0] == Flags::gamma);
      CHECK(p.en3[1] == Flags::gamma);
      CHECK(p.en3[2] == Flags::gamma);
    }

    SECTION("remove")
    {
      auto e = p.remove_en3(Flags::delta);

      CHECK(e == p.en3.end() - 1);
      CHECK(3 == p.en3.size());
      CHECK(p.en3[0] == Flags::alpha);
      CHECK(p.en3[1] == Flags::beta);
    }

    SECTION("remove_if")
    {
      auto e = p.remove_en3_if([](Flags i) { return i == Flags::delta; });

      CHECK(e == p.en3.end() - 1);
      CHECK(3 == p.en3.size());
      CHECK(p.en3[0] == Flags::alpha);
      CHECK(p.en3[1] == Flags::beta);
    }

    SECTION("erase")
    {
      p.erase_en3(Flags::delta);

      CHECK(2 == p.en3.size());
      CHECK(p.en3[0] == Flags::alpha);
      CHECK(p.en3[1] == Flags::beta);
    }

    SECTION("erase_if")
    {
      p.erase_en3_if([](Flags i) { return i == Flags::delta; });

      CHECK(2 == p.en3.size());
      CHECK(p.en3[0] == Flags::alpha);
      CHECK(p.en3[1] == Flags::beta);
    }

    SECTION("reverse")
    {
      p.reverse_en3();

      REQUIRE(3 == p.en3.size());
      CHECK(p.en3[0] == Flags::beta);
      CHECK(p.en3[1] == Flags::delta);
      CHECK(p.en3[2] == Flags::alpha);
    }

    SECTION("rotate")
    {
      p.rotate_en3(p.en3.begin() + 1);

      REQUIRE(3 == p.en3.size());
      CHECK(p.en3[0] == Flags::delta);
      CHECK(p.en3[1] == Flags::beta);
      CHECK(p.en3[2] == Flags::alpha);
    }

    SECTION("sort")
    {
      p.sort_en3();

      REQUIRE(3 == p.en3.size());
      CHECK(p.en3[0] == Flags::alpha);
      CHECK(p.en3[1] == Flags::beta);
      CHECK(p.en3[2] == Flags::delta);
    }

    SECTION("sort with comparator")
    {
      p.sort_en3([](Flags l, Flags r) { return l.value > r.value; });

      REQUIRE(3 == p.en3.size());
      CHECK(p.en3[0] == Flags::delta);
      CHECK(p.en3[1] == Flags::beta);
      CHECK(p.en3[2] == Flags::alpha);
    }

    SECTION("any of")
    {
      CHECK(p.any_of_en3([](Flags x) { return x == Flags::delta; }));
      CHECK_FALSE(p.any_of_en3([](Flags x) { return x == Flags::gamma; }));
    }

    SECTION("all of")
    {
      CHECK_FALSE(p.all_of_en3([](Flags x) { return x == Flags::delta; }));
      CHECK(p.any_of_en3([](Flags x) { return int(x.value) >= 0; }));
    }

    SECTION("none of")
    {
      CHECK(p.none_of_en3([](Flags x) { return x == Flags::gamma; }));
      CHECK_FALSE(p.none_of_en3([](Flags x) { return x == Flags::delta; }));
    }

    SECTION("any of is")
    {
      CHECK(p.any_of_en3_is(Flags::delta));
      CHECK_FALSE(p.any_of_en3_is(Flags::gamma));
    }

    SECTION("all of are")
    {
      CHECK_FALSE(p.all_of_en3_are(Flags::delta));
      p.fill_en3(Flags::gamma);
      CHECK(p.all_of_en3_are(Flags::gamma));
    }

    SECTION("none of value")
    {
      CHECK_FALSE(p.none_of_en3_is(Flags::delta));
      CHECK(p.none_of_en3_is(Flags::gamma));
    }

    SECTION("for each")
    {
      std::vector<Flags> x;
      p.for_each_en3([&x](Flags i) { x.emplace_back(i); });

      CHECK(x[0] == Flags::alpha);
      CHECK(x[1] == Flags::delta);
      CHECK(x[2] == Flags::beta);
    }

    SECTION("find")
    {
      CHECK(p.find_in_en3(Flags::delta) == p.en3.begin() + 1);
      CHECK(p.find_in_en3(Flags::gamma) == p.en3.end());
    }

    SECTION("find if")
    {
      CHECK(p.find_in_en3_if([](Flags x) { return x == Flags::delta; }) == p.en3.begin() + 1);
      CHECK(p.find_in_en3_if([](Flags x) { return x == Flags::gamma; }) == p.en3.end());
    }

    SECTION("count")
    {
      CHECK(p.count_in_en3(Flags::delta) == 1);
      CHECK(p.count_in_en3(Flags::gamma) == 0);
    }

    SECTION("count if")
    {
      CHECK(p.count_in_en3_if([](Flags x) { return x == Flags::delta; }) == 1);
      CHECK(p.count_in_en3_if([](Flags x) { return x == Flags::gamma; }) == 0);
    }
  }
}
