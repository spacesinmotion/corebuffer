#define CATCH_CONFIG_FAST_COMPILE
#include "catch2/catch.hpp"

#include "parser.h"

Package parse(const std::string &source)
{
  Package p;
  REQUIRE_NOTHROW(Parser(source, p).parse());
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

  REQUIRE((p.types.size() == 1 && p.types.front().is_Table() && p.types.front().as_Table().member.size() == 1));
  CHECK(p.types.front().as_Table().member[0].isBaseType);
  CHECK_FALSE(p.types.front().as_Table().member[0].isVector);
  CHECK(p.types.front().as_Table().member[0].pointer == Pointer::Plain);
  CHECK(p.types.front().as_Table().member[0].type == type);
  CHECK(p.types.front().as_Table().member[0].defaultValue.value == defValue);
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
      REQUIRE((p.types.size() == 3 && p.types.back().is_Enum()));
      CHECK(p.types.back().as_Enum().name == "Access");

      REQUIRE(p.types.back().as_Enum().entries.size() == 3);
      CHECK(p.types.back().as_Enum().entries[0].name == "Private");
      CHECK(p.types.back().as_Enum().entries[0].value == 0);
      CHECK(p.types.back().as_Enum().entries[1].name == "Public");
      CHECK(p.types.back().as_Enum().entries[1].value == 3);
      CHECK(p.types.back().as_Enum().entries[2].name == "Protected");
      CHECK(p.types.back().as_Enum().entries[2].value == 4);
    }

    SECTION("Checking parsed XXX")
    {
      REQUIRE(p.types.size() == 3);
      REQUIRE(p.types[0].is_Table());
      REQUIRE(p.types[1].is_Table());

      CHECK(p.types[0].as_Table().name == "XXX");
      CHECK(p.types[0].as_Table().isComplexType);

      REQUIRE(p.types[0].as_Table().member.size() == 4);

      CHECK(p.types[0].as_Table().member[0].isBaseType);
      CHECK(!p.types[0].as_Table().member[0].isVector);
      CHECK(p.types[0].as_Table().member[0].name == "bill");
      CHECK(p.types[0].as_Table().member[0].type == "std::int32_t");
      CHECK(p.types[0].as_Table().member[0].defaultValue.value == "1");
      CHECK(p.types[0].as_Table().member[0].pointer == Pointer::Plain);

      CHECK(!p.types[0].as_Table().member[1].isVector);
      CHECK(p.types[0].as_Table().member[1].isBaseType);
      CHECK(p.types[0].as_Table().member[1].name == "aul");
      CHECK(p.types[0].as_Table().member[1].type == "bool");
      CHECK(p.types[0].as_Table().member[1].defaultValue.value == "false");
      CHECK(p.types[0].as_Table().member[1].pointer == Pointer::Plain);

      CHECK(!p.types[0].as_Table().member[2].isVector);
      CHECK(p.types[0].as_Table().member[2].isBaseType);
      CHECK(p.types[0].as_Table().member[2].name == "kaul");
      CHECK(p.types[0].as_Table().member[2].type == "float");
      CHECK(p.types[0].as_Table().member[2].defaultValue.value == "0.0f");
      CHECK(p.types[0].as_Table().member[2].pointer == Pointer::Plain);

      CHECK(p.types[0].as_Table().member[3].isVector);
      CHECK(p.types[0].as_Table().member[3].isBaseType);
      CHECK(p.types[0].as_Table().member[3].name == "baul");
      CHECK(p.types[0].as_Table().member[3].type == "float");
      CHECK(p.types[0].as_Table().member[3].defaultValue.value.empty());
      CHECK(p.types[0].as_Table().member[3].pointer == Pointer::Plain);
    }

    SECTION("Checking parsed XXX")
    {
      REQUIRE(p.types.size() == 3);
      REQUIRE(p.types[0].is_Table());
      REQUIRE(p.types[1].is_Table());

      CHECK(p.types[1].as_Table().name == "YYY");

      REQUIRE(p.types[1].as_Table().member.size() == 2);

      CHECK(!p.types[1].as_Table().member[0].isVector);
      CHECK(!p.types[1].as_Table().member[0].isBaseType);
      CHECK(p.types[1].as_Table().member[0].name == "still");
      CHECK(p.types[1].as_Table().member[0].type == "XXX");
      CHECK(p.types[1].as_Table().member[0].defaultValue.value.empty());
      CHECK(p.types[1].as_Table().member[0].pointer == Pointer::Unique);

      CHECK(p.types[1].as_Table().member[1].isVector);
      CHECK(!p.types[1].as_Table().member[1].isBaseType);
      CHECK(p.types[1].as_Table().member[1].name == "sub");
      CHECK(p.types[1].as_Table().member[1].type == "YYY");
      CHECK(p.types[1].as_Table().member[1].defaultValue.value.empty());
      CHECK(p.types[1].as_Table().member[1].pointer == Pointer::Shared);
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

    REQUIRE((p.types.size() == 2 && p.types.back().is_Table() && p.types.back().as_Table().member.size() == 3));

    CHECK(!p.types.back().as_Table().member[0].isBaseType);
    CHECK(p.types.back().as_Table().member[0].pointer == Pointer::Unique);

    CHECK(!p.types.back().as_Table().member[1].isBaseType);
    CHECK(p.types.back().as_Table().member[1].pointer == Pointer::Shared);

    CHECK(!p.types.back().as_Table().member[2].isBaseType);
    CHECK(p.types.back().as_Table().member[2].pointer == Pointer::Weak);
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

    REQUIRE((p.types.size() == 2 && p.types.back().is_Table() && p.types.back().as_Table().member.size() == 4));

    CHECK(p.types.back().as_Table().isComplexType);

    CHECK(!p.types.back().as_Table().member[0].isBaseType);
    CHECK(p.types.back().as_Table().member[0].isVector);
    CHECK(p.types.back().as_Table().member[0].pointer == Pointer::Unique);

    CHECK(!p.types.back().as_Table().member[1].isBaseType);
    CHECK(p.types.back().as_Table().member[1].isVector);
    CHECK(p.types.back().as_Table().member[1].pointer == Pointer::Shared);

    CHECK(!p.types.back().as_Table().member[2].isBaseType);
    CHECK(p.types.back().as_Table().member[2].isVector);
    CHECK(p.types.back().as_Table().member[2].pointer == Pointer::Weak);

    CHECK(p.types.back().as_Table().member[3].isBaseType);
    CHECK(p.types.back().as_Table().member[3].isVector);
    CHECK(p.types.back().as_Table().member[3].pointer == Pointer::Plain);
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

    REQUIRE(p.types.size() == 2);
    REQUIRE(p.types.front().is_Enum());
    REQUIRE(p.types.back().is_Table());
    CHECK(p.types.front().as_Enum().entries.size() == 3);
    CHECK(p.types.back().as_Table().member.size() == 1);
    CHECK_FALSE(p.types.back().as_Table().isComplexType);
  }

  SECTION("default values for enums")
  {
    const auto p = parse(R"(
package Scope.Sub;
version "0.0";
root_type Dummy;

enum EnumTypes { alpha, beta = 4, gamma, delta = 42 }

table Dummy
{
  en1:EnumTypes;
  en2:EnumTypes=delta;
})");

    REQUIRE((p.types.size() == 2 && p.types.front().is_Enum() && p.types.front().as_Enum().entries.size() == 4));
    REQUIRE((p.types.size() == 2 && p.types.back().is_Table() && p.types.back().as_Table().member.size() == 2));

    CHECK(p.types[1].as_Table().member[0].defaultValue.value == "Scope::Sub::EnumTypes::alpha");
    CHECK(p.types[1].as_Table().member[1].defaultValue.value == "Scope::Sub::EnumTypes::delta");
    CHECK_FALSE(p.types[1].as_Table().isComplexType);
  }

  SECTION("default values for enums without package")
  {
    const auto p = parse(R"(
version "0.0";
root_type Dummy;

enum EnumTypes { alpha, beta = 4, gamma, delta = 42 }

table Dummy
{
  en1:EnumTypes;
  en2:EnumTypes=delta;
})");

    REQUIRE((p.types.size() == 2 && p.types.front().is_Enum() && p.types.front().as_Enum().entries.size() == 4));
    REQUIRE((p.types.size() == 2 && p.types.back().is_Table() && p.types.back().as_Table().member.size() == 2));

    CHECK(p.types[1].as_Table().member[0].defaultValue.value == "EnumTypes::alpha");
    CHECK(p.types[1].as_Table().member[1].defaultValue.value == "EnumTypes::delta");
    CHECK_FALSE(p.types[1].as_Table().isComplexType);
  }

  SECTION("empty table or enum")
  {
    parse("table Dummy {}");
    parse("enum Dummy {}");
  }

  SECTION("default values for bool")
  {
    auto p = parse(
        "table A {\n"
        "a:bool = false;\n"
        "b:bool = true;\n"
        "}");
    REQUIRE(p.types.size() == 1);
    REQUIRE(p.types.front().is_Table());
    CHECK_FALSE(p.types.front().as_Table().isComplexType);
  }

  SECTION("vectors are complex")
  {
    auto p = parse(
        "table A {\n"
        "a:[int];\n"
        "}");
    REQUIRE(p.types.size() == 1);
    REQUIRE(p.types.front().is_Table());
    CHECK(p.types.front().as_Table().isComplexType);
  }

  SECTION("pointers are complex")
  {
    auto p = parse(
        "table B{}"
        "table A {\n"
        "a:shared B;\n"
        "}");
    REQUIRE(p.types.size() == 2);
    REQUIRE(p.types.back().is_Table());
    CHECK(p.types.back().as_Table().isComplexType);
  }

  SECTION("recursive table is complex")
  {
    auto p = parse(
        "table A {\n"
        "a:shared A;\n"
        "}");
    REQUIRE(p.types.size() == 1);
    REQUIRE(p.types.back().is_Table());
    CHECK(p.types.back().as_Table().isComplexType);
  }

  SECTION("strings are complex")
  {
    auto p = parse(
        "table A {\n"
        "a:string;\n"
        "}");
    REQUIRE(p.types.size() == 1);
    REQUIRE(p.types.back().is_Table());
    CHECK(p.types.back().as_Table().isComplexType);
  }

  SECTION("trailing ',' enum") { parse("enum Dummy {a,b,}"); }

  SECTION("base type names with more than one identifier") { parse("table Dummy {a:unsigned int;}"); }

  SECTION("comments available")
  {
    parse(
        "table /* comment */ Dummy { //line comment \n"
        "a:unsigned int;\n"
        "} /* at end */");

    parse(
        "table D { a:unsigned int;} \n"
        "// line comment at end");
  }

  SECTION("table special function parsing")
  {
    SECTION("special initializer parsing")
    {
      const auto p = parse(
          "table Dummy {\n"
          "  a:int;\n"
          "  b:float;\n"
          "  init(a,b);\n"
          "  init(b);\n"
          "  init();\n"
          "}");

      REQUIRE((p.types.size() == 1 && p.types[0].is_Table()));

      REQUIRE(p.types[0].as_Table().methods.size() == 3);
      CHECK(p.types[0].as_Table().methods[0].name == "init");
      CHECK(p.types[0].as_Table().methods[1].name == "init");
      CHECK(p.types[0].as_Table().methods[2].name == "init");

      REQUIRE(p.types[0].as_Table().methods[0].parameter.size() == 2);
      CHECK(p.types[0].as_Table().methods[0].parameter[0].name == "a");
      CHECK(p.types[0].as_Table().methods[0].parameter[1].name == "b");

      CHECK(p.types[0].as_Table().methods[1].parameter.size() == 1);
      CHECK(p.types[0].as_Table().methods[2].parameter.empty());
    }
  }

  SECTION("union tables")
  {
    SECTION("basic parsing")
    {
      parse("union U { }\n");
      parse("union U { A}\n");
      parse("union U { A, B }\n");
      parse("union U { A, B, }\n");
    }

    SECTION("parsing content test")
    {
      const auto p = parse("union U { A, B }\n");
      REQUIRE((p.types.size() == 1 && p.types[0].is_Union()));
      CHECK(p.types[0].as_Union().name == "U");

      REQUIRE(p.types[0].as_Union().tables.size() == 2);
      CHECK(p.types[0].as_Union().tables[0].value == "A");
      CHECK(p.types[0].as_Union().tables[1].value == "B");
    }
  }

  SECTION("flags")
  {
    SECTION("basic parsing")
    {
      parse("flag U { }\n");
      parse("flag U { a }\n");
      parse("flag U { v, x }\n");
      parse("flag U { b, a, }\n");
    }

    SECTION("parsing content test")
    {
      const auto p = parse("flag U { a, b }\n");
      REQUIRE((p.types.size() == 1 && p.types[0].is_Flag()));
      CHECK(p.types[0].as_Flag().name == "U");

      REQUIRE(p.types[0].as_Flag().entries.size() == 2);
      CHECK(p.types[0].as_Flag().entries[0].value == "a");
      CHECK(p.types[0].as_Flag().entries[1].value == "b");
    }
  }
}
