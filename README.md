
##  What's  the  CoreBuffer?
Is a code generation tool that is highly inspired by [Protocol Buffers](https://developers.google.com/protocol-buffers/)
and [FlatBuffers](https://google.github.io/flatbuffers/). But as these tools concentrate on fast information transfer,
CoreBuffer is designed to automatically generate binary serialization of complex data structures.

### Features
* automatic generation of [c++](https://en.wikipedia.org/wiki/C++) code from a simple
  [IDL](https://en.wikipedia.org/wiki/Interface_description_language)
* code is generated into one header *(easy to use)*
* directly manipulate and create `structs` in your code
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
package Example.Shop;
version "0.1";
root_type Shop;

table Item {
  name:string;
  price:float;
}

table Customer {
  name:string;
  address:string;
}

table Order {
  items:[shared Item];
  customer:shared Customer;
}

table Shop {
  items:[shared Item];
  customers:[shared Customer];
  orders:[Order];
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
* providing error messages for incorrect IDL input
* more convenient types and functions ...
