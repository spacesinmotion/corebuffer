#pragma once

#include <vector>
#include <string>
#include <ostream>
#include <istream>
#include <memory>
#include <array>
#include <algorithm>
#include <type_traits>

namespace Scope {

template<typename T>
struct AlwaysFalse : std::false_type {};

struct BaseTypes;
struct PointerBaseTypes;
struct Initializer;
struct Root;

struct BaseTypes {
  std::int32_t a{0};
  std::int16_t aa{0};
  std::int64_t ab{0};
  bool b{false};
  float c{0.0f};
  double d{0.0};
  std::int8_t e{0};
  std::int16_t f{0};
  std::int32_t g{0};
  std::int64_t h{0};
  std::uint8_t i{0u};
  std::uint16_t j{0u};
  std::uint32_t k{0u};
  std::uint64_t l{0u};
  std::string m;

  BaseTypes() = default;

  friend bool operator==(const BaseTypes&l, const BaseTypes&r) {
    return 
      l.a == r.a
      && l.aa == r.aa
      && l.ab == r.ab
      && l.b == r.b
      && l.c == r.c
      && l.d == r.d
      && l.e == r.e
      && l.f == r.f
      && l.g == r.g
      && l.h == r.h
      && l.i == r.i
      && l.j == r.j
      && l.k == r.k
      && l.l == r.l
      && l.m == r.m;
  }

  friend bool operator!=(const BaseTypes&l, const BaseTypes&r) {
    return 
      l.a != r.a
      || l.aa != r.aa
      || l.ab != r.ab
      || l.b != r.b
      || l.c != r.c
      || l.d != r.d
      || l.e != r.e
      || l.f != r.f
      || l.g != r.g
      || l.h != r.h
      || l.i != r.i
      || l.j != r.j
      || l.k != r.k
      || l.l != r.l
      || l.m != r.m;
  }
};

struct PointerBaseTypes {
  std::vector<std::string> b1;
  std::vector<std::int32_t> x;

  PointerBaseTypes() = default;

  friend bool operator==(const PointerBaseTypes&l, const PointerBaseTypes&r) {
    return 
      l.b1 == r.b1
      && l.x == r.x;
  }

  friend bool operator!=(const PointerBaseTypes&l, const PointerBaseTypes&r) {
    return 
      l.b1 != r.b1
      || l.x != r.x;
  }

  template<class T> void fill_b1(const T &v) {
    std::fill(b1.begin(), b1.end(), v);
  }

  template<class Generator> void generate_b1(Generator gen) {
    std::generate(b1.begin(), b1.end(), gen);
  }

  template<class T> std::vector<std::string>::iterator remove_b1(const T &v) {
    return std::remove(b1.begin(), b1.end(), v);
  }
  template<class Pred> std::vector<std::string>::iterator remove_b1_if(Pred v) {
    return std::remove_if(b1.begin(), b1.end(), v);
  }

  template<class T> void erase_b1(const T &v) {
    b1.erase(remove_b1(v));
  }
  template<class Pred> void erase_b1_if(Pred v) {
    b1.erase(remove_b1_if(v));
  }

  void reverse_b1() {
    std::reverse(b1.begin(), b1.end());
  }

  void rotate_b1(std::vector<std::string>::iterator i) {
    std::rotate(b1.begin(), i, b1.end());
  }

  void sort_b1() {
    std::sort(b1.begin(), b1.end());
  }
  template<class Comp> void sort_b1(Comp p) {
    std::sort(b1.begin(), b1.end(), p);
  }

  template<class Comp> bool any_of_b1(Comp p) {
    return std::any_of(b1.begin(), b1.end(), p);
  }
  template<class T> bool any_of_b1_is(const T &p) {
    return any_of_b1([&p](const std::string &x) { return x == p; });
  }

