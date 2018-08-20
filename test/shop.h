#pragma once

#include <vector>
#include <string>
#include <ostream>
#include <istream>
#include <memory>
#include <array>
#include <algorithm>
#include <type_traits>

namespace Example {
namespace Shop {

template<typename T>
struct AlwaysFalse : std::false_type {};

struct Item;
struct Customer;
struct Order;
struct Shop;

struct Item {
  std::string name;
  float price{0.0f};

  Item() = default;

  friend bool operator==(const Item&l, const Item&r) {
    return 
      l.name == r.name
      && l.price == r.price;
  }

  friend bool operator!=(const Item&l, const Item&r) {
    return 
      l.name != r.name
      || l.price != r.price;
  }

private:
  unsigned int io_counter_{0};
  friend struct Shop_io;
};

struct Customer {
  std::string name;
  std::string address;

  Customer() = default;

  friend bool operator==(const Customer&l, const Customer&r) {
    return 
      l.name == r.name
      && l.address == r.address;
  }

  friend bool operator!=(const Customer&l, const Customer&r) {
    return 
      l.name != r.name
      || l.address != r.address;
  }

private:
  unsigned int io_counter_{0};
  friend struct Shop_io;
};

struct Order {
  std::vector<std::shared_ptr<Item>> items;
  std::shared_ptr<Customer> customer;

  Order() = default;

  friend bool operator==(const Order&l, const Order&r) {
    return 
      l.items == r.items
      && l.customer == r.customer;
  }

  friend bool operator!=(const Order&l, const Order&r) {
    return 
      l.items != r.items
      || l.customer != r.customer;
  }
  template<class T> void fill_items(const T &v) {
    std::fill(items.begin(), items.end(), v);
  }

  template<class Generator> void generate_items(Generator gen) {
    std::generate(items.begin(), items.end(), gen);
  }

  template<class T> std::vector<std::shared_ptr<Item>>::iterator remove_items(const T &v) {
    return std::remove(items.begin(), items.end(), v);
  }
  template<class Pred> std::vector<std::shared_ptr<Item>>::iterator remove_items_if(Pred v) {
    return std::remove_if(items.begin(), items.end(), v);
  }

  template<class T> void erase_items(const T &v) {
    items.erase(remove_items(v));
  }
  template<class Pred> void erase_items_if(Pred v) {
    items.erase(remove_items_if(v));
  }

  void reverse_items() {
    std::reverse(items.begin(), items.end());
  }

  void rotate_items(std::vector<std::shared_ptr<Item>>::iterator i) {
    std::rotate(items.begin(), i, items.end());
  }

  template<class Comp> void sort_items(Comp p) {
    std::sort(items.begin(), items.end(), p);
  }

  template<class Comp> bool any_of_items(Comp p) {
    return std::any_of(items.begin(), items.end(), p);
  }
  template<class T> bool any_of_items_is(const T &p) {
    return any_of_items([&p](const Item &x) { return *p == x; });
  }

  template<class Comp> bool all_of_items(Comp p) {
    return std::all_of(items.begin(), items.end(), p);
  }
  template<class T> bool all_of_items_are(const T &p) {
    return all_of_items([&p](const Item &x) { return *p == x; });
  }

  template<class Comp> bool none_of_items(Comp p) {
    return std::none_of(items.begin(), items.end(), p);
  }
  template<class T> bool none_of_items_is(const T &p) {
    return none_of_items([&p](const Item &x) { return *p == x; });
  }

  template<class Fn> Fn for_each_items(Fn p) {
    return std::for_each(items.begin(), items.end(), p);
  }

  template<class T> std::vector<std::shared_ptr<Item>>::iterator find_in_items(const T &p) {
    return std::find(items.begin(), items.end(), p);
  }
  template<class Comp> std::vector<std::shared_ptr<Item>>::iterator find_in_items_if(Comp p) {
    return std::find_if(items.begin(), items.end(), p);
  }

  template<class T>   typename std::iterator_traits<std::vector<std::shared_ptr<Item>>::iterator>::difference_type count_in_items(const T &p) {
    return std::count(items.begin(), items.end(), p);
  }
  template<class Comp>   typename std::iterator_traits<std::vector<std::shared_ptr<Item>>::iterator>::difference_type count_in_items_if(Comp p) {
    return std::count_if(items.begin(), items.end(), p);
  }

};

struct Shop {
  std::vector<std::shared_ptr<Item>> items;
  std::vector<std::shared_ptr<Customer>> customers;
  std::vector<Order> orders;

  Shop() = default;

  friend bool operator==(const Shop&l, const Shop&r) {
    return 
      l.items == r.items
      && l.customers == r.customers
      && l.orders == r.orders;
  }

