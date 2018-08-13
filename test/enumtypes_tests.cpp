#define CATCH_CONFIG_FAST_COMPILE
#include "catch/catch.hpp"

#include "enumtypes.h"

#include <sstream>

TEST_CASE("EnumTypes test", "[output, enum]")
{
  SECTION("defined values")
  {
    CHECK(0 == int(Scope::EnumTypes::alpha));
    CHECK(4 == int(Scope::EnumTypes::beta));
    CHECK(5 == int(Scope::EnumTypes::gamma));
    CHECK(42 == int(Scope::EnumTypes::delta));
  }

  SECTION("defined value list")
  {
    auto definedValues = Scope::EnumTypesValues();

    REQUIRE(definedValues.size() == 4);

    CHECK(Scope::EnumTypes::alpha == definedValues[0]);
    CHECK(Scope::EnumTypes::beta == definedValues[1]);
    CHECK(Scope::EnumTypes::gamma == definedValues[2]);
    CHECK(Scope::EnumTypes::delta == definedValues[3]);
  }

  SECTION("defined value names")
  {
    CHECK(Scope::ValueName(Scope::EnumTypes::alpha) == std::string("alpha"));
    CHECK(Scope::ValueName(Scope::EnumTypes::beta) == std::string("beta"));
    CHECK(Scope::ValueName(Scope::EnumTypes::gamma) == std::string("gamma"));
    CHECK(Scope::ValueName(Scope::EnumTypes::delta) == std::string("delta"));
  }

  SECTION("default values")
  {
    Scope::Dummy d;
    CHECK(Scope::EnumTypes::alpha == d.en1);
    CHECK(Scope::EnumTypes::delta == d.en2);
  }

  SECTION("enums have minimum sizes")
  {
    CHECK(sizeof(Scope::EnumTypes) == sizeof(std::int8_t));
    CHECK(sizeof(Scope::Enum16) == sizeof(std::int16_t));
    CHECK(sizeof(Scope::Enum32) == sizeof(std::int32_t));
    CHECK(sizeof(Scope::EnumMinus8) == sizeof(std::int8_t));
  }

  SECTION("reading whats written")
  {
    Scope::Dummy dOut;
    dOut.en1 = Scope::EnumTypes::beta;
    dOut.en2 = Scope::EnumTypes::gamma;
    dOut.en3.emplace_back(Scope::EnumTypes::beta);
    dOut.en3.emplace_back(Scope::EnumTypes::gamma);
    dOut.en4 = std::make_unique<Scope::EnumTypes>(Scope::EnumTypes::gamma);

    std::stringstream sOut;
    Scope::Dummy_io().WriteDummy(sOut, dOut);

    const auto buffer = sOut.str();
    std::stringstream sIn(buffer);

    Scope::Dummy dIn;
    Scope::Dummy_io().ReadDummy(sIn, dIn);

    CHECK(Scope::EnumTypes::beta == dIn.en1);
    CHECK(Scope::EnumTypes::gamma == dIn.en2);

    REQUIRE(dIn.en3.size() == 2);
    CHECK(dIn.en3[0] == Scope::EnumTypes::beta);
    CHECK(dIn.en3[1] == Scope::EnumTypes::gamma);

    REQUIRE(dIn.en4 != nullptr);
    CHECK(*dIn.en4 == Scope::EnumTypes::gamma);
  }
}
