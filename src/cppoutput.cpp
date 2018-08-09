#include "cppoutput.h"

using std::endl;

void WriteNameSpaceBegin(std::ostream &o, const string &path, int pos = 0)
{
  if (path.empty())
    return;

  const auto end = path.find('.', pos);
  o << "namespace " << path.substr(pos, end) << " {" << endl;
  if (end != string::npos)
    WriteNameSpaceBegin(o, path, int(end) + 1);
}

void WriteNameSpaceEnd(std::ostream &o, const string &path, int pos = 0)
{
  if (path.empty())
    return;

  const auto end = path.find('.', pos);
  o << "}" << endl;
  if (end != string::npos)
    WriteNameSpaceEnd(o, path, int(end) + 1);
}

void WriteForwardDeclarations(std::ostream &o, const Package &p)
{
  if (!p.tables.empty())
    o << endl;
  for (const auto &t : p.tables)
    o << "struct " << t.name << ";" << endl;
  if (!p.tables.empty())
    o << endl;
}

void WriteEnumDeclaration(std::ostream &o, const Enum &e)
{
  o << "enum class " << e.name << " : "
    << "unsigned int {" << endl;
  for (const auto &v : e.entries)
    o << "  " << v.first << " = " << v.second << "," << endl;
  o << "};" << endl << endl;
}

void WriteEnumFunctions(std::ostream &o, const Enum &e)
{
  o << "inline const std::array<" << e.name << "," << e.entries.size() << "> & " << e.name << "Values() {" << endl;
  o << "  static const std::array<" << e.name << "," << e.entries.size() << "> values {" << endl;
  for (const auto &v : e.entries)
    o << "    " << e.name << "::" << v.first << "," << endl;
  o << "  };" << endl;
  o << "  return values;" << endl;
  o << "};" << endl << endl;

  o << "inline const char * ValueName(const " << e.name << " &v) {" << endl;
  o << "  switch(v) {" << endl;
  for (const auto &v : e.entries)
    o << "    case " << e.name << "::" << v.first << ": return \"" << v.first << "\";" << endl;
  o << "  }" << endl;
  o << "  return \"<error>\";" << endl;
  o << "};" << endl << endl;
}

void WriteEnumDeclarations(std::ostream &o, const vector<Enum> &enums)
{
  for (const auto &e : enums)
    WriteEnumDeclaration(o, e);
}

void WriteBaseTypecIoFnuctions(std::ostream &o, const Package &p)
{
  o << "template<typename T> void Write(std::ostream &o, const T *v) {" << endl;
  o << "  static_assert(AlwaysFalse<T>::value, \"Something not implemented\");" << endl;
  o << "}" << endl << endl;

  o << "template<typename T> void Write(std::ostream &o, const T &v) {" << endl;
  o << "  o.write(reinterpret_cast<const char *>(&v), sizeof(T));" << endl;
  o << "}" << endl << endl;

  o << "template<typename T> void Write(std::ostream &o, const std::vector<T> &v) {" << endl;
  o << "  Write(o, v.size());" << endl;
  o << "  o.write(reinterpret_cast<const char *>(v.data()), sizeof(T) * v.size());" << endl;
  o << "}" << endl << endl;

  o << "template<typename T> void Write(std::ostream &o, const std::unique_ptr<T> &v) {" << endl;
  o << "  static_assert(AlwaysFalse<T>::value, \"Something not implemented\");" << endl;
  o << "}" << endl << endl;

  o << "template<typename T> void Write(std::ostream &o, const std::shared_ptr<T> &v) {" << endl;
  o << "  static_assert(AlwaysFalse<T>::value, \"Something not implemented\");" << endl;
  o << "}" << endl << endl;

  o << "template<typename T> void Write(std::ostream &o, const std::weak_ptr<T> &v) {" << endl;
  o << "  static_assert(AlwaysFalse<T>::value, \"Something not implemented\");" << endl;
  o << "}" << endl << endl;

  o << "template<typename T> void Write(std::ostream &o, const std::vector<std::unique_ptr<T>> &v) {" << endl;
  o << "  static_assert(AlwaysFalse<T>::value, \"Something not implemented\");" << endl;
  o << "}" << endl << endl;

  o << "template<typename T> void Write(std::ostream &o, const std::vector<std::shared_ptr<T>> &v) {" << endl;
  o << "  static_assert(AlwaysFalse<T>::value, \"Something not implemented\");" << endl;
  o << "}" << endl << endl;

  o << "template<typename T> void Write(std::ostream &o, const std::vector<std::weak_ptr<T>> &v) {" << endl;
  o << "  static_assert(AlwaysFalse<T>::value, \"Something not implemented\");" << endl;
  o << "}" << endl << endl;

  o << "void Write(std::ostream &o, const std::string &v) {" << endl;
  o << "  Write(o, v.size());" << endl;
  o << "  o.write(v.data(), v.size());" << endl;
  o << "}" << endl << endl;

  o << "void Write(std::ostream &o, const char *v) {" << endl;
  o << "  Write(o, std::string(v));" << endl;
  o << "}" << endl << endl;

  o << "template<typename T> void Read(std::istream &i, T &v) {" << endl;
  o << "  i.read(reinterpret_cast<char *>(&v), sizeof(T));" << endl;
  o << "}" << endl << endl;

  o << "template<typename T> void Read(std::istream &i, std::vector<T> &v) {" << endl;
  o << "  typename std::vector<T>::size_type s{0};" << endl;
  o << "  Read(i, s);" << endl;
  o << "  v.resize(s);" << endl;
  o << "  i.read(reinterpret_cast<char *>(v.data()), sizeof(T) * s);" << endl;
  o << "}" << endl << endl;

  o << "void Read(std::istream &i, std::string &v) {" << endl;
  o << "  std::string::size_type s{0};" << endl;
  o << "  Read(i, s);" << endl;
  o << "  v.resize(s);" << endl;
  o << "  i.read(&v[0], s);" << endl;
  o << "}" << endl << endl;

  for (const auto &b : p.baseTypes)
  {
    if (b.appearance == NoPointer)
      continue;
  }
}

