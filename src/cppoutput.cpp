#include "cppoutput.h"

#include <algorithm>
#include <limits>
#include <tuple>

using namespace std;

template <typename Predicate>
bool any_table_of(const Package &p, const Predicate &pr)
{
  return any_of(p.types.begin(), p.types.end(), [&pr](const Type &t) { return t.is_Table() && pr(t.as_Table()); }) ||
         any_of(p.baseTypes.begin(), p.baseTypes.end(), pr);
}

template <typename Predicate>
bool any_union_of(const Package &p, const Predicate &pr)
{
  return any_of(p.types.begin(), p.types.end(), [&pr](const Type &u) { return u.is_Union() && pr(u.as_Union()); });
}

template <typename Predicate>
bool any_enum_of(const Package &p, const Predicate &pr)
{
  return any_of(p.types.begin(), p.types.end(), [&pr](const Type &u) { return u.is_Enum() && pr(u.as_Enum()); });
}

template <class T>
bool hasUniqueAppearance(const T &t)
{
  return ((t.appearance & UniqueAppearance) == UniqueAppearance) ||
         ((t.appearance & UniqueVectorAppearance) == UniqueVectorAppearance);
}

bool someThingIsUnique(const Package &p)
{
  return any_table_of(p, hasUniqueAppearance<Table>) || any_union_of(p, hasUniqueAppearance<Union>);
}

template <class T>
bool hasUniqueVectorAppearance(const T &t)
{
  return (t.appearance & UniqueVectorAppearance) == UniqueVectorAppearance;
};

bool someThingIsUniqueVector(const Package &p)
{
  return any_table_of(p, hasUniqueVectorAppearance<Table>) || any_union_of(p, hasUniqueVectorAppearance<Union>);
}

template <class T>
bool hasSharedAppearance(const T &t)
{
  return ((t.appearance & SharedAppearance) == SharedAppearance) ||
         ((t.appearance & SharedVectorAppearance) == SharedVectorAppearance);
}

bool someThingIsShared(const Package &p)
{
  return any_table_of(p, hasSharedAppearance<Table>) || any_union_of(p, hasSharedAppearance<Union>);
}

template <class T>
bool hasSharedVectorAppearance(const T &t)
{
  return (t.appearance & SharedVectorAppearance) == SharedVectorAppearance;
};

bool someThingIsSharedVector(const Package &p)
{
  return any_table_of(p, hasSharedVectorAppearance<Table>) || any_union_of(p, hasSharedVectorAppearance<Union>);
}

template <class T>
bool isComplex(const T &t)
{
  return t.isComplexType;
}

template <>
bool isComplex(const Union &)
{
  return true;
}

template <class T>
bool hasWeakAppearance(const T &t)
{
  return ((t.appearance & WeakAppearance) == WeakAppearance) ||
         ((t.appearance & WeakVectorAppearance) == WeakVectorAppearance);
}

bool someThingIsWeak(const Package &p)
{
  return any_table_of(p, hasWeakAppearance<Table>) || any_union_of(p, hasWeakAppearance<Union>);
}

template <class T>
bool hasWeakVectorAppearance(const T &t)
{
  return (t.appearance & WeakVectorAppearance) == WeakVectorAppearance;
};

bool someThingIsWeakVector(const Package &p)
{
  return any_table_of(p, hasWeakVectorAppearance<Table>) || any_union_of(p, hasWeakVectorAppearance<Union>);
}

template <class T>
bool hasPlainVectorAppearance(const T &t)
{
  return ((t.appearance & VectorAppearance) == VectorAppearance);
}

bool hasVectorOfString(const Package &p)
{
  return any_table_of(p, [](const Table &t) {
    return any_of(t.member.begin(), t.member.end(),
                  [](const Member &m) { return m.isBaseType && m.isVector && m.type == "std::string"; });
  });
}

bool hasPlainString(const Package &p)
{
  return any_table_of(p, [](const Table &t) {
    return any_of(t.member.begin(), t.member.end(),
                  [](const Member &m) { return m.isBaseType && !m.isVector && m.type == "std::string"; });
  });
}

bool isEnum(const Package &p, const string &type)
{
  return any_enum_of(p, [&type](const Enum &e) { return e.name == type; });
}

void WriteNameSpaceBegin(ostream &o, const string &path, int pos = 0)
{
  if (path.empty())
    return;

  const auto end = path.find('.', pos);
  o << "namespace " << path.substr(pos, end) << " {" << endl;
  if (end != string::npos)
    WriteNameSpaceBegin(o, path, int(end) + 1);
}

void WriteNameSpaceEnd(ostream &o, const string &path, int pos = 0)
{
  if (path.empty())
    return;

  const auto end = path.find('.', pos);
  o << "}" << endl;
  if (end != string::npos)
    WriteNameSpaceEnd(o, path, int(end) + 1);
}

void WriteForwardDeclarations(ostream &o, const Package &p)
{
  for (const auto &t : p.types)
    if (t.is_Table())
      o << endl << "struct " << t.as_Table().name << ";";
    else if (t.is_Union())
      o << endl << "struct " << t.as_Union().name << ";";
  o << endl << endl;
}

void WriteEnumDeclaration(ostream &o, const Enum &e)
{
  int64_t maxEntry = 0ul;
  for (const auto &entry : e.entries)
    maxEntry = max(entry.value, maxEntry);

  string type = "std::int8_t";
  if (maxEntry > numeric_limits<uint32_t>::max())
    type = "std::int64_t";
  else if (maxEntry > numeric_limits<uint16_t>::max())
    type = "std::int32_t";
  else if (maxEntry > numeric_limits<uint8_t>::max())
    type = "std::int16_t";

  o << "enum class " << e.name << " : " << type << " {" << endl;
  for (const auto &v : e.entries)
    o << "  " << v.name << " = " << v.value << "," << endl;
  o << "};" << endl << endl;
}

