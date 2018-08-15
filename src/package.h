#ifndef PACKAGE_H
#define PACKAGE_H

#include "fileposition.h"

#include <string>
#include <vector>

using std::pair;
using std::string;
using std::vector;

struct Attribute
{
  Attribute() = default;
  Attribute(const string &v, const FilePosition &fp) : value(v), location(fp) {}

  string value;
  FilePosition location;
};

struct EnumEntry
{
  EnumEntry(const std::string &n, std::size_t v, const FilePosition &fp) : name(n), value(v), location(fp) {}

  string name;
  std::int64_t value;
  FilePosition location;
};

struct Enum
{
  explicit Enum(const string &n, const FilePosition &fp = FilePosition()) : name(n), location(fp) {}

  string name;
  vector<EnumEntry> entries;
  FilePosition location;
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

  SomeVectorAppearance = VectorAppearance | WeakVectorAppearance | SharedVectorAppearance | UniqueVectorAppearance,
};

struct Member
{
  Member(const std::string &n, const FilePosition &fp) : name(n), location(fp) {}

  string name;
  string type;
  Attribute defaultValue;
  bool isVector{false};
  bool isBaseType{false};
  Pointer pointer{Pointer::Plain};
  FilePosition location;
};

struct Parameter
{
  Parameter(const std::string &n, const FilePosition &fp) : name(n), location(fp) {}

  string name;
  FilePosition location;
};

struct Method
{
  Method(const std::string &n, const FilePosition &fp) : name(n), location(fp) {}

  string name;
  FilePosition location;
  vector<Parameter> parameter;
};

struct Table
{
  explicit Table(const string &n, const FilePosition &fp = FilePosition()) : name(n), location(fp) {}

  string name;
  vector<Member> member;
  vector<Method> methods;
  unsigned char appearance{0};
  FilePosition location;
};

struct Package
{
  Attribute path;
  Attribute version;
  Attribute root_type;
  vector<Table> tables;
  vector<Table> baseTypes;
  vector<Enum> enums;
};

#endif  // PACKAGE_H