  friend bool operator!=(const Shop&l, const Shop&r) {
    return 
      l.items != r.items
      || l.customers != r.customers
      || l.orders != r.orders;
  }
  template<class T> void fill_items(const T &v) {
    std::fill(items.begin(), items.end(), v);
  }

  template<class Generator> void generate_items(Generator gen) {
    std::generate(items.begin(), items.end(), gen);
  }

  template<class T> std::vector<std::shared_ptr<Item>>::iterator remove_items(const T &v) {
    return std::remove(items.begin(), items.end(), v);
  }
  template<class Pred> std::vector<std::shared_ptr<Item>>::iterator remove_items_if(Pred v) {
    return std::remove_if(items.begin(), items.end(), v);
  }

  template<class T> void erase_items(const T &v) {
    items.erase(remove_items(v));
  }
  template<class Pred> void erase_items_if(Pred v) {
    items.erase(remove_items_if(v));
  }

  void reverse_items() {
    std::reverse(items.begin(), items.end());
  }

  void rotate_items(std::vector<std::shared_ptr<Item>>::iterator i) {
    std::rotate(items.begin(), i, items.end());
  }

  template<class Comp> void sort_items(Comp p) {
    std::sort(items.begin(), items.end(), p);
  }

  template<class Comp> bool any_of_items(Comp p) {
    return std::any_of(items.begin(), items.end(), p);
  }
  template<class T> bool any_of_items_is(const T &p) {
    return any_of_items([&p](const Item &x) { return *p == x; });
  }

  template<class Comp> bool all_of_items(Comp p) {
    return std::all_of(items.begin(), items.end(), p);
  }
  template<class T> bool all_of_items_are(const T &p) {
    return all_of_items([&p](const Item &x) { return *p == x; });
  }

  template<class Comp> bool none_of_items(Comp p) {
    return std::none_of(items.begin(), items.end(), p);
  }
  template<class T> bool none_of_items_is(const T &p) {
    return none_of_items([&p](const Item &x) { return *p == x; });
  }

  template<class Fn> Fn for_each_items(Fn p) {
    return std::for_each(items.begin(), items.end(), p);
  }

  template<class T> std::vector<std::shared_ptr<Item>>::iterator find_in_items(const T &p) {
    return std::find(items.begin(), items.end(), p);
  }
  template<class Comp> std::vector<std::shared_ptr<Item>>::iterator find_in_items_if(Comp p) {
    return std::find_if(items.begin(), items.end(), p);
  }

  template<class T>   typename std::iterator_traits<std::vector<std::shared_ptr<Item>>::iterator>::difference_type count_in_items(const T &p) {
    return std::count(items.begin(), items.end(), p);
  }
  template<class Comp>   typename std::iterator_traits<std::vector<std::shared_ptr<Item>>::iterator>::difference_type count_in_items_if(Comp p) {
    return std::count_if(items.begin(), items.end(), p);
  }

  template<class T> void fill_customers(const T &v) {
    std::fill(customers.begin(), customers.end(), v);
  }

  template<class Generator> void generate_customers(Generator gen) {
    std::generate(customers.begin(), customers.end(), gen);
  }

  template<class T> std::vector<std::shared_ptr<Customer>>::iterator remove_customers(const T &v) {
    return std::remove(customers.begin(), customers.end(), v);
  }
  template<class Pred> std::vector<std::shared_ptr<Customer>>::iterator remove_customers_if(Pred v) {
    return std::remove_if(customers.begin(), customers.end(), v);
  }

  template<class T> void erase_customers(const T &v) {
    customers.erase(remove_customers(v));
  }
  template<class Pred> void erase_customers_if(Pred v) {
    customers.erase(remove_customers_if(v));
  }

  void reverse_customers() {
    std::reverse(customers.begin(), customers.end());
  }

  void rotate_customers(std::vector<std::shared_ptr<Customer>>::iterator i) {
    std::rotate(customers.begin(), i, customers.end());
  }

  template<class Comp> void sort_customers(Comp p) {
    std::sort(customers.begin(), customers.end(), p);
  }

  template<class Comp> bool any_of_customers(Comp p) {
    return std::any_of(customers.begin(), customers.end(), p);
  }
  template<class T> bool any_of_customers_is(const T &p) {
    return any_of_customers([&p](const Customer &x) { return *p == x; });
  }

  template<class Comp> bool all_of_customers(Comp p) {
    return std::all_of(customers.begin(), customers.end(), p);
  }
  template<class T> bool all_of_customers_are(const T &p) {
    return all_of_customers([&p](const Customer &x) { return *p == x; });
  }

