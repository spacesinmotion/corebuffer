#include "structurecheck.h"
#include "package.h"

#include <sstream>

using namespace std;

StructureCheck::StructureCheck(Package &p) : _package(p) {}

const std::vector<FileError> &StructureCheck::check()
{
  initNameSets();
  checkTables();
  checkEnums();
  checkFlags();
  checkUnions();
  checkPackage();
  checkRootType();
  checkBaseTypePointer();
  checkEnumTypePointer();
  checkFlagTypePointer();
  checkDefaultValues();

  return _errors;
}

void StructureCheck::initNameSets()
{
  checkDuplicateTables();
  checkDuplicateEnums();
  checkDuplicateFlags();
  checkDuplicateUnions();
}

void StructureCheck::checkDuplicateTables()
{
  for (const auto &t : _package.types)
    if (t.is_Table() && !_tableNames.emplace(t.as_Table().name).second)
      _errors.emplace_back("table '" + t.as_Table().name + "' already defined.", t.as_Table().location);
}

void StructureCheck::checkDuplicateEnums()
{
  for (const auto &e : _package.types)
    if (e.is_Enum() && (!_enumNames.emplace(e.as_Enum().name).second || tableExists(e.as_Enum().name)))
      _errors.emplace_back("enum '" + e.as_Enum().name + "' already defined.", e.as_Enum().location);
}

void StructureCheck::checkDuplicateFlags()
{
  for (const auto &e : _package.types)
    if (e.is_Flag() &&
        (!_flagNames.emplace(e.as_Flag().name).second || tableExists(e.as_Flag().name) || enumExists(e.as_Flag().name)))
      _errors.emplace_back("flag '" + e.as_Flag().name + "' already defined.", e.as_Flag().location);
}

void StructureCheck::checkDuplicateUnions()
{
  for (const auto &u : _package.types)
    if (u.is_Union() && (!_unionNames.emplace(u.as_Union().name).second || tableExists(u.as_Union().name) ||
                         enumExists(u.as_Union().name) || flagExists(u.as_Union().name)))
      _errors.emplace_back("union '" + u.as_Union().name + "' already defined.", u.as_Union().location);
}

void StructureCheck::checkTables()
{
  checkEmptyTables();
  for (const auto &t : _package.types)
  {
    if (!t.is_Table())
      continue;

    checkDuplicateTableMembers(t.as_Table());
    checkMemberTypes(t.as_Table());
    checksMethods(t.as_Table());
  }
}

void StructureCheck::checkEmptyTables()
{
  for (const auto &t : _package.types)
    if (t.is_Table() && t.as_Table().member.empty())
      _errors.emplace_back("Empty table '" + t.as_Table().name + "'.", t.as_Table().location);
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
    if (!isValidType(m.type))
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
  for (const auto &e : _package.types)
    if (e.is_Enum())
      checkDuplicateEnumEntries(e.as_Enum());
}

void StructureCheck::checkEmptyEnums()
{
  for (const auto &e : _package.types)
    if (e.is_Enum() && e.as_Enum().entries.empty())
      _errors.emplace_back("Empty enum '" + e.as_Enum().name + "'.", e.as_Enum().location);
}

void StructureCheck::checkDuplicateEnumEntries(const Enum &e)
{
  unordered_set<string> names;
  for (const auto &entry : e.entries)
    if (!names.emplace(entry.name).second)
      _errors.emplace_back("enum value '" + entry.name + "' already defined for '" + e.name + "'.", entry.location);
}

void StructureCheck::checkFlags()
{
  checkEmptyFlags();
  for (const auto &f : _package.types)
    if (f.is_Flag())
      checkDuplicateFlagEntries(f.as_Flag());
}

void StructureCheck::checkEmptyFlags()
{
  for (const auto &e : _package.types)
    if (e.is_Flag() && e.as_Flag().entries.empty())
      _errors.emplace_back("Empty flag '" + e.as_Flag().name + "'.", e.as_Flag().location);
}

void StructureCheck::checkDuplicateFlagEntries(const Flag &f)
{
  unordered_set<string> names;
  for (const auto &entry : f.entries)
    if (!names.emplace(entry.value).second)
      _errors.emplace_back("flag value '" + entry.value + "' already defined for '" + f.name + "'.", entry.location);
}

void StructureCheck::checkUnions()
{
  checkEmptyUnions();
  for (const auto &u : _package.types)
  {
    if (!u.is_Union())
      continue;
    checkDuplicateUnionEntries(u.as_Union());
    checkTableReferences(u.as_Union());
  }
}

void StructureCheck::checkEmptyUnions()
{
  for (const auto &u : _package.types)
    if (u.is_Union() && u.as_Union().tables.empty())
      _errors.emplace_back("Empty union '" + u.as_Union().name + "'.", u.as_Union().location);
}

void StructureCheck::checkDuplicateUnionEntries(const Union &u)
{
  unordered_set<string> names;
  for (const auto &t : u.tables)
    if (!names.emplace(t.value).second)
      _errors.emplace_back("union entry '" + t.value + "' already defined for '" + u.name + "'.", t.location);
}

void StructureCheck::checkTableReferences(const Union &u)
{
  for (const auto &t : u.tables)
    if (!tableExists(t.value))
      _errors.emplace_back("Unknown table '" + t.value + "'.", t.location);
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
  for (const auto &t : _package.types)
    if (t.is_Table())
      for (const auto &m : t.as_Table().member)
        if (m.isBaseType && m.pointer != Pointer::Plain)
          _errors.emplace_back("base types cannot be pointer.", m.location);
}

