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
struct AB;

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

struct AB {
  AB() = default;
  AB(const AB &o) { _clone(o); }
  AB& operator=(const AB &o) { _destroy(); _clone(o); return *this; }

  AB(const A &v)
    : _A(new A(v))
    , _selection(_A_selection)
  {}
  AB(A &&v)
    : _A(new A(std::forward<A>(v)))
    , _selection(_A_selection)
  {}
  AB & operator=(const A &v) {
    _destroy();
    _A = new A(v);
    _selection = _A_selection;
    return *this;
  }
  AB & operator=(A &&v) {
    _destroy();
    _A = new A(std::forward<A>(v));
    _selection = _A_selection;
    return *this;
  }

  AB(const B &v)
    : _B(new B(v))
    , _selection(_B_selection)
  {}
  AB(B &&v)
    : _B(new B(std::forward<B>(v)))
    , _selection(_B_selection)
  {}
  AB & operator=(const B &v) {
    _destroy();
    _B = new B(v);
    _selection = _B_selection;
    return *this;
  }
  AB & operator=(B &&v) {
    _destroy();
    _B = new B(std::forward<B>(v));
    _selection = _B_selection;
    return *this;
  }

  ~AB() {
    _destroy();
  }

  bool is_Defined() const noexcept { return _selection != no_selection; }
  bool is_A() const noexcept { return _selection == _A_selection; }
  const A & as_A() const noexcept { return *_A; }
  A & as_A() { return *_A; }

  bool is_B() const noexcept { return _selection == _B_selection; }
  const B & as_B() const noexcept { return *_B; }
  B & as_B() { return *_B; }

  friend bool operator==(const AB&ab, const A &o) noexcept  { return ab.is_A() && ab.as_A() == o; }
  friend bool operator==(const A &o, const AB&ab) noexcept  { return ab.is_A() && o == ab.as_A(); }
  friend bool operator!=(const AB&ab, const A &o) noexcept  { return !ab.is_A() || ab.as_A() != o; }
  friend bool operator!=(const A &o, const AB&ab) noexcept  { return !ab.is_A() || o != ab.as_A(); }

  friend bool operator==(const AB&ab, const B &o) noexcept  { return ab.is_B() && ab.as_B() == o; }
  friend bool operator==(const B &o, const AB&ab) noexcept  { return ab.is_B() && o == ab.as_B(); }
  friend bool operator!=(const AB&ab, const B &o) noexcept  { return !ab.is_B() || ab.as_B() != o; }
  friend bool operator!=(const B &o, const AB&ab) noexcept  { return !ab.is_B() || o != ab.as_B(); }

  bool operator==(const AB &o) const noexcept
  {
    if (this == &o)
      return true;
    if (_selection != o._selection)
      return false;
    switch(_selection) {
    case no_selection: while(false); /* hack for coverage tool */ return true;
    case _A_selection: return *_A == *o._A;
    case _B_selection: return *_B == *o._B;
    }
    return false; // without this line there is a msvc warning I do not understand.
  }

  bool operator!=(const AB &o) const noexcept
  {
    if (this == &o)
      return false;
    if (_selection != o._selection)
      return true;
    switch(_selection) {
    case no_selection: while(false); /* hack for coverage tool */ return false;
    case _A_selection: return *_A != *o._A;
    case _B_selection: return *_B != *o._B;
    }
    return false; // without this line there is a msvc warning I do not understand.
  }

private:
  void _clone(const AB &o) noexcept
  {
     _selection = o._selection;
    switch(_selection) {
    case no_selection: while(false); /* hack for coverage tool */ break;
    case _A_selection: _A = new A(*o._A); break;
    case _B_selection: _B = new B(*o._B); break;
    }
  }

  void _destroy() noexcept {
    switch(_selection) {
    case no_selection: while(false); /* hack for coverage tool */ break;
    case _A_selection: delete _A; break;
    case _B_selection: delete _B; break;
    }
  }

  union {
    A * _A;
    B * _B;
  };

  enum Selection_t {
    no_selection,
    _A_selection,
    _B_selection,
  };

  Selection_t _selection{no_selection};
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
