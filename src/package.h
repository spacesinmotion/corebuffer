#ifndef PACKAGE_H
#define PACKAGE_H

#include <string>
#include <vector>

using std::pair;
using std::string;
using std::vector;

struct Enum
{
  string name;
  vector<pair<string, int>> entries;
};

enum class Pointer : unsigned char
{
  Plain,
  Weak,
  Unique,
  Shared
};

enum PointerAppearance
{
  NoPointer = 0x00,
  WeakAppearance = 0x01,
  UniqueAppearance = 0x02,
  SharedAppearance = 0x04,
  VectorAppearance = 0x08,
  WeakVectorAppearance = 0x10,
  UniqueVectorAppearance = 0x20,
  SharedVectorAppearance = 0x40,
  PlainAppearance = 0x80,
};

struct Member
{
  string name;
  string type;
  string defaultValue;
  bool isVector{false};
  bool isBaseType{false};
  Pointer pointer{Pointer::Plain};
};

struct Table
{
  Table() = default;
  explicit Table(const string &n) : name(n) {}

  string name;
  vector<Member> member;
  unsigned char appearance{0};
};

struct Package
{
  string path;
  string version;
  string root_type;
  vector<Table> tables;
  vector<Table> baseTypes;
  vector<Enum> enums;
};

#endif  // PACKAGE_H