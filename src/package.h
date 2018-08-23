#ifndef PACKAGE_H
#define PACKAGE_H

#include "fileposition.h"

#include <string>
#include <unordered_map>
#include <vector>

using std::pair;
using std::string;
using std::unordered_map;
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

struct Union
{
  explicit Union(const string &n, const FilePosition &fp = FilePosition()) : name(n), location(fp) {}

  string name;
  FilePosition location;

  unsigned char appearance{0};
  vector<Attribute> tables;
};

struct Type
{
  Type() = default;
  Type(const Type &o) { _clone(o); }
  Type &operator=(const Type &) = delete;

  Type(const Enum &v) : _Enum(new Enum(v)), _selection(_Enum_selection) {}
  Type(Enum &&v) : _Enum(new Enum(std::forward<Enum>(v))), _selection(_Enum_selection) {}
  Type(const Table &v) : _Table(new Table(v)), _selection(_Table_selection) {}
  Type(Table &&v) : _Table(new Table(std::forward<Table>(v))), _selection(_Table_selection) {}
  Type(const Union &v) : _Union(new Union(v)), _selection(_Union_selection) {}
  Type(Union &&v) : _Union(new Union(std::forward<Union>(v))), _selection(_Union_selection) {}

  ~Type() { _destroy(); }

  bool is_Defined() const noexcept { return _selection != no_selection; }

  bool is_Enum() const noexcept { return _selection == _Enum_selection; }
  const Enum &as_Enum() const noexcept { return *_Enum; }
  Enum &as_Enum() { return *_Enum; }

  bool is_Table() const noexcept { return _selection == _Table_selection; }
  const Table &as_Table() const noexcept { return *_Table; }
  Table &as_Table() { return *_Table; }

  bool is_Union() const noexcept { return _selection == _Union_selection; }
  const Union &as_Union() const noexcept { return *_Union; }
  Union &as_Union() { return *_Union; }

private:
  void _clone(const Type &o) noexcept
  {
    _selection = o._selection;
    switch (_selection)
    {
      case no_selection:
        break;
      case _Enum_selection:
        _Enum = new Enum(*o._Enum);
        break;
      case _Table_selection:
        _Table = new Table(*o._Table);
        break;
      case _Union_selection:
        _Union = new Union(*o._Union);
        break;
    }
  }

  void _destroy() noexcept
  {
    switch (_selection)
    {
      case no_selection:
        break;
      case _Enum_selection:
        delete _Enum;
        break;
      case _Table_selection:
        delete _Table;
        break;
      case _Union_selection:
        delete _Union;
        break;
    }
    no_value = nullptr;
  }

  union
  {
    struct NoValue_t *no_value{nullptr};
    Enum *_Enum;
    Table *_Table;
    Union *_Union;
  };

  enum Selection_t
  {
    no_selection,
    _Enum_selection,
    _Table_selection,
    _Union_selection,
  };

  Selection_t _selection{no_selection};
  friend struct Package_io;
};

struct Package
{
  Attribute path;
  Attribute version;
  Attribute root_type;
  vector<Table> baseTypes;

  vector<Type> types;
};

#endif  // PACKAGE_H
