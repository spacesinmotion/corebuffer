#include "parser.h"

#include <cassert>
#include <sstream>

static string WHITESPACE = " \t\n\r";
static string DIDGET_NO_ZERO = "123456789";
static string DIDGET = "0" + DIDGET_NO_ZERO;
static string IDENTIFIER_BEGIN = "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static string IDENTIFIER_MID = IDENTIFIER_BEGIN + DIDGET;

Parser::Parser(const std::string &t, Package &p) : text(t), package(p) {}

bool Parser::parse()
{
  while (readMainContent())
    ;

  skipComment();
  if (end())
    return true;
  return false;
}

char Parser::front() const
{
  return text[pos];
}

void Parser::skip(size_t count)
{
  pos += count;
}

string Parser::take(size_t count)
{
  string e = "";
  for (size_t i = 0; i < count && !end(); ++i)
  {
    e += front();
    skip();
  }
  return e;
}

size_t Parser::state() const
{
  return pos;
}

void Parser::rewind(size_t p)
{
  pos = p;
}

bool Parser::end() const
{
  return pos >= text.size();
}

bool Parser::contains(const std::string &s, char c)
{
  for (auto w : s)
    if (w == c)
      return true;
  return false;
}

void Parser::skipWhiteSpace()
{
  while (!end() && contains(WHITESPACE, front()))
    skip();
}

void Parser::skipLine()
{
  while (!end() && take(1) != "\n")
    ;
}

void Parser::skipComment()
{
  skipWhiteSpace();

  auto s = state();
  auto start = take(2);
  if (start == "/*")
  {
    char l1 = '\0';
    char l2 = '\0';
    while (!end() && (l1 != '*' || l2 != '/'))
    {
      l1 = l2;
      l2 = take().front();
    }
    return skipComment();
  }
  if (start == "//")
  {
    skipLine();
    return skipComment();
  }

  rewind(s);
}

string Parser::readIdentifier()
{
  skipComment();
  if (!end() && contains(IDENTIFIER_BEGIN, front()))
  {
    auto e = take();
    while (!end() && contains(IDENTIFIER_MID, front()))
    {
      e += take();
    }
    return e;
  }
  return "";
}

bool Parser::read(const string &key)
{
  skipComment();

  auto s = state();
  if (take(key.size()) == key)
    return true;

  rewind(s);
  return false;
}

bool Parser::readPackage()
{
  auto s = state();

  if (read("package") && readPackagePath() && read(";"))
    return true;

  rewind(s);
  return false;
}

bool Parser::readPackagePath()
{
  auto s = state();

  const auto name = readIdentifier();
  if (!name.empty())
  {
    package.path += name;
    if (read("."))
    {
      package.path += ".";
      if (readPackagePath())
        return true;
    }
    else
      return true;
  }

  rewind(s);
  return false;
}

bool Parser::readVersion()
{
  auto s = state();

  if (read("version"))
  {
    if (readString(package.version) && read(";"))
      return true;
  }

  rewind(s);
  return false;
}

bool Parser::readMainContent()
{
  return readTable() || readEnum() || readPackage() || readVersion();
}

bool Parser::readTable()
{
  auto s = state();

  if (read("table"))
  {
    Table t;
    t.name = readIdentifier();
    if (!t.name.empty())
    {
      bool ok = true;
      //      if (read(":"))
      //        ok = readClassBaseList(_class);

      if (ok && readScopeStatement([this, &t]() {
            while (readTableMember(t))
              ;
            return true;
          }))
      {
        package.tables.push_back(t);
        return true;
      }
    }
  }

  rewind(s);
  return false;
}

bool Parser::readEnum()
{
  auto s = state();

  if (read("enum"))
  {
    Enum e;
    e.name = readIdentifier();
    if (!e.name.empty())
    {
      if (readScopeStatement([this, &e]() { return readEnumEntryList(e, 0); }))
      {
        package.enums.push_back(e);
        return true;
      }
    }
  }

  rewind(s);
  return false;
}