  template<class Comp> bool none_of_customers(Comp p) {
    return std::none_of(customers.begin(), customers.end(), p);
  }
  template<class T> bool none_of_customers_is(const T &p) {
    return none_of_customers([&p](const Customer &x) { return *p == x; });
  }

  template<class Fn> Fn for_each_customers(Fn p) {
    return std::for_each(customers.begin(), customers.end(), p);
  }

  template<class T> std::vector<std::shared_ptr<Customer>>::iterator find_in_customers(const T &p) {
    return std::find(customers.begin(), customers.end(), p);
  }
  template<class Comp> std::vector<std::shared_ptr<Customer>>::iterator find_in_customers_if(Comp p) {
    return std::find_if(customers.begin(), customers.end(), p);
  }

  template<class T>   typename std::iterator_traits<std::vector<std::shared_ptr<Customer>>::iterator>::difference_type count_in_customers(const T &p) {
    return std::count(customers.begin(), customers.end(), p);
  }
  template<class Comp>   typename std::iterator_traits<std::vector<std::shared_ptr<Customer>>::iterator>::difference_type count_in_customers_if(Comp p) {
    return std::count_if(customers.begin(), customers.end(), p);
  }

  template<class T> void fill_orders(const T &v) {
    std::fill(orders.begin(), orders.end(), v);
  }

  template<class Generator> void generate_orders(Generator gen) {
    std::generate(orders.begin(), orders.end(), gen);
  }

  template<class T> std::vector<Order>::iterator remove_orders(const T &v) {
    return std::remove(orders.begin(), orders.end(), v);
  }
  template<class Pred> std::vector<Order>::iterator remove_orders_if(Pred v) {
    return std::remove_if(orders.begin(), orders.end(), v);
  }

  template<class T> void erase_orders(const T &v) {
    orders.erase(remove_orders(v));
  }
  template<class Pred> void erase_orders_if(Pred v) {
    orders.erase(remove_orders_if(v));
  }

  void reverse_orders() {
    std::reverse(orders.begin(), orders.end());
  }

  void rotate_orders(std::vector<Order>::iterator i) {
    std::rotate(orders.begin(), i, orders.end());
  }

  template<class Comp> void sort_orders(Comp p) {
    std::sort(orders.begin(), orders.end(), p);
  }

  template<class Comp> bool any_of_orders(Comp p) {
    return std::any_of(orders.begin(), orders.end(), p);
  }
  template<class T> bool any_of_orders_is(const T &p) {
    return any_of_orders([&p](const Order &x) { return p == x; });
  }

  template<class Comp> bool all_of_orders(Comp p) {
    return std::all_of(orders.begin(), orders.end(), p);
  }
  template<class T> bool all_of_orders_are(const T &p) {
    return all_of_orders([&p](const Order &x) { return p == x; });
  }

  template<class Comp> bool none_of_orders(Comp p) {
    return std::none_of(orders.begin(), orders.end(), p);
  }
  template<class T> bool none_of_orders_is(const T &p) {
    return none_of_orders([&p](const Order &x) { return p == x; });
  }

  template<class Fn> Fn for_each_orders(Fn p) {
    return std::for_each(orders.begin(), orders.end(), p);
  }

  template<class T> std::vector<Order>::iterator find_in_orders(const T &p) {
    return std::find(orders.begin(), orders.end(), p);
  }
  template<class Comp> std::vector<Order>::iterator find_in_orders_if(Comp p) {
    return std::find_if(orders.begin(), orders.end(), p);
  }

  template<class T>   typename std::iterator_traits<std::vector<Order>::iterator>::difference_type count_in_orders(const T &p) {
    return std::count(orders.begin(), orders.end(), p);
  }
  template<class Comp>   typename std::iterator_traits<std::vector<Order>::iterator>::difference_type count_in_orders_if(Comp p) {
    return std::count_if(orders.begin(), orders.end(), p);
  }

};

struct Shop_io {
private:
  unsigned int Item_count_{0};
  std::vector<std::shared_ptr<Item>> Item_references_;

  unsigned int Customer_count_{0};
  std::vector<std::shared_ptr<Customer>> Customer_references_;

  template<typename T> void Write(std::ostream &, const T *) {
    static_assert(AlwaysFalse<T>::value, "Something not implemented");
  }

  template<typename T> void Write(std::ostream &o, const T &v) {
    o.write(reinterpret_cast<const char *>(&v), sizeof(T));
  }

  template<typename T> void Write(std::ostream &o, const std::vector<T> &v) {
    Write(o, v.size());
    o.write(reinterpret_cast<const char *>(v.data()), sizeof(T) * v.size());
  }

