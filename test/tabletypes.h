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

struct TableA;
struct TableB;
struct TableD;
struct TableC;

template<typename T> bool operator==(const std::weak_ptr<T> &l, const std::weak_ptr<T> &r) {
  return l.lock() == r.lock();
}

template<typename T> bool operator!=(const std::weak_ptr<T> &l, const std::weak_ptr<T> &r) {
  return l.lock() != r.lock();
}

struct TableA {
  std::string name;
  std::unique_ptr<TableD> d1;
  std::weak_ptr<TableD> d2;
  std::shared_ptr<TableD> d3;
  std::shared_ptr<TableD> d4;

  TableA() = default;
  TableA(const std::shared_ptr<TableD> &d3_)
    : d3(d3_)
  {}
  TableA(const std::shared_ptr<TableD> &d2_, std::unique_ptr<TableD> d1_)
    : d1(std::move(d1_))
    , d2(d2_)
  {}

  friend bool operator==(const TableA&l, const TableA&r) {
    return 
      l.name == r.name
      && l.d1 == r.d1
      && l.d2 == r.d2
      && l.d3 == r.d3
      && l.d4 == r.d4;
  }

  friend bool operator!=(const TableA&l, const TableA&r) {
    return 
      l.name != r.name
      || l.d1 != r.d1
      || l.d2 != r.d2
      || l.d3 != r.d3
      || l.d4 != r.d4;
  }

private:
  unsigned int io_counter_{0};
  friend struct TableC_io;
};

struct TableB {
  std::string name;

  TableB() = default;

  friend bool operator==(const TableB&l, const TableB&r) {
    return 
      l.name == r.name;
  }

  friend bool operator!=(const TableB&l, const TableB&r) {
    return 
      l.name != r.name;
  }

private:
  unsigned int io_counter_{0};
  friend struct TableC_io;
};

struct TableD {
  std::string name;
  std::shared_ptr<TableA> a;

  TableD() = default;

  friend bool operator==(const TableD&l, const TableD&r) {
    return 
      l.name == r.name
      && l.a == r.a;
  }

  friend bool operator!=(const TableD&l, const TableD&r) {
    return 
      l.name != r.name
      || l.a != r.a;
  }

private:
  unsigned int io_counter_{0};
  friend struct TableC_io;
};

struct TableC {
  std::vector<TableA> a;
  std::vector<TableB> b;
  std::vector<std::unique_ptr<TableA>> c;
  std::vector<std::shared_ptr<TableB>> d;
  std::vector<std::weak_ptr<TableB>> e;

  TableC() = default;
  TableC(std::vector<std::unique_ptr<TableA>> c_)
    : c(std::move(c_))
  {}

  friend bool operator==(const TableC&l, const TableC&r) {
    return 
      l.a == r.a
      && l.b == r.b
      && l.c == r.c
      && l.d == r.d
      && l.e == r.e;
  }

  friend bool operator!=(const TableC&l, const TableC&r) {
    return 
      l.a != r.a
      || l.b != r.b
      || l.c != r.c
      || l.d != r.d
      || l.e != r.e;
  }
  template<class T> void fill_a(const T &v) {
    std::fill(a.begin(), a.end(), v);
  }

  template<class Generator> void generate_a(Generator gen) {
    std::generate(a.begin(), a.end(), gen);
  }

  template<class T> std::vector<TableA>::iterator remove_a(const T &v) {
    return std::remove(a.begin(), a.end(), v);
  }
  template<class Pred> std::vector<TableA>::iterator remove_a_if(Pred v) {
    return std::remove_if(a.begin(), a.end(), v);
  }

  template<class T> void erase_a(const T &v) {
    a.erase(remove_a(v));
  }
  template<class Pred> void erase_a_if(Pred v) {
    a.erase(remove_a_if(v));
  }

  void reverse_a() {
    std::reverse(a.begin(), a.end());
  }

  void rotate_a(std::vector<TableA>::iterator i) {
    std::rotate(a.begin(), i, a.end());
  }

  template<class Comp> void sort_a(Comp p) {
    std::sort(a.begin(), a.end(), p);
  }

  template<class Comp> bool any_of_a(Comp p) {
    return std::any_of(a.begin(), a.end(), p);
  }
  template<class T> bool any_of_a_is(const T &p) {
    return any_of_a([&p](const TableA &x) { return p == x; });
  }

  template<class Comp> bool all_of_a(Comp p) {
    return std::all_of(a.begin(), a.end(), p);
  }
  template<class T> bool all_of_a_are(const T &p) {
    return all_of_a([&p](const TableA &x) { return p == x; });
  }

