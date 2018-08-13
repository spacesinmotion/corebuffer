#ifndef STURCTURECHECK_H
#define STURCTURECHECK_H

#include "fileerror.h"

#include <unordered_set>
#include <vector>

struct Package;

class StructureCheck
{
public:
  StructureCheck(Package &p);

  const std::vector<FileError> &check();

private:
  void checkDuplicateTables();
  void checkEmptyTables();

  void checkDuplicateEnums();
  void checkEmptyEnums();

  void checkPackage();
  void checkRootType();

private:
  Package &_package;
  std::vector<FileError> _errors;

  std::unordered_set<std::string> _tableNames;
  std::unordered_set<std::string> _enumNames;
};

#endif  // STURCTURECHECK_H
