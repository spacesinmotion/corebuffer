#pragma once

#include <vector>
#include <string>
#include <ostream>
#include <istream>
#include <memory>
#include <array>
#include <type_traits>

namespace CoreData {

template<typename T>
struct AlwaysFalse : std::false_type {};

struct EnumEntry;
struct Enum;
struct Member;
struct Table;
struct Package;

enum class Pointer : std::int8_t {
  Plain = 0,
  Weak = 1,
  Unique = 2,
  Shared = 3,
};

enum class PointerAppearance : std::int8_t {
  NoPointer = 0,
  WeakAppearance = 1,
  UniqueAppearance = 2,
  SharedAppearance = 3,
  VectorAppearance = 4,
  WeakVectorAppearance = 5,
  UniqueVectorAppearance = 6,
  SharedVectorAppearance = 7,
  PlainAppearance = 8,
};

struct EnumEntry {
  std::string name;
  std::int32_t value{0};
};

struct Enum {
  std::string name;
  std::vector<EnumEntry> entries;
};

struct Member {
  std::string name;
  std::string type;
  std::string defaultValue;
  bool isVector{false};
  bool isBaseType{false};
  Pointer pointer{CoreData::Pointer::Plain};
};

struct Table {
  std::string name;
  std::vector<Member> member;
  std::uint8_t appearance{0u};
};

struct Package {
  std::string path;
  std::string version;
  std::string root_type;
  std::vector<Table> tables;
  std::vector<Table> baseTypes;
  std::vector<Enum> enums;
};

bool operator==(const EnumEntry&l, const EnumEntry&r) {
  return 
    l.name == r.name
    && l.value == r.value;
}

bool operator!=(const EnumEntry&l, const EnumEntry&r) {
  return 
    l.name != r.name
    || l.value != r.value;
}

bool operator==(const Enum&l, const Enum&r) {
  return 
    l.name == r.name
    && l.entries == r.entries;
}

bool operator!=(const Enum&l, const Enum&r) {
  return 
    l.name != r.name
    || l.entries != r.entries;
}

bool operator==(const Member&l, const Member&r) {
  return 
    l.name == r.name
    && l.type == r.type
    && l.defaultValue == r.defaultValue
    && l.isVector == r.isVector
    && l.isBaseType == r.isBaseType
    && l.pointer == r.pointer;
}

bool operator!=(const Member&l, const Member&r) {
  return 
    l.name != r.name
    || l.type != r.type
    || l.defaultValue != r.defaultValue
    || l.isVector != r.isVector
    || l.isBaseType != r.isBaseType
    || l.pointer != r.pointer;
}

bool operator==(const Table&l, const Table&r) {
  return 
    l.name == r.name
    && l.member == r.member
    && l.appearance == r.appearance;
}

bool operator!=(const Table&l, const Table&r) {
  return 
    l.name != r.name
    || l.member != r.member
    || l.appearance != r.appearance;
}

bool operator==(const Package&l, const Package&r) {
  return 
    l.path == r.path
    && l.version == r.version
    && l.root_type == r.root_type
    && l.tables == r.tables
    && l.baseTypes == r.baseTypes
    && l.enums == r.enums;
}

bool operator!=(const Package&l, const Package&r) {
  return 
    l.path != r.path
    || l.version != r.version
    || l.root_type != r.root_type
    || l.tables != r.tables
    || l.baseTypes != r.baseTypes
    || l.enums != r.enums;
}

inline const std::array<Pointer,4> & PointerValues() {
  static const std::array<Pointer,4> values {{
    Pointer::Plain,
    Pointer::Weak,
    Pointer::Unique,
    Pointer::Shared,
  }};
  return values;
};

inline const char * ValueName(const Pointer &v) {
  switch(v) {
    case Pointer::Plain: return "Plain";
    case Pointer::Weak: return "Weak";
    case Pointer::Unique: return "Unique";
    case Pointer::Shared: return "Shared";
  }
  return "<error>";
};

inline const std::array<PointerAppearance,9> & PointerAppearanceValues() {
  static const std::array<PointerAppearance,9> values {{
    PointerAppearance::NoPointer,
    PointerAppearance::WeakAppearance,
    PointerAppearance::UniqueAppearance,
    PointerAppearance::SharedAppearance,
    PointerAppearance::VectorAppearance,
    PointerAppearance::WeakVectorAppearance,
    PointerAppearance::UniqueVectorAppearance,
    PointerAppearance::SharedVectorAppearance,
    PointerAppearance::PlainAppearance,
  }};
  return values;
};

inline const char * ValueName(const PointerAppearance &v) {
  switch(v) {
    case PointerAppearance::NoPointer: return "NoPointer";
    case PointerAppearance::WeakAppearance: return "WeakAppearance";
    case PointerAppearance::UniqueAppearance: return "UniqueAppearance";
    case PointerAppearance::SharedAppearance: return "SharedAppearance";
    case PointerAppearance::VectorAppearance: return "VectorAppearance";
    case PointerAppearance::WeakVectorAppearance: return "WeakVectorAppearance";
    case PointerAppearance::UniqueVectorAppearance: return "UniqueVectorAppearance";
    case PointerAppearance::SharedVectorAppearance: return "SharedVectorAppearance";
    case PointerAppearance::PlainAppearance: return "PlainAppearance";
  }
  return "<error>";
};

struct Package_io {
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

