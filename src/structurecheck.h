#ifndef STURCTURECHECK_H
#define STURCTURECHECK_H

#include "fileerror.h"

#include <unordered_set>
#include <vector>

struct Package;
struct Table;
struct Enum;
struct Union;
struct Method;

class StructureCheck
{
public:
  StructureCheck(Package &p);

  const std::vector<FileError> &check();

private:
  void initNameSets();
  void checkDuplicateTables();
  void checkDuplicateEnums();
  void checkDuplicateUnions();

  void checkTables();
  void checkEmptyTables();
  void checkDuplicateTableMembers(const Table &t);
  void checkMemberTypes(const Table &t);

  void checksMethods(const Table &t);
  void checkIfMethodsExist(const Table &t);
  void checkMethodForParameter(const Table &t);

  void checkEnums();
  void checkEmptyEnums();
  void checkDuplicateEnumEntries(const Enum &e);

  void checkUnions();
  void checkEmptyUnions();
  void checkDuplicateUnionEntries(const Union &u);
  void checkTableReferences(const Union &u);

  void checkPackage();
  void checkRootType();

  void checkBaseTypePointer();
  void checkEnumTypePointer();

  void checkDefaultValues();
  void checkDefaultValuesOfBaseTypes();
  void checkDefaultValuesOfEnums();
  void checkDefaultValuesOfTables();

  bool isBaseType(const std::string &name);
  bool tableExists(const std::string &name);
  bool enumExists(const std::string &name);

  std::string methodParameterKey(const Method &m);
  std::string methodTypeParameterKey(const Table &t, const Method &m);

  bool isFloat(const std::string &number);
  bool isIntegral(const std::string &number);
  bool isString(const std::string &text);
  bool isBool(const std::string &text);

private:
  Package &_package;
  std::vector<FileError> _errors;

  std::unordered_set<std::string> _tableNames;
  std::unordered_set<std::string> _enumNames;
  std::unordered_set<std::string> _unionNames;
};

#endif  // STURCTURECHECK_H
