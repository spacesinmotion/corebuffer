#define CATCH_CONFIG_FAST_COMPILE
#include "catch/catch.hpp"

#include "parser.h"

Package parse(const std::string &source)
{
  Package p;
  REQUIRE(Parser(source, p).parse());
  return p;
}

void checkBaseType(const std::string &member, const std::string &type, const std::string &defValue)
{
  const auto p = parse(R"(
package Scope;
version "0.0";
root_type BaseTypes;

table BaseTypes {)" + member +
                       "\n}");

  REQUIRE((p.tables.size() == 1 && p.tables.front().member.size() == 1));
  CHECK(p.tables.front().member[0].isBaseType);
  CHECK_FALSE(p.tables.front().member[0].isVector);
  CHECK(p.tables.front().member[0].pointer == Pointer::Plain);
  CHECK(p.tables.front().member[0].type == type);
  CHECK(p.tables.front().member[0].defaultValue == defValue);
}

TEST_CASE("Parsing packages", "[parser]")
{
  SECTION("Simple start up test ")
  {
    const auto p = parse(R"(
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
)");

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

  SECTION("base types")
  {
    checkBaseType("m:int;", "std::int32_t", "0");
    checkBaseType("m:byte;", "std::int8_t", "0");
    checkBaseType("m:short;", "std::int16_t", "0");
    checkBaseType("m:long;", "std::int64_t", "0");
    checkBaseType("m:unsigned;", "std::uint32_t", "0u");
    checkBaseType("m:unsigned int;", "std::uint32_t", "0u");
    checkBaseType("m:unsigned byte;", "std::uint8_t", "0u");
    checkBaseType("m:unsigned char;", "std::uint8_t", "0u");
    checkBaseType("m:unsigned short;", "std::uint16_t", "0u");
    checkBaseType("m:unsigned long;", "std::uint64_t", "0u");

    checkBaseType("m: i8;", "std::int8_t", "0");
    checkBaseType("m:i16;", "std::int16_t", "0");
    checkBaseType("m:i32;", "std::int32_t", "0");
    checkBaseType("m:i64;", "std::int64_t", "0");
    checkBaseType("m: ui8;", "std::uint8_t", "0u");
    checkBaseType("m:ui16;", "std::uint16_t", "0u");
    checkBaseType("m:ui32;", "std::uint32_t", "0u");
    checkBaseType("m:ui64;", "std::uint64_t", "0u");

    checkBaseType("m:int8_t;", "std::int8_t", "0");
    checkBaseType("m:int16_t;", "std::int16_t", "0");
    checkBaseType("m:int32_t;", "std::int32_t", "0");
    checkBaseType("m:int64_t;", "std::int64_t", "0");
    checkBaseType("m:uint8_t;", "std::uint8_t", "0u");
    checkBaseType("m:uint16_t;", "std::uint16_t", "0u");
    checkBaseType("m:uint32_t;", "std::uint32_t", "0u");
    checkBaseType("m:uint64_t;", "std::uint64_t", "0u");

    checkBaseType("m:float;", "float", "0.0f");
    checkBaseType("m:f32;", "float", "0.0f");

    checkBaseType("m:double;", "double", "0.0");
    checkBaseType("m:f64;", "double", "0.0");

    checkBaseType("m:bool;", "bool", "false");

    checkBaseType("m:string;", "std::string", "");
  }

  SECTION("Pointer types")
  {
    const auto p = parse(R"(
package Scope;
version "0.0";
root_type PointerTypes;

table Test { a:int = 42; }

table PointerTypes {
  a:unique Test;
  b:shared Test;
  c:weak Test;
})");

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
    const auto p = parse(R"(
package Scope;
version "0.0";
root_type VectorTypes;

table Test { a:int = 42; }

table VectorTypes {
  a:[unique Test];
  b:[shared Test];
  c:[weak Test];
  d:[float];
})");

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
    const auto p = parse(R"(
package Scope;
version "0.0";
root_type VectorTypes;

enum Test { a, b, c }

table VectorTypes {
  a:Test;
})");

    CHECK((p.enums.size() == 1 && p.enums.back().entries.size() == 3));
    CHECK((p.tables.size() == 1 && p.tables.back().member.size() == 1));
  }

  SECTION("Defautl values for enums")
  {
    const auto p = parse(R"(
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
})");

    REQUIRE((p.enums.size() == 1 && p.enums.back().entries.size() == 4));
    REQUIRE((p.tables.size() == 1 && p.tables.back().member.size() == 2));

    CHECK(p.tables[0].member[0].defaultValue == "Scope::EnumTypes::alpha");
    CHECK(p.tables[0].member[1].defaultValue == "Scope::EnumTypes::delta");
  }

  SECTION("empty table or enum")
  {
    parse("table Dummy {}");
    parse("enum Dummy {}");
  }

  SECTION("trailing ',' enum") { parse("enum Dummy {a,b,}"); }

  SECTION("base type names with more than one identifier") { parse("table Dummy {a:unsigned int;}"); }
}
