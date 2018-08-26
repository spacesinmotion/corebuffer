
branch | status 
--- |:---:
master | [![Build status](https://ci.appveyor.com/api/projects/status/x0xdasqqpq1evc08/branch/master?svg=true)](https://ci.appveyor.com/project/spacesinmotion/corebuffer/branch/master) 
develop | [![Build status](https://ci.appveyor.com/api/projects/status/x0xdasqqpq1evc08/branch/develop?svg=true)](https://ci.appveyor.com/project/spacesinmotion/corebuffer/branch/develop)

##  What is CoreBuffer?
It is a code generation tool that is highly inspired by [Protocol Buffers](https://developers.google.com/protocol-buffers/)
and [FlatBuffers](https://google.github.io/flatbuffers/). But these tools concentrate on fast information transfer,
CoreBuffer is designed to automatically generate binary serialization of complex data structures.

### Features
* automatic generation of [c++](https://en.wikipedia.org/wiki/C++) code from a simple
  [IDL](https://en.wikipedia.org/wiki/Interface_description_language)
* generation into one header only *(easy to use)*
* directly create and manipulate `structs` in your code
* provide *read* and *write* functions

##  Build CoreBuffer compiler

```sh
$  mkdir  build
$  cd  build
$  cmake  ..
$  cmake.exe  --build  .  --target  all
```

##  Usage

```sh
$  CoreBufferC <input.cor> <output.h>
```

## Documentation

* [IDL documentation](doc/idl.md) - structures used to define *CoreBuffer*

##  Example

### IDL

As example we will use the following [IDL](https://en.wikipedia.org/wiki/Interface_description_language) defining data
for a little shop with items and orders *(There might be better solutions, but ...)*.


```
package Example.Game;
version "0.1";
root_type Hero;

table Spell {
  manaCost:float;
  cooldown:float;
}

table Technique {
  damage:float;
  strength:float;
}

union Ability { Spell, Technique }

enum Category { Carries, Jungler, Initiator, Support }

table Hero {
  name:string;
  category:Category;

  health:float;
  mana:float;

  abilities:[Ability];
}
```

### Code generation

Lets say the code above is in a file named `shop.cor`. Then we create the c++ code with:

```sh
$ CoreBufferC shop.cor shop.h
```

### Code usage

The data is generated as plain structs and could be used that way:

```cpp

#include "shop.h"
#include <fstream>

int main()
{
  Shop s;

  auto kicks = std::make_shared<Item>();
  kicks->name = "Cool Kicks";
  kicks->price = 2.45f;
  s.items.emplace_back(kicks);

  auto john = std::make_shared<Customer>();
  john->name = "John Doe";
  john->address = "Street 4, TownTown";
  s.customers.emplace_back(john);

  s.orders.emplace_back();
  s.orders.back().customer = john;
  s.orders.back().items.emplace_back(kicks);

```

The reading and writing methods are provided in a struct with suffix `_io`:


```cpp
  std::ofstream fo("shop.dat");
  Shop_io().WriteShop(fo, s);
  fo.close();

  Shop read_in;
  std::ifstream fi("shop.dat");
  Shop_io().ReadShop(fi, read_in);
  fi.close();

  return 0;
}
```

## ToDo

* write more documentation
* more convenient types and functions ...
