# IDL Documentation

This is simply a list of all *keywords* and *structures* that could be used in your **CoreBuffer** definition.

## Package

**Example:**
```
package RootScope.SubScope.Example;
```

This is the definition of the data collection your are defining here. Basically this is used to defines the namespaces
the created structs are nested in. This statement should only appear once per file.

## Tables

**Example:**
```
table Player {
  name:string;
  hitPoints:int;
  clan:shared Clan;
  items:[Item];
}
```

Tables are the main structure of each *CoreBuffer* definition. They define collections of different Data. Each needs an
unique name and defines a list of named child structures or base types.

### Base types

Each base type aims to represent the binary form that has to be saved on disk. For integral values there are signed and
unsigned versions. Floating point values and strings are also available.

```
i8
i16
i32
i64

ui8
ui16
ui32
ui64

f32
f64

string
bool
```

There are aliases to the typical c types.

```
int => i32
unsigned short => i16
long => i64

float => f32
double => f64
...
```

### Pointer

Each table member could whether be `plain` *(default)*, `unique`, `shared`, `weak`.

Without any of these markers the members will be `plain`. This means the values are stored per value.

If they are defined as `unique`, `shared` or `weak`, they will become the corresponding c++ stl pointer types.

Plain c pointers are not supported ...

### Vector

Braces like `[...]` define vector data and are used to list multiple items of the same type as one member.

You could define vectors of pointers but no pointers of vectors. Nesting is also not allowed here.

### Initializers (construction)

**Example:**
```
table Player {
  name:string;
  hitPoints:int;

  init(name);
  init(name, hitPoints);
}
```

Beside the default construction you could add construction per value with the *special function* `init`. By listing the
parameter to be initialized as parameter you define the constructor in *c++* code.


## Enums

**Example:**
```
enum DrinkingVessel { Mug, Jar=3, Bowl }
```

Enumeration types are quite the same as in c++. There are some convenient functions created to work with these kind of
types.

Enumerations could not be defined as `root_type`

## Union

**Example:**
```
table Mug { /*...*/ }
table Jar { /*...*/ }
table Bowl { /*...*/ }

enum Stuff { Mug, Jar, Bowl }
```

Union types are combination of tables. Theye are one of the defined values *(or undefined)* . There are convenient
functions created to work with these kind of types.

Union types could not be defined as `root_type`

## root_type

**Example:**
```
root_type Player;
```

This defines the entry point for the io operations. The statement is always *requied* and the value has to be an
existing table.

## version

That defines an *optional* string value that is used by io operations to prevent loading errors for data files saved
with older versions of the IDL.