  template<class Comp> bool none_of_a(Comp p) {
    return std::none_of(a.begin(), a.end(), p);
  }
  template<class T> bool none_of_a_is(const T &p) {
    return none_of_a([&p](const TableA &x) { return p == x; });
  }

  template<class Fn> Fn for_each_a(Fn p) {
    return std::for_each(a.begin(), a.end(), p);
  }

  template<class T> std::vector<TableA>::iterator find_in_a(const T &p) {
    return std::find(a.begin(), a.end(), p);
  }
  template<class Comp> std::vector<TableA>::iterator find_in_a_if(Comp p) {
    return std::find_if(a.begin(), a.end(), p);
  }

  template<class T>   typename std::iterator_traits<std::vector<TableA>::iterator>::difference_type count_in_a(const T &p) {
    return std::count(a.begin(), a.end(), p);
  }
  template<class Comp>   typename std::iterator_traits<std::vector<TableA>::iterator>::difference_type count_in_a_if(Comp p) {
    return std::count_if(a.begin(), a.end(), p);
  }

  template<class T> void fill_b(const T &v) {
    std::fill(b.begin(), b.end(), v);
  }

  template<class Generator> void generate_b(Generator gen) {
    std::generate(b.begin(), b.end(), gen);
  }

  template<class T> std::vector<TableB>::iterator remove_b(const T &v) {
    return std::remove(b.begin(), b.end(), v);
  }
  template<class Pred> std::vector<TableB>::iterator remove_b_if(Pred v) {
    return std::remove_if(b.begin(), b.end(), v);
  }

  template<class T> void erase_b(const T &v) {
    b.erase(remove_b(v));
  }
  template<class Pred> void erase_b_if(Pred v) {
    b.erase(remove_b_if(v));
  }

  void reverse_b() {
    std::reverse(b.begin(), b.end());
  }

  void rotate_b(std::vector<TableB>::iterator i) {
    std::rotate(b.begin(), i, b.end());
  }

  template<class Comp> void sort_b(Comp p) {
    std::sort(b.begin(), b.end(), p);
  }

  template<class Comp> bool any_of_b(Comp p) {
    return std::any_of(b.begin(), b.end(), p);
  }
  template<class T> bool any_of_b_is(const T &p) {
    return any_of_b([&p](const TableB &x) { return p == x; });
  }

  template<class Comp> bool all_of_b(Comp p) {
    return std::all_of(b.begin(), b.end(), p);
  }
  template<class T> bool all_of_b_are(const T &p) {
    return all_of_b([&p](const TableB &x) { return p == x; });
  }

  template<class Comp> bool none_of_b(Comp p) {
    return std::none_of(b.begin(), b.end(), p);
  }
  template<class T> bool none_of_b_is(const T &p) {
    return none_of_b([&p](const TableB &x) { return p == x; });
  }

  template<class Fn> Fn for_each_b(Fn p) {
    return std::for_each(b.begin(), b.end(), p);
  }

  template<class T> std::vector<TableB>::iterator find_in_b(const T &p) {
    return std::find(b.begin(), b.end(), p);
  }
  template<class Comp> std::vector<TableB>::iterator find_in_b_if(Comp p) {
    return std::find_if(b.begin(), b.end(), p);
  }

  template<class T>   typename std::iterator_traits<std::vector<TableB>::iterator>::difference_type count_in_b(const T &p) {
    return std::count(b.begin(), b.end(), p);
  }
  template<class Comp>   typename std::iterator_traits<std::vector<TableB>::iterator>::difference_type count_in_b_if(Comp p) {
    return std::count_if(b.begin(), b.end(), p);
  }

  template<class T> void fill_c(const T &v) {
    std::fill(c.begin(), c.end(), v);
  }

  template<class Generator> void generate_c(Generator gen) {
    std::generate(c.begin(), c.end(), gen);
  }

  template<class T> std::vector<std::unique_ptr<TableA>>::iterator remove_c(const T &v) {
    return std::remove(c.begin(), c.end(), v);
  }
  template<class Pred> std::vector<std::unique_ptr<TableA>>::iterator remove_c_if(Pred v) {
    return std::remove_if(c.begin(), c.end(), v);
  }

  template<class T> void erase_c(const T &v) {
    c.erase(remove_c(v));
  }
  template<class Pred> void erase_c_if(Pred v) {
    c.erase(remove_c_if(v));
  }

  void reverse_c() {
    std::reverse(c.begin(), c.end());
  }

  void rotate_c(std::vector<std::unique_ptr<TableA>>::iterator i) {
    std::rotate(c.begin(), i, c.end());
  }