void WriteEnumFunctions(ostream &o, const Enum &e)
{
  o << "inline const std::array<" << e.name << "," << e.entries.size() << "> & " << e.name << "Values() {" << endl;
  o << "  static const std::array<" << e.name << "," << e.entries.size() << "> values {{" << endl;
  for (const auto &v : e.entries)
    o << "    " << e.name << "::" << v.name << "," << endl;
  o << "  }};" << endl;
  o << "  return values;" << endl;
  o << "};" << endl << endl;

  o << "inline const char * ValueName(const " << e.name << " &v) {" << endl;
  o << "  switch(v) {" << endl;
  for (const auto &v : e.entries)
    o << "    case " << e.name << "::" << v.name << ": return \"" << v.name << "\";" << endl;
  o << "  }" << endl;
  o << "  return \"<error>\";" << endl;
  o << "};" << endl << endl;
}

void WriteBaseTypeIoFnuctions(ostream &o, const Package &p)
{
  static const auto notImplementedAssert = "    static_assert(AlwaysFalse<T>::value, \"Something not implemented\");";

  o << "  template<typename T> void Write(std::ostream &, const T *) {" << endl;
  o << notImplementedAssert << endl;
  o << "  }" << endl << endl;

  o << "  template<typename T> void Write(std::ostream &o, const T &v) {" << endl;
  o << "    o.write(reinterpret_cast<const char *>(&v), sizeof(T));" << endl;
  o << "  }" << endl << endl;

  o << "  template<typename T> void Write(std::ostream &o, const std::vector<T> &v) {" << endl;
  o << "    Write(o, v.size());" << endl;
  o << "    o.write(reinterpret_cast<const char *>(v.data()), sizeof(T) * v.size());" << endl;
  o << "  }" << endl << endl;

  if (hasVectorOfString(p))
  {
    o << "  void Write(std::ostream &o, const std::vector<std::string> &v) {" << endl;
    o << "    Write(o, v.size());" << endl;
    o << "    for (const auto &entry : v)" << endl;
    o << "      Write(o, entry);" << endl;
    o << "  }" << endl << endl;
  }

  if (someThingIsUnique(p))
  {
    o << "  template<typename T> void Write(std::ostream &o, const std::unique_ptr<T> &v) {" << endl;
    o << "    if (!v) {" << endl;
    o << "      o.write(\"\\x0\", 1);" << endl;
    o << "    } else {" << endl;
    o << "      o.write(\"\\x1\", 1);" << endl;
    o << "      Write(o, *v);" << endl;
    o << "    }" << endl;
    o << "  }" << endl << endl;
  }

  if (someThingIsShared(p))
  {
    o << "  template<typename T> void Write(std::ostream &o, const std::shared_ptr<T> &v, unsigned int &counter) {"
      << endl;
    o << "    if (!v) {" << endl;
    o << "      o.write(\"\\x0\", 1);" << endl;
    o << "    } else if (v->io_counter_== 0) {" << endl;
    o << "      v->io_counter_ = ++counter;" << endl;
    o << "      o.write(\"\\x1\", 1);" << endl;
    o << "      Write(o, *v);" << endl;
    o << "    } else {" << endl;
    o << "      o.write(\"\\x2\", 1);" << endl;
    o << "      Write(o, v->io_counter_);" << endl;
    o << "    }" << endl;
    o << "  }" << endl << endl;
  }

  if (someThingIsUniqueVector(p))
  {
    o << "  template<typename T> void Write(std::ostream &o, const std::vector<std::unique_ptr<T>> &v) {" << endl;
    o << "    Write(o, v.size());" << endl;
    o << "    for (const auto &entry : v)" << endl;
    o << "      Write(o, entry);" << endl;
    o << "  }" << endl << endl;
  }

  if (someThingIsSharedVector(p))
  {
    o << "  template<typename T> void Write(std::ostream &o, const std::vector<std::shared_ptr<T>> &v) {" << endl;
    o << "    Write(o, v.size());" << endl;
    o << "    for (const auto &entry : v)" << endl;
    o << "      Write(o, entry);" << endl;
    o << "  }" << endl << endl;
  }

  if (someThingIsWeakVector(p))
  {
    o << "  template<typename T> void Write(std::ostream &o, const std::vector<std::weak_ptr<T>> &v) {" << endl;
    o << "    Write(o, v.size());" << endl;
    o << "    for (const auto &entry : v)" << endl;
    o << "      Write(o, entry);" << endl;
    o << "  }" << endl << endl;
  }

  o << "  template<typename T> void Write(std::ostream &, const std::shared_ptr<T> &) {" << endl;
  o << notImplementedAssert << endl;
  o << "  }" << endl << endl;

  if (someThingIsWeak(p))
  {
    o << "  template<typename T> void Write(std::ostream &, const std::weak_ptr<T> &) {" << endl;
    o << notImplementedAssert << endl;
    o << "  }" << endl << endl;
  }

  if (hasPlainString(p))
  {
    o << "  void Write(std::ostream &o, const std::string &v) {" << endl;
    o << "    Write(o, v.size());" << endl;
    o << "    o.write(v.data(), v.size());" << endl;
    o << "  }" << endl << endl;
  }

  o << "  template<typename T> void Read(std::istream &i, T &v) {" << endl;
  o << "    i.read(reinterpret_cast<char *>(&v), sizeof(T));" << endl;
  o << "  }" << endl << endl;

  if (someThingIsUnique(p))
  {
    o << "  template<typename T> void Read(std::istream &i, std::unique_ptr<T> &v) {" << endl;
    o << "    char ref = 0;" << endl;
    o << "    i.read(&ref, 1);" << endl;
    o << "    if (ref == '\\x1') {" << endl;
    o << "      v = std::unique_ptr<T>(new T);" << endl;
    o << "      Read(i, *v);" << endl;
    o << "    }" << endl;
    o << "  }" << endl << endl;
  }

  o << "  template<typename T> void Read(std::istream &, std::shared_ptr<T> &) {" << endl;
  o << notImplementedAssert << endl;
  o << "  }" << endl << endl;

  if (someThingIsWeak(p))
  {
    o << "  template<typename T> void Read(std::istream &, std::weak_ptr<T> &) {" << endl;
    o << notImplementedAssert << endl;
    o << "  }" << endl << endl;
  }

  if (someThingIsShared(p))
  {
    o << "  template<typename T> void Read(std::istream &s, std::shared_ptr<T> &v, std::vector<std::shared_ptr<T>> &cache) {"
      << endl;
    o << "    char ref = 0;" << endl;
    o << "    s.read(&ref, 1);" << endl;
    o << "    if (ref == '\\x1') {" << endl;
    o << "      v = std::make_shared<T>();" << endl;
    o << "      cache.push_back(v);" << endl;
    o << "      Read(s, *v);" << endl;
    o << "    } else if (ref == '\\x2') {" << endl;
    o << "      unsigned int index = 0;" << endl;
    o << "      Read(s, index);" << endl;
    o << "      v = cache[index - 1];" << endl;
    o << "    }" << endl;
    o << "  }" << endl << endl;
  }

  if (someThingIsUniqueVector(p))
  {
    o << "  template<typename T> void Read(std::istream &s, std::vector<std::unique_ptr<T>> &v) {" << endl;
    o << "    auto size = v.size();" << endl;
    o << "    Read(s, size);" << endl;
    o << "    v.resize(size);" << endl;
    o << "    for (auto &entry : v)" << endl;
    o << "      Read(s, entry);" << endl;
    o << "  }" << endl << endl;
  }

  if (someThingIsSharedVector(p))
  {
    o << "  template<typename T> void Read(std::istream &s, std::vector<std::shared_ptr<T>> &v) {" << endl;
    o << "    auto size = v.size();" << endl;
    o << "    Read(s, size);" << endl;
    o << "    v.resize(size);" << endl;
    o << "    for (auto &entry : v)" << endl;
    o << "      Read(s, entry);" << endl;
    o << "  }" << endl << endl;
  }

  if (someThingIsWeakVector(p))
  {
    o << "  template<typename T> void Read(std::istream &s, std::vector<std::weak_ptr<T>> &v) {" << endl;
    o << "    auto size = v.size();" << endl;
    o << "    Read(s, size);" << endl;
    o << "    v.resize(size);" << endl;
    o << "    for (auto &entry : v)" << endl;
    o << "      Read(s, entry);" << endl;
    o << "  }" << endl << endl;
  }

  o << "  template<typename T> void Read(std::istream &i, std::vector<T> &v) {" << endl;
  o << "    typename std::vector<T>::size_type s{0};" << endl;
  o << "    Read(i, s);" << endl;
  o << "    v.resize(s);" << endl;
  o << "    i.read(reinterpret_cast<char *>(v.data()), sizeof(T) * s);" << endl;
  o << "  }" << endl << endl;

  if (hasVectorOfString(p))
  {
    o << "  void Read(std::istream &i, std::vector<std::string> &v) {" << endl;
    o << "    auto size = v.size();" << endl;
    o << "    Read(i, size);" << endl;
    o << "    v.resize(size);" << endl;
    o << "    for (auto &entry : v)" << endl;
    o << "      Read(i, entry);" << endl;
    o << "  }" << endl << endl;
  }

  if (hasPlainString(p))
  {
    o << "  void Read(std::istream &i, std::string &v) {" << endl;
    o << "    std::string::size_type s{0};" << endl;
    o << "    Read(i, s);" << endl;
    o << "    v.resize(s);" << endl;
    o << "    i.read(&v[0], s);" << endl;
    o << "  }" << endl << endl;
  }
}

