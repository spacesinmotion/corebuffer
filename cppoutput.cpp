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

  for (const auto &t : p.tables)
  {
    o << "static void Write(std::ostream &, const " << t.name << " &);" << endl;
    o << "static void Read(std::istream &, " << t.name << " &);" << endl << endl;
  }
}

void WriteDefaultOutputFunctions(std::ostream &o)
{
  o << "inline void Write(std::ostream &o, std::int8_t i) {" << endl;
  o << "  o.write(\"\\x1\", 1);" << endl;
  o << "  o.write(reinterpret_cast<const char *>(&i), 1);" << endl;
  o << "}" << endl << endl;

  o << "inline void Write(std::ostream &o, std::int16_t i) {" << endl;
  o << "  if (std::numeric_limits<std::int8_t>::lowest() < i && i < std::numeric_limits<std::int8_t>::max())" << endl;
  o << "    Write(o, std::int8_t(i));" << endl;
  o << "  else {" << endl;
  o << "    o.write(\"\\x2\", 1);" << endl;
  o << "    o.write(reinterpret_cast<const char *>(&i), 2);" << endl;
  o << "  }" << endl;
  o << "}" << endl << endl;

  o << "inline void Write(std::ostream &o, std::int32_t i) {" << endl;
  o << "  if (std::numeric_limits<std::int16_t>::lowest() < i && i < std::numeric_limits<std::int16_t>::max())" << endl;
  o << "    Write(o, std::int16_t(i));" << endl;
  o << "  else {" << endl;
  o << "    o.write(\"\\x4\", 1);" << endl;
  o << "    o.write(reinterpret_cast<const char *>(&i), 4);" << endl;
  o << "  }" << endl;
  o << "}" << endl << endl;

  o << "inline void Write(std::ostream &o, std::int64_t i) {" << endl;
  o << "  if (std::numeric_limits<std::int32_t>::lowest() < i && i < std::numeric_limits<std::int32_t>::max())" << endl;
  o << "    Write(o, std::int32_t(i));" << endl;
  o << "  else {" << endl;
  o << "    o.write(\"\\x8\", 1);" << endl;
  o << "    o.write(reinterpret_cast<const char *>(&i), 8);" << endl;
  o << "  }" << endl;
  o << "}" << endl << endl;

  o << "inline void Write(std::ostream &o, std::uint8_t i) {" << endl;
  o << "  o.write(\"\\x1\", 1);" << endl;
  o << "  o.write(reinterpret_cast<const char *>(&i), 1);" << endl;
  o << "}" << endl << endl;

  o << "inline void Write(std::ostream &o, std::uint16_t i) {" << endl;
  o << "  if (i < std::numeric_limits<std::uint8_t>::max())" << endl;
  o << "    Write(o, std::uint8_t(i));" << endl;
  o << "  else {" << endl;
  o << "    o.write(\"\\x2\", 1);" << endl;
  o << "    o.write(reinterpret_cast<const char *>(&i), 2);" << endl;
  o << "  }" << endl;
  o << "}" << endl << endl;

  o << "inline void Write(std::ostream &o, std::uint32_t i) {" << endl;
  o << "  if (i < std::numeric_limits<std::uint16_t>::max())" << endl;
  o << "    Write(o, std::uint16_t(i));" << endl;
  o << "  else {" << endl;
  o << "    o.write(\"\\x4\", 1);" << endl;
  o << "    o.write(reinterpret_cast<const char *>(&i), 4);" << endl;
  o << "  }" << endl;
  o << "}" << endl << endl;

  o << "inline void Write(std::ostream &o, std::uint64_t i) {" << endl;
  o << "  if (i < std::numeric_limits<std::uint32_t>::max())" << endl;
  o << "    Write(o, std::uint32_t(i));" << endl;
  o << "  else {" << endl;
  o << "    o.write(\"\\x8\", 1);" << endl;
  o << "    o.write(reinterpret_cast<const char *>(&i), 8);" << endl;
  o << "  }" << endl;
  o << "}" << endl << endl;

  o << "inline void Write(std::ostream &o, float f) {" << endl;
  o << "  o.write(\"\\x4\", 1);" << endl;
  o << "  o.write(reinterpret_cast<const char *>(&f), 4);" << endl;
  o << "}" << endl << endl;

  o << "inline void Write(std::ostream &o, double f) {" << endl;
  o << "  if (f < double(std::numeric_limits<float>::max()) && f > double(std::numeric_limits<float>::lowest()))"
    << endl;
  o << "    Write(o, float(f));" << endl;
  o << "  else {" << endl;
  o << "    o.write(\"\\x8\", 1);" << endl;
  o << "    o.write(reinterpret_cast<const char *>(&f), 8);" << endl;
  o << "  }" << endl;
  o << "}" << endl << endl;

  o << "inline void Write(std::ostream &o, const std::string &t) {" << endl;
  o << "  Write(o, t.size());" << endl;
  o << "  o.write(t.c_str(), t.size());" << endl;
  o << "}" << endl << endl;

  o << R"(
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
)";
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
  o << "};" << endl << endl;
}

