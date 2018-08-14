#pragma once

#include <vector>
#include <string>
#include <ostream>
#include <istream>
#include <memory>
#include <array>
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

private:
  unsigned int io_counter_{0};
  friend struct Shop_io;
};

struct Customer {
  std::string name;
  std::string address;

  Customer() = default;

private:
  unsigned int io_counter_{0};
  friend struct Shop_io;
};

struct Order {
  std::vector<std::shared_ptr<Item>> items;
  std::shared_ptr<Customer> customer;

  Order() = default;
};

struct Shop {
  std::vector<std::shared_ptr<Item>> items;
  std::vector<std::shared_ptr<Customer>> customers;
  std::vector<Order> orders;

  Shop() = default;
};

bool operator==(const Item&l, const Item&r) {
  return 
    l.name == r.name
    && l.price == r.price;
}

bool operator!=(const Item&l, const Item&r) {
  return 
    l.name != r.name
    || l.price != r.price;
}

bool operator==(const Customer&l, const Customer&r) {
  return 
    l.name == r.name
    && l.address == r.address;
}

bool operator!=(const Customer&l, const Customer&r) {
  return 
    l.name != r.name
    || l.address != r.address;
}

bool operator==(const Order&l, const Order&r) {
  return 
    l.items == r.items
    && l.customer == r.customer;
}

bool operator!=(const Order&l, const Order&r) {
  return 
    l.items != r.items
    || l.customer != r.customer;
}

bool operator==(const Shop&l, const Shop&r) {
  return 
    l.items == r.items
    && l.customers == r.customers
    && l.orders == r.orders;
}

bool operator!=(const Shop&l, const Shop&r) {
  return 
    l.items != r.items
    || l.customers != r.customers
    || l.orders != r.orders;
}

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