ostream &WriteType(ostream &o, const Member &m)
{
  if (m.isVector)
    o << "std::vector<";
  if (m.pointer == Pointer::Weak)
    o << "std::weak_ptr<";
  if (m.pointer == Pointer::Unique)
    o << "std::unique_ptr<";
  if (m.pointer == Pointer::Shared)
    o << "std::shared_ptr<";
  o << m.type;
  if (m.pointer != Pointer::Plain)
    o << ">";
  if (m.isVector)
    o << ">";
  return o;
}

ostream &WriteParameterType(ostream &o, const Member &m)
{
  if (m.pointer != Pointer::Unique)
    o << "const ";

  auto mm = m;
  if (!m.isVector && m.pointer == Pointer::Weak)
    mm.pointer = Pointer::Shared;

  WriteType(o, mm) << " ";
  if (m.pointer != Pointer::Unique)
    o << "&";

  return o << m.name << "_";
}

std::pair<int, const Member *> memberEntry(const Table &t, const string &name)
{
  int i = 0;
  for (const auto &m : t.member)
  {
    if (m.name == name)
      return make_pair(i, &m);
    ++i;
  }
  throw std::runtime_error("Member not defined!");
}

std::vector<std::pair<int, const Member *>> indexParameter(const Table &t, const Method &m)
{
  std::vector<std::pair<int, const Member *>> parameter;

  for (const auto &p : m.parameter)
    parameter.emplace_back(memberEntry(t, p.name));

  return parameter;
}

