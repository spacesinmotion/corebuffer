#define CATCH_CONFIG_FAST_COMPILE
#include "catch2/catch.hpp"

#include "tabletypes.h"

#include <sstream>

using namespace Scope;

TEST_CASE("TableType test", "[output, table]")
{
  SECTION("Compare operator")
  {
    CHECK(TableA() == TableA());
    CHECK_FALSE(TableA() != TableA());

    CHECK(TableB() == TableB());
    CHECK_FALSE(TableB() != TableB());

    CHECK(TableC() == TableC());
    CHECK_FALSE(TableC() != TableC());
  }

  SECTION("reading whats written")
  {
    TableC c;
    c.b.emplace_back();
    c.b.back().name = "TableB1";
    c.b.emplace_back();
    c.b.back().name = "TableB2";

    TableC cIn;
    CHECK_FALSE(c == cIn);

    std::stringstream sOut;
    TableC_io().WriteTableC(sOut, c);

    const auto buffer = sOut.str();
    std::stringstream sIn(buffer);

    TableC_io().ReadTableC(sIn, cIn);

    CHECK(c == cIn);
  }

  SECTION("reading whats written with shared data")
  {
    std::stringstream sOut;
    {
      TableC c;
      c.a.name = "TableA";
      c.a.d1.reset(new TableD());
      c.a.d1->name = "TableD_1";
      c.a.d3 = std::make_shared<TableD>();
      c.a.d3->name = "TableD_3";
      c.a.d2 = c.a.d3;
      c.a.d4 = c.a.d3;

      TableC_io().WriteTableC(sOut, c);
    }

    const auto buffer = sOut.str();
    std::stringstream sIn(buffer);

    TableC cIn;
    TableC_io().ReadTableC(sIn, cIn);

    CHECK(cIn.a.name == "TableA");

    REQUIRE(cIn.a.d1);
    REQUIRE(cIn.a.d2.lock());
    REQUIRE(cIn.a.d3);
    REQUIRE(cIn.a.d4);

    CHECK(cIn.a.d1->name == "TableD_1");
    CHECK(cIn.a.d2.lock()->name == "TableD_3");
    CHECK(cIn.a.d3->name == "TableD_3");
    CHECK(cIn.a.d4->name == "TableD_3");

    CHECK(cIn.a.d2.lock() == cIn.a.d3);
    CHECK(cIn.a.d4 == cIn.a.d3);
  }

  SECTION("reading whats written with shared data and cross releation ship")
  {
    std::stringstream sOut;

    {
      TableC c;
      c.a.name = "TableA";
      c.a.d3 = std::make_shared<TableD>();
      c.a.d3->name = "TableD_3";
      c.a.d3->a = std::make_shared<TableA>();
      c.a.d3->a->d2 = c.a.d3;
      c.a.d3->a->d3 = c.a.d3;
      c.a.d3->a->d4 = c.a.d3;

      TableC_io().WriteTableC(sOut, c);
    }

    const auto buffer = sOut.str();
    std::stringstream sIn(buffer);

    TableC cIn;
    TableC_io().ReadTableC(sIn, cIn);

    CHECK(cIn.a.name == "TableA");

    REQUIRE(cIn.a.d3);
    CHECK(cIn.a.d3->name == "TableD_3");

    REQUIRE(cIn.a.d3->a);
    REQUIRE(cIn.a.d3->a->d2.lock());
    REQUIRE(cIn.a.d3->a->d3);
    REQUIRE(cIn.a.d3->a->d4);
    CHECK(cIn.a.d3->a->d2.lock() == cIn.a.d3);
    CHECK(cIn.a.d3->a->d3 == cIn.a.d3);
    CHECK(cIn.a.d3->a->d4 == cIn.a.d3);
  }

  SECTION("reading whats written with shared data in vectors")
  {
    std::stringstream sOut;
    {
      TableC c;
      c.c.emplace_back(new TableB);
      c.c.back()->name = "TableB_c";
      c.d.emplace_back(new TableB);
      c.d.back()->name = "TableB_d";
      c.d.push_back(c.d.front());
      c.e.push_back(c.d.front());
      c.e.push_back(c.d.front());
      TableC_io().WriteTableC(sOut, c);
    }

    const auto buffer = sOut.str();
    std::stringstream sIn(buffer);

    TableC cIn;
    TableC_io().ReadTableC(sIn, cIn);

    REQUIRE(cIn.c.size() == 1);
    REQUIRE(cIn.d.size() == 2);
    REQUIRE(cIn.e.size() == 2);

    REQUIRE(cIn.c.front());
    REQUIRE(cIn.d[0]);
    REQUIRE(cIn.d[1]);
    REQUIRE(cIn.e[0].lock());
    REQUIRE(cIn.e[1].lock());

    CHECK(cIn.c.back()->name == "TableB_c");
    CHECK(cIn.d.back()->name == "TableB_d");
    CHECK(cIn.d[0] == cIn.d[1]);
    CHECK(cIn.e[1].lock() == cIn.d[1]);
    CHECK(cIn.e[1].lock() == cIn.d[1]);
  }

  SECTION("Compare operations")
  {
    TableD d1;
    TableD d2;
    CHECK(d1 == d2);
    CHECK_FALSE(d1 != d2);

    d1.name = "xxx";
    d2.name = "yyy";

    CHECK_FALSE(d1 == d2);
    CHECK(d1 != d2);
  }

  SECTION("Reading fails with wrong data")
  {
    TableC c;

    std::stringstream s1("FALSE");
    CHECK_FALSE(TableC_io().ReadTableC(s1, c));

    std::stringstream s2("COREfails");
    CHECK_FALSE(TableC_io().ReadTableC(s2, c));
  }

  SECTION("initializing methods with pointer")
  {
    auto d = std::make_shared<TableD>();

    auto _inited = TableA(d);

    CHECK(nullptr == _inited.d1);
    CHECK(nullptr == _inited.d2.lock());
    CHECK(d == _inited.d3);
    CHECK(nullptr == _inited.d4);

    _inited = TableA(d, std::unique_ptr<TableD>(new TableD));

    CHECK(nullptr != _inited.d1);
    CHECK(d == _inited.d2.lock());
    CHECK(nullptr == _inited.d3);
    CHECK(nullptr == _inited.d4);
  }

  SECTION("initializing methods vector of unique pointer")
  {
    std::vector<std::unique_ptr<TableB>> list;
    list.emplace_back(new TableB);
    list.emplace_back(new TableB);

    TableC _inited(std::move(list));

    CHECK(_inited.c.size() == 2);
  }

  SECTION("vector algorithm table")
  {
    TableC p;
    p.b.emplace_back("x");
    p.b.emplace_back("xx");
    p.b.emplace_back("xxx");
    REQUIRE(p.b.size() == 3);

    SECTION("generate")
    {
      std::string x("x");
      p.generate_b([&x]() { return TableB(x += "x"); });
      CHECK(p.b[0] == TableB("xx"));
      CHECK(p.b[1] == TableB("xxx"));
      CHECK(p.b[2] == TableB("xxxx"));
    }

    SECTION("fill")
    {
      p.fill_b(TableB("42"));

      CHECK(p.b[0] == TableB("42"));
      CHECK(p.b[1] == TableB("42"));
      CHECK(p.b[2] == TableB("42"));
    }

    SECTION("remove")
    {
      auto e = p.remove_b(TableB("xx"));

      CHECK(e == p.b.end() - 1);
      CHECK(3 == p.b.size());
      CHECK(p.b[0] == TableB("x"));
      CHECK(p.b[1] == TableB("xxx"));
    }

    SECTION("remove_if")
    {
      auto e = p.remove_b_if([](const TableB &b) { return b == TableB("xx"); });

      CHECK(e == p.b.end() - 1);
      CHECK(3 == p.b.size());
      CHECK(p.b[0] == TableB("x"));
      CHECK(p.b[1] == TableB("xxx"));
    }

    SECTION("erase")
    {
      p.erase_b(TableB("xx"));

      CHECK(2 == p.b.size());
      CHECK(p.b[0] == TableB("x"));
      CHECK(p.b[1] == TableB("xxx"));
    }

    SECTION("erase_if")
    {
      p.erase_b_if([](const TableB &b) { return b == TableB("xx"); });

      CHECK(2 == p.b.size());
      CHECK(p.b[0] == TableB("x"));
      CHECK(p.b[1] == TableB("xxx"));
    }

    SECTION("reverse")
    {
      p.reverse_b();

      REQUIRE(3 == p.b.size());
      CHECK(p.b[0] == TableB("xxx"));
      CHECK(p.b[1] == TableB("xx"));
      CHECK(p.b[2] == TableB("x"));
    }

    SECTION("rotate")
    {
      p.rotate_b(p.b.begin() + 1);

      REQUIRE(3 == p.b.size());
      CHECK(p.b[0] == TableB("xx"));
      CHECK(p.b[1] == TableB("xxx"));
      CHECK(p.b[2] == TableB("x"));
    }

    SECTION("sort with comparator")
    {
      p.b[0] = TableB("32");
      p.b[1] = TableB("124");
      p.b[2] = TableB("42");
      p.sort_b([](const TableB &l, const TableB &r) { return l.name > r.name; });

      REQUIRE(3 == p.b.size());
      CHECK(p.b[0] == TableB("42"));
      CHECK(p.b[1] == TableB("32"));
      CHECK(p.b[2] == TableB("124"));
    }

    SECTION("any of")
    {
      CHECK(p.any_of_b([](const TableB &x) { return x.name == "xx"; }));
      CHECK_FALSE(p.any_of_b([](const TableB &x) { return x.name == "42"; }));
    }

    SECTION("all of")
    {
      CHECK(p.all_of_b([](const TableB &x) { return !x.name.empty() && x.name[0] == 'x'; }));
      CHECK_FALSE(p.all_of_b([](const TableB &x) { return x.name.size() == 2; }));
    }

    SECTION("none of")
    {
      CHECK_FALSE(p.none_of_b([](const TableB &x) { return !x.name.empty() && x.name[0] == 'x'; }));
      CHECK_FALSE(p.none_of_b([](const TableB &x) { return x.name.size() == 2; }));
      CHECK(p.none_of_b([](const TableB &x) { return x.name.size() > 3; }));
    }

    SECTION("any of is")
    {
      CHECK(p.any_of_b_is(TableB("xx")));
      CHECK_FALSE(p.any_of_b_is(TableB("42")));
    }

    SECTION("all of are")
    {
      CHECK_FALSE(p.all_of_b_are(TableB("42")));
      p.fill_b(TableB("42"));
      CHECK(p.b.size() == 3);
      CHECK(p.all_of_b_are(TableB("42")));
    }

    SECTION("none of value")
    {
      CHECK_FALSE(p.none_of_b_is(TableB("x")));
      CHECK(p.none_of_b_is(TableB("42")));
    }

    SECTION("for each")
    {
      std::string x;
      p.for_each_b([&x](const TableB &i) { x += i.name; });
      CHECK(x == std::string("x") + "xx" + "xxx");
    }

    SECTION("find")
    {
      CHECK(p.find_in_b(TableB("xx").name) == p.b.begin() + 1);
      CHECK(p.find_in_b(TableB("42").name) == p.b.end());
    }

    SECTION("find if")
    {
      CHECK(p.find_in_b_if([](const TableB &x) { return x.name == "xx"; }) == p.b.begin() + 1);
      CHECK(p.find_in_b_if([](const TableB &x) { return x.name == "42"; }) == p.b.end());
    }

    SECTION("count")
    {
      CHECK(p.count_in_b(TableB("xx")) == 1);
      CHECK(p.count_in_b(TableB("42")) == 0);
    }

    SECTION("count if")
    {
      CHECK(p.count_in_b_if([](const TableB &x) { return x.name.size() % 2 == 1; }) == 2);
      CHECK(p.count_in_b_if([](const TableB &x) { return x.name.size() > 42; }) == 0);
    }
  }

  SECTION("vector algorithm unique table")
  {
    TableC p;
    p.c.emplace_back(new TableB("x"));
    p.c.emplace_back(new TableB("xx"));
    p.c.emplace_back(new TableB("xxx"));
    REQUIRE(p.c.size() == 3);

    SECTION("generate")
    {
      std::string x("x");
      p.generate_c([&x]() { return std::unique_ptr<TableB>(new TableB(x += "x")); });
      CHECK(p.c[0]->name == "xx");
      CHECK(p.c[1]->name == "xxx");
      CHECK(p.c[2]->name == "xxxx");
    }

    SECTION("remove")
    {
      auto e = p.remove_c(p.c[1]);

      CHECK(e == p.c.end() - 1);
      CHECK(3 == p.c.size());
      CHECK(p.c[0]->name == "x");
      CHECK(p.c[1]->name == "xxx");
    }

    SECTION("remove_if")
    {
      auto e = p.remove_c_if([](const std::unique_ptr<TableB> &b) { return *b == TableB("xx"); });

      CHECK(e == p.c.end() - 1);
      CHECK(3 == p.c.size());
      CHECK(p.c[0]->name == "x");
      CHECK(p.c[1]->name == "xxx");
    }

    SECTION("erase")
    {
      p.erase_c(p.c[1]);

      CHECK(2 == p.c.size());
      CHECK(p.c[0]->name == "x");
      CHECK(p.c[1]->name == "xxx");
    }

    SECTION("erase_if")
    {
      p.erase_c_if([](const std::unique_ptr<TableB> &b) { return *b == TableB("xx"); });

      CHECK(2 == p.c.size());
      CHECK(p.c[0]->name == "x");
      CHECK(p.c[1]->name == "xxx");
    }

    SECTION("reverse")
    {
      p.reverse_c();

      REQUIRE(3 == p.c.size());
      CHECK(p.c[0]->name == "xxx");
      CHECK(p.c[1]->name == "xx");
      CHECK(p.c[2]->name == "x");
    }

    SECTION("rotate")
    {
      p.rotate_c(p.c.begin() + 1);

      REQUIRE(3 == p.c.size());
      CHECK(p.c[0]->name == "xx");
      CHECK(p.c[1]->name == "xxx");
      CHECK(p.c[2]->name == "x");
    }

    SECTION("sort with comparator")
    {
      p.c[0] = std::unique_ptr<TableB>(new TableB("32"));
      p.c[1] = std::unique_ptr<TableB>(new TableB("124"));
      p.c[2] = std::unique_ptr<TableB>(new TableB("42"));
      p.sort_c([](const std::unique_ptr<TableB> &l, const std::unique_ptr<TableB> &r) { return l->name > r->name; });

      REQUIRE(3 == p.c.size());
      CHECK(p.c[0]->name == "42");
      CHECK(p.c[1]->name == "32");
      CHECK(p.c[2]->name == "124");
    }

    SECTION("any of")
    {
      CHECK(p.any_of_c([](const std::unique_ptr<TableB> &x) { return x->name == "xx"; }));
      CHECK_FALSE(p.any_of_c([](const std::unique_ptr<TableB> &x) { return x->name == "42"; }));
    }

    SECTION("all of")
    {
      CHECK(p.all_of_c([](const std::unique_ptr<TableB> &x) { return !x->name.empty() && x->name[0] == 'x'; }));
      CHECK_FALSE(p.all_of_c([](const std::unique_ptr<TableB> &x) { return x->name.size() == 2; }));
    }

    SECTION("none of")
    {
      CHECK_FALSE(p.none_of_c([](const std::unique_ptr<TableB> &x) { return !x->name.empty() && x->name[0] == 'x'; }));
      CHECK_FALSE(p.none_of_c([](const std::unique_ptr<TableB> &x) { return x->name.size() == 2; }));
      CHECK(p.none_of_c([](const std::unique_ptr<TableB> &x) { return x->name.size() > 3; }));
    }

    SECTION("any of is")
    {
      p.c.push_back(nullptr);
      CHECK(p.any_of_c_is(TableB("xx")));
      CHECK_FALSE(p.any_of_c_is(TableB("42")));
    }

    SECTION("all of are")
    {
      p.c.push_back(nullptr);
      CHECK_FALSE(p.all_of_c_are(TableB("42")));
      for (auto &c : p.c)
        if (c)
          c->name = "42";
      CHECK_FALSE(p.all_of_c_are(TableB("42")));
      p.c.pop_back();
      CHECK(p.all_of_c_are(TableB("42")));
    }

    SECTION("none of value")
    {
      p.c.push_back(nullptr);
      CHECK_FALSE(p.none_of_c_is(TableB("x")));
      CHECK(p.none_of_c_is(TableB("42")));
    }

    SECTION("for each")
    {
      std::string x;
      p.for_each_c([&x](const std::unique_ptr<TableB> &i) { x += i->name; });
      CHECK(x == std::string("x") + "xx" + "xxx");
    }

    SECTION("find")
    {
      CHECK(p.find_in_c(p.c[1]) == p.c.begin() + 1);
      CHECK(p.find_in_c(nullptr) == p.c.end());
    }

    SECTION("find if")
    {
      CHECK(p.find_in_c_if([](const std::unique_ptr<TableB> &x) { return x->name == "xx"; }) == p.c.begin() + 1);
      CHECK(p.find_in_c_if([](const std::unique_ptr<TableB> &x) { return x->name == "42"; }) == p.c.end());
    }

    SECTION("count")
    {
      CHECK(p.count_in_c(p.c[1]) == 1);
      CHECK(p.count_in_c(nullptr) == 0);
    }

    SECTION("count if")
    {
      CHECK(p.count_in_c_if([](const std::unique_ptr<TableB> &x) { return x->name.size() % 2 == 1; }) == 2);
      CHECK(p.count_in_c_if([](const std::unique_ptr<TableB> &x) { return x->name.size() > 42; }) == 0);
    }
  }
}