  template<class Comp> void sort_c(Comp p) {
    std::sort(c.begin(), c.end(), p);
  }

  template<class Comp> bool any_of_c(Comp p) {
    return std::any_of(c.begin(), c.end(), p);
  }
  template<class T> bool any_of_c_is(const T &p) {
    return any_of_c([&p](const TableA &x) { return *p == x; });
  }

  template<class Comp> bool all_of_c(Comp p) {
    return std::all_of(c.begin(), c.end(), p);
  }
  template<class T> bool all_of_c_are(const T &p) {
    return all_of_c([&p](const TableA &x) { return *p == x; });
  }

  template<class Comp> bool none_of_c(Comp p) {
    return std::none_of(c.begin(), c.end(), p);
  }
  template<class T> bool none_of_c_is(const T &p) {
    return none_of_c([&p](const TableA &x) { return *p == x; });
  }

  template<class Fn> Fn for_each_c(Fn p) {
    return std::for_each(c.begin(), c.end(), p);
  }

  template<class T> std::vector<std::unique_ptr<TableA>>::iterator find_in_c(const T &p) {
    return std::find(c.begin(), c.end(), p);
  }
  template<class Comp> std::vector<std::unique_ptr<TableA>>::iterator find_in_c_if(Comp p) {
    return std::find_if(c.begin(), c.end(), p);
  }

  template<class T>   typename std::iterator_traits<std::vector<std::unique_ptr<TableA>>::iterator>::difference_type count_in_c(const T &p) {
    return std::count(c.begin(), c.end(), p);
  }
  template<class Comp>   typename std::iterator_traits<std::vector<std::unique_ptr<TableA>>::iterator>::difference_type count_in_c_if(Comp p) {
    return std::count_if(c.begin(), c.end(), p);
  }

  template<class T> void fill_d(const T &v) {
    std::fill(d.begin(), d.end(), v);
  }

  template<class Generator> void generate_d(Generator gen) {
    std::generate(d.begin(), d.end(), gen);
  }

  template<class T> std::vector<std::shared_ptr<TableB>>::iterator remove_d(const T &v) {
    return std::remove(d.begin(), d.end(), v);
  }
  template<class Pred> std::vector<std::shared_ptr<TableB>>::iterator remove_d_if(Pred v) {
    return std::remove_if(d.begin(), d.end(), v);
  }

  template<class T> void erase_d(const T &v) {
    d.erase(remove_d(v));
  }
  template<class Pred> void erase_d_if(Pred v) {
    d.erase(remove_d_if(v));
  }

  void reverse_d() {
    std::reverse(d.begin(), d.end());
  }

  void rotate_d(std::vector<std::shared_ptr<TableB>>::iterator i) {
    std::rotate(d.begin(), i, d.end());
  }

  template<class Comp> void sort_d(Comp p) {
    std::sort(d.begin(), d.end(), p);
  }

  template<class Comp> bool any_of_d(Comp p) {
    return std::any_of(d.begin(), d.end(), p);
  }
  template<class T> bool any_of_d_is(const T &p) {
    return any_of_d([&p](const TableB &x) { return *p == x; });
  }

  template<class Comp> bool all_of_d(Comp p) {
    return std::all_of(d.begin(), d.end(), p);
  }
  template<class T> bool all_of_d_are(const T &p) {
    return all_of_d([&p](const TableB &x) { return *p == x; });
  }

  template<class Comp> bool none_of_d(Comp p) {
    return std::none_of(d.begin(), d.end(), p);
  }
  template<class T> bool none_of_d_is(const T &p) {
    return none_of_d([&p](const TableB &x) { return *p == x; });
  }

  template<class Fn> Fn for_each_d(Fn p) {
    return std::for_each(d.begin(), d.end(), p);
  }

  template<class T> std::vector<std::shared_ptr<TableB>>::iterator find_in_d(const T &p) {
    return std::find(d.begin(), d.end(), p);
  }
  template<class Comp> std::vector<std::shared_ptr<TableB>>::iterator find_in_d_if(Comp p) {
    return std::find_if(d.begin(), d.end(), p);
  }

  template<class T>   typename std::iterator_traits<std::vector<std::shared_ptr<TableB>>::iterator>::difference_type count_in_d(const T &p) {
    return std::count(d.begin(), d.end(), p);
  }
  template<class Comp>   typename std::iterator_traits<std::vector<std::shared_ptr<TableB>>::iterator>::difference_type count_in_d_if(Comp p) {
    return std::count_if(d.begin(), d.end(), p);
  }

  template<class T> void fill_e(const T &v) {
    std::fill(e.begin(), e.end(), v);
  }