  template<class Comp> bool all_of_b1(Comp p) {
    return std::all_of(b1.begin(), b1.end(), p);
  }
  template<class T> bool all_of_b1_are(const T &p) {
    return all_of_b1([&p](const std::string &x) { return x == p; });
  }

  template<class Comp> bool none_of_b1(Comp p) {
    return std::none_of(b1.begin(), b1.end(), p);
  }
  template<class T> bool none_of_b1_is(const T &p) {
    return none_of_b1([&p](const std::string &x) { return x == p; });
  }

  template<class Fn> Fn for_each_b1(Fn p) {
    return std::for_each(b1.begin(), b1.end(), p);
  }

  template<class T> std::vector<std::string>::iterator find_in_b1(const T &p) {
    return std::find(b1.begin(), b1.end(), p);
  }
  template<class Comp> std::vector<std::string>::iterator find_in_b1_if(Comp p) {
    return std::find_if(b1.begin(), b1.end(), p);
  }

  template<class T>   typename std::iterator_traits<std::vector<std::string>::iterator>::difference_type count_in_b1(const T &p) {
    return std::count(b1.begin(), b1.end(), p);
  }
  template<class Comp>   typename std::iterator_traits<std::vector<std::string>::iterator>::difference_type count_in_b1_if(Comp p) {
    return std::count_if(b1.begin(), b1.end(), p);
  }

  template<class T> void fill_x(const T &v) {
    std::fill(x.begin(), x.end(), v);
  }

  template<class Generator> void generate_x(Generator gen) {
    std::generate(x.begin(), x.end(), gen);
  }

  template<class T> std::vector<std::int32_t>::iterator remove_x(const T &v) {
    return std::remove(x.begin(), x.end(), v);
  }
  template<class Pred> std::vector<std::int32_t>::iterator remove_x_if(Pred v) {
    return std::remove_if(x.begin(), x.end(), v);
  }

  template<class T> void erase_x(const T &v) {
    x.erase(remove_x(v));
  }
  template<class Pred> void erase_x_if(Pred v) {
    x.erase(remove_x_if(v));
  }

  void reverse_x() {
    std::reverse(x.begin(), x.end());
  }

  void rotate_x(std::vector<std::int32_t>::iterator i) {
    std::rotate(x.begin(), i, x.end());
  }

  void sort_x() {
    std::sort(x.begin(), x.end());
  }
  template<class Comp> void sort_x(Comp p) {
    std::sort(x.begin(), x.end(), p);
  }

  template<class Comp> bool any_of_x(Comp p) {
    return std::any_of(x.begin(), x.end(), p);
  }
  template<class T> bool any_of_x_is(const T &p) {
    return any_of_x([&p](const std::int32_t &x) { return x == p; });
  }

  template<class Comp> bool all_of_x(Comp p) {
    return std::all_of(x.begin(), x.end(), p);
  }
  template<class T> bool all_of_x_are(const T &p) {
    return all_of_x([&p](const std::int32_t &x) { return x == p; });
  }

  template<class Comp> bool none_of_x(Comp p) {
    return std::none_of(x.begin(), x.end(), p);
  }
  template<class T> bool none_of_x_is(const T &p) {
    return none_of_x([&p](const std::int32_t &x) { return x == p; });
  }

  template<class Fn> Fn for_each_x(Fn p) {
    return std::for_each(x.begin(), x.end(), p);
  }

  template<class T> std::vector<std::int32_t>::iterator find_in_x(const T &p) {
    return std::find(x.begin(), x.end(), p);
  }
  template<class Comp> std::vector<std::int32_t>::iterator find_in_x_if(Comp p) {
    return std::find_if(x.begin(), x.end(), p);
  }

  template<class T>   typename std::iterator_traits<std::vector<std::int32_t>::iterator>::difference_type count_in_x(const T &p) {
    return std::count(x.begin(), x.end(), p);
  }
  template<class Comp>   typename std::iterator_traits<std::vector<std::int32_t>::iterator>::difference_type count_in_x_if(Comp p) {
    return std::count_if(x.begin(), x.end(), p);
  }
};

struct Initializer {
  std::int32_t a{3};
  float b{4.2f};
  std::uint64_t c{999933030303003u};

