#ifndef STURCTURECHECK_H
#define STURCTURECHECK_H

#include "fileerror.h"

#include <unordered_set>
#include <vector>

struct Package;
struct Table;
struct Enum;

class StructureCheck
{
public:
  StructureCheck(Package &p);

  const std::vector<FileError> &check();

private:
  void initNameSets();
  void checkDuplicateTables();
  void checkDuplicateEnums();

  void checkTables();
  void checkEmptyTables();
  void checkDuplicateTableMembers(const Table &t);
  void checkMemberTypes(const Table &t);

  void checkEnums();
  void checkEmptyEnums();
  void checkDuplicateEnumEntries(const Enum &e);

  void checkPackage();
  void checkRootType();

  void checkBaseTypePointer();
  void checkEnumTypePointer();

  bool isBaseType(const std::string &name);
  bool tableExists(const std::string &name);
  bool enumExists(const std::string &name);

private:
  Package &_package;
  std::vector<FileError> _errors;

  std::unordered_set<std::string> _tableNames;
  std::unordered_set<std::string> _enumNames;
};

#endif  // STURCTURECHECK_H
