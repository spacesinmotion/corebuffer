#pragma once

#include <vector>
#include <string>
#include <ostream>
#include <istream>
#include <memory>
#include <array>
#include <algorithm>
#include <type_traits>

namespace UnionTypes {

template<typename T>
struct AlwaysFalse : std::false_type {};

struct A;
struct B;
struct AB;
struct Root;

template<typename T> bool operator==(const std::weak_ptr<T> &l, const std::weak_ptr<T> &r) {
  return l.lock() == r.lock();
}

template<typename T> bool operator!=(const std::weak_ptr<T> &l, const std::weak_ptr<T> &r) {
  return l.lock() != r.lock();
}

struct A {
  std::string name;

  A() = default;
  A(const std::string &name_)
    : name(name_)
  {}

  friend bool operator==(const A&l, const A&r) {
    return 
      l.name == r.name;
  }

  friend bool operator!=(const A&l, const A&r) {
    return 
      l.name != r.name;
  }
};

struct B {
  std::int64_t size{0};

  B() = default;
  B(const std::int64_t &size_)
    : size(size_)
  {}

  friend bool operator==(const B&l, const B&r) {
    return 
      l.size == r.size;
  }

  friend bool operator!=(const B&l, const B&r) {
    return 
      l.size != r.size;
  }
};

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
  void clear() { *this = AB(); }

  bool is_A() const noexcept { return _selection == _A_selection; }
  const A & as_A() const noexcept { return *_A; }
  A & as_A() { return *_A; }
  template<typename... Args> A & create_A(Args&&... args) {
    return (*this = A(std::forward<Args>(args)...)).as_A();
  }

  bool is_B() const noexcept { return _selection == _B_selection; }
  const B & as_B() const noexcept { return *_B; }
  B & as_B() { return *_B; }
  template<typename... Args> B & create_B(Args&&... args) {
    return (*this = B(std::forward<Args>(args)...)).as_B();
  }

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
    no_value = nullptr;
  }

  union {
    struct NoValue_t *no_value{nullptr};
    A * _A;
    B * _B;
  };

  enum Selection_t {
    no_selection,
    _A_selection,
    _B_selection,
  };

  Selection_t _selection{no_selection};
  unsigned int io_counter_{0};
  friend struct Root_io;
};

struct Root {
  A a;
  B b;
  std::shared_ptr<AB> c;
  std::weak_ptr<AB> cw;
  std::unique_ptr<AB> d;
  std::vector<AB> e;
  AB f;
  std::unique_ptr<AB> empty;
  std::unique_ptr<AB> null;

  Root() = default;

  friend bool operator==(const Root&l, const Root&r) {
    return 
      l.a == r.a
      && l.b == r.b
      && l.c == r.c
      && l.cw == r.cw
      && l.d == r.d
      && l.e == r.e
      && l.f == r.f
      && l.empty == r.empty
      && l.null == r.null;
  }

  friend bool operator!=(const Root&l, const Root&r) {
    return 
      l.a != r.a
      || l.b != r.b
      || l.c != r.c
      || l.cw != r.cw
      || l.d != r.d
      || l.e != r.e
      || l.f != r.f
      || l.empty != r.empty
      || l.null != r.null;
  }

  template<class T> void fill_e(const T &v) {
    std::fill(e.begin(), e.end(), v);
  }

  template<class Generator> void generate_e(Generator gen) {
    std::generate(e.begin(), e.end(), gen);
  }

