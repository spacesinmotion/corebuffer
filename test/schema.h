#pragma once

#include <vector>
#include <string>
#include <ostream>
#include <istream>
#include <memory>
#include <array>
#include <algorithm>
#include <type_traits>

namespace CoreBuffer {

template<typename T>
struct AlwaysFalse : std::false_type {};

struct EnumEntry;
struct Enum;
struct Member;
struct Table;
struct Union;
struct BaseType;
struct Representation;
struct Type;
struct Package;

enum class Pointer : std::int8_t {
  Plain = 0,
  Weak = 1,
  Unique = 2,
  Shared = 3,
};

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

struct EnumEntry {
  std::string name;
  std::int32_t value{0};

  EnumEntry() = default;
  EnumEntry(const std::string &name_, const std::int32_t &value_)
    : name(name_)
    , value(value_)
  {}

  friend bool operator==(const EnumEntry&l, const EnumEntry&r) {
    return 
      l.name == r.name
      && l.value == r.value;
  }

  friend bool operator!=(const EnumEntry&l, const EnumEntry&r) {
    return 
      l.name != r.name
      || l.value != r.value;
  }
};

struct Enum {
  std::vector<EnumEntry> entries;

  Enum() = default;

  friend bool operator==(const Enum&l, const Enum&r) {
    return 
      l.entries == r.entries;
  }

  friend bool operator!=(const Enum&l, const Enum&r) {
    return 
      l.entries != r.entries;
  }

  template<class T> void fill_entries(const T &v) {
    std::fill(entries.begin(), entries.end(), v);
  }

  template<class Generator> void generate_entries(Generator gen) {
    std::generate(entries.begin(), entries.end(), gen);
  }

  template<class T> std::vector<EnumEntry>::iterator remove_entries(const T &v) {
    return std::remove(entries.begin(), entries.end(), v);
  }
  template<class Pred> std::vector<EnumEntry>::iterator remove_entries_if(Pred v) {
    return std::remove_if(entries.begin(), entries.end(), v);
  }

  template<class T> void erase_entries(const T &v) {
    entries.erase(remove_entries(v));
  }
  template<class Pred> void erase_entries_if(Pred v) {
    entries.erase(remove_entries_if(v));
  }

  void reverse_entries() {
    std::reverse(entries.begin(), entries.end());
  }

  void rotate_entries(std::vector<EnumEntry>::iterator i) {
    std::rotate(entries.begin(), i, entries.end());
  }

  template<class Comp> void sort_entries(Comp p) {
    std::sort(entries.begin(), entries.end(), p);
  }

  template<class Comp> bool any_of_entries(Comp p) {
    return std::any_of(entries.begin(), entries.end(), p);
  }
  template<class T> bool any_of_entries_is(const T &p) {
    return any_of_entries([&p](const EnumEntry &x) { return x == p; });
  }

  template<class Comp> bool all_of_entries(Comp p) {
    return std::all_of(entries.begin(), entries.end(), p);
  }
  template<class T> bool all_of_entries_are(const T &p) {
    return all_of_entries([&p](const EnumEntry &x) { return x == p; });
  }

  template<class Comp> bool none_of_entries(Comp p) {
    return std::none_of(entries.begin(), entries.end(), p);
  }
  template<class T> bool none_of_entries_is(const T &p) {
    return none_of_entries([&p](const EnumEntry &x) { return x == p; });
  }

  template<class Fn> Fn for_each_entries(Fn p) {
    return std::for_each(entries.begin(), entries.end(), p);
  }

  template<class T> std::vector<EnumEntry>::iterator find_in_entries(const T &p) {
    return std::find(entries.begin(), entries.end(), p);
  }
  template<class Comp> std::vector<EnumEntry>::iterator find_in_entries_if(Comp p) {
    return std::find_if(entries.begin(), entries.end(), p);
  }

  template<class T>   typename std::iterator_traits<std::vector<EnumEntry>::iterator>::difference_type count_in_entries(const T &p) {
    return std::count(entries.begin(), entries.end(), p);
  }
  template<class Comp>   typename std::iterator_traits<std::vector<EnumEntry>::iterator>::difference_type count_in_entries_if(Comp p) {
    return std::count_if(entries.begin(), entries.end(), p);
  }
};

struct Member {
  std::string name;
  std::string type;
  std::string defaultValue;
  bool isVector{false};
  bool isBaseType{false};
  Pointer pointer{CoreBuffer::Pointer::Plain};

