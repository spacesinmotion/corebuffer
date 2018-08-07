
#define CATCH_CONFIG_FAST_COMPILE
#include "catch/catch.hpp"

#include "basetypes.h"

TEST_CASE("BaseType test", "[]")
{
  SECTION("default values")
  {
    Scope::BaseTypes bt;

    CHECK(bt.a == 0);
    CHECK(bt.aa == 0);
    CHECK(bt.ab == 0);
    CHECK(bt.b == false);
    CHECK(bt.c == 0.0f);
    CHECK(bt.d == 0.0);
    CHECK(bt.e == 0);
    CHECK(bt.f == 0);
    CHECK(bt.g == 0);
    CHECK(bt.h == 0);
    CHECK(bt.i == 0u);
    CHECK(bt.j == 0u);
    CHECK(bt.k == 0u);
    CHECK(bt.l == 0u);
  }
}
