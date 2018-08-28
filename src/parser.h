#ifndef PARSER_H
#define PARSER_H

#include "fileposition.h"
#include "package.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

using std::unique_ptr;

class Parser
{
  using string = std::string;
  using size_t = std::size_t;

public:
  Parser(const string &t, Package &p);

  void parse();

private:
  char front() const;
  void skip();
  string take(size_t count = 1);
  const FilePosition &state() const;
  FilePosition stateBefor(size_t letters) const;
  void rewind(const FilePosition &p);
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
  bool readRootType();

  bool readMainContent();
  bool readTable();
  bool readEnum();
  bool readFlag();

  bool readUnion();
  bool readUnionEntries(Union &u);

  bool readScopeStatement(const std::function<bool()> &scopeContent);

  bool readEnumEntryList(Enum &e, size_t lastValue);
  bool readEnumMemberDefault(size_t &v);

  bool readFlagEntryList(Flag &f);

  bool readTableMemberOrMethod(Table &t);
  bool readTableMethod(Table &t);
  vector<Parameter> readIdentifierList();
  bool readTableMember(Table &t);
  bool readTableMemberDefault(Member &m);
  bool readTypeDefinition(Member &m);
  bool readTypeVector(Member &m);
  std::pair<Pointer, bool> readTypePointer();
  bool readTypeIdentifier(Member &m);
  string readIdentifierGroup();

  bool readBaseType(std::string &val);
  bool readBool(std::string &val);
  bool readNumber(std::string &val);
  bool readString(std::string &val);

  void initBaseTypes();
  Table *tableForType(const std::string &t) const;
  Union *unionForType(const std::string &t) const;
  Enum *enumForType(const std::string &t) const;
  void updateTableAppearance();
  std::string fullPackageScope() const;

  bool isComplexType(const std::string &t, std::unordered_set<const Table *> &visited) const;
  bool isComplexType(const Table &t, std::unordered_set<const Table *> &visited) const;

private:
  const string &text;
  FilePosition _state;

  std::unordered_map<string, string> aliases;
  std::unordered_map<string, string> defaults;
  Package &package;
};

#endif  // PARSER_H