  template<class Generator> void generate_e(Generator gen) {
    std::generate(e.begin(), e.end(), gen);
  }

  template<class T> std::vector<std::weak_ptr<TableB>>::iterator remove_e(const T &v) {
    return std::remove(e.begin(), e.end(), v);
  }
  template<class Pred> std::vector<std::weak_ptr<TableB>>::iterator remove_e_if(Pred v) {
    return std::remove_if(e.begin(), e.end(), v);
  }

  template<class T> void erase_e(const T &v) {
    e.erase(remove_e(v));
  }
  template<class Pred> void erase_e_if(Pred v) {
    e.erase(remove_e_if(v));
  }

  void reverse_e() {
    std::reverse(e.begin(), e.end());
  }

  void rotate_e(std::vector<std::weak_ptr<TableB>>::iterator i) {
    std::rotate(e.begin(), i, e.end());
  }

  template<class Comp> void sort_e(Comp p) {
    std::sort(e.begin(), e.end(), p);
  }

  template<class Comp> bool any_of_e(Comp p) {
    return std::any_of(e.begin(), e.end(), p);
  }
  template<class T> bool any_of_e_is(const T &p) {
    return any_of_e([&p](const TableB &x) { return *p.lock() == x; });
  }

  template<class Comp> bool all_of_e(Comp p) {
    return std::all_of(e.begin(), e.end(), p);
  }
  template<class T> bool all_of_e_are(const T &p) {
    return all_of_e([&p](const TableB &x) { return *p.lock() == x; });
  }

  template<class Comp> bool none_of_e(Comp p) {
    return std::none_of(e.begin(), e.end(), p);
  }
  template<class T> bool none_of_e_is(const T &p) {
    return none_of_e([&p](const TableB &x) { return *p.lock() == x; });
  }

  template<class Fn> Fn for_each_e(Fn p) {
    return std::for_each(e.begin(), e.end(), p);
  }

  template<class T> std::vector<std::weak_ptr<TableB>>::iterator find_in_e(const T &p) {
    return std::find(e.begin(), e.end(), p);
  }
  template<class Comp> std::vector<std::weak_ptr<TableB>>::iterator find_in_e_if(Comp p) {
    return std::find_if(e.begin(), e.end(), p);
  }

  template<class T>   typename std::iterator_traits<std::vector<std::weak_ptr<TableB>>::iterator>::difference_type count_in_e(const T &p) {
    return std::count(e.begin(), e.end(), p);
  }
  template<class Comp>   typename std::iterator_traits<std::vector<std::weak_ptr<TableB>>::iterator>::difference_type count_in_e_if(Comp p) {
    return std::count_if(e.begin(), e.end(), p);
  }

};

struct TableC_io {
private:
  unsigned int TableA_count_{0};
  std::vector<std::shared_ptr<TableA>> TableA_references_;

  unsigned int TableB_count_{0};
  std::vector<std::shared_ptr<TableB>> TableB_references_;

  unsigned int TableD_count_{0};
  std::vector<std::shared_ptr<TableD>> TableD_references_;

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

