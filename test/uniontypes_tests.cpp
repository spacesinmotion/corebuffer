
#define CATCH_CONFIG_FAST_COMPILE
#include <sstream>
#include "catch2/catch.hpp"
#include "uniontypes.h"

using namespace UnionTypes;

TEST_CASE("case", "[tag]")
{
  SECTION("compare operations")
  {
    CHECK(A() == A());
    CHECK_FALSE(A() != A());

    CHECK(B() == B());
    CHECK_FALSE(B() != B());

    CHECK(Root() == Root());
    CHECK_FALSE(Root() != Root());
  }

  SECTION("reading whats written")
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