  template<typename T> void Write(std::ostream &o, const std::shared_ptr<T> &v, unsigned int &counter) {
    if (!v) {
      o.write("\x0", 1);
    } else if (v->io_counter_== 0) {
      v->io_counter_ = ++counter;
      o.write("\x1", 1);
      Write(o, *v);
    } else {
      o.write("\x2", 1);
      Write(o, v->io_counter_);
    }
  }

  template<typename T> void Write(std::ostream &o, const std::vector<std::shared_ptr<T>> &v) {
    Write(o, v.size());
    for (const auto &entry : v)
      Write(o, entry);
  }

  template<typename T> void Write(std::ostream &, const std::shared_ptr<T> &) {
    static_assert(AlwaysFalse<T>::value, "Something not implemented");
  }

  void Write(std::ostream &o, const std::string &v) {
    Write(o, v.size());
    o.write(v.data(), v.size());
  }

  template<typename T> void Read(std::istream &i, T &v) {
    i.read(reinterpret_cast<char *>(&v), sizeof(T));
  }

  template<typename T> void Read(std::istream &, std::shared_ptr<T> &) {
    static_assert(AlwaysFalse<T>::value, "Something not implemented");
  }

  template<typename T> void Read(std::istream &s, std::shared_ptr<T> &v, std::vector<std::shared_ptr<T>> &cache) {
    char ref = 0;
    s.read(&ref, 1);
    if (ref == '\x1') {
      v = std::make_shared<T>();
      cache.push_back(v);
      Read(s, *v);
    } else if (ref == '\x2') {
      unsigned int index = 0;
      Read(s, index);
      v = cache[index - 1];
    }
  }

  template<typename T> void Read(std::istream &s, std::vector<std::shared_ptr<T>> &v) {
    auto size = v.size();
    Read(s, size);
    v.resize(size);
    for (auto &entry : v)
      Read(s, entry);
  }

  template<typename T> void Read(std::istream &i, std::vector<T> &v) {
    typename std::vector<T>::size_type s{0};
    Read(i, s);
    v.resize(s);
    i.read(reinterpret_cast<char *>(v.data()), sizeof(T) * s);
  }

  void Read(std::istream &i, std::string &v) {
    std::string::size_type s{0};
    Read(i, s);
    v.resize(s);
    i.read(&v[0], s);
  }

  void Write(std::ostream &o, const Item &v) {
    Write(o, v.name);
    Write(o, v.price);
  }

  void Write(std::ostream &o, const std::shared_ptr<Item> &v) {
    Write(o, v, Item_count_);
  }

  void Read(std::istream &s, Item &v) {
    Read(s, v.name);
    Read(s, v.price);
  }

  void Read(std::istream &s, std::shared_ptr<Item> &v) {
    Read(s, v, Item_references_);
  }

  void Write(std::ostream &o, const Customer &v) {
    Write(o, v.name);
    Write(o, v.address);
  }

  void Write(std::ostream &o, const std::shared_ptr<Customer> &v) {
    Write(o, v, Customer_count_);
  }

  void Read(std::istream &s, Customer &v) {
    Read(s, v.name);
    Read(s, v.address);
  }

  void Read(std::istream &s, std::shared_ptr<Customer> &v) {
    Read(s, v, Customer_references_);
  }

  void Write(std::ostream &o, const Order &v) {
    Write(o, v.items);
    Write(o, v.customer);
  }

  void Write(std::ostream &o, const std::vector<Order> &v) {
    Write(o, v.size());
    for (const auto &entry : v)
      Write(o, entry);
  }

  void Read(std::istream &s, Order &v) {
    Read(s, v.items);
    Read(s, v.customer);
  }

  void Read(std::istream &s, std::vector<Order> &v) {
    auto size = v.size();
    Read(s, size);
    v.resize(size);
    for (auto &entry : v)
      Read(s, entry);
  }

  void Write(std::ostream &o, const Shop &v) {
    Write(o, v.items);
    Write(o, v.customers);
    Write(o, v.orders);
  }

  void Read(std::istream &s, Shop &v) {
    Read(s, v.items);
    Read(s, v.customers);
    Read(s, v.orders);
  }

public:
  void WriteShop(std::ostream &o, const Shop &v) {
    Item_count_ = 0;
    Customer_count_ = 0;

    o.write("CORE", 4);
    o.write("0.1", 3);
    Write(o, v);
  }

  bool ReadShop(std::istream &i, Shop &v) {
    Item_references_.clear();
    Customer_references_.clear();

    std::string marker("0000");
    i.read(&marker[0], 4);
    if (marker != "CORE")
      return false;
    std::string version(3, '_');
    i.read(&version[0], 3);
    if (version != "0.1")
      return false;
    Read(i, v);
    return true;
  }

};
}
}