std::ostream &WriteType(std::ostream &o, const Member &m)
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

void WriteTableDeclaration(std::ostream &o, const Table &t, const std::string &root_type)
{
  o << "struct " << t.name << " {" << endl;
  for (const auto &m : t.member)
  {
    o << "  ";
    WriteType(o, m) << " " << m.name;
    if (!m.defaultValue.empty())
      o << "{" << m.defaultValue << "}";
    o << ";" << endl;
  }
  o << "private:" << endl;
  o << "  unsigned int io_counter_{0};" << endl;
  o << "  friend struct " << root_type << "_io;" << endl;
  o << "};" << endl << endl;
}

void WriteTableOutputFunctions(std::ostream &o, const Table &t)
{
  o << "void Write(std::ostream &o, const " << t.name << " &v) {" << endl;
  for (const auto &m : t.member)
  {
    auto n = "v." + m.name;
    if (m.isVector && !m.isBaseType)
    {
      o << "  o << " << n << ".size();" << endl;
      n = "entry";
      o << "  for (const auto &" << n << " : v." << m.name << ") {" << endl;
      o << "  ";
    }
    if (m.pointer == Pointer::Plain || m.isBaseType)
    {
      o << "  Write(o, " << n << ");" << endl;
    }
    else if (m.pointer == Pointer::Unique)
    {
      o << "  if (!" << n << ") {" << endl;
      o << "    o.write(\"\\x0\", 1);" << endl;
      o << "  } else {" << endl;
      o << "    o.write(\"\\x1\", 1);" << endl;
      o << "    Write(o, *" << n << ");" << endl;
      o << "  }" << endl;
    }
    else
    {
      o << "  {" << endl;
      o << "    const auto t = " << n << (m.pointer == Pointer::Weak ? ".lock()" : "") << ";" << endl;
      o << "    if (!t) {" << endl;
      o << "      o.write(\"\\x0\", 1);" << endl;
      o << "    } else if (t->io_counter_== 0) {" << endl;
      o << "      o.write(\"\\x1\", 1);" << endl;
      o << "      Write(o, *t);" << endl;
      o << "      t->io_counter_ = ++" << m.type << "_count_;" << endl;
      o << "    } else {" << endl;
      o << "      o.write(\"\\x2\", 1);" << endl;
      o << "      Write(o, t->io_counter_);" << endl;
      o << "    }" << endl;
      o << "  }" << endl;
    }
    if (m.isVector && !m.isBaseType)
      o << "  };" << endl;
  }
  o << "}" << endl << endl;
}

void WriteTableInputFunctions(std::ostream &o, const Table &t)
{
  o << "void Read(std::istream &s, " << t.name << " &v) {" << endl;
  for (const auto &m : t.member)
  {
    auto n = "v." + m.name;
    if (m.isVector && !m.isBaseType)
    {
      n = "entry";
      o << "  ";
      WriteType(o, m) << "::size_type " << m.name << "_size{0};" << endl;
      o << "  s >> " << m.name << "_size;" << endl;
      o << "  v." << m.name << ".resize(" << m.name << "_size);" << endl;
      o << "  for (auto &" << n << " : v." << m.name << ") {" << endl;
      o << "  ";
    }
    if (m.pointer == Pointer::Plain || m.isBaseType)
    {
      o << "  Read(s, " << n << ");" << endl;
    }
    else if (m.pointer == Pointer::Unique)
    {
      o << "  {" << endl;
      o << "    char ref = 0;" << endl;
      o << "    s.read(&ref, 1);" << endl;
      o << "    if (ref == '\\x1') {" << endl;
      o << "      " << n << " = std::make_unique<" << m.type << ">();" << endl;
      o << "      Read(s, *" << n << ");" << endl;
      o << "    }" << endl;
      o << "  }" << endl;
    }
    else
    {
      o << "  {" << endl;
      o << "    char ref = 0;" << endl;
      o << "    s.read(&ref, 1);" << endl;
      o << "    if (ref == '\\x1') {" << endl;
      o << "      auto t = std::make_shared<" << m.type << ">();" << endl;
      o << "      Read(s, *t);" << endl;
      o << "      " << m.type << "_references_.push_back(t);" << endl;
      o << "      " << n << " = t;" << endl;
      o << "    } else if (ref == '\\x2') {" << endl;
      o << "      unsigned int index = 0;" << endl;
      o << "      Read(s, index);" << endl;
      o << "      " << n << " = " << m.type << "_references_[index - 1];" << endl;
      o << "    }" << endl;
      o << "  }" << endl;
    }
    if (m.isVector && !m.isBaseType)
      o << "  };" << endl;
  }
  o << "}" << endl << endl;
}

