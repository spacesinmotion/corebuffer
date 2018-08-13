#ifndef STURCTURECHECK_H
#define STURCTURECHECK_H

#include "fileerror.h"

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

private:
  Package &_package;
  std::vector<FileError> _errors;
};

#endif  // STURCTURECHECK_H