  Member() = default;
  Member(const std::string &name_, const std::string &type_)
    : name(name_)
    , type(type_)
  {}

  friend bool operator==(const Member&l, const Member&r) {
    return 
      l.name == r.name
      && l.type == r.type
      && l.defaultValue == r.defaultValue
      && l.isVector == r.isVector
      && l.isBaseType == r.isBaseType
      && l.pointer == r.pointer;
  }

  friend bool operator!=(const Member&l, const Member&r) {
    return 
      l.name != r.name
      || l.type != r.type
      || l.defaultValue != r.defaultValue
      || l.isVector != r.isVector
      || l.isBaseType != r.isBaseType
      || l.pointer != r.pointer;
  }
};

struct Table {
  std::vector<Member> member;
  std::uint8_t appearance{0u};

  Table() = default;

  friend bool operator==(const Table&l, const Table&r) {
    return 
      l.member == r.member
      && l.appearance == r.appearance;
  }

  friend bool operator!=(const Table&l, const Table&r) {
    return 
      l.member != r.member
      || l.appearance != r.appearance;
  }

  template<class T> void fill_member(const T &v) {
    std::fill(member.begin(), member.end(), v);
  }

  template<class Generator> void generate_member(Generator gen) {
    std::generate(member.begin(), member.end(), gen);
  }

  template<class T> std::vector<Member>::iterator remove_member(const T &v) {
    return std::remove(member.begin(), member.end(), v);
  }
  template<class Pred> std::vector<Member>::iterator remove_member_if(Pred v) {
    return std::remove_if(member.begin(), member.end(), v);
  }

  template<class T> void erase_member(const T &v) {
    member.erase(remove_member(v));
  }
  template<class Pred> void erase_member_if(Pred v) {
    member.erase(remove_member_if(v));
  }

  void reverse_member() {
    std::reverse(member.begin(), member.end());
  }

  void rotate_member(std::vector<Member>::iterator i) {
    std::rotate(member.begin(), i, member.end());
  }

  template<class Comp> void sort_member(Comp p) {
    std::sort(member.begin(), member.end(), p);
  }

  template<class Comp> bool any_of_member(Comp p) {
    return std::any_of(member.begin(), member.end(), p);
  }
  template<class T> bool any_of_member_is(const T &p) {
    return any_of_member([&p](const Member &x) { return x == p; });
  }

  template<class Comp> bool all_of_member(Comp p) {
    return std::all_of(member.begin(), member.end(), p);
  }
  template<class T> bool all_of_member_are(const T &p) {
    return all_of_member([&p](const Member &x) { return x == p; });
  }

  template<class Comp> bool none_of_member(Comp p) {
    return std::none_of(member.begin(), member.end(), p);
  }
  template<class T> bool none_of_member_is(const T &p) {
    return none_of_member([&p](const Member &x) { return x == p; });
  }

  template<class Fn> Fn for_each_member(Fn p) {
    return std::for_each(member.begin(), member.end(), p);
  }

  template<class T> std::vector<Member>::iterator find_in_member(const T &p) {
    return std::find(member.begin(), member.end(), p);
  }
  template<class Comp> std::vector<Member>::iterator find_in_member_if(Comp p) {
    return std::find_if(member.begin(), member.end(), p);
  }

  template<class T>   typename std::iterator_traits<std::vector<Member>::iterator>::difference_type count_in_member(const T &p) {
    return std::count(member.begin(), member.end(), p);
  }
  template<class Comp>   typename std::iterator_traits<std::vector<Member>::iterator>::difference_type count_in_member_if(Comp p) {
    return std::count_if(member.begin(), member.end(), p);
  }
};

struct Union {
  std::vector<std::string> tables;

  Union() = default;

  friend bool operator==(const Union&l, const Union&r) {
    return 
      l.tables == r.tables;
  }

  friend bool operator!=(const Union&l, const Union&r) {
    return 
      l.tables != r.tables;
  }

  template<class T> void fill_tables(const T &v) {
    std::fill(tables.begin(), tables.end(), v);
  }

  template<class Generator> void generate_tables(Generator gen) {
    std::generate(tables.begin(), tables.end(), gen);
  }

  template<class T> std::vector<std::string>::iterator remove_tables(const T &v) {
    return std::remove(tables.begin(), tables.end(), v);
  }
  template<class Pred> std::vector<std::string>::iterator remove_tables_if(Pred v) {
    return std::remove_if(tables.begin(), tables.end(), v);
  }

