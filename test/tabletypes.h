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

struct TableA;
struct TableB;
struct TableD;
struct TableC;

struct TableA {
  std::string name;
  std::unique_ptr<TableD> d1;
  std::weak_ptr<TableD> d2;
  std::shared_ptr<TableD> d3;
  std::shared_ptr<TableD> d4;
private:
  unsigned int io_counter_{0};
  friend struct TableC_io;
};

struct TableB {
  std::string name;
private:
  unsigned int io_counter_{0};
  friend struct TableC_io;
};

struct TableD {
  std::string name;
  std::shared_ptr<TableA> a;
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
};

template<typename T> bool operator==(const std::weak_ptr<T> &l, const std::weak_ptr<T> &r) {
  return l.lock() == r.lock();
}

template<typename T> bool operator!=(const std::weak_ptr<T> &l, const std::weak_ptr<T> &r) {
  return l.lock() != r.lock();
}

bool operator==(const TableA&l, const TableA&r) {
  return 
    l.name == r.name
    && l.d1 == r.d1
    && l.d2 == r.d2
    && l.d3 == r.d3
    && l.d4 == r.d4;
}

bool operator!=(const TableA&l, const TableA&r) {
  return 
    l.name != r.name
    || l.d1 != r.d1
    || l.d2 != r.d2
    || l.d3 != r.d3
    || l.d4 != r.d4;
}

bool operator==(const TableB&l, const TableB&r) {
  return 
    l.name == r.name;
}

bool operator!=(const TableB&l, const TableB&r) {
  return 
    l.name != r.name;
}

bool operator==(const TableD&l, const TableD&r) {
  return 
    l.name == r.name
    && l.a == r.a;
}

bool operator!=(const TableD&l, const TableD&r) {
  return 
    l.name != r.name
    || l.a != r.a;
}

bool operator==(const TableC&l, const TableC&r) {
  return 
    l.a == r.a
    && l.b == r.b
    && l.c == r.c
    && l.d == r.d
    && l.e == r.e;
}

bool operator!=(const TableC&l, const TableC&r) {
  return 
    l.a != r.a
    || l.b != r.b
    || l.c != r.c
    || l.d != r.d
    || l.e != r.e;
}

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
