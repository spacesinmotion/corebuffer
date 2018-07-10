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
  o << "};" << endl << endl;
}

void WriteEnums(std::ostream &o, const vector<Enum> &enums)
{
  for (const auto &e : enums)
  {
    WriteEnumDeclaration(o, e);
    WriteEnumFunctions(o, e);
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

#include <iomanip>

void WriteTableDeclarations(std::ostream &o, const vector<Table> &tables)
{
  for (const auto &t : tables)
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
    o << "};" << endl << endl;
  }
}

void WriteCppCode(std::ostream &o, const Package &p)
{
  o << "#pragma once" << endl << endl;
  o << "#include <vector>" << endl;
  o << "#include <string>" << endl;
  o << "#include <array>" << endl << endl;

  WriteNameSpaceBegin(o, p.path);

  WriteForwardDeclarations(o, p);
  WriteEnums(o, p.enums);
  WriteTableDeclarations(o, p.tables);

  WriteNameSpaceEnd(o, p.path);
}