  template<typename T> void Write(std::ostream &o, const std::unique_ptr<T> &v) {
    if (!v) {
      o.write("\x0", 1);
    } else {
      o.write("\x1", 1);
      Write(o, *v);
    }
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

  template<typename T> void Write(std::ostream &o, const std::vector<std::unique_ptr<T>> &v) {
    Write(o, v.size());
    for (const auto &entry : v)
      Write(o, entry);
  }

  template<typename T> void Write(std::ostream &o, const std::vector<std::shared_ptr<T>> &v) {
    Write(o, v.size());
    for (const auto &entry : v)
      Write(o, entry);
  }

  template<typename T> void Write(std::ostream &o, const std::vector<std::weak_ptr<T>> &v) {
    Write(o, v.size());
    for (const auto &entry : v)
      Write(o, entry);
  }

  template<typename T> void Write(std::ostream &, const std::shared_ptr<T> &) {
    static_assert(AlwaysFalse<T>::value, "Something not implemented");
  }

  template<typename T> void Write(std::ostream &, const std::weak_ptr<T> &) {
    static_assert(AlwaysFalse<T>::value, "Something not implemented");
  }

  void Write(std::ostream &o, const std::string &v) {
    Write(o, v.size());
    o.write(v.data(), v.size());
  }

  template<typename T> void Read(std::istream &i, T &v) {
    i.read(reinterpret_cast<char *>(&v), sizeof(T));
  }

  template<typename T> void Read(std::istream &i, std::unique_ptr<T> &v) {
    char ref = 0;
    i.read(&ref, 1);
    if (ref == '\x1') {
      v = std::unique_ptr<T>(new T);
      Read(i, *v);
    }
  }

  template<typename T> void Read(std::istream &, std::shared_ptr<T> &) {
    static_assert(AlwaysFalse<T>::value, "Something not implemented");
  }

  template<typename T> void Read(std::istream &, std::weak_ptr<T> &) {
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

  template<typename T> void Read(std::istream &s, std::vector<std::unique_ptr<T>> &v) {
    auto size = v.size();
    Read(s, size);
    v.resize(size);
    for (auto &entry : v)
      Read(s, entry);
  }

  template<typename T> void Read(std::istream &s, std::vector<std::shared_ptr<T>> &v) {
    auto size = v.size();
    Read(s, size);
    v.resize(size);
    for (auto &entry : v)
      Read(s, entry);
  }

  template<typename T> void Read(std::istream &s, std::vector<std::weak_ptr<T>> &v) {
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

  void Write(std::ostream &o, const TableA &v) {
    Write(o, v.name);
    Write(o, v.d1);
    Write(o, v.d2);
    Write(o, v.d3);
    Write(o, v.d4);
  }

  void Write(std::ostream &o, const std::shared_ptr<TableA> &v) {
    Write(o, v, TableA_count_);
  }

  void Write(std::ostream &o, const std::vector<TableA> &v) {
    Write(o, v.size());
    for (const auto &entry : v)
      Write(o, entry);
  }

  void Read(std::istream &s, TableA &v) {
    Read(s, v.name);
    Read(s, v.d1);
    Read(s, v.d2);
    Read(s, v.d3);
    Read(s, v.d4);
  }

  void Read(std::istream &s, std::shared_ptr<TableA> &v) {
    Read(s, v, TableA_references_);
  }

  void Read(std::istream &s, std::vector<TableA> &v) {
    auto size = v.size();
    Read(s, size);
    v.resize(size);
    for (auto &entry : v)
      Read(s, entry);
  }

  void Write(std::ostream &o, const TableB &v) {
    Write(o, v.name);
  }

  void Write(std::ostream &o, const std::shared_ptr<TableB> &v) {
    Write(o, v, TableB_count_);
  }

  void Write(std::ostream &o, const std::weak_ptr<TableB> &v) {
    Write(o, v.lock(), TableB_count_);
  }

  void Write(std::ostream &o, const std::vector<TableB> &v) {
    Write(o, v.size());
    for (const auto &entry : v)
      Write(o, entry);
  }

  void Read(std::istream &s, TableB &v) {
    Read(s, v.name);
  }

  void Read(std::istream &s, std::shared_ptr<TableB> &v) {
    Read(s, v, TableB_references_);
  }

  void Read(std::istream &s, std::weak_ptr<TableB> &v) {
    auto t = v.lock();
    Read(s, t, TableB_references_);
    v = t;
  }

  void Read(std::istream &s, std::vector<TableB> &v) {
    auto size = v.size();
    Read(s, size);
    v.resize(size);
    for (auto &entry : v)
      Read(s, entry);
  }

  void Write(std::ostream &o, const TableD &v) {
    Write(o, v.name);
    Write(o, v.a);
  }

  void Write(std::ostream &o, const std::shared_ptr<TableD> &v) {
    Write(o, v, TableD_count_);
  }

  void Write(std::ostream &o, const std::weak_ptr<TableD> &v) {
    Write(o, v.lock(), TableD_count_);
  }

  void Read(std::istream &s, TableD &v) {
    Read(s, v.name);
    Read(s, v.a);
  }

  void Read(std::istream &s, std::shared_ptr<TableD> &v) {
    Read(s, v, TableD_references_);
  }

  void Read(std::istream &s, std::weak_ptr<TableD> &v) {
    auto t = v.lock();
    Read(s, t, TableD_references_);
    v = t;
  }

  void Write(std::ostream &o, const TableC &v) {
    Write(o, v.a);
    Write(o, v.b);
    Write(o, v.c);
    Write(o, v.d);
    Write(o, v.e);
  }

  void Read(std::istream &s, TableC &v) {
    Read(s, v.a);
    Read(s, v.b);
    Read(s, v.c);
    Read(s, v.d);
    Read(s, v.e);
  }

public:
  void WriteTableC(std::ostream &o, const TableC &v) {
    TableA_count_ = 0;
    TableB_count_ = 0;
    TableD_count_ = 0;

    o.write("CORE", 4);
    o.write("0.0", 3);
    Write(o, v);
  }

  bool ReadTableC(std::istream &i, TableC &v) {
    TableA_references_.clear();
    TableB_references_.clear();
    TableD_references_.clear();

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
