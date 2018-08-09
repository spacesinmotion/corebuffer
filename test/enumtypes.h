#pragma once

#include <vector>
#include <string>
#include <ostream>
#include <istream>
#include <memory>
#include <type_traits>
#include <array>

namespace Scope {
template<typename T>
struct AlwaysFalse : std::false_type {};

struct Dummy;

enum class EnumTypes : unsigned int {
  alpha = 0,
  beta = 4,
  gamma = 5,
  delta = 42,
};

struct Dummy {
  EnumTypes en1{Scope::EnumTypes::alpha};
  EnumTypes en2{Scope::EnumTypes::delta};
private:
  unsigned int io_counter_{0};
  friend struct Dummy_io;
};

bool operator==(const Dummy&l, const Dummy&r) {
  return 
    l.en1 == r.en1
    && l.en2 == r.en2;
}

bool operator!=(const Dummy&l, const Dummy&r) {
  return 
    l.en1 != r.en1
    || l.en2 != r.en2;
}

inline const std::array<EnumTypes,4> & EnumTypesValues() {
  static const std::array<EnumTypes,4> values {
    EnumTypes::alpha,
    EnumTypes::beta,
    EnumTypes::gamma,
    EnumTypes::delta,
  };
  return values;
};

inline const char * ValueName(const EnumTypes &v) {
  switch(v) {
    case EnumTypes::alpha: return "alpha";
    case EnumTypes::beta: return "beta";
    case EnumTypes::gamma: return "gamma";
    case EnumTypes::delta: return "delta";
  }
  return "<error>";
};

struct Dummy_io {
private:
template<typename T> void Write(std::ostream &o, const T *v) {
  static_assert(AlwaysFalse<T>::value, "Something not implemented");
}

template<typename T> void Write(std::ostream &o, const T &v) {
  o.write(reinterpret_cast<const char *>(&v), sizeof(T));
}

template<typename T> void Write(std::ostream &o, const std::vector<T> &v) {
  Write(o, v.size());
  o.write(reinterpret_cast<const char *>(v.data()), sizeof(T) * v.size());
}

template<typename T> void Write(std::ostream &o, const std::unique_ptr<T> &v) {
  static_assert(AlwaysFalse<T>::value, "Something not implemented");
}

template<typename T> void Write(std::ostream &o, const std::shared_ptr<T> &v) {
  static_assert(AlwaysFalse<T>::value, "Something not implemented");
}

template<typename T> void Write(std::ostream &o, const std::weak_ptr<T> &v) {
  static_assert(AlwaysFalse<T>::value, "Something not implemented");
}

template<typename T> void Write(std::ostream &o, const std::vector<std::unique_ptr<T>> &v) {
  static_assert(AlwaysFalse<T>::value, "Something not implemented");
}

template<typename T> void Write(std::ostream &o, const std::vector<std::shared_ptr<T>> &v) {
  static_assert(AlwaysFalse<T>::value, "Something not implemented");
}

template<typename T> void Write(std::ostream &o, const std::vector<std::weak_ptr<T>> &v) {
  static_assert(AlwaysFalse<T>::value, "Something not implemented");
}

void Write(std::ostream &o, const std::string &v) {
  Write(o, v.size());
  o.write(v.data(), v.size());
}

void Write(std::ostream &o, const char *v) {
  Write(o, std::string(v));
}

template<typename T> void Read(std::istream &i, T &v) {
  i.read(reinterpret_cast<char *>(&v), sizeof(T));
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

void Write(std::ostream &o, const Dummy &v) {
  Write(o, v.en1);
  Write(o, v.en2);
}

void Read(std::istream &s, Dummy &v) {
  Read(s, v.en1);
  Read(s, v.en2);
}

  unsigned int Dummy_count_{0};
  std::vector<std::shared_ptr<Dummy>> Dummy_references_;

public:
void WriteDummy(std::ostream &o, const Dummy &v) {
  Dummy_count_ = 0;

  o.write("CORE", 4);
  o.write("0.0", 3);
  Write(o, v);
}

bool ReadDummy(std::istream &i, Dummy &v) {
  Dummy_references_.clear();

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