  template<class T> void erase_tables(const T &v) {
    tables.erase(remove_tables(v));
  }
  template<class Pred> void erase_tables_if(Pred v) {
    tables.erase(remove_tables_if(v));
  }

  void reverse_tables() {
    std::reverse(tables.begin(), tables.end());
  }

  void rotate_tables(std::vector<std::string>::iterator i) {
    std::rotate(tables.begin(), i, tables.end());
  }

  void sort_tables() {
    std::sort(tables.begin(), tables.end());
  }
  template<class Comp> void sort_tables(Comp p) {
    std::sort(tables.begin(), tables.end(), p);
  }

  template<class Comp> bool any_of_tables(Comp p) {
    return std::any_of(tables.begin(), tables.end(), p);
  }
  template<class T> bool any_of_tables_is(const T &p) {
    return any_of_tables([&p](const std::string &x) { return x == p; });
  }

  template<class Comp> bool all_of_tables(Comp p) {
    return std::all_of(tables.begin(), tables.end(), p);
  }
  template<class T> bool all_of_tables_are(const T &p) {
    return all_of_tables([&p](const std::string &x) { return x == p; });
  }

  template<class Comp> bool none_of_tables(Comp p) {
    return std::none_of(tables.begin(), tables.end(), p);
  }
  template<class T> bool none_of_tables_is(const T &p) {
    return none_of_tables([&p](const std::string &x) { return x == p; });
  }

  template<class Fn> Fn for_each_tables(Fn p) {
    return std::for_each(tables.begin(), tables.end(), p);
  }

  template<class T> std::vector<std::string>::iterator find_in_tables(const T &p) {
    return std::find(tables.begin(), tables.end(), p);
  }
  template<class Comp> std::vector<std::string>::iterator find_in_tables_if(Comp p) {
    return std::find_if(tables.begin(), tables.end(), p);
  }

  template<class T>   typename std::iterator_traits<std::vector<std::string>::iterator>::difference_type count_in_tables(const T &p) {
    return std::count(tables.begin(), tables.end(), p);
  }
  template<class Comp>   typename std::iterator_traits<std::vector<std::string>::iterator>::difference_type count_in_tables_if(Comp p) {
    return std::count_if(tables.begin(), tables.end(), p);
  }
};

struct BaseType {
  bool isComplex{false};

  BaseType() = default;
  BaseType(const bool &isComplex_)
    : isComplex(isComplex_)
  {}

  friend bool operator==(const BaseType&l, const BaseType&r) {
    return 
      l.isComplex == r.isComplex;
  }

  friend bool operator!=(const BaseType&l, const BaseType&r) {
    return 
      l.isComplex != r.isComplex;
  }
};

struct Representation {
  Representation() = default;
  Representation(const Representation &o) { _clone(o); }
  Representation& operator=(const Representation &o) { _destroy(); _clone(o); return *this; }

  Representation(const BaseType &v)
    : _BaseType(new BaseType(v))
    , _selection(_BaseType_selection)
  {}
  Representation(BaseType &&v)
    : _BaseType(new BaseType(std::forward<BaseType>(v)))
    , _selection(_BaseType_selection)
  {}
  Representation & operator=(const BaseType &v) {
    _destroy();
    _BaseType = new BaseType(v);
    _selection = _BaseType_selection;
    return *this;
  }
  Representation & operator=(BaseType &&v) {
    _destroy();
    _BaseType = new BaseType(std::forward<BaseType>(v));
    _selection = _BaseType_selection;
    return *this;
  }

  Representation(const Enum &v)
    : _Enum(new Enum(v))
    , _selection(_Enum_selection)
  {}
  Representation(Enum &&v)
    : _Enum(new Enum(std::forward<Enum>(v)))
    , _selection(_Enum_selection)
  {}
  Representation & operator=(const Enum &v) {
    _destroy();
    _Enum = new Enum(v);
    _selection = _Enum_selection;
    return *this;
  }
  Representation & operator=(Enum &&v) {
    _destroy();
    _Enum = new Enum(std::forward<Enum>(v));
    _selection = _Enum_selection;
    return *this;
  }

  Representation(const Table &v)
    : _Table(new Table(v))
    , _selection(_Table_selection)
  {}
  Representation(Table &&v)
    : _Table(new Table(std::forward<Table>(v)))
    , _selection(_Table_selection)
  {}
  Representation & operator=(const Table &v) {
    _destroy();
    _Table = new Table(v);
    _selection = _Table_selection;
    return *this;
  }
  Representation & operator=(Table &&v) {
    _destroy();
    _Table = new Table(std::forward<Table>(v));
    _selection = _Table_selection;
    return *this;
  }