void WriteTableInitializer(ostream &o, const Table &t)
{
  o << endl << "  " << t.name << "() = default;" << endl;
  for (const auto &method : t.methods)
  {
    if (method.name != "init")
      continue;

    auto parameter = indexParameter(t, method);
    o << "  " << t.name << "(";
    string comma;
    for (const auto &p : parameter)
    {
      o << comma;
      WriteParameterType(o, *p.second);
      comma = ", ";
    }
    o << ")" << endl;
    comma = ": ";
    std::sort(parameter.begin(), parameter.end());
    for (const auto &p : parameter)
    {
      bool isUnique = p.second->pointer == Pointer::Unique;
      o << "    " << comma << p.second->name << "(";
      if (isUnique)
        o << "std::move(" << p.second->name << "_))" << endl;
      else
        o << p.second->name << "_)" << endl;
      comma = ", ";
    }
    o << "  {}" << endl;
  }
}

string defaultValueLiteral(const Member &m)
{
  if (m.isBaseType)
  {
    if (m.type == "float")
      return m.defaultValue.value.back() == 'f' ? "" : "f";
    else if (m.type.find("std::u") == 0)
      return m.defaultValue.value.back() == 'u' ? "" : "u";
  }
  return string();
}

void WriteTableCompareFunctions(ostream &o, const Table &t)
{
  o << endl << "  friend bool operator==(const " << t.name << "&l, const " << t.name << "&r) {" << endl;
  o << "    return ";
  bool first = true;
  for (const auto &m : t.member)
  {
    o << endl << "      " << (first ? "" : "&& ") << "l." << m.name << " == r." << m.name;
    first = false;
  }
  o << ";" << endl;
  o << "  }" << endl << endl;

  o << "  friend bool operator!=(const " << t.name << "&l, const " << t.name << "&r) {" << endl;
  o << "    return ";
  first = true;
  for (const auto &m : t.member)
  {
    o << endl << "      " << (first ? "" : "|| ") << "l." << m.name << " != r." << m.name;
    first = false;
  }
  o << ";" << endl;
  o << "  }" << endl;
}