  template<class T> std::vector<AB>::iterator remove_e(const T &v) {
    return std::remove(e.begin(), e.end(), v);
  }
  template<class Pred> std::vector<AB>::iterator remove_e_if(Pred v) {
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

  void rotate_e(std::vector<AB>::iterator i) {
    std::rotate(e.begin(), i, e.end());
  }

  template<class Comp> void sort_e(Comp p) {
    std::sort(e.begin(), e.end(), p);
  }

  template<class Comp> bool any_of_e(Comp p) {
    return std::any_of(e.begin(), e.end(), p);
  }
  template<class T> bool any_of_e_is(const T &p) {
    return any_of_e([&p](const AB &x) { return p == x; });
  }

  template<class Comp> bool all_of_e(Comp p) {
    return std::all_of(e.begin(), e.end(), p);
  }
  template<class T> bool all_of_e_are(const T &p) {
    return all_of_e([&p](const AB &x) { return p == x; });
  }

  template<class Comp> bool none_of_e(Comp p) {
    return std::none_of(e.begin(), e.end(), p);
  }
  template<class T> bool none_of_e_is(const T &p) {
    return none_of_e([&p](const AB &x) { return p == x; });
  }

  template<class Fn> Fn for_each_e(Fn p) {
    return std::for_each(e.begin(), e.end(), p);
  }

  template<class T> std::vector<AB>::iterator find_in_e(const T &p) {
    return std::find(e.begin(), e.end(), p);
  }
  template<class Comp> std::vector<AB>::iterator find_in_e_if(Comp p) {
    return std::find_if(e.begin(), e.end(), p);
  }

  template<class T>   typename std::iterator_traits<std::vector<AB>::iterator>::difference_type count_in_e(const T &p) {
    return std::count(e.begin(), e.end(), p);
  }
  template<class Comp>   typename std::iterator_traits<std::vector<AB>::iterator>::difference_type count_in_e_if(Comp p) {
    return std::count_if(e.begin(), e.end(), p);
  }
};

struct Root_io {
private:
  unsigned int AB_count_{0};
  std::vector<std::shared_ptr<AB>> AB_references_;

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

  void Write(std::ostream &o, const AB &v) {
    o.write(reinterpret_cast<const char*>(&v._selection), sizeof(AB::Selection_t));
    switch(v._selection) {
    case AB::no_selection: while(false); /* hack for coverage tool */ break;
    case AB::_A_selection: Write(o, v.as_A()); break;
    case AB::_B_selection: Write(o, v.as_B()); break;
    }
  }

  void Write(std::ostream &o, const std::shared_ptr<AB> &v) {
    Write(o, v, AB_count_);
  }

  void Write(std::ostream &o, const std::weak_ptr<AB> &v) {
    Write(o, v.lock(), AB_count_);
  }

  void Write(std::ostream &o, const std::vector<AB> &v) {
    Write(o, v.size());
    for (const auto &entry : v)
      Write(o, entry);
  }

  void Read(std::istream &i, AB &v) {
    i.read(reinterpret_cast<char*>(&v._selection), sizeof(AB::Selection_t));
    switch(v._selection) {
    case AB::no_selection: while(false); /* hack for coverage tool */ break;
    case AB::_A_selection: Read(i, v.create_A()); break;
    case AB::_B_selection: Read(i, v.create_B()); break;
    }
  }

  void Read(std::istream &s, std::shared_ptr<AB> &v) {
    Read(s, v, AB_references_);
  }

  void Read(std::istream &s, std::weak_ptr<AB> &v) {
    auto t = v.lock();
    Read(s, t, AB_references_);
    v = t;
  }

  void Read(std::istream &s, std::vector<AB> &v) {
    auto size = v.size();
    Read(s, size);
    v.resize(size);
    for (auto &entry : v)
      Read(s, entry);
  }

  void Write(std::ostream &o, const Root &v) {
    Write(o, v.a);
    Write(o, v.b);
    Write(o, v.c);
    Write(o, v.cw);
    Write(o, v.d);
    Write(o, v.e);
    Write(o, v.f);
    Write(o, v.empty);
    Write(o, v.null);
  }

  void Read(std::istream &s, Root &v) {
    Read(s, v.a);
    Read(s, v.b);
    Read(s, v.c);
    Read(s, v.cw);
    Read(s, v.d);
    Read(s, v.e);
    Read(s, v.f);
    Read(s, v.empty);
    Read(s, v.null);
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
