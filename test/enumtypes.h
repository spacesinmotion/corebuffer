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

struct Dummy;

enum class EnumTypes : std::int8_t {
  alpha = 0,
  beta = 4,
  gamma = 5,
  delta = 42,
};

inline const std::array<EnumTypes,4> & EnumTypesValues() {
  static const std::array<EnumTypes,4> values {{
    EnumTypes::alpha,
    EnumTypes::beta,
    EnumTypes::gamma,
    EnumTypes::delta,
  }};
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

enum class Enum16 : std::int16_t {
  a = 512,
};

inline const std::array<Enum16,1> & Enum16Values() {
  static const std::array<Enum16,1> values {{
    Enum16::a,
  }};
  return values;
};

inline const char * ValueName(const Enum16 &v) {
  switch(v) {
    case Enum16::a: return "a";
  }
  return "<error>";
};

enum class Enum32 : std::int32_t {
  a = 128000,
};

inline const std::array<Enum32,1> & Enum32Values() {
  static const std::array<Enum32,1> values {{
    Enum32::a,
  }};
  return values;
};

inline const char * ValueName(const Enum32 &v) {
  switch(v) {
    case Enum32::a: return "a";
  }
  return "<error>";
};

enum class Enum64 : std::int64_t {
  a = 4294967297,
};

inline const std::array<Enum64,1> & Enum64Values() {
  static const std::array<Enum64,1> values {{
    Enum64::a,
  }};
  return values;
};

inline const char * ValueName(const Enum64 &v) {
  switch(v) {
    case Enum64::a: return "a";
  }
  return "<error>";
};

enum class EnumMinus8 : std::int8_t {
  a = -1,
};

inline const std::array<EnumMinus8,1> & EnumMinus8Values() {
  static const std::array<EnumMinus8,1> values {{
    EnumMinus8::a,
  }};
  return values;
};

inline const char * ValueName(const EnumMinus8 &v) {
  switch(v) {
    case EnumMinus8::a: return "a";
  }
  return "<error>";
};

struct Dummy {
  EnumTypes en1{Scope::EnumTypes::alpha};
  EnumTypes en2{Scope::EnumTypes::delta};
  std::vector<EnumTypes> en3;

  Dummy() = default;

  friend bool operator==(const Dummy&l, const Dummy&r) {
    return 
      l.en1 == r.en1
      && l.en2 == r.en2
      && l.en3 == r.en3;
  }

  friend bool operator!=(const Dummy&l, const Dummy&r) {
    return 
      l.en1 != r.en1
      || l.en2 != r.en2
      || l.en3 != r.en3;
  }
};

struct Dummy_io {
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

  void Write(std::ostream &o, const Dummy &v) {
    Write(o, v.en1);
    Write(o, v.en2);
    Write(o, v.en3);
  }

  void Read(std::istream &s, Dummy &v) {
    Read(s, v.en1);
    Read(s, v.en2);
    Read(s, v.en3);
  }

public:
  void WriteDummy(std::ostream &o, const Dummy &v) {

    o.write("CORE", 4);
    o.write("0.0", 3);
    Write(o, v);
  }

  bool ReadDummy(std::istream &i, Dummy &v) {

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