void WriteMemberVectorFunctions(ostream &o, const Package &p, const Member &m)
{
  if (!m.isVector)
    return;

  o << endl;
  if (m.pointer != Pointer::Unique)
  {
    o << "  template<class T> void fill_" << m.name << "(const T &v) {" << endl;
    o << "    std::fill(" << m.name << ".begin(), " << m.name << ".end(), v);" << endl;
    o << "  }" << endl << endl;
  }

  o << "  template<class Generator> void generate_" << m.name << "(Generator gen) {" << endl;
  o << "    std::generate(" << m.name << ".begin(), " << m.name << ".end(), gen);" << endl;
  o << "  }" << endl << endl;

  o << "  template<class T> ";
  WriteType(o, m) << "::iterator remove_" << m.name << "(const T &v) {" << endl;
  o << "    return std::remove(" << m.name << ".begin(), " << m.name << ".end(), v);" << endl;
  o << "  }" << endl;
  o << "  template<class Pred> ";
  WriteType(o, m) << "::iterator remove_" << m.name << "_if(Pred v) {" << endl;
  o << "    return std::remove_if(" << m.name << ".begin(), " << m.name << ".end(), v);" << endl;
  o << "  }" << endl << endl;

  o << "  template<class T> void erase_" << m.name << "(const T &v) {" << endl;
  o << "    " << m.name << ".erase(remove_" << m.name << "(v));" << endl;
  o << "  }" << endl;
  o << "  template<class Pred> void erase_" << m.name << "_if(Pred v) {" << endl;
  o << "    " << m.name << ".erase(remove_" << m.name << "_if(v));" << endl;
  o << "  }" << endl << endl;

  o << "  void reverse_" << m.name << "() {" << endl;
  o << "    std::reverse(" << m.name << ".begin(), " << m.name << ".end());" << endl;
  o << "  }" << endl << endl;

  o << "  void rotate_" << m.name << "(";
  WriteType(o, m) << "::iterator i) {" << endl;
  o << "    std::rotate(" << m.name << ".begin(), i, " << m.name << ".end());" << endl;
  o << "  }" << endl << endl;

  if (m.isBaseType || isEnum(p, m.type))
  {
    o << "  void sort_" << m.name << "() {" << endl;
    o << "    std::sort(" << m.name << ".begin(), " << m.name << ".end());" << endl;
    o << "  }" << endl;
  }
  o << "  template<class Comp> void sort_" << m.name << "(Comp p) {" << endl;
  o << "    std::sort(" << m.name << ".begin(), " << m.name << ".end(), p);" << endl;
  o << "  }" << endl << endl;

  o << "  template<class Comp> bool any_of_" << m.name << "(Comp p) {" << endl;
  o << "    return std::any_of(" << m.name << ".begin(), " << m.name << ".end(), p);" << endl;
  o << "  }" << endl;
  o << "  template<class T> bool any_of_" << m.name << "_is(const T &p) {" << endl;
  o << "    return any_of_" << m.name << "([&p](const ";
  auto mm = m;
  mm.isVector = false;
  auto mmm = mm;
  mmm.pointer = Pointer::Shared;
  WriteType(o, mm) << " &x) { return ";
  if (m.pointer == Pointer::Weak)
    o << "x.lock() && ";
  else if (m.pointer != Pointer::Plain)
    o << "x && ";
  o << (m.pointer != Pointer::Plain ? "*" : "") << "x" << (m.pointer == Pointer::Weak ? ".lock()" : "") << " == p; });"
    << endl;
  o << "  }" << endl << endl;
  if (m.pointer == Pointer::Shared || m.pointer == Pointer::Weak)
  {
    o << "  bool any_of_" << m.name << "_is(const ";
    WriteType(o, mmm) << " &p) {" << endl;
    o << "    return any_of_" << m.name << "([&p](const ";
    WriteType(o, mm) << " &x) { return ";
    o << "x" << (m.pointer == Pointer::Weak ? ".lock()" : "") << " == p; });" << endl;
    o << "  }" << endl << endl;
  }

  o << "  template<class Comp> bool all_of_" << m.name << "(Comp p) {" << endl;
  o << "    return std::all_of(" << m.name << ".begin(), " << m.name << ".end(), p);" << endl;
  o << "  }" << endl;
  o << "  template<class T> bool all_of_" << m.name << "_are(const T &p) {" << endl;
  o << "    return all_of_" << m.name << "([&p](const ";
  WriteType(o, mm) << " &x) { return ";
  if (m.pointer == Pointer::Weak)
    o << "x.lock() && ";
  else if (m.pointer != Pointer::Plain)
    o << "x && ";
  o << (m.pointer != Pointer::Plain ? "*" : "") << "x" << (m.pointer == Pointer::Weak ? ".lock()" : "") << " == p; });"
    << endl;
  o << "  }" << endl << endl;
  if (m.pointer == Pointer::Shared || m.pointer == Pointer::Weak)
  {
    o << "  bool all_of_" << m.name << "_are(const ";
    WriteType(o, mmm) << " &p) {" << endl;
    o << "    return all_of_" << m.name << "([&p](const ";
    WriteType(o, mm) << " &x) { return ";
    o << "x" << (m.pointer == Pointer::Weak ? ".lock()" : "") << " == p; });" << endl;
    o << "  }" << endl << endl;
  }

  o << "  template<class Comp> bool none_of_" << m.name << "(Comp p) {" << endl;
  o << "    return std::none_of(" << m.name << ".begin(), " << m.name << ".end(), p);" << endl;
  o << "  }" << endl;
  o << "  template<class T> bool none_of_" << m.name << "_is(const T &p) {" << endl;
  o << "    return none_of_" << m.name << "([&p](const ";
  WriteType(o, mm) << " &x) { return ";
  if (m.pointer == Pointer::Weak)
    o << "x.lock() && ";
  else if (m.pointer != Pointer::Plain)
    o << "x && ";
  o << (m.pointer != Pointer::Plain ? "*" : "") << "x" << (m.pointer == Pointer::Weak ? ".lock()" : "") << " == p; });"
    << endl;
  o << "  }" << endl << endl;
  if (m.pointer == Pointer::Shared || m.pointer == Pointer::Weak)
  {
    o << "  bool none_of_" << m.name << "_is(const ";
    WriteType(o, mmm) << " &p) {" << endl;
    o << "    return none_of_" << m.name << "([&p](const ";
    WriteType(o, mm) << " &x) { return ";
    o << "x" << (m.pointer == Pointer::Weak ? ".lock()" : "") << " == p; });" << endl;
    o << "  }" << endl << endl;
  }

  o << "  template<class Fn> Fn for_each_" << m.name << "(Fn p) {" << endl;
  o << "    return std::for_each(" << m.name << ".begin(), " << m.name << ".end(), p);" << endl;
  o << "  }" << endl << endl;

  o << "  template<class T> ";
  WriteType(o, m) << "::iterator find_in_" << m.name << "(const T &p) {" << endl;
  o << "    return std::find(" << m.name << ".begin(), " << m.name << ".end(), p);" << endl;
  o << "  }" << endl;
  o << "  template<class Comp> ";
  WriteType(o, m) << "::iterator find_in_" << m.name << "_if(Comp p) {" << endl;
  o << "    return std::find_if(" << m.name << ".begin(), " << m.name << ".end(), p);" << endl;
  o << "  }" << endl << endl;

  o << "  template<class T> ";
  o << "  typename std::iterator_traits<";
  WriteType(o, m) << "::iterator>::difference_type count_in_" << m.name << "(const T &p) {" << endl;
  o << "    return std::count(" << m.name << ".begin(), " << m.name << ".end(), p);" << endl;
  o << "  }" << endl;
  o << "  template<class Comp> ";
  o << "  typename std::iterator_traits<";
  WriteType(o, m) << "::iterator>::difference_type count_in_" << m.name << "_if(Comp p) {" << endl;
  o << "    return std::count_if(" << m.name << ".begin(), " << m.name << ".end(), p);" << endl;
  o << "  }" << endl;
}

void WriteTableDeclaration(ostream &o, const Package &p, const Table &t, const string &root_type)
{
  o << "struct " << t.name << " {" << endl;
  for (const auto &m : t.member)
  {
    o << "  ";
    WriteType(o, m) << " " << m.name;
    if (!m.isVector && m.pointer == Pointer::Plain && !m.defaultValue.value.empty())
      o << "{" << m.defaultValue.value << defaultValueLiteral(m) << "}";
    o << ";" << endl;
  }

  WriteTableInitializer(o, t);

  WriteTableCompareFunctions(o, t);

  for (const auto &m : t.member)
    WriteMemberVectorFunctions(o, p, m);

  if (hasSharedAppearance(t))
  {
    o << endl;
    o << "private:" << endl;
    o << "  unsigned int io_counter_{0};" << endl;
    o << "  friend struct " << root_type << "_io;" << endl;
  }
  o << "};" << endl << endl;
}