  void Write(std::ostream &o, const EnumEntry &v) {
    Write(o, v.name);
    Write(o, v.value);
  }

  void Write(std::ostream &o, const std::vector<EnumEntry> &v) {
    Write(o, v.size());
    for (const auto &entry : v)
      Write(o, entry);
  }

  void Read(std::istream &s, EnumEntry &v) {
    Read(s, v.name);
    Read(s, v.value);
  }

  void Read(std::istream &s, std::vector<EnumEntry> &v) {
    auto size = v.size();
    Read(s, size);
    v.resize(size);
    for (auto &entry : v)
      Read(s, entry);
  }

  void Write(std::ostream &o, const Enum &v) {
    Write(o, v.name);
    Write(o, v.entries);
  }

  void Write(std::ostream &o, const std::vector<Enum> &v) {
    Write(o, v.size());
    for (const auto &entry : v)
      Write(o, entry);
  }

  void Read(std::istream &s, Enum &v) {
    Read(s, v.name);
    Read(s, v.entries);
  }

  void Read(std::istream &s, std::vector<Enum> &v) {
    auto size = v.size();
    Read(s, size);
    v.resize(size);
    for (auto &entry : v)
      Read(s, entry);
  }

  void Write(std::ostream &o, const Member &v) {
    Write(o, v.name);
    Write(o, v.type);
    Write(o, v.defaultValue);
    Write(o, v.isVector);
    Write(o, v.isBaseType);
    Write(o, v.pointer);
  }

  void Write(std::ostream &o, const std::vector<Member> &v) {
    Write(o, v.size());
    for (const auto &entry : v)
      Write(o, entry);
  }

  void Read(std::istream &s, Member &v) {
    Read(s, v.name);
    Read(s, v.type);
    Read(s, v.defaultValue);
    Read(s, v.isVector);
    Read(s, v.isBaseType);
    Read(s, v.pointer);
  }

  void Read(std::istream &s, std::vector<Member> &v) {
    auto size = v.size();
    Read(s, size);
    v.resize(size);
    for (auto &entry : v)
      Read(s, entry);
  }

  void Write(std::ostream &o, const Table &v) {
    Write(o, v.name);
    Write(o, v.member);
    Write(o, v.appearance);
  }

  void Write(std::ostream &o, const std::vector<Table> &v) {
    Write(o, v.size());
    for (const auto &entry : v)
      Write(o, entry);
  }

  void Read(std::istream &s, Table &v) {
    Read(s, v.name);
    Read(s, v.member);
    Read(s, v.appearance);
  }

  void Read(std::istream &s, std::vector<Table> &v) {
    auto size = v.size();
    Read(s, size);
    v.resize(size);
    for (auto &entry : v)
      Read(s, entry);
  }

  void Write(std::ostream &o, const Package &v) {
    Write(o, v.path);
    Write(o, v.version);
    Write(o, v.root_type);
    Write(o, v.tables);
    Write(o, v.baseTypes);
    Write(o, v.enums);
  }

  void Read(std::istream &s, Package &v) {
    Read(s, v.path);
    Read(s, v.version);
    Read(s, v.root_type);
    Read(s, v.tables);
    Read(s, v.baseTypes);
    Read(s, v.enums);
  }

public:
  void WritePackage(std::ostream &o, const Package &v) {

    o.write("CORE", 4);
    o.write("0.1", 3);
    Write(o, v);
  }

  bool ReadPackage(std::istream &i, Package &v) {

    std::string marker("0000");
    i.read(&marker[0], 4);
    if (marker != "CORE")
      return false;
    std::string version(3, '_');
    i.read(&version[0], 3);
    if (version != "0.1")
      return false;
    Read(i, v);
    return true;
  }

};
}
