#pragma once

#include <vector>
#include <string>
#include <ostream>
#include <istream>
#include <memory>
#include <array>
#include <algorithm>
#include <type_traits>

namespace FlagScope {

template<typename T>
struct AlwaysFalse : std::false_type {};

struct Dummy;

struct Flags {
  enum value_t {
    _none = 0,
    alpha = 1,
    beta = 2,
    gamma = 4,
    delta = 8,
  };

  inline Flags(value_t v = _none) : value(v) {}

  bool operator[](const value_t &a) const { return (value & a) == a; }
  bool operator[](const Flags &a) const { return (value & a.value) == a.value; }
  friend bool operator==(const Flags &a, const Flags &b) { return a.value == b.value; }
  friend bool operator!=(const Flags &a, const Flags &b) { return a.value != b.value; }

  std::int8_t value;
};

inline Flags operator|(Flags::value_t a, Flags::value_t b)
{
  return Flags::value_t((static_cast<std::int8_t>(a)) | (static_cast<std::int8_t>(b)));
}
inline Flags operator|(const Flags &a, const Flags &b)
{
  return Flags::value_t((static_cast<std::int8_t>(a.value)) | (static_cast<std::int8_t>(b.value)));
}
inline Flags &operator|=(Flags &a, const Flags &b)
{
  return reinterpret_cast<Flags &>((reinterpret_cast<std::int8_t &>(a.value)) |= (static_cast<std::int8_t>(b.value)));
}
inline Flags operator&(Flags::value_t a, Flags::value_t b)
{
  return Flags::value_t((static_cast<std::int8_t>(a)) & (static_cast<std::int8_t>(b)));
}
inline Flags operator&(const Flags &a, const Flags &b)
{
  return Flags::value_t((static_cast<std::int8_t>(a.value)) & (static_cast<std::int8_t>(b.value)));
}
inline Flags &operator&=(Flags &a, const Flags &b)
{
  return reinterpret_cast<Flags &>((reinterpret_cast<std::int8_t &>(a.value)) &= (static_cast<std::int8_t>(b.value)));
}
inline Flags operator~(Flags::value_t a) { return Flags::value_t(~static_cast<std::int8_t>(a)); }
inline Flags operator~(const Flags &a) { return Flags::value_t(~a.value); }
inline Flags operator^(Flags::value_t a, Flags::value_t b)
{
  return Flags::value_t((static_cast<std::int8_t>(a)) ^ (static_cast<std::int8_t>(b)));
}
inline Flags operator^(const Flags &a, const Flags &b)
{
  return Flags::value_t((static_cast<std::int8_t>(a.value)) ^ (static_cast<std::int8_t>(b.value)));
}
inline Flags &operator^=(Flags &a, const Flags &b)
{
  return reinterpret_cast<Flags &>((reinterpret_cast<std::int8_t &>(a.value)) ^= (static_cast<std::int8_t>(b.value)));
}
inline bool operator<(const Flags &a, const Flags &b)
{
  return a.value < b.value;
}
inline bool operator>(const Flags &a, const Flags &b)
{
  return a.value > b.value;
}
inline bool operator>=(const Flags &a, const Flags &b)
{
  return a.value >= b.value;
}
inline bool operator<=(const Flags &a, const Flags &b)
{
  return a.value <= b.value;
}

inline const std::array<Flags::value_t, 4> &FlagsValues()
{
  static const std::array<Flags::value_t, 4> values{{
      Flags::alpha,
      Flags::beta,
      Flags::gamma,
      Flags::delta,
  }};
  return values;
};

inline const char *ValueName(const Flags::value_t &v)
{
  switch (v)
  {
    default:
      break;
    case Flags::alpha:
      return "alpha";
    case Flags::beta:
      return "beta";
    case Flags::gamma:
      return "gamma";
    case Flags::delta:
      return "delta";
  }
  return "<error>";
};
struct Dummy {
  Flags en1;
  Flags en2{FlagScope::Flags::gamma};
  std::vector<Flags> en3;

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

  template<class T> void fill_en3(const T &v) {
    std::fill(en3.begin(), en3.end(), v);
  }

  template<class Generator> void generate_en3(Generator gen) {
    std::generate(en3.begin(), en3.end(), gen);
  }

  template<class T> std::vector<Flags>::iterator remove_en3(const T &v) {
    return std::remove(en3.begin(), en3.end(), v);
  }
  template<class Pred> std::vector<Flags>::iterator remove_en3_if(Pred v) {
    return std::remove_if(en3.begin(), en3.end(), v);
  }

  template<class T> void erase_en3(const T &v) {
    en3.erase(remove_en3(v));
  }
  template<class Pred> void erase_en3_if(Pred v) {
    en3.erase(remove_en3_if(v));
  }

  void reverse_en3() {
    std::reverse(en3.begin(), en3.end());
  }

  void rotate_en3(std::vector<Flags>::iterator i) {
    std::rotate(en3.begin(), i, en3.end());
  }

  void sort_en3() {
    std::sort(en3.begin(), en3.end());
  }
  template<class Comp> void sort_en3(Comp p) {
    std::sort(en3.begin(), en3.end(), p);
  }

  template<class Comp> bool any_of_en3(Comp p) {
    return std::any_of(en3.begin(), en3.end(), p);
  }
  template<class T> bool any_of_en3_is(const T &p) {
    return any_of_en3([&p](const Flags &x) { return x == p; });
  }

  template<class Comp> bool all_of_en3(Comp p) {
    return std::all_of(en3.begin(), en3.end(), p);
  }
  template<class T> bool all_of_en3_are(const T &p) {
    return all_of_en3([&p](const Flags &x) { return x == p; });
  }

  template<class Comp> bool none_of_en3(Comp p) {
    return std::none_of(en3.begin(), en3.end(), p);
  }
  template<class T> bool none_of_en3_is(const T &p) {
    return none_of_en3([&p](const Flags &x) { return x == p; });
  }

  template<class Fn> Fn for_each_en3(Fn p) {
    return std::for_each(en3.begin(), en3.end(), p);
  }

  template<class T> std::vector<Flags>::iterator find_in_en3(const T &p) {
    return std::find(en3.begin(), en3.end(), p);
  }
  template<class Comp> std::vector<Flags>::iterator find_in_en3_if(Comp p) {
    return std::find_if(en3.begin(), en3.end(), p);
  }

  template<class T>   typename std::iterator_traits<std::vector<Flags>::iterator>::difference_type count_in_en3(const T &p) {
    return std::count(en3.begin(), en3.end(), p);
  }
  template<class Comp>   typename std::iterator_traits<std::vector<Flags>::iterator>::difference_type count_in_en3_if(Comp p) {
    return std::count_if(en3.begin(), en3.end(), p);
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