template <class T>
void WritePointerOutputFor(ostream &o, const T &t)
{
  if (hasSharedAppearance(t))
  {
    o << "  void Write(std::ostream &o, const std::shared_ptr<" << t.name << "> &v) {" << endl;
    o << "    Write(o, v, " << t.name << "_count_);" << endl;
    o << "  }" << endl << endl;
  }
  if (hasWeakAppearance(t))
  {
    o << "  void Write(std::ostream &o, const std::weak_ptr<" << t.name << "> &v) {" << endl;
    o << "    Write(o, v.lock(), " << t.name << "_count_);" << endl;
    o << "  }" << endl << endl;
  }
  if (isComplex(t) && hasPlainVectorAppearance(t))
  {
    o << "  void Write(std::ostream &o, const std::vector<" << t.name << "> &v) {" << endl;
    o << "    Write(o, v.size());" << endl;
    o << "    for (const auto &entry : v)" << endl;
    o << "      Write(o, entry);" << endl;
    o << "  }" << endl << endl;
  }
}

void WriteTableOutput(ostream &o, const Table &t)
{
  if (isComplex(t))
  {
    o << "  void Write(std::ostream &o, const " << t.name << " &v) {" << endl;
    for (const auto &m : t.member)
      o << "    Write(o, v." << m.name << ");" << endl;
    o << "  }" << endl << endl;
  }

  WritePointerOutputFor(o, t);
}

template <class T>
void WritePointerInputFor(ostream &o, const T &t)
{
  if (hasSharedAppearance(t))
  {
    o << "  void Read(std::istream &s, std::shared_ptr<" << t.name << "> &v) {" << endl;
    o << "    Read(s, v, " << t.name << "_references_);" << endl;
    o << "  }" << endl << endl;
  }
  if (hasWeakAppearance(t))
  {
    o << "  void Read(std::istream &s, std::weak_ptr<" << t.name << "> &v) {" << endl;
    o << "    auto t = v.lock();" << endl;
    o << "    Read(s, t, " << t.name << "_references_);" << endl;
    o << "    v = t;" << endl;
    o << "  }" << endl << endl;
  }
  if (isComplex(t) && hasPlainVectorAppearance(t))
  {
    o << "  void Read(std::istream &s, std::vector<" << t.name << "> &v) {" << endl;
    o << "    auto size = v.size();" << endl;
    o << "    Read(s, size);" << endl;
    o << "    v.resize(size);" << endl;
    o << "    for (auto &entry : v)" << endl;
    o << "      Read(s, entry);" << endl;
    o << "  }" << endl << endl;
  }
}

void WriteTableInput(ostream &o, const Table &t)
{
  if (isComplex(t))
  {
    o << "  void Read(std::istream &s, " << t.name << " &v) {" << endl;
    for (const auto &m : t.member)
      o << "    Read(s, v." << m.name << ");" << endl;
    o << "  }" << endl << endl;
  }

  WritePointerInputFor(o, t);
}

void WriteCompareOperatorForWeakPointer(ostream &o)
{
  o << "template<typename T> bool operator==(const std::weak_ptr<T> &l, const std::weak_ptr<T> &r) {" << endl;
  o << "  return l.lock() == r.lock();" << endl;
  o << "}" << endl << endl;

  o << "template<typename T> bool operator!=(const std::weak_ptr<T> &l, const std::weak_ptr<T> &r) {" << endl;
  o << "  return l.lock() != r.lock();" << endl;
  o << "}" << endl << endl;
}

