#ifndef PARSER_H
#define PARSER_H

#include "package.h"

#include <functional>
#include <memory>
#include <string>

using std::string;
using std::unique_ptr;

class Parser
{
public:
  Parser(const string &t, Package &p);

  bool parse();

private:
  char front() const;
  void skip(size_t count = 1);
  string take(size_t count = 1);
  size_t state() const;
  void rewind(size_t p);
  bool end() const;

  static bool contains(const string &s, char c);

  void skipWhiteSpace();
  void skipComment();
  void skipLine();

  string readIdentifier();
  bool read(const string &key);

  bool readPackage();
  bool readPackagePath();

  bool readVersion();

  bool readMainContent();
  bool readTable();
  bool readEnum();

  bool readScopeStatement(const std::function<bool()> &scopeContent);

  bool readEnumEntryList(Enum &e, int lastValue);
  bool readEnumMemberDefault(int &v);

  bool readTableMember(Table &t);
  bool readTableMemberDefault(Member &m);
  bool readTypeDefinition(Member &m);
  bool readTypeVector(Member &m);
  Pointer readTypePointer();
  bool readTypeIdentifier(Member &m);

  bool readBaseType(std::string &val);
  bool readBool(std::string &val);
  bool readNumber(std::string &val);
  bool readString(std::string &val);

private:
  const string &text;
  size_t pos{0};

  Package &package;
};

#endif  // PARSER_H
