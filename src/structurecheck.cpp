#include "structurecheck.h"
#include "package.h"

using namespace std;

StructureCheck::StructureCheck(Package &p) : _package(p) {}

const std::vector<FileError> &StructureCheck::check()
{
  initNameSets();
  checkTables();
  checkEnums();
  checkPackage();
  checkRootType();
  checkBaseTypePointer();
  checkEnumTypePointer();

  return _errors;
}

void StructureCheck::initNameSets()
{
  checkDuplicateTables();
  checkDuplicateEnums();
}

void StructureCheck::checkDuplicateTables()
{
  for (const auto &t : _package.tables)
    if (!_tableNames.emplace(t.name).second)
      _errors.emplace_back("Duplicate table '" + t.name + "'.", t.location);
}

void StructureCheck::checkDuplicateEnums()
{
  for (const auto &e : _package.enums)
    if (!_enumNames.emplace(e.name).second)
      _errors.emplace_back("Duplicate enum '" + e.name + "'.", e.location);

  for (const auto &e : _package.enums)
    if (tableExists(e.name))
      _errors.emplace_back("enum '" + e.name + "' already defined as table.", e.location);
}

void StructureCheck::checkTables()
{
  checkEmptyTables();
  for (const auto &t : _package.tables)
  {
    checkDuplicateTableMembers(t);
    checkMemberTypes(t);
    checksMethods(t);
  }
}

void StructureCheck::checkEmptyTables()
{
  for (const auto &t : _package.tables)
    if (t.member.empty())
      _errors.emplace_back("Empty table '" + t.name + "'.", t.location);
}

void StructureCheck::checkDuplicateTableMembers(const Table &t)
{
  unordered_set<string> names;
  for (const auto &m : t.member)
    if (!names.emplace(m.name).second)
      _errors.emplace_back("table member '" + m.name + "' already defined for '" + t.name + "'.", m.location);
}

void StructureCheck::checkMemberTypes(const Table &t)
{
  for (const auto &m : t.member)
    if (!isBaseType(m.type) && !tableExists(m.type) && !enumExists(m.type))
      _errors.emplace_back("Unknown type '" + m.type + "'.", m.location);
}

void StructureCheck::checksMethods(const Table &t)
{
  checkIfMethodsExist(t);
  checkMethodForParameter(t);
}

void StructureCheck::checkIfMethodsExist(const Table &t)
{
  static const unordered_set<string> knownMethods{"init"};
  for (const auto &m : t.methods)
    if (knownMethods.find(m.name) == knownMethods.end())
      _errors.emplace_back("unknown method '" + m.name + "' in table '" + t.name + "'.", m.location);
}

void StructureCheck::checkMethodForParameter(const Table &t)
{
  if (t.methods.empty())
    return;

  unordered_set<string> methodsDefinedByName;
  unordered_set<string> methodsDefinedByType;
  unordered_set<string> member;
  for (const auto &m : t.member)
    member.emplace(m.name);

  for (const auto &m : t.methods)
  {
    if (m.parameter.empty())
    {
      _errors.emplace_back("No parameter defined for method '" + m.name + "'.", m.location);
      continue;
    }

    unordered_set<string> parameterSet;
    for (const auto &p : m.parameter)
      if (!parameterSet.emplace(p.name).second)
        _errors.emplace_back("duplicate parameter '" + p.name + "' in method '" + m.name + "'.", p.location);

    bool unknownParameter = false;
    for (const auto &p : m.parameter)
    {
      if (member.find(p.name) != member.end())
        continue;
      _errors.emplace_back("unknown parameter '" + p.name + "' in method '" + m.name + "'.", p.location);
      unknownParameter = true;
    }
    if (unknownParameter)
      continue;

    const auto definedByName = !methodsDefinedByName.emplace(methodParameterKey(m)).second;
    const auto definedByType = !methodsDefinedByType.emplace(methodTypeParameterKey(t, m)).second;
    if (definedByName)
      _errors.emplace_back("method '" + m.name + "' already defined.", m.location);
    else if (!definedByName && definedByType)
      _errors.emplace_back("method '" + m.name + "' already defined implicitly by parameter.", m.location);
  }
}

void StructureCheck::checkEnums()
{
  checkEmptyEnums();
  for (const auto &e : _package.enums)
    checkDuplicateEnumEntries(e);
}

void StructureCheck::checkEmptyEnums()
{
  for (const auto &e : _package.enums)
    if (e.entries.empty())
      _errors.emplace_back("Empty enum '" + e.name + "'.", e.location);
}

void StructureCheck::checkDuplicateEnumEntries(const Enum &e)
{
  unordered_set<string> names;
  for (const auto &entry : e.entries)
    if (!names.emplace(entry.name).second)
      _errors.emplace_back("enum value '" + entry.name + "' already defined for '" + e.name + "'.", entry.location);
}

void StructureCheck::checkPackage()
{
  if (_package.path.value.empty())
  {
    _errors.emplace_back("No package defined.", _package.path.location);
    return;
  }
}

void StructureCheck::checkRootType()
{
  if (_package.root_type.value.empty())
  {
    _errors.emplace_back("No root_type defined.", _package.root_type.location);
    return;
  }

  if (!tableExists(_package.root_type.value))
  {
    if (!enumExists(_package.root_type.value))
      _errors.emplace_back("Unknown root_type '" + _package.root_type.value + "'.", _package.root_type.location);
    else
      _errors.emplace_back("root_type '" + _package.root_type.value + "' is an enum.", _package.root_type.location);
  }
}

void StructureCheck::checkBaseTypePointer()
{
  for (const auto &t : _package.tables)
    for (const auto &m : t.member)
      if (m.isBaseType && m.pointer != Pointer::Plain)
        _errors.emplace_back("base types cannot be pointer.", m.location);
}

void StructureCheck::checkEnumTypePointer()
{
  for (const auto &t : _package.tables)
    for (const auto &m : t.member)
      if (!m.isBaseType && m.pointer != Pointer::Plain && enumExists(m.type))
        _errors.emplace_back("enums cannot be pointer.", m.location);
}

bool StructureCheck::isBaseType(const string &name)
{
  for (const auto &bt : _package.baseTypes)
    if (bt.name == name)
      return true;
  return false;
}

bool StructureCheck::tableExists(const string &name)
{
  return _tableNames.find(name) != _tableNames.end();
}

bool StructureCheck::enumExists(const string &name)
{
  return _enumNames.find(name) != _enumNames.end();
}

string StructureCheck::methodParameterKey(const Method &m)
{
  auto n = m.name;
  for (const auto &p : m.parameter)
    n += p.name;
  return n;
}

string StructureCheck::methodTypeParameterKey(const Table &t, const Method &m)
{
  auto n = m.name;
  for (const auto &p : m.parameter)
  {
    for (const auto &b : t.member)
    {
      if (p.name == b.name)
      {
        n += b.type;
        break;
      }
    }
  }
  return n;
}