void WriteUnionStruct(ostream &o, const Union &u, const string &root_type)
{
  o << "struct " << u.name << " {" << endl;

  o << "  " << u.name << "() = default;" << endl;
  o << "  " << u.name << "(const " << u.name << " &o) { _clone(o); }" << endl;
  o << "  " << u.name << "& operator=(const " << u.name << " &o) { _destroy(); _clone(o); return *this; }" << endl
    << endl;

  for (const auto &t : u.tables)
  {
    o << "  " << u.name << "(const " << t.value << " &v)" << endl;
    o << "    : _" << t.value << "(new " << t.value << "(v))" << endl;
    o << "    , _selection(_" << t.value << "_selection)" << endl;
    o << "  {}" << endl;
    o << "  " << u.name << "(" << t.value << " &&v)" << endl;
    o << "    : _" << t.value << "(new " << t.value << "(std::forward<" << t.value << ">(v)))" << endl;
    o << "    , _selection(_" << t.value << "_selection)" << endl;
    o << "  {}" << endl;
    o << "  " << u.name << " & operator=(const " << t.value << " &v) {" << endl;
    o << "    _destroy();" << endl;
    o << "    _" << t.value << " = new " << t.value << "(v);" << endl;
    o << "    _selection = _" << t.value << "_selection;" << endl;
    o << "    return *this;" << endl;
    o << "  }" << endl;
    o << "  " << u.name << " & operator=(" << t.value << " &&v) {" << endl;
    o << "    _destroy();" << endl;
    o << "    _" << t.value << " = new " << t.value << "(std::forward<" << t.value << ">(v));" << endl;
    o << "    _selection = _" << t.value << "_selection;" << endl;
    o << "    return *this;" << endl;
    o << "  }" << endl << endl;
  }

  o << "  ~" << u.name << "() {" << endl;
  o << "    _destroy();" << endl;
  o << "  }" << endl << endl;

  o << "  bool is_Defined() const noexcept { return _selection != no_selection; }" << endl;
  o << "  void clear() { *this = " << u.name << "(); }" << endl << endl;
  for (const auto &t : u.tables)
  {
    o << "  bool is_" << t.value << "() const noexcept { return _selection == _" << t.value << "_selection; }" << endl;
    o << "  const " << t.value << " & as_" << t.value << "() const noexcept { return *_" << t.value << "; }" << endl;
    o << "  " << t.value << " & as_" << t.value << "() { return *_" << t.value << "; }" << endl;

    o << "  template<typename... Args> " << t.value << " & create_" << t.value << "(Args&&... args) {" << endl;
    o << "    return (*this = " << t.value << "(std::forward<Args>(args)...)).as_" << t.value << "();" << endl;
    o << "  }" << endl << endl;
  }

  for (const auto &t : u.tables)
  {
    o << "  friend bool operator==(const " << u.name << "&ab, const " << t.value << " &o) noexcept";
    o << "  { return ab.is_" << t.value << "() && ab.as_" << t.value << "() == o; }" << endl;
    o << "  friend bool operator==(const " << t.value << " &o, const " << u.name << "&ab) noexcept";
    o << "  { return ab.is_" << t.value << "() && o == ab.as_" << t.value << "(); }" << endl;
    o << "  friend bool operator!=(const " << u.name << "&ab, const " << t.value << " &o) noexcept";
    o << "  { return !ab.is_" << t.value << "() || ab.as_" << t.value << "() != o; }" << endl;
    o << "  friend bool operator!=(const " << t.value << " &o, const " << u.name << "&ab) noexcept";
    o << "  { return !ab.is_" << t.value << "() || o != ab.as_" << t.value << "(); }" << endl << endl;
  }

  o << "  bool operator==(const " << u.name << " &o) const noexcept" << endl;
  o << "  {" << endl;
  o << "    if (this == &o)" << endl;
  o << "      return true;" << endl;
  o << "    if (_selection != o._selection)" << endl;
  o << "      return false;" << endl;
  o << "    switch(_selection) {" << endl;
  o << "    case no_selection: while(false); /* hack for coverage tool */ return true;" << endl;
  for (const auto &t : u.tables)
    o << "    case _" << t.value << "_selection: return *_" << t.value << " == *o._" << t.value << ";" << endl;
  o << "    }" << endl;
  o << "    return false; // without this line there is a msvc warning I do not understand." << endl;
  o << "  }" << endl << endl;

  o << "  bool operator!=(const " << u.name << " &o) const noexcept" << endl;
  o << "  {" << endl;
  o << "    if (this == &o)" << endl;
  o << "      return false;" << endl;
  o << "    if (_selection != o._selection)" << endl;
  o << "      return true;" << endl;
  o << "    switch(_selection) {" << endl;
  o << "    case no_selection: while(false); /* hack for coverage tool */ return false;" << endl;
  for (const auto &t : u.tables)
    o << "    case _" << t.value << "_selection: return *_" << t.value << " != *o._" << t.value << ";" << endl;
  o << "    }" << endl;
  o << "    return false; // without this line there is a msvc warning I do not understand." << endl;
  o << "  }" << endl << endl;

  o << "private:" << endl;
  o << "  void _clone(const " << u.name << " &o) noexcept" << endl;
  o << "  {" << endl;
  o << "     _selection = o._selection;" << endl;
  o << "    switch(_selection) {" << endl;
  o << "    case no_selection: while(false); /* hack for coverage tool */ break;" << endl;
  for (const auto &t : u.tables)
    o << "    case _" << t.value << "_selection: _" << t.value << " = new " << t.value << "(*o._" << t.value
      << "); break;" << endl;
  o << "    }" << endl;
  o << "  }" << endl << endl;

  o << "  void _destroy() noexcept {" << endl;
  o << "    switch(_selection) {" << endl;
  o << "    case no_selection: while(false); /* hack for coverage tool */ break;" << endl;
  for (const auto &t : u.tables)
    o << "    case _" << t.value << "_selection: delete _" << t.value << "; break;" << endl;
  o << "    }" << endl;
  o << "    no_value = nullptr;" << endl;
  o << "  }" << endl << endl;

  o << "  union {" << endl;
  o << "    struct NoValue_t *no_value{nullptr};" << endl;

  for (const auto &t : u.tables)
    o << "    " << t.value << " * _" << t.value << ";" << endl;
  o << "  };" << endl << endl;
  o << "  enum Selection_t {" << endl;
  o << "    no_selection," << endl;
  for (const auto &t : u.tables)
    o << "    _" << t.value << "_selection," << endl;
  o << "  };" << endl << endl;

  o << "  Selection_t _selection{no_selection};" << endl;
  if (hasSharedAppearance(u))
    o << "  unsigned int io_counter_{0};" << endl;
  o << "  friend struct " << root_type << "_io;" << endl;
  o << "};" << endl << endl;
}

void WriteTypeStructs(ostream &o, const Package &p)
{
  if (someThingIsWeak(p))
    WriteCompareOperatorForWeakPointer(o);

  for (const auto &t : p.types)
  {
    if (t.is_Table())
      WriteTableDeclaration(o, p, t.as_Table(), p.root_type.value);
    else if (t.is_Union())
      WriteUnionStruct(o, t.as_Union(), p.root_type.value);
    else if (t.is_Enum())
    {
      WriteEnumDeclaration(o, t.as_Enum());
      WriteEnumFunctions(o, t.as_Enum());
    }
  }
}