void WriteTableCompareFunctions(std::ostream &o, const Table &t)
{
  o << "bool operator==(const " << t.name << "&l, const " << t.name << "&r) {" << endl;
  o << "  return ";
  bool first = true;
  for (const auto &m : t.member)
  {
    const auto suf = (m.pointer == Pointer::Weak) ? ".lock()" : "";
    o << endl << "    " << (first ? "" : "&& ") << "l." << m.name << suf << " == r." << m.name << suf;
    first = false;
  }
  o << ";" << endl;
  o << "}" << endl << endl;

  o << "bool operator!=(const " << t.name << "&l, const " << t.name << "&r) {" << endl;
  o << "  return ";
  first = true;
  for (const auto &m : t.member)
  {
    const auto suf = (m.pointer == Pointer::Weak) ? ".lock()" : "";
    o << endl << "    " << (first ? "" : "|| ") << "l." << m.name << suf << " != r." << m.name << suf;
    first = false;
  }
  o << ";" << endl;
  o << "}" << endl << endl;
}

void WriteTableDeclarations(std::ostream &o, const Package &p)
{
  for (const auto &t : p.tables)
    WriteTableDeclaration(o, t, p.root_type);
  for (const auto &t : p.tables)
    WriteTableCompareFunctions(o, t);
}

void WriteTables(std::ostream &o, const vector<Table> &tables)
{
  for (const auto &t : tables)
  {
    WriteTableOutputFunctions(o, t);
    WriteTableInputFunctions(o, t);
  }
}

void WriteBaseIO(std::ostream &o, const Package &p)
{
  o << "void Write" << p.root_type << "(std::ostream &o, const " << p.root_type << " &v) {" << endl;
  for (const auto &t : p.tables)
    o << "  " << t.name << "_count_ = 0;" << endl;

  o << endl << "  o.write(\"CORE\", 4);" << endl;
  o << "  o.write(\"" << p.version << "\", " << p.version.size() << ");" << endl;
  o << "  Write(o, v);" << endl;
  o << "}" << endl << endl;

  o << "bool Read" << p.root_type << "(std::istream &i, " << p.root_type << " &v) {" << endl;

  for (const auto &t : p.tables)
    o << "  " << t.name << "_references_.clear();" << endl;

  o << endl << "  std::string marker(\"0000\");" << endl;
  o << "  i.read(&marker[0], 4);" << endl;
  o << "  if (marker != \"CORE\")" << endl;
  o << "    return false;" << endl;

  o << "  std::string version(" << p.version.size() << ", '_');" << endl;
  o << "  i.read(&version[0], " << p.version.size() << ");" << endl;
  o << "  if (version != \"" << p.version << "\")" << endl;
  o << "    return false;" << endl;

  o << "  Read(i, v);" << endl;
  o << "  return true;" << endl;
  o << "}" << endl << endl;
}

void WriteCppCode(std::ostream &o, const Package &p)
{
  o << "#pragma once" << endl << endl;
  o << "#include <vector>" << endl;
  o << "#include <string>" << endl;
  o << "#include <ostream>" << endl;
  o << "#include <istream>" << endl;
  o << "#include <memory>" << endl;
  o << "#include <type_traits>" << endl;
  o << "#include <array>" << endl << endl;

  WriteNameSpaceBegin(o, p.path);

  o << "template<typename T>" << endl;
  o << "struct AlwaysFalse : std::false_type {};" << endl;

  WriteForwardDeclarations(o, p);
  WriteEnumDeclarations(o, p.enums);
  WriteTableDeclarations(o, p);

  for (const auto &e : p.enums)
    WriteEnumFunctions(o, e);

  o << "struct " << p.root_type << "_io {" << endl;
  o << "private:" << endl;

  WriteBaseTypecIoFnuctions(o, p);

  WriteTables(o, p.tables);

  for (const auto &t : p.tables)
    o << "  unsigned int " << t.name << "_count_{0};" << endl;
  for (const auto &t : p.tables)
    o << "  std::vector<std::shared_ptr<" << t.name << ">> " << t.name << "_references_;" << endl;

  o << endl << "public:" << endl;
  WriteBaseIO(o, p);

  o << "};" << endl;

  WriteNameSpaceEnd(o, p.path);
}
