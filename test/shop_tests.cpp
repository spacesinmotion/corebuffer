#define CATCH_CONFIG_FAST_COMPILE
#include "catch/catch.hpp"

#include "shop.h"

#include <fstream>

TEST_CASE("Shop example test", "[output]")
{
  using namespace Example::Shop;

  SECTION("Usage example (simple build without errors here)")
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
}
