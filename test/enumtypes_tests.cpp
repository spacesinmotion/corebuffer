
#define CATCH_CONFIG_FAST_COMPILE
#include "catch/catch.hpp"

#include "enumtypes.h"

TEST_CASE("EnumTypes test", "[]")
{
  SECTION("defined values")
  {
    CHECK(0 == int(Scope::EnumTypes::alpha));
    CHECK(4 == int(Scope::EnumTypes::beta));
    CHECK(5 == int(Scope::EnumTypes::gamma));
    CHECK(42 == int(Scope::EnumTypes::delta));
  }

  SECTION("Default Values")
  {
    Scope::Dummy d;
    CHECK(Scope::EnumTypes::alpha == d.en1);
    CHECK(Scope::EnumTypes::delta == d.en2);
  }
}
