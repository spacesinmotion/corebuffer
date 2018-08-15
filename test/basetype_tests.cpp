#define CATCH_CONFIG_FAST_COMPILE
#include "catch2/catch.hpp"

#include "basetypes.h"

#include <sstream>

TEST_CASE("base type output test", "[output, base types]")
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

  SECTION("Compare operator")
  {
    Scope::BaseTypes bt1;
    Scope::BaseTypes bt2;
    CHECK(bt1 == bt2);
    CHECK_FALSE(bt1 != bt2);
  }

  SECTION("reading whats written")
  {
    Scope::Root dOut;
    dOut.a.a = 1;
    dOut.a.aa = 2;
    dOut.a.ab = 3;
    dOut.a.b = true;
    dOut.a.c = 0.1f;
    dOut.a.d = 0.2;
    dOut.a.e = 5;
    dOut.a.f = 6;
    dOut.a.g = 7;
    dOut.a.h = 8;
    dOut.a.i = 9;
    dOut.a.j = 10;
    dOut.a.k = 11;
    dOut.a.l = 12;

    Scope::Root dIn;
    CHECK_FALSE(dOut == dIn);

    std::stringstream sOut;
    Scope::Root_io().WriteRoot(sOut, dOut);

    const auto buffer = sOut.str();
    std::stringstream sIn(buffer);

    Scope::Root_io().ReadRoot(sIn, dIn);

    CHECK(dOut == dIn);
  }

  SECTION("reading whats written vector of strings")
  {
    Scope::Root dOut;
    dOut.b.b1.emplace_back("Hallo");
    dOut.b.b1.emplace_back("Welt");

    Scope::Root dIn;
    CHECK_FALSE(dOut == dIn);

    std::stringstream sOut;
    Scope::Root_io().WriteRoot(sOut, dOut);

    const auto buffer = sOut.str();
    std::stringstream sIn(buffer);

    Scope::Root_io().ReadRoot(sIn, dIn);

    CHECK(dOut == dIn);
  }

  SECTION("initializing methods")
  {
    auto _default = Scope::Initializer();
    auto _inited = Scope::Initializer(123);

    CHECK(_inited.a == 123);
    CHECK(_inited.a != _default.a);
    CHECK(_inited.b == _default.b);

    _inited = Scope::Initializer(32.1f, 345);

    CHECK(_inited.a != _default.a);
    CHECK(_inited.b != _default.b);
    CHECK(_inited.a == 345);
    CHECK(_inited.b == 32.1f);
  }

  SECTION("Compare operations")
  {
    SECTION("Pointer")
    {
      Scope::PointerBaseTypes d1;
      Scope::PointerBaseTypes d2;

      CHECK(d1 == d2);
      CHECK_FALSE(d1 != d2);

      d1.b1.emplace_back("xxx");
      d2.b1.emplace_back("yyy");

      CHECK_FALSE(d1 == d2);
      CHECK(d1 != d2);
    }

    SECTION("Root")
    {
      Scope::Root d1;
      Scope::Root d2;

      CHECK(d1 == d2);
      CHECK_FALSE(d1 != d2);

      d1.b.b1.emplace_back("xxx");
      d2.b.b1.emplace_back("yyy");

      CHECK_FALSE(d1 == d2);
      CHECK(d1 != d2);
    }

    SECTION("Initializer")
    {
      CHECK(Scope::Initializer() == Scope::Initializer());
      CHECK_FALSE(Scope::Initializer() != Scope::Initializer());

      CHECK_FALSE(Scope::Initializer(2.1f, 4) == Scope::Initializer(2.21f, 14));
      CHECK(Scope::Initializer(2.1f, 4) != Scope::Initializer(2.21f, 14));
    }
  }

  SECTION("Reading fails with wrong data")
  {
    Scope::Root r;

    std::stringstream s1("FALSE");
    CHECK_FALSE(Scope::Root_io().ReadRoot(s1, r));

    std::stringstream s2("COREfails");
    CHECK_FALSE(Scope::Root_io().ReadRoot(s2, r));
  }
}
