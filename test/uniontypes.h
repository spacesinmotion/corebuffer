#pragma once

#include <vector>
#include <string>
#include <ostream>
#include <istream>
#include <memory>
#include <array>
#include <type_traits>

namespace UnionTypes {

template<typename T>
struct AlwaysFalse : std::false_type {};

struct A;
struct B;
struct Root;

struct A {
  std::string name;

  A() = default;
  A(const std::string &name_)
    : name(name_)
  {}
};

struct B {
  std::int64_t size{0};

  B() = default;
  B(const std::int64_t &size_)
    : size(size_)
  {}
};

struct Root {
  A a;
  B b;

  Root() = default;
};

bool operator==(const A&l, const A&r) {
  return 
    l.name == r.name;
}

bool operator!=(const A&l, const A&r) {
  return 
    l.name != r.name;
}

bool operator==(const B&l, const B&r) {
  return 
    l.size == r.size;
}

bool operator!=(const B&l, const B&r) {
  return 
    l.size != r.size;
}

bool operator==(const Root&l, const Root&r) {
  return 
    l.a == r.a
    && l.b == r.b;
}

bool operator!=(const Root&l, const Root&r) {
  return 
    l.a != r.a
    || l.b != r.b;
}

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

  void Read(std::istream &i, std::string &v) {
    std::string::size_type s{0};
    Read(i, s);
    v.resize(s);
    i.read(&v[0], s);
  }

  void Write(std::ostream &o, const A &v) {
    Write(o, v.name);
  }

  void Read(std::istream &s, A &v) {
    Read(s, v.name);
  }

  void Write(std::ostream &o, const B &v) {
    Write(o, v.size);
  }

  void Read(std::istream &s, B &v) {
    Read(s, v.size);
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