  Initializer() = default;
  Initializer(const std::int32_t &a_)
    : a(a_)
  {}
  Initializer(const float &b_, const std::int32_t &a_)
    : a(a_)
    , b(b_)
  {}

  friend bool operator==(const Initializer&l, const Initializer&r) {
    return 
      l.a == r.a
      && l.b == r.b
      && l.c == r.c;
  }

  friend bool operator!=(const Initializer&l, const Initializer&r) {
    return 
      l.a != r.a
      || l.b != r.b
      || l.c != r.c;
  }
};

struct Root {
  BaseTypes a;
  PointerBaseTypes b;
  Initializer c;

  Root() = default;

  friend bool operator==(const Root&l, const Root&r) {
    return 
      l.a == r.a
      && l.b == r.b
      && l.c == r.c;
  }

  friend bool operator!=(const Root&l, const Root&r) {
    return 
      l.a != r.a
      || l.b != r.b
      || l.c != r.c;
  }
};

struct Root_io {
private:
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

  void Write(std::ostream &o, const std::vector<std::string> &v) {
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

  template<typename T> void Read(std::istream &i, std::vector<T> &v) {
    typename std::vector<T>::size_type s{0};
    Read(i, s);
    v.resize(s);
    i.read(reinterpret_cast<char *>(v.data()), sizeof(T) * s);
  }

  void Read(std::istream &i, std::vector<std::string> &v) {
    auto size = v.size();
    Read(i, size);
    v.resize(size);
    for (auto &entry : v)
      Read(i, entry);
  }

  void Read(std::istream &i, std::string &v) {
    std::string::size_type s{0};
    Read(i, s);
    v.resize(s);
    i.read(&v[0], s);
  }

  void Write(std::ostream &o, const BaseTypes &v) {
    Write(o, v.a);
    Write(o, v.aa);
    Write(o, v.ab);
    Write(o, v.b);
    Write(o, v.c);
    Write(o, v.d);
    Write(o, v.e);
    Write(o, v.f);
    Write(o, v.g);
    Write(o, v.h);
    Write(o, v.i);
    Write(o, v.j);
    Write(o, v.k);
    Write(o, v.l);
    Write(o, v.m);
  }

  void Read(std::istream &s, BaseTypes &v) {
    Read(s, v.a);
    Read(s, v.aa);
    Read(s, v.ab);
    Read(s, v.b);
    Read(s, v.c);
    Read(s, v.d);
    Read(s, v.e);
    Read(s, v.f);
    Read(s, v.g);
    Read(s, v.h);
    Read(s, v.i);
    Read(s, v.j);
    Read(s, v.k);
    Read(s, v.l);
    Read(s, v.m);
  }

  void Write(std::ostream &o, const PointerBaseTypes &v) {
    Write(o, v.b1);
    Write(o, v.x);
  }

  void Read(std::istream &s, PointerBaseTypes &v) {
    Read(s, v.b1);
    Read(s, v.x);
  }

  void Write(std::ostream &o, const Root &v) {
    Write(o, v.a);
    Write(o, v.b);
    Write(o, v.c);
  }

  void Read(std::istream &s, Root &v) {
    Read(s, v.a);
    Read(s, v.b);
    Read(s, v.c);
  }

public:
  void WriteRoot(std::ostream &o, const Root &v) {

    o.write("CORE", 4);
    o.write("0.0", 3);
    Write(o, v);
  }

  bool ReadRoot(std::istream &i, Root &v) {

    std::string marker("0000");
    i.read(&marker[0], 4);
    if (marker != "CORE")
      return false;
    std::string version(3, '_');
    i.read(&version[0], 3);
    if (version != "0.0")
      return false;
    Read(i, v);
    return true;
  }

};
}
