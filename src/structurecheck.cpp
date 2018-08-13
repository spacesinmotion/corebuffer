#include "structurecheck.h"
#include "package.h"

using namespace std;

StructureCheck::StructureCheck(Package &p) : _package(p) {}

const std::vector<FileError> &StructureCheck::check()
{
  checkDuplicateTables();
  checkEmptyTables();

  checkDuplicateEnums();
  checkEmptyEnums();

  checkPackage();
  checkRootType();

  return _errors;
}

void StructureCheck::checkDuplicateTables()
{
  for (const auto &t : _package.tables)
    if (!_tableNames.emplace(t.name).second)
      _errors.emplace_back("Duplicate table '" + t.name + "'.", t.location);
}

void StructureCheck::checkEmptyTables()
{
  for (const auto &t : _package.tables)
    if (t.member.empty())
      _errors.emplace_back("Empty table '" + t.name + "'.", t.location);
}

void StructureCheck::checkDuplicateEnums()
{
  for (const auto &e : _package.enums)
    if (!_enumNames.emplace(e.name).second)
      _errors.emplace_back("Duplicate enum '" + e.name + "'.", e.location);

  for (const auto &e : _package.enums)
    if (_tableNames.find(e.name) != _tableNames.end())
      _errors.emplace_back("enum '" + e.name + "' already defined as table.", e.location);
}

void StructureCheck::checkEmptyEnums()
{
  for (const auto &e : _package.enums)
    if (e.entries.empty())
      _errors.emplace_back("Empty enum '" + e.name + "'.", e.location);
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

  if (_tableNames.find(_package.root_type.value) == _tableNames.end())
  {
    if (_enumNames.find(_package.root_type.value) == _enumNames.end())
      _errors.emplace_back("Unknown root_type '" + _package.root_type.value + "'.", _package.root_type.location);
    else
      _errors.emplace_back("root_type '" + _package.root_type.value + "' is an enum.", _package.root_type.location);
  }
}
