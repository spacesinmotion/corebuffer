#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include "cppoutput.h"
#include "fileerror.h"
#include "parser.h"
#include "structurecheck.h"

#include "args/args.hxx"

#ifndef COREBUFFER_VERSION
#define COREBUFFER_VERSION "<undefined>"
#endif
#ifndef COREBUFFER_BUILD
#define COREBUFFER_BUILD "<undefined>"
#endif

using namespace std;

void usageError(const string &msg, const args::ArgumentParser &args)
{
  cerr << msg << endl << endl;
  cerr << args << endl;
}

void logError(const string &file, const FileError &pe)
{
  cerr << file << ":" << pe._state.line << ":" << pe._state.column << ": error: " << pe.what() << endl;
}

int compile(const string &input, const string &output, const args::ArgumentParser &args)
{
  ifstream t(input);
  if (!t)
  {
    usageError("can not open input '" + input + "'.", args);
    return 2;
  }

  string source((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());

  Package p;
  try
  {
    Parser(source, p).parse();

    auto errors = StructureCheck(p).check();
    sort(errors.begin(), errors.end(),
         [](const FileError &l, const FileError &r) { return l._state.pos < r._state.pos; });
    if (!errors.empty())
    {
      for (const auto &pe : errors)
        logError(input, pe);
      return 3;
    }
  }
  catch (const FileError &pe)
  {
    logError(input, pe);
    return 3;
  }

  ofstream o(output);
  if (!o)
  {
    usageError("can not open output '" + output + "'.", args);
    return 4;
  }

  WriteCppCode(o, p);

  return 0;
}

int main(int argc, char *argv[])
{
  args::ArgumentParser args(
      "CoreBuffer compiler. Code generation tool for convenient automatically generated binary serialization of "
      "complex data structures.",
      __DATE__ " CoreBufferC " COREBUFFER_VERSION " build " COREBUFFER_BUILD);
  args::HelpFlag help(args, "help", "Display this help menu", {'h', "help"});
  args::Flag version(args, "version", "display the program version", {"version"});
  args::Positional<string> input(args, "<input.cor>", "the CoreBuffer IDL descripting input file");
  args::Positional<string> output(args, "<output.h>", "the c++ header output");

  try
  {
    args.ParseCLI(argc, argv);
  }
  catch (args::Help)
  {
    cout << args;
    return 0;
  }
  catch (args::ParseError e)
  {
    usageError(e.what(), args);
    return 1;
  }

  if (version)
  {
    cout << COREBUFFER_VERSION << endl;
    return 0;
  }

  if (!input || !output)
  {
    usageError("missing commad line argument(s).", args);
    return 1;
  }

  return compile(input.Get(), output.Get(), args);
}
