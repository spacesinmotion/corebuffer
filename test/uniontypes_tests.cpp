
#define CATCH_CONFIG_FAST_COMPILE
#include <sstream>
#include "catch2/catch.hpp"
#include "uniontypes.h"

using namespace UnionTypes;

A testA()
{
  return A("hello");
}

void checkA(const AB &ab)
{
  REQUIRE(ab.is_Defined());
  REQUIRE(ab.is_A());
  REQUIRE_NOTHROW(ab.as_A());
  CHECK_FALSE(ab.is_B());
  CHECK(ab.as_A().name == "hello");
}

B testB()
{
  return B(42);
}
void checkB(const AB &ab)
{
  REQUIRE(ab.is_Defined());
  REQUIRE(ab.is_B());
  REQUIRE_NOTHROW(ab.as_B());
  CHECK(ab.as_B().size == 42);
}

TEST_CASE("case", "[tag]")
{
  SECTION("Use union")
  {
    SECTION("default")
    {
      AB ab;
      CHECK(!ab.is_Defined());
    }

    SECTION("construct copy")
    {
      const AB a = testA();
      checkA(AB(a));
      const AB b = testB();
      checkB(AB(b));
    }

    SECTION("assignment")
    {
      AB ab;
      const AB a = testA();
      ab = a;
      checkA(ab);

      const AB b = testB();
      ab = b;
      checkB(ab);

      AB xx;
      ab = xx;
      CHECK(!ab.is_Defined());
    }

    SECTION("construct sub types r-value ")
    {
      checkA(AB(testA()));
      checkB(AB(testB()));
    }

    SECTION("construct sub types l-value ")
    {
      const A a = testA();
      checkA(AB(a));

      const B b = testB();
      checkB(AB(b));
    }

    SECTION("Assignment sub types r-value")
    {
      AB ab;
      ab = testA();
      checkA(ab);

      ab = testB();
      checkB(ab);
    }

    SECTION("Assignment sub types l-value ")
    {
      AB ab;
      const A a = testA();
      ab = a;
      checkA(ab);

      const B b = testB();
      ab = b;
      checkB(ab);
    }

    SECTION("Assignment sub types l-value ")
    {
      AB ab = testA();
      ab.as_A().name = "HALLO";
      CHECK(AB(testA()) != ab);

      ab = testB();
      ab.as_B().size = 53;
      CHECK(AB(testB()) != ab);
    }

    SECTION("clear content")
    {
      AB ab = testA();
      CHECK(ab.is_Defined());
      CHECK(ab.is_A());
      CHECK_FALSE(ab.is_B());

      ab.clear();
      CHECK_FALSE(ab.is_Defined());
      CHECK_FALSE(ab.is_A());
      CHECK_FALSE(ab.is_B());
    }

    SECTION("compare equal")
    {
      AB n;
      AB a = testA();
      AB b = testB();

      CHECK(n == n);
      CHECK(n == AB());
      CHECK(a == AB(testA()));
      CHECK(b == AB(testB()));

      CHECK_FALSE(a == n);
      CHECK_FALSE(a == b);
      CHECK_FALSE(n == b);
    }

    SECTION("compare not equal")
    {
      AB n;
      AB a = testA();
      AB b = testB();

      CHECK_FALSE(n != n);
      CHECK_FALSE(n != AB());
      CHECK_FALSE(a != AB(testA()));
      CHECK_FALSE(b != AB(testB()));

      CHECK(a != n);
      CHECK(a != b);
      CHECK(n != b);
    }

    SECTION("compare equal subtype l r")
    {
      AB n;
      AB a = testA();
      AB b = testB();

      CHECK(a == testA());
      CHECK(b == testB());

      CHECK_FALSE(n == testA());
      CHECK_FALSE(n == testB());
      CHECK_FALSE(a == testB());
      CHECK_FALSE(b == testA());
    }

    SECTION("compare equal subtype r l")
    {
      AB n;
      AB a = testA();
      AB b = testB();

      CHECK(testA() == a);
      CHECK(testB() == b);

      CHECK_FALSE(testA() == n);
      CHECK_FALSE(testB() == n);
      CHECK_FALSE(testB() == a);
      CHECK_FALSE(testA() == b);
    }

    SECTION("compare not equal subtype l r")
    {
      AB n;
      AB a = testA();
      AB b = testB();

      CHECK_FALSE(a != testA());
      CHECK_FALSE(b != testB());

      CHECK(n != testA());
      CHECK(n != testB());
      CHECK(a != testB());
      CHECK(b != testA());
    }

    SECTION("compare not equal subtype l r")
    {
      AB n;
      AB a = testA();
      AB b = testB();

      CHECK_FALSE(testA() != a);
      CHECK_FALSE(testB() != b);

      CHECK(testA() != n);
      CHECK(testB() != n);
      CHECK(testB() != a);
      CHECK(testA() != b);
    }
  }

  SECTION("general compare operations")
  {
    CHECK(A() == A());
    CHECK_FALSE(A() != A());

    CHECK(B() == B());
    CHECK_FALSE(B() != B());

    CHECK(Root() == Root());
    CHECK_FALSE(Root() != Root());
  }

  SECTION("reading whats written plain union")
  {
    std::stringstream sOut;
    {
      Root root;

      CHECK_FALSE(root.f.is_Defined());
      root.f.create_A("bear boy");
      REQUIRE((root.f.is_Defined() && root.f.is_A()));

      Root_io().WriteRoot(sOut, root);
    }

    {
      const auto buffer = sOut.str();
      std::stringstream sIn(buffer);

      Root rootIn;
      Root_io().ReadRoot(sIn, rootIn);

      REQUIRE((rootIn.f.is_Defined() && rootIn.f.is_A()));
      CHECK(rootIn.f.as_A().name == "bear boy");
    }
  }

  SECTION("reading whats written pointer union")
  {
    std::stringstream sOut;
    {
      Root root;
      root.c = std::make_shared<AB>();
      root.c->create_A("Hallo");
      root.cw = root.c;

      root.d = std::unique_ptr<AB>(new AB);
      root.d->create_B(54);

      root.empty = std::unique_ptr<AB>(new AB);

      Root_io().WriteRoot(sOut, root);
    }

    const auto buffer = sOut.str();
    std::stringstream sIn(buffer);

    Root rootIn;
    Root_io().ReadRoot(sIn, rootIn);

    REQUIRE(rootIn.c != nullptr);
    REQUIRE(rootIn.c->is_A());
    CHECK(rootIn.c->as_A().name == "Hallo");

    REQUIRE(rootIn.cw.lock() != nullptr);
    CHECK(rootIn.cw.lock() == rootIn.c);

    REQUIRE(rootIn.d != nullptr);
    REQUIRE(rootIn.d->is_B());
    CHECK(rootIn.d->as_B().size == 54);

    REQUIRE(rootIn.empty != nullptr);
    CHECK_FALSE(rootIn.empty->is_Defined());

    CHECK(rootIn.null == nullptr);
  }

  SECTION("reading whats written vector")
  {
    std::stringstream sOut;
    {
      Root root;
      root.e.emplace_back(A("funny name"));
      root.e.emplace_back(B(54));

      Root_io().WriteRoot(sOut, root);
    }

    const auto buffer = sOut.str();
    std::stringstream sIn(buffer);
    Root rootIn;
    Root_io().ReadRoot(sIn, rootIn);

    REQUIRE(rootIn.e.size() == 2);

    REQUIRE(rootIn.e[0].is_A());
    CHECK(rootIn.e[0].as_A().name == "funny name");

    REQUIRE(rootIn.e[1].is_B());
    CHECK(rootIn.e[1].as_B().size == 54);
  }

  SECTION("reading whats written non union types")
  {
    Root root;
    root.a = A("Hallo");
    root.b = B(42);

    Root rootIn;
    CHECK_FALSE(root == rootIn);

    std::stringstream sOut;
    Root_io().WriteRoot(sOut, root);

    const auto buffer = sOut.str();
    std::stringstream sIn(buffer);

    Root_io().ReadRoot(sIn, rootIn);

    CHECK(root == rootIn);
  }

  SECTION("Reading fails with wrong data")
  {
    Root r;

    std::istringstream s1("FALSE");
    CHECK_FALSE(Root_io().ReadRoot(s1, r));

    std::istringstream s2("COREfails");
    CHECK_FALSE(Root_io().ReadRoot(s2, r));
  }
}