void StructureCheck::checkEnumTypePointer()
{
  for (const auto &t : _package.types)
    if (t.is_Table())
      for (const auto &m : t.as_Table().member)
        if (!m.isBaseType && m.pointer != Pointer::Plain && enumExists(m.type))
          _errors.emplace_back("enums cannot be pointer.", m.location);
}

void StructureCheck::checkFlagTypePointer()
{
  for (const auto &t : _package.types)
    if (t.is_Table())
      for (const auto &m : t.as_Table().member)
        if (!m.isBaseType && m.pointer != Pointer::Plain && flagExists(m.type))
          _errors.emplace_back("flags cannot be pointer.", m.location);
}

bool StructureCheck::isFloat(const string &number)
{
  istringstream iss(number);
  double f;
  iss >> noskipws >> f;
  return iss.eof() && !iss.fail();
}

bool StructureCheck::isIntegral(const string &number)
{
  istringstream iss(number);
  long long i;
  iss >> noskipws >> i;
  return iss.eof() && !iss.fail();
}

bool StructureCheck::isString(const string &text)
{
  return !text.empty() && text.front() == '\"' && text.back() == '\"';
}

bool StructureCheck::isBool(const string &text)
{
  return text == "true" || text == "false";
}

void StructureCheck::checkDefaultValues()
{
  checkDefaultValuesOfBaseTypes();
  checkDefaultValuesOfEnums();
  checkDefaultValuesOfFlags();
  checkDefaultValuesOfTables();
}

void StructureCheck::checkDefaultValuesOfBaseTypes()
{
  for (const auto &t : _package.types)
  {
    if (!t.is_Table())
      continue;
    for (const auto &m : t.as_Table().member)
    {
      if (!m.isBaseType || m.defaultValue.location.isAtStart())
        continue;

      if (m.type.find("std::uint") == 0 || m.type.find("std::int") == 0)
      {
        if (!isIntegral(m.defaultValue.value))
          _errors.emplace_back("only integral values can be assigned here.", m.defaultValue.location);
      }
      else if (m.type == "float" || m.type == "double")
      {
        if (!isFloat(m.defaultValue.value))
          _errors.emplace_back("only floating point values can be assigned here.", m.defaultValue.location);
      }
      else if (m.type == "bool")
      {
        if (!isBool(m.defaultValue.value))
          _errors.emplace_back("only boolean values can be assigned here.", m.defaultValue.location);
      }
      else if (m.type == "std::string" && !isString(m.defaultValue.value))
        _errors.emplace_back("only string values can be assigned here.", m.defaultValue.location);
    }
  }
}

void StructureCheck::checkDefaultValuesOfEnums()
{
  for (const auto &t : _package.types)
  {
    if (!t.is_Table())
      continue;
    for (const auto &m : t.as_Table().member)
    {
      if (!enumExists(m.type) || m.defaultValue.value.empty())
        continue;

      if (isFloat(m.defaultValue.value) || isString(m.defaultValue.value) || isBool(m.defaultValue.value))
        _errors.emplace_back("only values of '" + m.type + "' can be assigned here.", m.defaultValue.location);
      else if (m.defaultValue.value.find(m.type) == string::npos)
        _errors.emplace_back("unknown value '" + m.defaultValue.value + "' for enum '" + m.type + "'.",
                             m.defaultValue.location);
    }
  }
}

void StructureCheck::checkDefaultValuesOfFlags()
{
  for (const auto &t : _package.types)
  {
    if (!t.is_Table())
      continue;
    for (const auto &m : t.as_Table().member)
    {
      if (!flagExists(m.type) || m.defaultValue.value.empty())
        continue;

      if (isFloat(m.defaultValue.value) || isString(m.defaultValue.value) || isBool(m.defaultValue.value))
        _errors.emplace_back("only values of '" + m.type + "' can be assigned here.", m.defaultValue.location);
      else if (m.defaultValue.value.find(m.type) == string::npos)
        _errors.emplace_back("unknown value '" + m.defaultValue.value + "' for flag '" + m.type + "'.",
                             m.defaultValue.location);
    }
  }
}

void StructureCheck::checkDefaultValuesOfTables()
{
  for (const auto &t : _package.types)
  {
    if (!t.is_Table())
      continue;
    for (const auto &m : t.as_Table().member)
    {
      if (m.defaultValue.value.empty() || enumExists(m.type))
        continue;

      if (m.pointer != Pointer::Plain)
        _errors.emplace_back("default values for pointer are not supported.", m.defaultValue.location);
      else if (m.isVector)
        _errors.emplace_back("default values for vectors are not supported.", m.defaultValue.location);
      else if (tableExists(m.type))
        _errors.emplace_back("default values for tables are not supported.", m.defaultValue.location);
    }
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

bool StructureCheck::flagExists(const string &name)
{
  return _flagNames.find(name) != _flagNames.end();
}

bool StructureCheck::unionExists(const string &name)
{
  return _unionNames.find(name) != _unionNames.end();
}

bool StructureCheck::isValidType(const string &name)
{
  return isBaseType(name) || tableExists(name) || unionExists(name) || enumExists(name) || flagExists(name);
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
