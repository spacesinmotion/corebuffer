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

  SECTION("vector algorithm shared table")
  {
    TableC p;
    p.d.emplace_back(new TableB("x"));
    p.d.emplace_back(new TableB("xx"));
    p.d.emplace_back(new TableB("xxx"));
    REQUIRE(p.d.size() == 3);

    SECTION("fill")
    {
      p.fill_d(std::make_shared<TableB>("42"));

      CHECK(p.d[0] == p.d[1]);
      CHECK(p.d[0] == p.d[2]);
      CHECK(*p.d[2] == TableB("42"));
    }

    SECTION("generate")
    {
      std::string x("x");
      p.generate_d([&x]() { return std::make_shared<TableB>(x += "x"); });
      CHECK(p.d[0]->name == "xx");
      CHECK(p.d[1]->name == "xxx");
      CHECK(p.d[2]->name == "xxxx");
    }

    SECTION("remove")
    {
      auto e = p.remove_d(p.d[1]);

      CHECK(e == p.d.end() - 1);
      CHECK(3 == p.d.size());
      CHECK(p.d[0]->name == "x");
      CHECK(p.d[1]->name == "xxx");
    }

    SECTION("remove_if")
    {
      auto e = p.remove_d_if([](const std::shared_ptr<TableB> &b) { return *b == TableB("xx"); });

      CHECK(e == p.d.end() - 1);
      CHECK(3 == p.d.size());
      CHECK(p.d[0]->name == "x");
      CHECK(p.d[1]->name == "xxx");
    }

    SECTION("erase")
    {
      p.erase_d(p.d[1]);

      CHECK(2 == p.d.size());
      CHECK(p.d[0]->name == "x");
      CHECK(p.d[1]->name == "xxx");
    }

    SECTION("erase_if")
    {
      p.erase_d_if([](const std::shared_ptr<TableB> &b) { return *b == TableB("xx"); });

      CHECK(2 == p.d.size());
      CHECK(p.d[0]->name == "x");
      CHECK(p.d[1]->name == "xxx");
    }

    SECTION("reverse")
    {
      p.reverse_d();

      REQUIRE(3 == p.d.size());
      CHECK(p.d[0]->name == "xxx");
      CHECK(p.d[1]->name == "xx");
      CHECK(p.d[2]->name == "x");
    }

    SECTION("rotate")
    {
      p.rotate_d(p.d.begin() + 1);

      REQUIRE(3 == p.d.size());
      CHECK(p.d[0]->name == "xx");
      CHECK(p.d[1]->name == "xxx");
      CHECK(p.d[2]->name == "x");
    }

    SECTION("sort with comparator")
    {
      p.d[0] = std::shared_ptr<TableB>(new TableB("32"));
      p.d[1] = std::shared_ptr<TableB>(new TableB("124"));
      p.d[2] = std::shared_ptr<TableB>(new TableB("42"));
      p.sort_d([](const std::shared_ptr<TableB> &l, const std::shared_ptr<TableB> &r) { return l->name > r->name; });

      REQUIRE(3 == p.d.size());
      CHECK(p.d[0]->name == "42");
      CHECK(p.d[1]->name == "32");
      CHECK(p.d[2]->name == "124");
    }

    SECTION("any of")
    {
      CHECK(p.any_of_d([](const std::shared_ptr<TableB> &x) { return x->name == "xx"; }));
      CHECK_FALSE(p.any_of_d([](const std::shared_ptr<TableB> &x) { return x->name == "42"; }));
    }

    SECTION("all of")
    {
      CHECK(p.all_of_d([](const std::shared_ptr<TableB> &x) { return !x->name.empty() && x->name[0] == 'x'; }));
      CHECK_FALSE(p.all_of_d([](const std::shared_ptr<TableB> &x) { return x->name.size() == 2; }));
    }

    SECTION("none of")
    {
      CHECK_FALSE(p.none_of_d([](const std::shared_ptr<TableB> &x) { return !x->name.empty() && x->name[0] == 'x'; }));
      CHECK_FALSE(p.none_of_d([](const std::shared_ptr<TableB> &x) { return x->name.size() == 2; }));
      CHECK(p.none_of_d([](const std::shared_ptr<TableB> &x) { return x->name.size() > 3; }));
    }

    SECTION("any of is")
    {
      p.d.push_back(nullptr);
      CHECK(p.any_of_d_is(TableB("xx")));
      CHECK_FALSE(p.any_of_d_is(TableB("42")));
    }

    SECTION("any of is pointer")
    {
      p.d.push_back(nullptr);
      CHECK(p.any_of_d_is(p.d[1]));
      CHECK_FALSE(p.any_of_d_is(std::make_shared<TableB>("42")));
    }

    SECTION("all of are")
    {
      p.d.push_back(nullptr);
      CHECK_FALSE(p.all_of_d_are(TableB("42")));
      for (auto &c : p.d)
        if (c)
          c->name = "42";
      CHECK_FALSE(p.all_of_d_are(TableB("42")));
      p.d.pop_back();
      CHECK(p.all_of_d_are(TableB("42")));
    }

    SECTION("all of are pointer")
    {
      p.d.push_back(nullptr);
      CHECK_FALSE(p.all_of_d_are(p.d[1]));
      CHECK_FALSE(p.all_of_d_are(std::make_shared<TableB>("42")));
      p.d = {p.d[2], p.d[2], p.d[2]};
      CHECK(p.all_of_d_are(p.d[2]));
    }

    SECTION("none of value")
    {
      p.d.push_back(nullptr);
      CHECK_FALSE(p.none_of_d_is(TableB("x")));
      CHECK(p.none_of_d_is(TableB("42")));
    }

    SECTION("none of value pointer")
    {
      p.d.push_back(nullptr);
      CHECK_FALSE(p.none_of_d_is(p.d[1]));
      CHECK(p.none_of_d_is(std::make_shared<TableB>("42")));
    }

    SECTION("for each")
    {
      std::string x;
      p.for_each_d([&x](const std::shared_ptr<TableB> &i) { x += i->name; });
      CHECK(x == std::string("x") + "xx" + "xxx");
    }

    SECTION("find")
    {
      CHECK(p.find_in_d(p.d[1]) == p.d.begin() + 1);
      CHECK(p.find_in_d(nullptr) == p.d.end());
    }

    SECTION("find if")
    {
      CHECK(p.find_in_d_if([](const std::shared_ptr<TableB> &x) { return x->name == "xx"; }) == p.d.begin() + 1);
      CHECK(p.find_in_d_if([](const std::shared_ptr<TableB> &x) { return x->name == "42"; }) == p.d.end());
    }

    SECTION("count")
    {
      CHECK(p.count_in_d(p.d[1]) == 1);
      CHECK(p.count_in_d(nullptr) == 0);
    }

    SECTION("count if")
    {
      CHECK(p.count_in_d_if([](const std::shared_ptr<TableB> &x) { return x->name.size() % 2 == 1; }) == 2);
      CHECK(p.count_in_d_if([](const std::shared_ptr<TableB> &x) { return x->name.size() > 42; }) == 0);
    }
  }

  SECTION("vector algorithm weak table")
  {
    TableC p;
    p.d.emplace_back(new TableB("x"));
    p.d.emplace_back(new TableB("xx"));
    p.d.emplace_back(new TableB("xxx"));
    p.e.emplace_back(p.d[0]);
    p.e.emplace_back(p.d[1]);
    p.e.emplace_back(p.d[2]);
    REQUIRE(p.e.size() == 3);

    SECTION("fill")
    {
      const auto x = std::make_shared<TableB>("42");
      p.fill_e(x);

      CHECK(p.e[0].lock() == p.e[1].lock());
      CHECK(p.e[0].lock() == p.e[2].lock());
      CHECK(p.e[2].lock() == x);
    }

    SECTION("generate")
    {
      std::string x("x");
      p.generate_e([&]() {
        p.d.emplace_back(new TableB(x += "x"));
        return p.d.back();
      });
      CHECK(p.e[0].lock()->name == "xx");
      CHECK(p.e[1].lock()->name == "xxx");
      CHECK(p.e[2].lock()->name == "xxxx");
      CHECK(p.e[0].lock() == p.d[3]);
      CHECK(p.e[1].lock() == p.d[4]);
      CHECK(p.e[2].lock() == p.d[5]);
    }

    SECTION("remove")
    {
      auto e = p.remove_e(p.e[1]);

      CHECK(e == p.e.end() - 1);
      CHECK(3 == p.e.size());
      CHECK(p.e[0].lock()->name == "x");
      CHECK(p.e[1].lock()->name == "xxx");
    }

    SECTION("remove_if")
    {
      auto e = p.remove_e_if([](const std::weak_ptr<TableB> &b) { return b.lock() && *b.lock() == TableB("xx"); });

      CHECK(e == p.e.end() - 1);
      CHECK(3 == p.e.size());
      CHECK(p.e[0].lock()->name == "x");
      CHECK(p.e[1].lock()->name == "xxx");
    }

    SECTION("erase")
    {
      p.erase_e(p.e[1]);

      CHECK(2 == p.e.size());
      CHECK(p.e[0].lock()->name == "x");
      CHECK(p.e[1].lock()->name == "xxx");
    }

    SECTION("erase_if")
    {
      p.erase_e_if([](const std::weak_ptr<TableB> &b) { return b.lock() && *b.lock() == TableB("xx"); });

      CHECK(2 == p.e.size());
      CHECK(p.e[0].lock()->name == "x");
      CHECK(p.e[1].lock()->name == "xxx");
    }

    SECTION("reverse")
    {
      p.reverse_e();

      REQUIRE(3 == p.e.size());
      CHECK(p.e[0].lock()->name == "xxx");
      CHECK(p.e[1].lock()->name == "xx");
      CHECK(p.e[2].lock()->name == "x");
    }

    SECTION("rotate")
    {
      p.rotate_e(p.e.begin() + 1);

      REQUIRE(3 == p.e.size());
      CHECK(p.e[0].lock()->name == "xx");
      CHECK(p.e[1].lock()->name == "xxx");
      CHECK(p.e[2].lock()->name == "x");
    }

    SECTION("sort with comparator")
    {
      p.d[0]->name = "32";
      p.d[1]->name = "124";
      p.d[2]->name = "42";
      p.sort_e([](const std::weak_ptr<TableB> &l, const std::weak_ptr<TableB> &r) {
        return l.lock() && r.lock() && l.lock()->name > r.lock()->name;
      });

      REQUIRE(3 == p.e.size());
      CHECK(p.e[0].lock()->name == "42");
      CHECK(p.e[1].lock()->name == "32");
      CHECK(p.e[2].lock()->name == "124");
    }

    SECTION("any of")
    {
      CHECK(p.any_of_e([](const std::weak_ptr<TableB> &x) { return x.lock()->name == "xx"; }));
      CHECK_FALSE(p.any_of_e([](const std::weak_ptr<TableB> &x) { return x.lock()->name == "42"; }));
    }

    SECTION("all of")
    {
      CHECK(p.all_of_e(
          [](const std::weak_ptr<TableB> &x) { return !x.lock()->name.empty() && x.lock()->name[0] == 'x'; }));
      CHECK_FALSE(p.all_of_e([](const std::weak_ptr<TableB> &x) { return x.lock()->name.size() == 2; }));
    }

    SECTION("none of")
    {
      CHECK_FALSE(p.none_of_e(
          [](const std::weak_ptr<TableB> &x) { return !x.lock()->name.empty() && x.lock()->name[0] == 'x'; }));
      CHECK_FALSE(p.none_of_e([](const std::weak_ptr<TableB> &x) { return x.lock()->name.size() == 2; }));
      CHECK(p.none_of_e([](const std::weak_ptr<TableB> &x) { return x.lock()->name.size() > 3; }));
    }

    SECTION("any of is")
    {
      p.e.push_back(std::make_shared<TableB>());
      CHECK(p.e.back().lock() == nullptr);
      CHECK(p.any_of_e_is(TableB("xx")));
      CHECK_FALSE(p.any_of_e_is(TableB("42")));
    }

    SECTION("any of is pointer")
    {
      p.e.push_back(std::make_shared<TableB>());
      CHECK(p.e.back().lock() == nullptr);
      CHECK(p.any_of_e_is(p.e[1].lock()));
      CHECK_FALSE(p.any_of_e_is(std::make_shared<TableB>("42")));
    }

    SECTION("all of are")
    {
      p.e.push_back(std::make_shared<TableB>());
      CHECK(p.e.back().lock() == nullptr);
      CHECK_FALSE(p.all_of_e_are(TableB("42")));
      for (auto &c : p.e)
        if (c.lock())
          c.lock()->name = "42";
      CHECK_FALSE(p.all_of_e_are(TableB("42")));
      p.e.pop_back();
      CHECK(p.all_of_e_are(TableB("42")));
    }

    SECTION("all of are pointer")
    {
      p.e.push_back(std::make_shared<TableB>());
      CHECK(p.e.back().lock() == nullptr);
      CHECK_FALSE(p.all_of_e_are(p.e[1].lock()));
      CHECK_FALSE(p.all_of_e_are(std::make_shared<TableB>("42")));
      p.e = {p.e[2], p.e[2], p.e[2]};
      CHECK(p.all_of_e_are(p.e[2].lock()));
    }

    SECTION("none of value")
    {
      p.e.push_back(std::make_shared<TableB>());
      CHECK(p.e.back().lock() == nullptr);
      CHECK_FALSE(p.none_of_e_is(TableB("x")));
      CHECK(p.none_of_e_is(TableB("42")));
    }

    SECTION("none of value pointer")
    {
      p.e.push_back(std::make_shared<TableB>());
      CHECK(p.e.back().lock() == nullptr);
      CHECK_FALSE(p.none_of_e_is(p.e[1].lock()));
      CHECK(p.none_of_e_is(std::make_shared<TableB>("42")));
    }

    SECTION("for each")
    {
      std::string x;
      p.for_each_e([&x](const std::weak_ptr<TableB> &i) { x += i.lock()->name; });
      CHECK(x == std::string("x") + "xx" + "xxx");
    }

    SECTION("find")
    {
      CHECK(p.find_in_e(p.e[1]) == p.e.begin() + 1);
      // CHECK(p.find_in_e(nullptr) == p.e.end());
    }

    SECTION("find if")
    {
      CHECK(p.find_in_e_if([](const std::weak_ptr<TableB> &x) { return x.lock()->name == "xx"; }) == p.e.begin() + 1);
      CHECK(p.find_in_e_if([](const std::weak_ptr<TableB> &x) { return x.lock()->name == "42"; }) == p.e.end());
    }

    SECTION("count")
    {
      CHECK(p.count_in_e(p.e[1]) == 1);
      // CHECK(p.count_in_e(p.d[1]) == 0);
    }

    SECTION("count if")
    {
      CHECK(p.count_in_e_if([](const std::weak_ptr<TableB> &x) { return x.lock()->name.size() % 2 == 1; }) == 2);
      CHECK(p.count_in_e_if([](const std::weak_ptr<TableB> &x) { return x.lock()->name.size() > 42; }) == 0);
    }
  }
}