  Representation(const Union &v)
    : _Union(new Union(v))
    , _selection(_Union_selection)
  {}
  Representation(Union &&v)
    : _Union(new Union(std::forward<Union>(v)))
    , _selection(_Union_selection)
  {}
  Representation & operator=(const Union &v) {
    _destroy();
    _Union = new Union(v);
    _selection = _Union_selection;
    return *this;
  }
  Representation & operator=(Union &&v) {
    _destroy();
    _Union = new Union(std::forward<Union>(v));
    _selection = _Union_selection;
    return *this;
  }

  ~Representation() {
    _destroy();
  }

  bool is_Defined() const noexcept { return _selection != no_selection; }
  void clear() { *this = Representation(); }

  bool is_BaseType() const noexcept { return _selection == _BaseType_selection; }
  const BaseType & as_BaseType() const noexcept { return *_BaseType; }
  BaseType & as_BaseType() { return *_BaseType; }
  template<typename... Args> BaseType & create_BaseType(Args&&... args) {
    return (*this = BaseType(std::forward<Args>(args)...)).as_BaseType();
  }

  bool is_Enum() const noexcept { return _selection == _Enum_selection; }
  const Enum & as_Enum() const noexcept { return *_Enum; }
  Enum & as_Enum() { return *_Enum; }
  template<typename... Args> Enum & create_Enum(Args&&... args) {
    return (*this = Enum(std::forward<Args>(args)...)).as_Enum();
  }

  bool is_Table() const noexcept { return _selection == _Table_selection; }
  const Table & as_Table() const noexcept { return *_Table; }
  Table & as_Table() { return *_Table; }
  template<typename... Args> Table & create_Table(Args&&... args) {
    return (*this = Table(std::forward<Args>(args)...)).as_Table();
  }

  bool is_Union() const noexcept { return _selection == _Union_selection; }
  const Union & as_Union() const noexcept { return *_Union; }
  Union & as_Union() { return *_Union; }
  template<typename... Args> Union & create_Union(Args&&... args) {
    return (*this = Union(std::forward<Args>(args)...)).as_Union();
  }

  friend bool operator==(const Representation&ab, const BaseType &o) noexcept  { return ab.is_BaseType() && ab.as_BaseType() == o; }
  friend bool operator==(const BaseType &o, const Representation&ab) noexcept  { return ab.is_BaseType() && o == ab.as_BaseType(); }
  friend bool operator!=(const Representation&ab, const BaseType &o) noexcept  { return !ab.is_BaseType() || ab.as_BaseType() != o; }
  friend bool operator!=(const BaseType &o, const Representation&ab) noexcept  { return !ab.is_BaseType() || o != ab.as_BaseType(); }

  friend bool operator==(const Representation&ab, const Enum &o) noexcept  { return ab.is_Enum() && ab.as_Enum() == o; }
  friend bool operator==(const Enum &o, const Representation&ab) noexcept  { return ab.is_Enum() && o == ab.as_Enum(); }
  friend bool operator!=(const Representation&ab, const Enum &o) noexcept  { return !ab.is_Enum() || ab.as_Enum() != o; }
  friend bool operator!=(const Enum &o, const Representation&ab) noexcept  { return !ab.is_Enum() || o != ab.as_Enum(); }

  friend bool operator==(const Representation&ab, const Table &o) noexcept  { return ab.is_Table() && ab.as_Table() == o; }
  friend bool operator==(const Table &o, const Representation&ab) noexcept  { return ab.is_Table() && o == ab.as_Table(); }
  friend bool operator!=(const Representation&ab, const Table &o) noexcept  { return !ab.is_Table() || ab.as_Table() != o; }
  friend bool operator!=(const Table &o, const Representation&ab) noexcept  { return !ab.is_Table() || o != ab.as_Table(); }

  friend bool operator==(const Representation&ab, const Union &o) noexcept  { return ab.is_Union() && ab.as_Union() == o; }
  friend bool operator==(const Union &o, const Representation&ab) noexcept  { return ab.is_Union() && o == ab.as_Union(); }
  friend bool operator!=(const Representation&ab, const Union &o) noexcept  { return !ab.is_Union() || ab.as_Union() != o; }
  friend bool operator!=(const Union &o, const Representation&ab) noexcept  { return !ab.is_Union() || o != ab.as_Union(); }

