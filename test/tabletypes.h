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
};

bool operator==(const TableA&l, const TableA&r) {
  return 
    l.name == r.name
    && l.d1 == r.d1
    && l.d2.lock() == r.d2.lock()
    && l.d3 == r.d3
    && l.d4 == r.d4;
}

bool operator!=(const TableA&l, const TableA&r) {
  return 
    l.name != r.name
    || l.d1 != r.d1
    || l.d2.lock() != r.d2.lock()
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
    && l.b == r.b;
}

bool operator!=(const TableC&l, const TableC&r) {
  return 
    l.a != r.a
    || l.b != r.b;
}

struct TableC_io {
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

template<typename T> void Write(std::ostream &, const std::unique_ptr<T> &) {
  static_assert(AlwaysFalse<T>::value, "Something not implemented");
}

template<typename T> void Write(std::ostream &, const std::shared_ptr<T> &) {
  static_assert(AlwaysFalse<T>::value, "Something not implemented");
}

template<typename T> void Write(std::ostream &, const std::weak_ptr<T> &) {
  static_assert(AlwaysFalse<T>::value, "Something not implemented");
}

template<typename T> void Write(std::ostream &, const std::vector<std::unique_ptr<T>> &) {
  static_assert(AlwaysFalse<T>::value, "Something not implemented");
}

template<typename T> void Write(std::ostream &, const std::vector<std::shared_ptr<T>> &) {
  static_assert(AlwaysFalse<T>::value, "Something not implemented");
}

template<typename T> void Write(std::ostream &, const std::vector<std::weak_ptr<T>> &) {
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

void Write(std::ostream &o, const TableA &v) {
  Write(o, v.name);
  if (!v.d1) {
    o.write("\x0", 1);
  } else {
    o.write("\x1", 1);
    Write(o, *v.d1);
  }
  {
    const auto t = v.d2.lock();
    if (!t) {
      o.write("\x0", 1);
    } else if (t->io_counter_== 0) {
      t->io_counter_ = ++TableD_count_;
      o.write("\x1", 1);
      Write(o, *t);
    } else {
      o.write("\x2", 1);
      Write(o, t->io_counter_);
    }
  }
  {
    const auto t = v.d3;
    if (!t) {
      o.write("\x0", 1);
    } else if (t->io_counter_== 0) {
      t->io_counter_ = ++TableD_count_;
      o.write("\x1", 1);
      Write(o, *t);
    } else {
      o.write("\x2", 1);
      Write(o, t->io_counter_);
    }
  }
  {
    const auto t = v.d4;
    if (!t) {
      o.write("\x0", 1);
    } else if (t->io_counter_== 0) {
      t->io_counter_ = ++TableD_count_;
      o.write("\x1", 1);
      Write(o, *t);
    } else {
      o.write("\x2", 1);
      Write(o, t->io_counter_);
    }
  }
}

void Read(std::istream &s, TableA &v) {
  Read(s, v.name);
  {
    char ref = 0;
    s.read(&ref, 1);
    if (ref == '\x1') {
      v.d1 = std::make_unique<TableD>();
      Read(s, *v.d1);
    }
  }
  {
    char ref = 0;
    s.read(&ref, 1);
    if (ref == '\x1') {
      auto t = std::make_shared<TableD>();
      TableD_references_.push_back(t);
      Read(s, *t);
      v.d2 = t;
    } else if (ref == '\x2') {
      unsigned int index = 0;
      Read(s, index);
      v.d2 = TableD_references_[index - 1];
    }
  }
  {
    char ref = 0;
    s.read(&ref, 1);
    if (ref == '\x1') {
      auto t = std::make_shared<TableD>();
      TableD_references_.push_back(t);
      Read(s, *t);
      v.d3 = t;
    } else if (ref == '\x2') {
      unsigned int index = 0;
      Read(s, index);
      v.d3 = TableD_references_[index - 1];
    }
  }
  {
    char ref = 0;
    s.read(&ref, 1);
    if (ref == '\x1') {
      auto t = std::make_shared<TableD>();
      TableD_references_.push_back(t);
      Read(s, *t);
      v.d4 = t;
    } else if (ref == '\x2') {
      unsigned int index = 0;
      Read(s, index);
      v.d4 = TableD_references_[index - 1];
    }
  }
}

void Write(std::ostream &o, const TableB &v) {
  Write(o, v.name);
}

void Read(std::istream &s, TableB &v) {
  Read(s, v.name);
}

void Write(std::ostream &o, const TableD &v) {
  Write(o, v.name);
  {
    const auto t = v.a;
    if (!t) {
      o.write("\x0", 1);
    } else if (t->io_counter_== 0) {
      t->io_counter_ = ++TableA_count_;
      o.write("\x1", 1);
      Write(o, *t);
    } else {
      o.write("\x2", 1);
      Write(o, t->io_counter_);
    }
  }
}

void Read(std::istream &s, TableD &v) {
  Read(s, v.name);
  {
    char ref = 0;
    s.read(&ref, 1);
    if (ref == '\x1') {
      auto t = std::make_shared<TableA>();
      TableA_references_.push_back(t);
      Read(s, *t);
      v.a = t;
    } else if (ref == '\x2') {
      unsigned int index = 0;
      Read(s, index);
      v.a = TableA_references_[index - 1];
    }
  }
}

void Write(std::ostream &o, const TableC &v) {
  o << v.a.size();
  for (const auto &entry : v.a) {
    Write(o, entry);
  };
  o << v.b.size();
  for (const auto &entry : v.b) {
    Write(o, entry);
  };
}

void Read(std::istream &s, TableC &v) {
  std::vector<TableA>::size_type a_size{0};
  s >> a_size;
  v.a.resize(a_size);
  for (auto &entry : v.a) {
    Read(s, entry);
  };
  std::vector<TableB>::size_type b_size{0};
  s >> b_size;
  v.b.resize(b_size);
  for (auto &entry : v.b) {
    Read(s, entry);
  };
}

  unsigned int TableA_count_{0};
  unsigned int TableD_count_{0};
  std::vector<std::shared_ptr<TableA>> TableA_references_;
  std::vector<std::shared_ptr<TableD>> TableD_references_;

public:
void WriteTableC(std::ostream &o, const TableC &v) {
  TableA_count_ = 0;
  TableD_count_ = 0;

  o.write("CORE", 4);
  o.write("0.0", 3);
  Write(o, v);
}

bool ReadTableC(std::istream &i, TableC &v) {
  TableA_references_.clear();
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