void WriteUnionOutput(ostream &o, const Union &u)
{
  o << "  void Write(std::ostream &o, const " << u.name << " &v) {" << endl;
  o << "    o.write(reinterpret_cast<const char*>(&v._selection), sizeof(" << u.name << "::Selection_t));" << endl;
  o << "    switch(v._selection) {" << endl;
  o << "    case " << u.name << "::no_selection: while(false); /* hack for coverage tool */ break;" << endl;
  for (const auto &t : u.tables)
    o << "    case " << u.name << "::_" << t.value << "_selection: Write(o, v.as_" << t.value << "()); break;" << endl;
  o << "    }" << endl;
  o << "  }" << endl << endl;

  WritePointerOutputFor(o, u);
}

void WriteUnionInput(ostream &o, const Union &u)
{
  o << "  void Read(std::istream &i, " << u.name << " &v) {" << endl;
  o << "    i.read(reinterpret_cast<char*>(&v._selection), sizeof(" << u.name << "::Selection_t));" << endl;
  o << "    switch(v._selection) {" << endl;
  o << "    case " << u.name << "::no_selection: while(false); /* hack for coverage tool */ break;" << endl;
  for (const auto &t : u.tables)
    o << "    case " << u.name << "::_" << t.value << "_selection: Read(i, v.create_" << t.value << "()); break;"
      << endl;
  o << "    }" << endl;
  o << "  }" << endl << endl;

  WritePointerInputFor(o, u);
}

void WriteTablesIOFunctions(ostream &o, const vector<Type> &types)
{
  for (const auto &t : types)
  {
    if (t.is_Table())
    {
      WriteTableOutput(o, t.as_Table());
      WriteTableInput(o, t.as_Table());
    }
    else if (t.is_Union())
    {
      WriteUnionOutput(o, t.as_Union());
      WriteUnionInput(o, t.as_Union());
    }
  }
}

void WriteBaseIO(ostream &o, const Package &p)
{
  o << "  void Write" << p.root_type.value << "(std::ostream &o, const " << p.root_type.value << " &v) {" << endl;
  for (const auto &t : p.types)
    if (t.is_Table() && hasSharedAppearance(t.as_Table()))
      o << "    " << t.as_Table().name << "_count_ = 0;" << endl;

  o << endl << "    o.write(\"CORE\", 4);" << endl;
  o << "    o.write(\"" << p.version.value << "\", " << p.version.value.size() << ");" << endl;
  o << "    Write(o, v);" << endl;
  o << "  }" << endl << endl;

  o << "  bool Read" << p.root_type.value << "(std::istream &i, " << p.root_type.value << " &v) {" << endl;

  for (const auto &t : p.types)
    if (t.is_Table() && hasSharedAppearance(t.as_Table()))
      o << "    " << t.as_Table().name << "_references_.clear();" << endl;

  o << endl << "    std::string marker(\"0000\");" << endl;
  o << "    i.read(&marker[0], 4);" << endl;
  o << "    if (marker != \"CORE\")" << endl;
  o << "      return false;" << endl;

  o << "    std::string version(" << p.version.value.size() << ", '_');" << endl;
  o << "    i.read(&version[0], " << p.version.value.size() << ");" << endl;
  o << "    if (version != \"" << p.version.value << "\")" << endl;
  o << "      return false;" << endl;

  o << "    Read(i, v);" << endl;
  o << "    return true;" << endl;
  o << "  }" << endl << endl;
}

void WriteIOStructMember(const Package &p, ostream &o)
{
  for (const auto &t : p.types)
  {
    if (t.is_Table() && hasSharedAppearance(t.as_Table()))
    {
      o << "  unsigned int " << t.as_Table().name << "_count_{0};" << endl;
      o << "  std::vector<std::shared_ptr<" << t.as_Table().name << ">> " << t.as_Table().name << "_references_;"
        << endl
        << endl;
    }
    else if (t.is_Union() && hasSharedAppearance(t.as_Union()))
    {
      o << "  unsigned int " << t.as_Union().name << "_count_{0};" << endl;
      o << "  std::vector<std::shared_ptr<" << t.as_Union().name << ">> " << t.as_Union().name << "_references_;"
        << endl
        << endl;
    }
  }
}

void WriteIOStruct(ostream &o, const Package &p)
{
  o << "struct " << p.root_type.value << "_io {" << endl;
  o << "private:" << endl;

  WriteIOStructMember(p, o);
  WriteBaseTypeIoFnuctions(o, p);
  WriteTablesIOFunctions(o, p.types);

  o << "public:" << endl;

  WriteBaseIO(o, p);

  o << "};" << endl;
}

void WriteHelperForNotImplementedTemplates(ostream &o)
{
  o << "template<typename T>" << endl;
  o << "struct AlwaysFalse : std::false_type {};" << endl;
}

void WriteCppCode(ostream &o, const Package &p)
{
  o << "#pragma once" << endl << endl;

  o << "#include <vector>" << endl;
  o << "#include <string>" << endl;
  o << "#include <ostream>" << endl;
  o << "#include <istream>" << endl;
  o << "#include <memory>" << endl;
  o << "#include <array>" << endl;
  o << "#include <algorithm>" << endl;
  o << "#include <type_traits>" << endl << endl;

  WriteNameSpaceBegin(o, p.path.value);
  o << endl;

  WriteHelperForNotImplementedTemplates(o);

  WriteForwardDeclarations(o, p);
  WriteTypeStructs(o, p);

  WriteIOStruct(o, p);

  WriteNameSpaceEnd(o, p.path.value);
}
