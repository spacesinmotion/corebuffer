#ifndef CPPOUTPUT_H
#define CPPOUTPUT_H

#include <ostream>

#include "package.h"

void WriteCppCode(std::ostream &o, const Package &p);

#endif  // CPPOUTPUT_H
