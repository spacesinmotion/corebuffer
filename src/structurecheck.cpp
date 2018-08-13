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
