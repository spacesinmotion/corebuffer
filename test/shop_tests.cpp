#define CATCH_CONFIG_FAST_COMPILE
#include "catch2/catch.hpp"

#include "shop.h"

#include <fstream>
#include <sstream>

TEST_CASE("Shop example test", "[output]")
{
  using namespace Example::Shop;

  SECTION("Usage example")
  {
    Shop s;

    auto kicks = std::make_shared<Item>();
    kicks->name = "Cool Kicks";
    kicks->price = 2.45f;
    s.items.emplace_back(kicks);

    auto john = std::make_shared<Customer>();
    john->name = "John Doe";
    john->address = "Street 4, TownTown";
    s.customers.emplace_back(john);

    s.orders.emplace_back();
    s.orders.back().customer = john;
    s.orders.back().items.emplace_back(kicks);

    std::ofstream fo("shop.dat");
    Shop_io().WriteShop(fo, s);
    fo.close();

    Shop si;
    std::ifstream fi("shop.dat");
    Shop_io().ReadShop(fi, si);
    fi.close();

    REQUIRE(si.orders.size() == 1);
    CHECK(si.orders.back().customer->name == "John Doe");
    REQUIRE(si.orders.back().items.size() == 1);
    CHECK(si.orders.back().items.front()->name == "Cool Kicks");
  }

  SECTION("error order to fill coverage")
  {
    Shop s;

    auto kicks = std::make_shared<Item>();
    kicks->name = "Cool Kicks";
    kicks->price = 2.45f;
    s.items.emplace_back(kicks);

    s.orders.emplace_back();
    s.orders.back().items.emplace_back(kicks);

    std::ofstream fo("shop_error.dat");
    Shop_io().WriteShop(fo, s);
    fo.close();

    Shop si;
    std::ifstream fi("shop_error.dat");
    Shop_io().ReadShop(fi, si);
    fi.close();

    REQUIRE(si.orders.size() == 1);
    CHECK(si.orders.back().customer == nullptr);
    REQUIRE(si.orders.back().items.size() == 1);
    CHECK(si.orders.back().items.front()->name == "Cool Kicks");
  }

  SECTION("compare operations")
  {
    CHECK(Shop() == Shop());
    CHECK_FALSE(Shop() != Shop());

    CHECK(Order() == Order());
    CHECK_FALSE(Order() != Order());

    CHECK(Customer() == Customer());
    CHECK_FALSE(Customer() != Customer());

    CHECK(Item() == Item());
    CHECK_FALSE(Item() != Item());
  }

  SECTION("Reading fails with wrong data")
  {
    Shop r;

    std::stringstream s1("FALSE");
    CHECK_FALSE(Shop_io().ReadShop(s1, r));

    std::stringstream s2("COREfails");
    CHECK_FALSE(Shop_io().ReadShop(s2, r));
  }
}