  bool operator==(const Representation &o) const noexcept
  {
    if (this == &o)
      return true;
    if (_selection != o._selection)
      return false;
    switch(_selection) {
    case no_selection: while(false); /* hack for coverage tool */ return true;
    case _BaseType_selection: return *_BaseType == *o._BaseType;
    case _Enum_selection: return *_Enum == *o._Enum;
    case _Table_selection: return *_Table == *o._Table;
    case _Union_selection: return *_Union == *o._Union;
    }
    return false; // without this line there is a msvc warning I do not understand.
  }

  bool operator!=(const Representation &o) const noexcept
  {
    if (this == &o)
      return false;
    if (_selection != o._selection)
      return true;
    switch(_selection) {
    case no_selection: while(false); /* hack for coverage tool */ return false;
    case _BaseType_selection: return *_BaseType != *o._BaseType;
    case _Enum_selection: return *_Enum != *o._Enum;
    case _Table_selection: return *_Table != *o._Table;
    case _Union_selection: return *_Union != *o._Union;
    }
    return false; // without this line there is a msvc warning I do not understand.
  }

private:
  void _clone(const Representation &o) noexcept
  {
     _selection = o._selection;
    switch(_selection) {
    case no_selection: while(false); /* hack for coverage tool */ break;
    case _BaseType_selection: _BaseType = new BaseType(*o._BaseType); break;
    case _Enum_selection: _Enum = new Enum(*o._Enum); break;
    case _Table_selection: _Table = new Table(*o._Table); break;
    case _Union_selection: _Union = new Union(*o._Union); break;
    }
  }

  void _destroy() noexcept {
    switch(_selection) {
    case no_selection: while(false); /* hack for coverage tool */ break;
    case _BaseType_selection: delete _BaseType; break;
    case _Enum_selection: delete _Enum; break;
    case _Table_selection: delete _Table; break;
    case _Union_selection: delete _Union; break;
    }
    no_value = nullptr;
  }

  union {
    struct NoValue_t *no_value{nullptr};
    BaseType * _BaseType;
    Enum * _Enum;
    Table * _Table;
    Union * _Union;
  };

  enum Selection_t {
    no_selection,
    _BaseType_selection,
    _Enum_selection,
    _Table_selection,
    _Union_selection,
  };

  Selection_t _selection{no_selection};
  friend struct Package_io;
};

struct Type {
  std::string name;
  std::uint8_t appearance{0u};
  Representation representation;

  Type() = default;
  Type(const std::string &name_, const Representation &representation_)
    : name(name_)
    , representation(representation_)
  {}

  friend bool operator==(const Type&l, const Type&r) {
    return 
      l.name == r.name
      && l.appearance == r.appearance
      && l.representation == r.representation;
  }

  friend bool operator!=(const Type&l, const Type&r) {
    return 
      l.name != r.name
      || l.appearance != r.appearance
      || l.representation != r.representation;
  }
};

struct Package {
  std::string path;
  std::string version;
  std::string root_type;
  std::vector<Type> types;

  Package() = default;
  Package(const std::string &path_, const std::string &version_, const std::string &root_type_)
    : path(path_)
    , version(version_)
    , root_type(root_type_)
  {}

  friend bool operator==(const Package&l, const Package&r) {
    return 
      l.path == r.path
      && l.version == r.version
      && l.root_type == r.root_type
      && l.types == r.types;
  }

  friend bool operator!=(const Package&l, const Package&r) {
    return 
      l.path != r.path
      || l.version != r.version
      || l.root_type != r.root_type
      || l.types != r.types;
  }

  template<class T> void fill_types(const T &v) {
    std::fill(types.begin(), types.end(), v);
  }

  template<class Generator> void generate_types(Generator gen) {
    std::generate(types.begin(), types.end(), gen);
  }

  template<class T> std::vector<Type>::iterator remove_types(const T &v) {
    return std::remove(types.begin(), types.end(), v);
  }
  template<class Pred> std::vector<Type>::iterator remove_types_if(Pred v) {
    return std::remove_if(types.begin(), types.end(), v);
  }

  template<class T> void erase_types(const T &v) {
    types.erase(remove_types(v));
  }
  template<class Pred> void erase_types_if(Pred v) {
    types.erase(remove_types_if(v));
  }

  void reverse_types() {
    std::reverse(types.begin(), types.end());
  }

