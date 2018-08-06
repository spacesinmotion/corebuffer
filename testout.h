#pragma once

#include <vector>
#include <string>
#include <ostream>
#include <istream>
#include <memory>
#include <array>

namespace SubC {
namespace scope {

struct XXX;
struct YYY;

static void Write(std::ostream &, const XXX &);
static void Read(std::istream &, XXX &);

static void Write(std::ostream &, const YYY &);
static void Read(std::istream &, YYY &);

enum class Access : unsigned int {
  Private = 1,
  Public = 3,
  Protected = 4,
};

struct XXX {
  std::int32_t bill{1};
  bool aul{false};
  float kaul;
  std::vector<float> baul;
private:
  unsigned int io_counter_{0};
  friend struct YYY_io;
};

struct YYY {
  std::unique_ptr<XXX> still;
  std::vector<std::shared_ptr<YYY>> sub;
private:
  unsigned int io_counter_{0};
  friend struct YYY_io;
};

inline const std::array<Access,3> & AccessValues() {
  static const std::array<Access,3> values {
    Access::Private,
    Access::Public,
    Access::Protected,
  };
  return values;
};

inline const char * ValueName(const Access &v) {
  switch(v) {
    case Access::Private: return "Private";
    case Access::Public: return "Public";
    case Access::Protected: return "Protected";
  }
};

struct YYY_io {
private:
template<typename T> void Write(std::ostream &o, const T *v) {
  static_assert(false, "Something not implemented");
}

template<typename T> void Write(std::ostream &o, const T &v) {
  o.write(reinterpret_cast<const char *>(&v), sizeof(T));
}

template<typename T> void Write(std::ostream &o, const std::vector<T> &v) {
  Write(o, v.size());
  o.write(reinterpret_cast<const char *>(v.data()), sizeof(T) * v.size());
}

template<typename T> void Write(std::ostream &o, const std::unique_ptr<T> &v) {
  static_assert(false, "Something not implemented");
}

template<typename T> void Write(std::ostream &o, const std::shared_ptr<T> &v) {
  static_assert(false, "Something not implemented");
}

template<typename T> void Write(std::ostream &o, const std::weak_ptr<T> &v) {
  static_assert(false, "Something not implemented");
}

template<typename T> void Write(std::ostream &o, const std::vector<std::unique_ptr<T>> &v) {
  static_assert(false, "Something not implemented");
}

template<typename T> void Write(std::ostream &o, const std::vector<std::shared_ptr<T>> &v) {
  static_assert(false, "Something not implemented");
}

template<typename T> void Write(std::ostream &o, const std::vector<std::weak_ptr<T>> &v) {
  static_assert(false, "Something not implemented");
}

template<> void Write(std::ostream &o, const std::string &v) {
  Write(o, v.size());
  o.write(v.data(), v.size());
}

template<> void Write(std::ostream &o, const char *v) {
  Write(o, std::string(v));
}

template<typename T> void Read(std::istream &i, T &v) {
  i.read(reinterpret_cast<char *>(&v), sizeof(T));
}

template<typename T> void Read(std::istream &i, std::vector<T> &v) {
  std::vector<T>::size_type s{0};
  Read(i, s);
  v.resize(s);
  i.read(reinterpret_cast<char *>(v.data()), sizeof(T) * s);
}

template<> void Read(std::istream &i, std::string &v) {
  std::string::size_type s{0};
  Read(i, s);
  v.resize(s);
  i.read(&v[0], s);
}

void Write(std::ostream &o, const XXX &v) {
  Write(o, v.bill);
  Write(o, v.aul);
  Write(o, v.kaul);
  Write(o, v.baul);
}

void Read(std::istream &s, XXX &v) {
  Read(s, v.bill);
  Read(s, v.aul);
  Read(s, v.kaul);
  Read(s, v.baul);
}

void Write(std::ostream &o, const YYY &v) {
  if (!v.still) {
    o.write("\x0", 1);
  } else {
    o.write("\x1", 1);
    Write(o, *v.still);
  }
  o << v.sub.size();
  for (const auto &entry : v.sub) {
    const auto t = entry;
  if (!t) {
    o.write("\x0", 1);
  } else if (t->io_counter_== 0) {
    o.write("\x1", 1);
    Write(o, *t);
    t->io_counter_ = ++YYY_count_;
  } else {
    o.write("\x2", 1);
    Write(o, t->io_counter_);
  }
  };
}

void Read(std::istream &s, YYY &v) {
  {
    char ref = 0;
    s.read(&ref, 1);
    if (ref == '\x1') {
      v.still = std::make_unique<XXX>();
      Read(s, *v.still);
    }
  }
  std::vector<std::shared_ptr<YYY>>::size_type sub_size{0};
  s >> sub_size;
  v.sub.resize(sub_size);
  for (auto &entry : v.sub) {
    {
    char ref = 0;
    s.read(&ref, 1);
    if (ref == '\x1') {
      auto t = std::make_shared<YYY>();
      Read(s, *t);
      YYY_references_.push_back(t);
      entry = t;
    } else if (ref == '\x2') {
      unsigned int index = 0;
      Read(s, index);
      entry = YYY_references_[index - 1];
    }
  }
  };
}

  unsigned int XXX_count_{0};
  unsigned int YYY_count_{0};
  std::vector<std::shared_ptr<XXX>> XXX_references_;
  std::vector<std::shared_ptr<YYY>> YYY_references_;

public:
void WriteYYY(std::ostream &o, const YYY &v) {
  XXX_count_ = 0;
  YYY_count_ = 0;

  o.write("CORE", 4);
  o.write("0.1.203", 7);
  Write(o, v);
}

bool ReadYYY(std::istream &i, YYY &v) {
  XXX_references_.clear();
  YYY_references_.clear();

  std::string marker("0000");
  i.read(&marker[0], 4);
  if (marker != "CORE")
    return false;
  std::string version(7, '_');
  i.read(&version[0], 7);
  if (version != "0.1.203")
    return false;
  Read(i, v);
  return true;
}

};
}
}