bool Parser::readEnumEntryList(Enum &e, int lastValue)
{
  auto s = state();

  const auto name = readIdentifier();
  if (!name.empty())
  {
    if (!readEnumMemberDefault(lastValue))
      ++lastValue;
    e.entries.emplace_back(name, lastValue);
    if (read(","))
    {
      if (readEnumEntryList(e, lastValue))
        return true;
    }
    else
      return true;
  }

  rewind(s);
  return false;
}

bool Parser::readTableMember(Table &t)
{
  auto s = state();

  Member m;
  m.name = readIdentifier();
  if (!m.name.empty())
  {
    if (read(":") && readTypeDefinition(m))
    {
      readTableMemberDefault(m);
      if (read(";"))
      {
        t.member.push_back(m);
        return true;
      }
    }
  }

  rewind(s);
  return false;
}

bool Parser::readTableMemberDefault(Member &m)
{
  auto s = state();

  string val;
  if (read("=") && readBaseType(val))
  {
    m.defaultValue = val;
    return true;
  }

  rewind(s);
  return false;
}

bool Parser::readEnumMemberDefault(int &v)
{
  auto s = state();

  string val;
  if (read("=") && readNumber(val))
  {
    std::stringstream ss(val);
    if (ss >> v)
      return true;
  }

  rewind(s);
  return false;
}

bool Parser::readTypeDefinition(Member &m)
{
  return readTypeVector(m) || readTypeIdentifier(m);
}

bool Parser::readTypeVector(Member &m)
{
  auto s = state();

  if (read("[") && readTypeIdentifier(m) && read("]"))
  {
    m.isVector = true;
    return true;
  }

  rewind(s);
  return false;
}

Pointer Parser::readTypePointer()
{
  auto s = state();

  const auto id = readIdentifier();
  if (id == "weak")
    return Pointer::Weak;
  else if (id == "unique")
    return Pointer::Unique;
  else if (id == "shared")
    return Pointer::Shared;
  else if (id == "plain")
    return Pointer::Plain;

  rewind(s);
  return Pointer::Plain;
}

bool Parser::readTypeIdentifier(Member &m)
{
  auto s = state();

  m.pointer = readTypePointer();
  const auto id = readIdentifier();
  if (!id.empty())
  {
    m.type = id;
    return true;
  }

  rewind(s);
  return false;
}

bool Parser::readScopeStatement(const std::function<bool()> &scopeContent)
{
  auto s = state();

  if (read("{"))
  {
    if (scopeContent() && read("}"))
      return true;
  }

  rewind(s);
  return false;
}

bool Parser::readBaseType(string &val)
{
  auto bt = readBool(val);

  if (!bt)
    bt = readNumber(val);
  if (!bt)
    bt = readString(val);

  return bt;
}

bool Parser::readBool(string &val)
{
  if (read("true"))
  {
    val = "true";
    return true;
  }
  if (read("false"))
  {
    val = "false";
    return true;
  }
  return false;
}

bool Parser::readNumber(string &i)
{
  auto s = state();

  skipComment();

  i = "";
  if (front() == '-' || front() == '+')
    i = take();
  while (!end() && contains(DIDGET, front()))
    i += take();
  if (front() == '.')
  {
    string sub;
    auto s2 = state();
    sub += take();
    if (!contains(DIDGET, front()))
      rewind(s2);
    else
    {
      while (!end() && contains(DIDGET, front()))
        sub += take();
      i += sub;
    }
  }

  if (!i.empty())
    return true;

  rewind(s);
  return false;
}

bool Parser::readString(string &val)
{
  auto s = state();

  if (read("\""))
  {
    while (!end() && front() != '\"')
      val += take();

    if (!end())
    {
      skip();
      return true;
    }
  }

  rewind(s);
  return false;
}
