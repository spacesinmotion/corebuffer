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
  int bill{1};
  bool aul{false};
  float kaul;
  std::vector<float> baul;
private:
  unsigned int io_counter_{0};
  friend class YYY_io;
};

struct YYY {
  std::unique_ptr<XXX> still;
  std::vector<std::shared_ptr<YYY>> sub;
private:
  unsigned int io_counter_{0};
  friend class YYY_io;
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
inline void Write(std::ostream &o, std::int8_t i) {
  o.write("\x1", 1);
  o.write(reinterpret_cast<const char *>(&i), 1);
}

inline void Write(std::ostream &o, std::int16_t i) {
  if (std::numeric_limits<std::int8_t>::lowest() < i && i < std::numeric_limits<std::int8_t>::max())
    Write(o, std::int8_t(i));
  else {
    o.write("\x2", 1);
    o.write(reinterpret_cast<const char *>(&i), 2);
  }
}

inline void Write(std::ostream &o, std::int32_t i) {
  if (std::numeric_limits<std::int16_t>::lowest() < i && i < std::numeric_limits<std::int16_t>::max())
    Write(o, std::int16_t(i));
  else {
    o.write("\x4", 1);
    o.write(reinterpret_cast<const char *>(&i), 4);
  }
}

inline void Write(std::ostream &o, std::int64_t i) {
  if (std::numeric_limits<std::int32_t>::lowest() < i && i < std::numeric_limits<std::int32_t>::max())
    Write(o, std::int32_t(i));
  else {
    o.write("\x8", 1);
    o.write(reinterpret_cast<const char *>(&i), 8);
  }
}

inline void Write(std::ostream &o, std::uint8_t i) {
  o.write("\x1", 1);
  o.write(reinterpret_cast<const char *>(&i), 1);
}

inline void Write(std::ostream &o, std::uint16_t i) {
  if (i < std::numeric_limits<std::uint8_t>::max())
    Write(o, std::uint8_t(i));
  else {
    o.write("\x2", 1);
    o.write(reinterpret_cast<const char *>(&i), 2);
  }
}

inline void Write(std::ostream &o, std::uint32_t i) {
  if (i < std::numeric_limits<std::uint16_t>::max())
    Write(o, std::uint16_t(i));
  else {
    o.write("\x4", 1);
    o.write(reinterpret_cast<const char *>(&i), 4);
  }
}

inline void Write(std::ostream &o, std::uint64_t i) {
  if (i < std::numeric_limits<std::uint32_t>::max())
    Write(o, std::uint32_t(i));
  else {
    o.write("\x8", 1);
    o.write(reinterpret_cast<const char *>(&i), 8);
  }
}

inline void Write(std::ostream &o, float f) {
  o.write("\x4", 1);
  o.write(reinterpret_cast<const char *>(&f), 4);
}

inline void Write(std::ostream &o, double f) {
  if (f < double(std::numeric_limits<float>::max()) && f > double(std::numeric_limits<float>::lowest()))
    Write(o, float(f));
  else {
    o.write("\x8", 1);
    o.write(reinterpret_cast<const char *>(&f), 8);
  }
}

inline void Write(std::ostream &o, const std::string &t) {
  Write(o, t.size());
  o.write(t.c_str(), t.size());
}


inline void Read(std::istream &s, std::int8_t &i)
{
  char c[2];
  s.read(c, 2);
  i = *reinterpret_cast<std::int8_t *>(&c[1]);
}

inline void Read(std::istream &s, std::int16_t &i)
{
  char c;
  s.read(&c, 1);
  if (c == '\x1')
  {
    s.read(&c, 1);
    i = *reinterpret_cast<std::int8_t *>(&c);
  }
  else
  {
    s.read(reinterpret_cast<char *>(&i), 2);
  }
}

inline void Read(std::istream &s, std::int32_t i)
{
  char c[2];
  s.read(&c[0], 1);
  if (c[0] == '\x1')
  {
    s.read(c, 1);
    i = std::int32_t(*reinterpret_cast<std::int8_t *>(&c[0]));
  }
  else if (c[0] == '\x2')
  {
    s.read(c, 2);
    i = std::int32_t(*reinterpret_cast<std::int16_t *>(&c));
  }
  else
  {
    s.read(reinterpret_cast<char *>(&i), 4);
  }
}

inline void Read(std::istream &s, std::int64_t &i)
{
  char c[4];
  s.read(&c[0], 1);
  if (c[0] == '\x1')
  {
    s.read(c, 1);
    i = std::int64_t(*reinterpret_cast<std::int8_t *>(&c[0]));
  }
  else if (c[0] == '\x2')
  {
    s.read(c, 2);
    i = std::int64_t(*reinterpret_cast<std::int16_t *>(&c));
  }
  else if (c[0] == '\x4')
  {
    s.read(c, 4);
    i = std::int64_t(*reinterpret_cast<std::int32_t *>(&c));
  }
  else
  {
    s.read(reinterpret_cast<char *>(&i), 8);
  }
}

inline void Read(std::istream &s, std::uint8_t &i)
{
  char c[2];
  s.read(c, 2);
  i = *reinterpret_cast<std::uint8_t *>(&c[1]);
}

inline void Read(std::istream &s, std::uint16_t &i)
{
  char c;
  s.read(&c, 1);
  if (c == '\x1')
  {
    s.read(&c, 1);
    i = *reinterpret_cast<std::uint8_t *>(&c);
  }
  else
  {
    s.read(reinterpret_cast<char *>(&i), 2);
  }
}

inline void Read(std::istream &s, std::uint32_t &i)
{
  char c[2];
  s.read(&c[0], 1);
  if (c[0] == '\x1')
  {
    s.read(c, 1);
    i = std::uint32_t(*reinterpret_cast<std::uint8_t *>(&c[0]));
  }
  else if (c[0] == '\x2')
  {
    s.read(c, 2);
    i = std::uint32_t(*reinterpret_cast<std::uint16_t *>(&c));
  }
  else
  {
    s.read(reinterpret_cast<char *>(&i), 4);
  }
}

inline void Read(std::istream &s, float &f)
{
  char c[5];
  s.read(&c[0], 5);
  f = *reinterpret_cast<float *>(&c[1]);
}

inline void Read(std::istream &s, double &f)
{
  char c[4];
  s.read(&c[0], 1);
  if (c[0] == '\x4')
  {
    s.read(c, 4);
    f = double(*reinterpret_cast<float *>(c));
  }
  else
  {
    s.read(reinterpret_cast<char *>(&f), 4);
  }
}

inline void Read(std::istream &s, std::uint64_t &i)
{
  char c[4];
  s.read(&c[0], 1);
  if (c[0] == '\x1')
  {
    s.read(c, 1);
    i = std::uint64_t(*reinterpret_cast<std::uint8_t *>(&c[0]));
  }
  else if (c[0] == '\x2')
  {
    s.read(c, 2);
    i = std::uint64_t(*reinterpret_cast<std::uint16_t *>(&c));
  }
  else if (c[0] == '\x4')
  {
    s.read(c, 4);
    i = std::uint64_t(*reinterpret_cast<std::uint32_t *>(&c));
  }
  else
  {
    s.read(reinterpret_cast<char *>(&i), 8);
  }
}

inline void Read(std::istream &s, std::string &t)
{
  std::string::size_type size = 0;
  Read(s, size);
  t.resize(size);
  s.read(&t[0], size);
}
inline void Write(std::ostream &o, const Access &v) {
  Write(o, static_cast<unsigned int>(v));
}

inline void Read(std::istream &s, Access &v) {
  unsigned int t{0};
  Read(s, t);
  v = static_cast<Access>(t);
}

void Write(std::ostream &o, const XXX &v) {
  Write(o, v.bill);
  Write(o, v.aul);
  Write(o, v.kaul);
  Write(o, v.baul.size());
  for (const auto &entry : v.baul) {
    Write(o, entry);
  };
}

void Read(std::istream &s, XXX &v) {
  Read(s, v.bill);
  Read(s, v.aul);
  Read(s, v.kaul);
  std::vector<float>::size_type baul_size{0};
  Read(s, baul_size);
  v.baul.resize(baul_size);
  for (auto &entry : v.baul) {
    Read(s, entry);
  };
}

void Write(std::ostream &o, const YYY &v) {
  if (!v.still) {
    o.write("\x0", 1);
  } else {
    o.write("\x1", 1);
    Write(o, *v.still);
  }
  Write(o, v.sub.size());
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
  Read(s, sub_size);
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
  Write(o, "0.1.203");
  Write(o, v);
}

bool ReadYYY(std::istream &i, YYY &v) {
  XXX_references_.clear();
  YYY_references_.clear();

  std::string marker("0000");
  i.read(&marker[0], 4);
  if (marker != "CORE")
    return false;
  std::string version;
  Read(i, version);
  if (version != "0.1.203")
    return false;
  Read(i, v);
  return true;
}

};
}
}