  void rotate_types(std::vector<Type>::iterator i) {
    std::rotate(types.begin(), i, types.end());
  }

  template<class Comp> void sort_types(Comp p) {
    std::sort(types.begin(), types.end(), p);
  }

  template<class Comp> bool any_of_types(Comp p) {
    return std::any_of(types.begin(), types.end(), p);
  }
  template<class T> bool any_of_types_is(const T &p) {
    return any_of_types([&p](const Type &x) { return x == p; });
  }

  template<class Comp> bool all_of_types(Comp p) {
    return std::all_of(types.begin(), types.end(), p);
  }
  template<class T> bool all_of_types_are(const T &p) {
    return all_of_types([&p](const Type &x) { return x == p; });
  }

  template<class Comp> bool none_of_types(Comp p) {
    return std::none_of(types.begin(), types.end(), p);
  }
  template<class T> bool none_of_types_is(const T &p) {
    return none_of_types([&p](const Type &x) { return x == p; });
  }

  template<class Fn> Fn for_each_types(Fn p) {
    return std::for_each(types.begin(), types.end(), p);
  }

  template<class T> std::vector<Type>::iterator find_in_types(const T &p) {
    return std::find(types.begin(), types.end(), p);
  }
  template<class Comp> std::vector<Type>::iterator find_in_types_if(Comp p) {
    return std::find_if(types.begin(), types.end(), p);
  }

  template<class T>   typename std::iterator_traits<std::vector<Type>::iterator>::difference_type count_in_types(const T &p) {
    return std::count(types.begin(), types.end(), p);
  }
  template<class Comp>   typename std::iterator_traits<std::vector<Type>::iterator>::difference_type count_in_types_if(Comp p) {
    return std::count_if(types.begin(), types.end(), p);
  }
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

  void Write(std::ostream &o, const std::vector<std::string> &v) {
    Write(o, v.size());
    for (const auto &entry : v)
      Write(o, entry);
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

  void Read(std::istream &i, std::vector<std::string> &v) {
    auto size = v.size();
    Read(i, size);
    v.resize(size);
    for (auto &entry : v)
      Read(i, entry);
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
    Write(o, v.entries);
  }

  void Read(std::istream &s, Enum &v) {
    Read(s, v.entries);
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
    Write(o, v.member);
    Write(o, v.appearance);
  }

  void Read(std::istream &s, Table &v) {
    Read(s, v.member);
    Read(s, v.appearance);
  }

  void Write(std::ostream &o, const Union &v) {
    Write(o, v.tables);
  }

  void Read(std::istream &s, Union &v) {
    Read(s, v.tables);
  }

  void Write(std::ostream &o, const Representation &v) {
    o.write(reinterpret_cast<const char*>(&v._selection), sizeof(Representation::Selection_t));
    switch(v._selection) {
    case Representation::no_selection: while(false); /* hack for coverage tool */ break;
    case Representation::_BaseType_selection: Write(o, v.as_BaseType()); break;
    case Representation::_Enum_selection: Write(o, v.as_Enum()); break;
    case Representation::_Table_selection: Write(o, v.as_Table()); break;
    case Representation::_Union_selection: Write(o, v.as_Union()); break;
    }
  }

  void Read(std::istream &i, Representation &v) {
    i.read(reinterpret_cast<char*>(&v._selection), sizeof(Representation::Selection_t));
    switch(v._selection) {
    case Representation::no_selection: while(false); /* hack for coverage tool */ break;
    case Representation::_BaseType_selection: Read(i, v.create_BaseType()); break;
    case Representation::_Enum_selection: Read(i, v.create_Enum()); break;
    case Representation::_Table_selection: Read(i, v.create_Table()); break;
    case Representation::_Union_selection: Read(i, v.create_Union()); break;
    }
  }

  void Write(std::ostream &o, const Type &v) {
    Write(o, v.name);
    Write(o, v.appearance);
    Write(o, v.representation);
  }

  void Write(std::ostream &o, const std::vector<Type> &v) {
    Write(o, v.size());
    for (const auto &entry : v)
      Write(o, entry);
  }

  void Read(std::istream &s, Type &v) {
    Read(s, v.name);
    Read(s, v.appearance);
    Read(s, v.representation);
  }

  void Read(std::istream &s, std::vector<Type> &v) {
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
    Write(o, v.types);
  }

  void Read(std::istream &s, Package &v) {
    Read(s, v.path);
    Read(s, v.version);
    Read(s, v.root_type);
    Read(s, v.types);
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
