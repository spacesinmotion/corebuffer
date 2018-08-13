#define CATCH_CONFIG_FAST_COMPILE
#include "catch/catch.hpp"

#include "parser.h"

TEST_CASE("Parsing packages", "[]")
{
  SECTION("Simple start up test ")
  {
    const auto source = R"(
package SubC.scope;
version "0.1.203";
root_type YYY;

table XXX {
bill:int = 1;
aul:bool = false;
kaul:float;
baul:[float];
}

table YYY {
still:unique XXX;
sub:[shared YYY];
}

enum Access { Private, Public = 3, Protected }
)";

    Package p;
    REQUIRE(Parser(source, p).parse());

    SECTION("Checking parsed structure")
    {
      CHECK(p.path.value == "SubC.scope");
      CHECK(p.version.value == "0.1.203");
      CHECK(p.root_type.value == "YYY");
    }

    SECTION("Checking parsed enum")
    {
      REQUIRE(p.enums.size() == 1);
      CHECK(p.enums.front().name == "Access");

      REQUIRE(p.enums.front().entries.size() == 3);
      CHECK(p.enums.front().entries[0].name == "Private");
      CHECK(p.enums.front().entries[0].value == 0);
      CHECK(p.enums.front().entries[1].name == "Public");
      CHECK(p.enums.front().entries[1].value == 3);
      CHECK(p.enums.front().entries[2].name == "Protected");
      CHECK(p.enums.front().entries[2].value == 4);
    }

    SECTION("Checking parsed XXX")
    {
      REQUIRE(p.tables.size() == 2);

      CHECK(p.tables[0].name == "XXX");

      REQUIRE(p.tables[0].member.size() == 4);

      CHECK(p.tables[0].member[0].isBaseType);
      CHECK(!p.tables[0].member[0].isVector);
      CHECK(p.tables[0].member[0].name == "bill");
      CHECK(p.tables[0].member[0].type == "std::int32_t");
      CHECK(p.tables[0].member[0].defaultValue == "1");
      CHECK(p.tables[0].member[0].pointer == Pointer::Plain);

      CHECK(!p.tables[0].member[1].isVector);
      CHECK(p.tables[0].member[1].isBaseType);
      CHECK(p.tables[0].member[1].name == "aul");
      CHECK(p.tables[0].member[1].type == "bool");
      CHECK(p.tables[0].member[1].defaultValue == "false");
      CHECK(p.tables[0].member[1].pointer == Pointer::Plain);

      CHECK(!p.tables[0].member[2].isVector);
      CHECK(p.tables[0].member[2].isBaseType);
      CHECK(p.tables[0].member[2].name == "kaul");
      CHECK(p.tables[0].member[2].type == "float");
      CHECK(p.tables[0].member[2].defaultValue == "0.0f");
      CHECK(p.tables[0].member[2].pointer == Pointer::Plain);

      CHECK(p.tables[0].member[3].isVector);
      CHECK(p.tables[0].member[3].isBaseType);
      CHECK(p.tables[0].member[3].name == "baul");
      CHECK(p.tables[0].member[3].type == "float");
      CHECK(p.tables[0].member[3].defaultValue.empty());
      CHECK(p.tables[0].member[3].pointer == Pointer::Plain);
    }

    SECTION("Checking parsed XXX")
    {
      REQUIRE(p.tables.size() == 2);

      CHECK(p.tables[1].name == "YYY");

      REQUIRE(p.tables[1].member.size() == 2);

      CHECK(!p.tables[1].member[0].isVector);
      CHECK(!p.tables[1].member[0].isBaseType);
      CHECK(p.tables[1].member[0].name == "still");
      CHECK(p.tables[1].member[0].type == "XXX");
      CHECK(p.tables[1].member[0].defaultValue.empty());
      CHECK(p.tables[1].member[0].pointer == Pointer::Unique);

      CHECK(p.tables[1].member[1].isVector);
      CHECK(!p.tables[1].member[1].isBaseType);
      CHECK(p.tables[1].member[1].name == "sub");
      CHECK(p.tables[1].member[1].type == "YYY");
      CHECK(p.tables[1].member[1].defaultValue.empty());
      CHECK(p.tables[1].member[1].pointer == Pointer::Shared);
    }
  }

  SECTION("Base types")
  {
    const auto source = R"(
package Scope;
version "0.0";
root_type BaseTypes;

table BaseTypes {
  a:int;
  aa:short;
  ab:long;
  b:bool;
  c:float;
  d:double;
  e:i8;
  f:i16;
  g:i32;
  h:i64;
  i:ui8;
  j:ui16;
  k:ui32;
  l:ui64;
  m:string;
}
)";

    Package p;
    REQUIRE(Parser(source, p).parse());

    REQUIRE((p.tables.size() == 1 && p.tables.front().member.size() == 15));

    CHECK(p.tables.front().member[0].isBaseType);
    CHECK(p.tables.front().member[0].defaultValue == "0");

    CHECK(p.tables.front().member[1].isBaseType);
    CHECK(p.tables.front().member[1].defaultValue == "0");

    CHECK(p.tables.front().member[2].isBaseType);
    CHECK(p.tables.front().member[2].defaultValue == "0");

    CHECK(p.tables.front().member[3].isBaseType);
    CHECK(p.tables.front().member[3].defaultValue == "false");

    CHECK(p.tables.front().member[4].isBaseType);
    CHECK(p.tables.front().member[4].defaultValue == "0.0f");

    CHECK(p.tables.front().member[5].isBaseType);
    CHECK(p.tables.front().member[5].defaultValue == "0.0");

    CHECK(p.tables.front().member[6].isBaseType);
    CHECK(p.tables.front().member[6].defaultValue == "0");

    CHECK(p.tables.front().member[7].isBaseType);
    CHECK(p.tables.front().member[7].defaultValue == "0");

    CHECK(p.tables.front().member[8].isBaseType);
    CHECK(p.tables.front().member[8].defaultValue == "0");

    CHECK(p.tables.front().member[9].isBaseType);
    CHECK(p.tables.front().member[9].defaultValue == "0");

    CHECK(p.tables.front().member[10].isBaseType);
    CHECK(p.tables.front().member[10].defaultValue == "0u");

    CHECK(p.tables.front().member[11].isBaseType);
    CHECK(p.tables.front().member[11].defaultValue == "0u");

    CHECK(p.tables.front().member[12].isBaseType);
    CHECK(p.tables.front().member[12].defaultValue == "0u");

    CHECK(p.tables.front().member[13].isBaseType);
    CHECK(p.tables.front().member[13].defaultValue == "0u");

    CHECK(p.tables.front().member[14].isBaseType);
    CHECK(p.tables.front().member[14].defaultValue.empty());
  }

  SECTION("Pointer types")
  {
    const auto source = R"(
package Scope;
version "0.0";
root_type PointerTypes;

table Test { a:int = 42; }

table PointerTypes {
  a:unique Test;
  b:shared Test;
  c:weak Test;
}
)";

    Package p;
    REQUIRE(Parser(source, p).parse());

    REQUIRE((p.tables.size() == 2 && p.tables.back().member.size() == 3));

    CHECK(!p.tables.back().member[0].isBaseType);
    CHECK(p.tables.back().member[0].pointer == Pointer::Unique);

    CHECK(!p.tables.back().member[1].isBaseType);
    CHECK(p.tables.back().member[1].pointer == Pointer::Shared);

    CHECK(!p.tables.back().member[2].isBaseType);
    CHECK(p.tables.back().member[2].pointer == Pointer::Weak);
  }

  SECTION("Vector types")
  {
    const auto source = R"(
package Scope;
version "0.0";
root_type VectorTypes;

table Test { a:int = 42; }

table VectorTypes {
  a:[unique Test];
  b:[shared Test];
  c:[weak Test];
  d:[float];
}
)";

    Package p;
    REQUIRE(Parser(source, p).parse());

    REQUIRE((p.tables.size() == 2 && p.tables.back().member.size() == 4));

    CHECK(!p.tables.back().member[0].isBaseType);
    CHECK(p.tables.back().member[0].isVector);
    CHECK(p.tables.back().member[0].pointer == Pointer::Unique);

    CHECK(!p.tables.back().member[1].isBaseType);
    CHECK(p.tables.back().member[1].isVector);
    CHECK(p.tables.back().member[1].pointer == Pointer::Shared);

    CHECK(!p.tables.back().member[2].isBaseType);
    CHECK(p.tables.back().member[2].isVector);
    CHECK(p.tables.back().member[2].pointer == Pointer::Weak);

    CHECK(p.tables.back().member[3].isBaseType);
    CHECK(p.tables.back().member[3].isVector);
    CHECK(p.tables.back().member[3].pointer == Pointer::Plain);
  }

  SECTION("Enums as member")
  {
    const auto source = R"(
package Scope;
version "0.0";
root_type VectorTypes;

enum Test { a, b, c }

table VectorTypes {
  a:Test;
}
)";

    Package p;
    REQUIRE(Parser(source, p).parse());

    REQUIRE((p.enums.size() == 1 && p.enums.back().entries.size() == 3));
    REQUIRE((p.tables.size() == 1 && p.tables.back().member.size() == 1));
  }

  SECTION("Defautl values for enums")
  {
    const auto source = R"(
package Scope;
version "0.0";
root_type Dummy;

enum EnumTypes
{
alpha,
beta = 4,
gamma,
delta = 42
}

table Dummy
{
en1:EnumTypes;
en2:EnumTypes=delta;
}
)";

    Package p;
    REQUIRE(Parser(source, p).parse());

    REQUIRE((p.enums.size() == 1 && p.enums.back().entries.size() == 4));
    REQUIRE((p.tables.size() == 1 && p.tables.back().member.size() == 2));

    CHECK(p.tables[0].member[0].defaultValue == "Scope::EnumTypes::alpha");
    CHECK(p.tables[0].member[1].defaultValue == "Scope::EnumTypes::delta");
  }

  SECTION("empty table")
  {
    const auto source = R"(
table Dummy {}
)";

    Package p;
    CHECK(Parser(source, p).parse());
  }

  SECTION("empty enum")
  {
    const auto source = R"(
enum Dummy {}
)";

    Package p;
    CHECK(Parser(source, p).parse());
  }

  SECTION("trailing ',' enum")
  {
    const auto source = R"(
enum Dummy {a,b,}
)";

    Package p;
    CHECK(Parser(source, p).parse());
  }
}