void WriteEnumOutputFunctions(std::ostream &o, const Enum &e)
{
  o << "inline void Write(std::ostream &o, const " << e.name << " &v) {" << endl;
  o << "  Write(o, static_cast<unsigned int>(v));" << endl;
  o << "}" << endl << endl;
}

void WriteEnumInputFunctions(std::ostream &o, const Enum &e)
{
  o << "inline void Read(std::istream &s, " << e.name << " &v) {" << endl;
  o << "  unsigned int t{0};" << endl;
  o << "  Read(s, t);" << endl;
  o << "  v = static_cast<" << e.name << ">(t);" << endl;
  o << "}" << endl << endl;
}

void WriteEnumDeclarations(std::ostream &o, const vector<Enum> &enums)
{
  for (const auto &e : enums)
  {
    WriteEnumDeclaration(o, e);
  }
}

void WriteEnumIOFunctions(std::ostream &o, const vector<Enum> &enums)
{
  for (const auto &e : enums)
  {
    WriteEnumOutputFunctions(o, e);
    WriteEnumInputFunctions(o, e);
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
    if (m.isVector)
    {
      n = "entry";
      o << "  Write(o, v." << m.name << ".size());" << endl;
      o << "  for (const auto &" << n << " : v." << m.name << ") {" << endl;
      o << "  ";
    }
    if (m.pointer == Pointer::Plain)
      o << "  Write(o, " << n << ");" << endl;
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
      o << "  const auto t = " << n << (m.pointer == Pointer::Weak ? ".lock()" : "") << ";" << endl;
      o << "  if (!t) {" << endl;
      o << "    o.write(\"\\x0\", 1);" << endl;
      o << "  } else if (t->io_counter_== 0) {" << endl;
      o << "    o.write(\"\\x1\", 1);" << endl;
      o << "    Write(o, *t);" << endl;
      o << "    t->io_counter_ = ++" << m.type << "_count_;" << endl;
      o << "  } else {" << endl;
      o << "    o.write(\"\\x2\", 1);" << endl;
      o << "    Write(o, t->io_counter_);" << endl;
      o << "  }" << endl;
    }
    if (m.isVector)
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
    if (m.isVector)
    {
      n = "entry";
      o << "  ";
      WriteType(o, m) << "::size_type " << m.name << "_size{0};" << endl;
      o << "  Read(s, " << m.name << "_size);" << endl;
      o << "  v." << m.name << ".resize(" << m.name << "_size);" << endl;
      o << "  for (auto &" << n << " : v." << m.name << ") {" << endl;
      o << "  ";
    }
    if (m.pointer == Pointer::Plain)
      o << "  Read(s, " << n << ");" << endl;
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
    if (m.isVector)
      o << "  };" << endl;
  }
  o << "}" << endl << endl;
}

void WriteTableDeclarations(std::ostream &o, const Package &p)
{
  for (const auto &t : p.tables)
    WriteTableDeclaration(o, t, p.root_type);
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
  o << "  Write(o, \"" << p.version << "\");" << endl;
  o << "  Write(o, v);" << endl;
  o << "}" << endl << endl;

  o << "bool Read" << p.root_type << "(std::istream &i, " << p.root_type << " &v) {" << endl;

  for (const auto &t : p.tables)
    o << "  " << t.name << "_references_.clear();" << endl;

  o << endl << "  std::string marker(\"0000\");" << endl;
  o << "  i.read(&marker[0], 4);" << endl;
  o << "  if (marker != \"CORE\")" << endl;
  o << "    return false;" << endl;

  o << "  std::string version;" << endl;
  o << "  Read(i, version);" << endl;
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
  o << "#include <array>" << endl << endl;

  WriteNameSpaceBegin(o, p.path);

  WriteForwardDeclarations(o, p);
  WriteEnumDeclarations(o, p.enums);
  WriteTableDeclarations(o, p);

  for (const auto &e : p.enums)
    WriteEnumFunctions(o, e);

  o << "struct " << p.root_type << "_io {" << endl;
  o << "private:" << endl;
  WriteDefaultOutputFunctions(o);
  WriteEnumIOFunctions(o, p.enums);
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
