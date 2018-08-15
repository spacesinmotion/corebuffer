#include "cppoutput.h"

#include <algorithm>
#include <limits>
#include <tuple>

using namespace std;

template <typename Predicate>
bool any_table_of(const Package &p, const Predicate &pr)
{
  return any_of(p.tables.begin(), p.tables.end(), pr) || any_of(p.baseTypes.begin(), p.baseTypes.end(), pr);
}

bool hasUniqueAppearance(const Table &t)
{
  return ((t.appearance & UniqueAppearance) == UniqueAppearance) ||
         ((t.appearance & UniqueVectorAppearance) == UniqueVectorAppearance);
}

bool someThingIsUnique(const Package &p)
{
  return any_table_of(p, hasUniqueAppearance);
}

bool someThingIsUniqueVector(const Package &p)
{
  static const auto hasUniqueVectorAppearance = [](const Table &t) {
    return (t.appearance & UniqueVectorAppearance) == UniqueVectorAppearance;
  };
  return any_table_of(p, hasUniqueVectorAppearance);
}

bool hasSharedAppearance(const Table &t)
{
  return ((t.appearance & SharedAppearance) == SharedAppearance) ||
         ((t.appearance & SharedVectorAppearance) == SharedVectorAppearance);
}

bool someThingIsShared(const Package &p)
{
  return any_table_of(p, hasSharedAppearance);
}

bool someThingIsSharedVector(const Package &p)
{
  static const auto hasSharedVectorAppearance = [](const Table &t) {
    return (t.appearance & SharedVectorAppearance) == SharedVectorAppearance;
  };
  return any_table_of(p, hasSharedVectorAppearance);
}

bool hasWeakAppearance(const Table &t)
{
  return ((t.appearance & WeakAppearance) == WeakAppearance) ||
         ((t.appearance & WeakVectorAppearance) == WeakVectorAppearance);
}

bool someThingIsWeak(const Package &p)
{
  return any_table_of(p, hasWeakAppearance);
}

bool someThingIsWeakVector(const Package &p)
{
  static const auto hasWeakVectorAppearance = [](const Table &t) {
    return (t.appearance & WeakVectorAppearance) == WeakVectorAppearance;
  };
  return any_table_of(p, hasWeakVectorAppearance);
}

bool hasPlainVectorAppearance(const Table &t)
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
  if (!p.tables.empty())
    o << endl;
  for (const auto &t : p.tables)
    o << "struct " << t.name << ";" << endl;
  if (!p.tables.empty())
    o << endl;
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

void WriteEnumDeclarations(ostream &o, const vector<Enum> &enums)
{
  for (const auto &e : enums)
    WriteEnumDeclaration(o, e);
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

  o << "  void Write(std::ostream &o, const std::string &v) {" << endl;
  o << "    Write(o, v.size());" << endl;
  o << "    o.write(v.data(), v.size());" << endl;
  o << "  }" << endl << endl;

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

  o << "  void Read(std::istream &i, std::string &v) {" << endl;
  o << "    std::string::size_type s{0};" << endl;
  o << "    Read(i, s);" << endl;
  o << "    v.resize(s);" << endl;
  o << "    i.read(&v[0], s);" << endl;
  o << "  }" << endl << endl;
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
      return m.defaultValue.back() == 'f' ? "" : "f";
    else if (m.type.find("std::u") == 0)
      return m.defaultValue.back() == 'u' ? "" : "u";
  }
  return string();
}

void WriteTableDeclaration(ostream &o, const Table &t, const string &root_type)
{
  o << "struct " << t.name << " {" << endl;
  for (const auto &m : t.member)
  {
    o << "  ";
    WriteType(o, m) << " " << m.name;
    if (!m.isVector && m.pointer == Pointer::Plain && !m.defaultValue.empty())
      o << "{" << m.defaultValue << defaultValueLiteral(m) << "}";
    o << ";" << endl;
  }

  WriteTableInitializer(o, t);

  if (hasSharedAppearance(t))
  {
    o << endl;
    o << "private:" << endl;
    o << "  unsigned int io_counter_{0};" << endl;
    o << "  friend struct " << root_type << "_io;" << endl;
  }
  o << "};" << endl << endl;
}

