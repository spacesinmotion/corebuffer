#include "structurecheck.h"
#include "package.h"

#include <unordered_set>

using namespace std;

StructureCheck::StructureCheck(Package &p) : _package(p) {}

const std::vector<FileError> &StructureCheck::check()
{
  checkDuplicateTables();
  checkEmptyTables();

  checkDuplicateEnums();
  checkEmptyEnums();

  return _errors;
}

void StructureCheck::checkDuplicateTables()
{
  unordered_set<string> tableNames;

  for (const auto &t : _package.tables)
    if (!tableNames.emplace(t.name).second)
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
  unordered_set<string> enumNames;
  for (const auto &e : _package.enums)
    if (!enumNames.emplace(e.name).second)
      _errors.emplace_back("Duplicate enum '" + e.name + "'.", e.location);
}

void StructureCheck::checkEmptyEnums()
{
  for (const auto &e : _package.enums)
    if (e.entries.empty())
      _errors.emplace_back("Empty enum '" + e.name + "'.", e.location);
}
