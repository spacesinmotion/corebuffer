#ifndef FILEPOSITION_H
#define FILEPOSITION_H

#include <cstddef>

struct FilePosition
{
  std::size_t pos{0};
  std::size_t line{1};
  std::size_t column{1};

  bool isAtStart() const { return pos == 0; }
};

#endif  // FILEPOSITION_H
