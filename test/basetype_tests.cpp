#define CATCH_CONFIG_FAST_COMPILE
#include "catch2/catch.hpp"

#include "basetypes.h"

#include <sstream>

using namespace Scope;

TEST_CASE("base type output test", "[output, base types]")
{
  SECTION("default values")
  {
    BaseTypes bt;

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
    BaseTypes bt1;
    BaseTypes bt2;
    CHECK(bt1 == bt2);
    CHECK_FALSE(bt1 != bt2);
  }

  SECTION("reading whats written")
  {
    Root dOut;
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

    Root dIn;
    CHECK_FALSE(dOut == dIn);

    std::stringstream sOut;
    Root_io().WriteRoot(sOut, dOut);

    const auto buffer = sOut.str();
    std::stringstream sIn(buffer);

    Root_io().ReadRoot(sIn, dIn);

    CHECK(dOut == dIn);
  }

  SECTION("reading whats written vector of strings")
  {
    Root dOut;
    dOut.b.b1.emplace_back("Hallo");
    dOut.b.b1.emplace_back("Welt");

    Root dIn;
    CHECK_FALSE(dOut == dIn);

    std::stringstream sOut;
    Root_io().WriteRoot(sOut, dOut);

    const auto buffer = sOut.str();
    std::stringstream sIn(buffer);

    Root_io().ReadRoot(sIn, dIn);

    CHECK(dOut == dIn);
  }

  SECTION("Reading fails with wrong data")
  {
    Root r;

    std::stringstream s1("FALSE");
    CHECK_FALSE(Root_io().ReadRoot(s1, r));

    std::stringstream s2("COREfails");
    CHECK_FALSE(Root_io().ReadRoot(s2, r));
  }

  SECTION("initializing methods")
  {
    auto _default = Initializer();
    auto _inited = Initializer(123);

    CHECK(_inited.a == 123);
    CHECK(_inited.a != _default.a);
    CHECK(_inited.b == _default.b);

    _inited = Initializer(32.1f, 345);

    CHECK(_inited.a != _default.a);
    CHECK(_inited.b != _default.b);
    CHECK(_inited.a == 345);
    CHECK(_inited.b == 32.1f);
  }

  SECTION("Compare operations")
  {
    SECTION("Pointer")
    {
      PointerBaseTypes d1;
      PointerBaseTypes d2;

      CHECK(d1 == d2);
      CHECK_FALSE(d1 != d2);

      d1.b1.emplace_back("xxx");
      d2.b1.emplace_back("yyy");

      CHECK_FALSE(d1 == d2);
      CHECK(d1 != d2);
    }

    SECTION("Root")
    {
      Root d1;
      Root d2;

      CHECK(d1 == d2);
      CHECK_FALSE(d1 != d2);

      d1.b.b1.emplace_back("xxx");
      d2.b.b1.emplace_back("yyy");

      CHECK_FALSE(d1 == d2);
      CHECK(d1 != d2);
    }

    SECTION("Initializer")
    {
      CHECK(Initializer() == Initializer());
      CHECK_FALSE(Initializer() != Initializer());

      CHECK_FALSE(Initializer(2.1f, 4) == Initializer(2.21f, 14));
      CHECK(Initializer(2.1f, 4) != Initializer(2.21f, 14));
    }
  }

  SECTION("vector algorithm")
  {
    PointerBaseTypes p;
    p.x.resize(3, 0);
    REQUIRE(p.x.size() == 3);

    SECTION("fill")
    {
      p.fill_x(42);

      CHECK(p.x[0] == 42);
      CHECK(p.x[1] == 42);
      CHECK(p.x[2] == 42);
    }

    SECTION("generate")
    {
      int x = 0;
      p.generate_x([&x]() { return x++; });

      CHECK(p.x[0] == 0);
      CHECK(p.x[1] == 1);
      CHECK(p.x[2] == 2);
    }

    SECTION("remove")
    {
      int x = 0;
      p.generate_x([&x]() { return x++; });
      auto e = p.remove_x(1);

      CHECK(e == p.x.end() - 1);
      CHECK(3 == p.x.size());
      CHECK(p.x[0] == 0);
      CHECK(p.x[1] == 2);
    }

    SECTION("remove_if")
    {
      int x = 0;
      p.generate_x([&x]() { return x++; });
      auto e = p.remove_x_if([](int i) { return i % 2 == 1; });

      CHECK(e == p.x.end() - 1);
      CHECK(3 == p.x.size());
      CHECK(p.x[0] == 0);
      CHECK(p.x[1] == 2);
    }

    SECTION("erase")
    {
      int x = 0;
      p.generate_x([&x]() { return x++; });
      p.erase_x(1);

      CHECK(2 == p.x.size());
      CHECK(p.x[0] == 0);
      CHECK(p.x[1] == 2);
    }

    SECTION("erase_if")
    {
      int x = 0;
      p.generate_x([&x]() { return x++; });
      p.erase_x_if([](int i) { return i % 2 == 1; });

      CHECK(2 == p.x.size());
      CHECK(p.x[0] == 0);
      CHECK(p.x[1] == 2);
    }

    SECTION("reverse")
    {
      int x = 0;
      p.generate_x([&x]() { return x++; });
      p.reverse_x();

      REQUIRE(3 == p.x.size());
      CHECK(p.x[0] == 2);
      CHECK(p.x[1] == 1);
      CHECK(p.x[2] == 0);
    }

    SECTION("rotate")
    {
      int x = 0;
      p.generate_x([&x]() { return x++; });
      p.rotate_x(p.x.begin() + 1);

      REQUIRE(3 == p.x.size());
      CHECK(p.x[0] == 1);
      CHECK(p.x[1] == 2);
      CHECK(p.x[2] == 0);
    }

    SECTION("sort")
    {
      p.x[0] = 32;
      p.x[1] = 12;
      p.x[2] = 42;
      p.sort_x();

      REQUIRE(3 == p.x.size());
      CHECK(p.x[0] == 12);
      CHECK(p.x[1] == 32);
      CHECK(p.x[2] == 42);
    }

    SECTION("sort with comparator")
    {
      p.x[0] = 32;
      p.x[1] = 12;
      p.x[2] = 42;
      p.sort_x([](int l, int r) { return l > r; });

      REQUIRE(3 == p.x.size());
      CHECK(p.x[0] == 42);
      CHECK(p.x[1] == 32);
      CHECK(p.x[2] == 12);
    }

    SECTION("any of")
    {
      int x = 0;
      p.generate_x([&x]() { return x++; });

      CHECK(p.any_of_x([](int x) { return x == 1; }));
      CHECK_FALSE(p.any_of_x([](int x) { return x == 42; }));
    }

    SECTION("all of")
    {
      int x = 0;
      p.generate_x([&x]() { return x++; });

      CHECK_FALSE(p.all_of_x([](int x) { return x == 1; }));
      CHECK(p.any_of_x([](int x) { return x >= 0; }));
    }

    SECTION("none of")
    {
      int x = 0;
      p.generate_x([&x]() { return x++; });

      CHECK_FALSE(p.none_of_x([](int x) { return x == 1; }));
      CHECK(p.none_of_x([](int x) { return x < 0; }));
    }

    SECTION("any of is")
    {
      int x = 0;
      p.generate_x([&x]() { return x++; });

      CHECK(p.any_of_x_is(1));
      CHECK_FALSE(p.any_of_x_is(42));
    }

    SECTION("all of are")
    {
      int x = 0;
      p.generate_x([&x]() { return x++; });

      CHECK_FALSE(p.all_of_x_are(1));
      p.x[0] = 1;
      p.x[2] = 1;
      CHECK(p.all_of_x_are(1));
    }

    SECTION("none of value")
    {
      int x = 0;
      p.generate_x([&x]() { return x++; });

      CHECK_FALSE(p.none_of_x_is(1));
      CHECK(p.none_of_x_is(42));
    }

    SECTION("for each")
    {
      int x = 0;
      p.generate_x([&x]() { return x++; });

      x = 0;
      p.for_each_x([&x](int i) { x += i; });

      CHECK(x == 0 + 1 + 2);
    }

    SECTION("find")
    {
      int x = 0;
      p.generate_x([&x]() { return x++; });

      CHECK(p.find_in_x(1) == p.x.begin() + 1);
      CHECK(p.find_in_x(42) == p.x.end());
    }

    SECTION("find if")
    {
      int x = 0;
      p.generate_x([&x]() { return x++; });

      CHECK(p.find_in_x_if([](int x) { return x == 1; }) == p.x.begin() + 1);
      CHECK(p.find_in_x_if([](int x) { return x == 42; }) == p.x.end());
    }

    SECTION("count")
    {
      int x = 0;
      p.generate_x([&x]() { return x++; });

      CHECK(p.count_in_x(1) == 1);
      CHECK(p.count_in_x(42) == 0);
    }

    SECTION("count if")
    {
      int x = 0;
      p.generate_x([&x]() { return x++; });

      CHECK(p.count_in_x_if([](int x) { return x == 1; }) == 1);
      CHECK(p.count_in_x_if([](int x) { return x % 2 == 0; }) == 2);
    }
  }
}
