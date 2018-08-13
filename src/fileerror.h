#ifndef FILEERROR_H
#define FILEERROR_H

#include "fileposition.h"

#include <stdexcept>

struct FileError : std::runtime_error
{
  FileError(const std::string &m, const FilePosition &s);

  FilePosition _state;
};
#endif  // FILEERROR_H
