#pragma once

#include <vector>
#include <string>
#include <ostream>
#include <istream>
#include <memory>
#include <array>
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

  PointerBaseTypes() = default;

  friend bool operator==(const PointerBaseTypes&l, const PointerBaseTypes&r) {
    return 
      l.b1 == r.b1;
  }

  friend bool operator!=(const PointerBaseTypes&l, const PointerBaseTypes&r) {
    return 
      l.b1 != r.b1;
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

  Root() = default;

  friend bool operator==(const Root&l, const Root&r) {
    return 
      l.a == r.a
      && l.b == r.b;
  }

  friend bool operator!=(const Root&l, const Root&r) {
    return 
      l.a != r.a
      || l.b != r.b;
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
  }

  void Read(std::istream &s, PointerBaseTypes &v) {
    Read(s, v.b1);
  }

  void Write(std::ostream &o, const Initializer &v) {
    Write(o, v.a);
    Write(o, v.b);
    Write(o, v.c);
  }

  void Read(std::istream &s, Initializer &v) {
    Read(s, v.a);
    Read(s, v.b);
    Read(s, v.c);
  }

  void Write(std::ostream &o, const Root &v) {
    Write(o, v.a);
    Write(o, v.b);
  }

  void Read(std::istream &s, Root &v) {
    Read(s, v.a);
    Read(s, v.b);
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
