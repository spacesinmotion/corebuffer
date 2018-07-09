#ifndef PACKAGE_H
#define PACKAGE_H

#include <string>
#include <vector>

using std::pair;
using std::string;
using std::vector;

struct Enum
{
  string name;
  vector<pair<string, int>> entries;
};

struct Member
{
  string name;
  string type;
  string defaultValue;
  bool isVector{false};
};

struct Table
{
  string name;
  vector<Member> member;
};

struct Package
{
  string path;
  string version;
  vector<Table> tables;
  vector<Enum> enums;
};

#endif  // PACKAGE_H