void WriteTableOutputFunctions(ostream &o, const Table &t)
{
  o << "  void Write(std::ostream &o, const " << t.name << " &v) {" << endl;
  for (const auto &m : t.member)
    o << "    Write(o, v." << m.name << ");" << endl;
  o << "  }" << endl << endl;

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
  if (hasPlainVectorAppearance(t))
  {
    o << "  void Write(std::ostream &o, const std::vector<" << t.name << "> &v) {" << endl;
    o << "    Write(o, v.size());" << endl;
    o << "    for (const auto &entry : v)" << endl;
    o << "      Write(o, entry);" << endl;
    o << "  }" << endl << endl;
  }
}

void WriteTableInputFunctions(ostream &o, const Table &t)
{
  o << "  void Read(std::istream &s, " << t.name << " &v) {" << endl;
  for (const auto &m : t.member)
    o << "    Read(s, v." << m.name << ");" << endl;
  o << "  }" << endl << endl;

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
  if (hasPlainVectorAppearance(t))
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

void WriteTableCompareFunctions(ostream &o, const Table &t)
{
  o << "bool operator==(const " << t.name << "&l, const " << t.name << "&r) {" << endl;
  o << "  return ";
  bool first = true;
  for (const auto &m : t.member)
  {
    o << endl << "    " << (first ? "" : "&& ") << "l." << m.name << " == r." << m.name;
    first = false;
  }
  o << ";" << endl;
  o << "}" << endl << endl;

  o << "bool operator!=(const " << t.name << "&l, const " << t.name << "&r) {" << endl;
  o << "  return ";
  first = true;
  for (const auto &m : t.member)
  {
    o << endl << "    " << (first ? "" : "|| ") << "l." << m.name << " != r." << m.name;
    first = false;
  }
  o << ";" << endl;
  o << "}" << endl << endl;
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

void WriteTableDeclarations(ostream &o, const Package &p)
{
  for (const auto &t : p.tables)
    WriteTableDeclaration(o, t, p.root_type.value);

  if (someThingIsWeak(p))
    WriteCompareOperatorForWeakPointer(o);

  for (const auto &t : p.tables)
    WriteTableCompareFunctions(o, t);
}

void WriteTablesIOFunctions(ostream &o, const vector<Table> &tables)
{
  for (const auto &t : tables)
  {
    WriteTableOutputFunctions(o, t);
    WriteTableInputFunctions(o, t);
  }
}

void WriteBaseIO(ostream &o, const Package &p)
{
  o << "  void Write" << p.root_type.value << "(std::ostream &o, const " << p.root_type.value << " &v) {" << endl;
  for (const auto &t : p.tables)
    if (hasSharedAppearance(t))
      o << "    " << t.name << "_count_ = 0;" << endl;

  o << endl << "    o.write(\"CORE\", 4);" << endl;
  o << "    o.write(\"" << p.version.value << "\", " << p.version.value.size() << ");" << endl;
  o << "    Write(o, v);" << endl;
  o << "  }" << endl << endl;

  o << "  bool Read" << p.root_type.value << "(std::istream &i, " << p.root_type.value << " &v) {" << endl;

  for (const auto &t : p.tables)
    if (hasSharedAppearance(t))
      o << "    " << t.name << "_references_.clear();" << endl;

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
  for (const auto &t : p.tables)
  {
    if (hasSharedAppearance(t))
    {
      o << "  unsigned int " << t.name << "_count_{0};" << endl;
      o << "  std::vector<std::shared_ptr<" << t.name << ">> " << t.name << "_references_;" << endl << endl;
    }
  }
}

void WriteIOStruct(ostream &o, const Package &p)
{
  o << "struct " << p.root_type.value << "_io {" << endl;
  o << "private:" << endl;

  WriteIOStructMember(p, o);
  WriteBaseTypeIoFnuctions(o, p);
  WriteTablesIOFunctions(o, p.tables);

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
  o << "#include <type_traits>" << endl << endl;

  WriteNameSpaceBegin(o, p.path.value);
  o << endl;

  WriteHelperForNotImplementedTemplates(o);

  WriteForwardDeclarations(o, p);
  WriteEnumDeclarations(o, p.enums);
  WriteTableDeclarations(o, p);

  for (const auto &e : p.enums)
    WriteEnumFunctions(o, e);

  WriteIOStruct(o, p);

  WriteNameSpaceEnd(o, p.path.value);
}
