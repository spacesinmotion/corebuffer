#ifndef FILEPOSITION_H
#define FILEPOSITION_H

struct FilePosition
{
  size_t pos{0};
  size_t line{1};
  size_t column{1};
};

#endif  // FILEPOSITION_H
