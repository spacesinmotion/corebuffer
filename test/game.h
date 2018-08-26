#pragma once

#include <vector>
#include <string>
#include <ostream>
#include <istream>
#include <memory>
#include <array>
#include <algorithm>
#include <type_traits>

namespace Example {
namespace Game {

template<typename T>
struct AlwaysFalse : std::false_type {};

struct Spell;
struct Technique;
struct Ability;
struct Hero;

struct Spell {
  float manaCost{0.0f};
  float cooldown{0.0f};

  Spell() = default;

  friend bool operator==(const Spell&l, const Spell&r) {
    return 
      l.manaCost == r.manaCost
      && l.cooldown == r.cooldown;
  }

  friend bool operator!=(const Spell&l, const Spell&r) {
    return 
      l.manaCost != r.manaCost
      || l.cooldown != r.cooldown;
  }
};

struct Technique {
  float damage{0.0f};
  float strength{0.0f};

  Technique() = default;

  friend bool operator==(const Technique&l, const Technique&r) {
    return 
      l.damage == r.damage
      && l.strength == r.strength;
  }

  friend bool operator!=(const Technique&l, const Technique&r) {
    return 
      l.damage != r.damage
      || l.strength != r.strength;
  }
};

struct Ability {
  Ability() = default;
  Ability(const Ability &o) { _clone(o); }
  Ability& operator=(const Ability &o) { _destroy(); _clone(o); return *this; }

  Ability(const Spell &v)
    : _Spell(new Spell(v))
    , _selection(_Spell_selection)
  {}
  Ability(Spell &&v)
    : _Spell(new Spell(std::forward<Spell>(v)))
    , _selection(_Spell_selection)
  {}
  Ability & operator=(const Spell &v) {
    _destroy();
    _Spell = new Spell(v);
    _selection = _Spell_selection;
    return *this;
  }
  Ability & operator=(Spell &&v) {
    _destroy();
    _Spell = new Spell(std::forward<Spell>(v));
    _selection = _Spell_selection;
    return *this;
  }

  Ability(const Technique &v)
    : _Technique(new Technique(v))
    , _selection(_Technique_selection)
  {}
  Ability(Technique &&v)
    : _Technique(new Technique(std::forward<Technique>(v)))
    , _selection(_Technique_selection)
  {}
  Ability & operator=(const Technique &v) {
    _destroy();
    _Technique = new Technique(v);
    _selection = _Technique_selection;
    return *this;
  }
  Ability & operator=(Technique &&v) {
    _destroy();
    _Technique = new Technique(std::forward<Technique>(v));
    _selection = _Technique_selection;
    return *this;
  }

  ~Ability() {
    _destroy();
  }

  bool is_Defined() const noexcept { return _selection != no_selection; }
  void clear() { *this = Ability(); }

  bool is_Spell() const noexcept { return _selection == _Spell_selection; }
  const Spell & as_Spell() const noexcept { return *_Spell; }
  Spell & as_Spell() { return *_Spell; }
  template<typename... Args> Spell & create_Spell(Args&&... args) {
    return (*this = Spell(std::forward<Args>(args)...)).as_Spell();
  }

  bool is_Technique() const noexcept { return _selection == _Technique_selection; }
  const Technique & as_Technique() const noexcept { return *_Technique; }
  Technique & as_Technique() { return *_Technique; }
  template<typename... Args> Technique & create_Technique(Args&&... args) {
    return (*this = Technique(std::forward<Args>(args)...)).as_Technique();
  }

  friend bool operator==(const Ability&ab, const Spell &o) noexcept  { return ab.is_Spell() && ab.as_Spell() == o; }
  friend bool operator==(const Spell &o, const Ability&ab) noexcept  { return ab.is_Spell() && o == ab.as_Spell(); }
  friend bool operator!=(const Ability&ab, const Spell &o) noexcept  { return !ab.is_Spell() || ab.as_Spell() != o; }
  friend bool operator!=(const Spell &o, const Ability&ab) noexcept  { return !ab.is_Spell() || o != ab.as_Spell(); }

  friend bool operator==(const Ability&ab, const Technique &o) noexcept  { return ab.is_Technique() && ab.as_Technique() == o; }
  friend bool operator==(const Technique &o, const Ability&ab) noexcept  { return ab.is_Technique() && o == ab.as_Technique(); }
  friend bool operator!=(const Ability&ab, const Technique &o) noexcept  { return !ab.is_Technique() || ab.as_Technique() != o; }
  friend bool operator!=(const Technique &o, const Ability&ab) noexcept  { return !ab.is_Technique() || o != ab.as_Technique(); }

  bool operator==(const Ability &o) const noexcept
  {
    if (this == &o)
      return true;
    if (_selection != o._selection)
      return false;
    switch(_selection) {
    case no_selection: while(false); /* hack for coverage tool */ return true;
    case _Spell_selection: return *_Spell == *o._Spell;
    case _Technique_selection: return *_Technique == *o._Technique;
    }
    return false; // without this line there is a msvc warning I do not understand.
  }

  bool operator!=(const Ability &o) const noexcept
  {
    if (this == &o)
      return false;
    if (_selection != o._selection)
      return true;
    switch(_selection) {
    case no_selection: while(false); /* hack for coverage tool */ return false;
    case _Spell_selection: return *_Spell != *o._Spell;
    case _Technique_selection: return *_Technique != *o._Technique;
    }
    return false; // without this line there is a msvc warning I do not understand.
  }

private:
  void _clone(const Ability &o) noexcept
  {
     _selection = o._selection;
    switch(_selection) {
    case no_selection: while(false); /* hack for coverage tool */ break;
    case _Spell_selection: _Spell = new Spell(*o._Spell); break;
    case _Technique_selection: _Technique = new Technique(*o._Technique); break;
    }
  }

  void _destroy() noexcept {
    switch(_selection) {
    case no_selection: while(false); /* hack for coverage tool */ break;
    case _Spell_selection: delete _Spell; break;
    case _Technique_selection: delete _Technique; break;
    }
    no_value = nullptr;
  }

  union {
    struct NoValue_t *no_value{nullptr};
    Spell * _Spell;
    Technique * _Technique;
  };

  enum Selection_t {
    no_selection,
    _Spell_selection,
    _Technique_selection,
  };

  Selection_t _selection{no_selection};
  friend struct Hero_io;
};

enum class Category : std::int8_t {
  Carries = 0,
  Jungler = 1,
  Initiator = 2,
  Support = 3,
};

inline const std::array<Category,4> & CategoryValues() {
  static const std::array<Category,4> values {{
    Category::Carries,
    Category::Jungler,
    Category::Initiator,
    Category::Support,
  }};
  return values;
};

inline const char * ValueName(const Category &v) {
  switch(v) {
    case Category::Carries: return "Carries";
    case Category::Jungler: return "Jungler";
    case Category::Initiator: return "Initiator";
    case Category::Support: return "Support";
  }
  return "<error>";
};

struct Hero {
  std::string name;
  Category category{Example::Game::Category::Carries};
  float health{0.0f};
  float mana{0.0f};
  std::vector<Ability> abilities;

  Hero() = default;

  friend bool operator==(const Hero&l, const Hero&r) {
    return 
      l.name == r.name
      && l.category == r.category
      && l.health == r.health
      && l.mana == r.mana
      && l.abilities == r.abilities;
  }

  friend bool operator!=(const Hero&l, const Hero&r) {
    return 
      l.name != r.name
      || l.category != r.category
      || l.health != r.health
      || l.mana != r.mana
      || l.abilities != r.abilities;
  }

  template<class T> void fill_abilities(const T &v) {
    std::fill(abilities.begin(), abilities.end(), v);
  }

  template<class Generator> void generate_abilities(Generator gen) {
    std::generate(abilities.begin(), abilities.end(), gen);
  }

  template<class T> std::vector<Ability>::iterator remove_abilities(const T &v) {
    return std::remove(abilities.begin(), abilities.end(), v);
  }
  template<class Pred> std::vector<Ability>::iterator remove_abilities_if(Pred v) {
    return std::remove_if(abilities.begin(), abilities.end(), v);
  }

  template<class T> void erase_abilities(const T &v) {
    abilities.erase(remove_abilities(v));
  }
  template<class Pred> void erase_abilities_if(Pred v) {
    abilities.erase(remove_abilities_if(v));
  }

  void reverse_abilities() {
    std::reverse(abilities.begin(), abilities.end());
  }

  void rotate_abilities(std::vector<Ability>::iterator i) {
    std::rotate(abilities.begin(), i, abilities.end());
  }

  template<class Comp> void sort_abilities(Comp p) {
    std::sort(abilities.begin(), abilities.end(), p);
  }

  template<class Comp> bool any_of_abilities(Comp p) {
    return std::any_of(abilities.begin(), abilities.end(), p);
  }
  template<class T> bool any_of_abilities_is(const T &p) {
    return any_of_abilities([&p](const Ability &x) { return x == p; });
  }

  template<class Comp> bool all_of_abilities(Comp p) {
    return std::all_of(abilities.begin(), abilities.end(), p);
  }
  template<class T> bool all_of_abilities_are(const T &p) {
    return all_of_abilities([&p](const Ability &x) { return x == p; });
  }

  template<class Comp> bool none_of_abilities(Comp p) {
    return std::none_of(abilities.begin(), abilities.end(), p);
  }
  template<class T> bool none_of_abilities_is(const T &p) {
    return none_of_abilities([&p](const Ability &x) { return x == p; });
  }

  template<class Fn> Fn for_each_abilities(Fn p) {
    return std::for_each(abilities.begin(), abilities.end(), p);
  }

  template<class T> std::vector<Ability>::iterator find_in_abilities(const T &p) {
    return std::find(abilities.begin(), abilities.end(), p);
  }
  template<class Comp> std::vector<Ability>::iterator find_in_abilities_if(Comp p) {
    return std::find_if(abilities.begin(), abilities.end(), p);
  }

  template<class T>   typename std::iterator_traits<std::vector<Ability>::iterator>::difference_type count_in_abilities(const T &p) {
    return std::count(abilities.begin(), abilities.end(), p);
  }
  template<class Comp>   typename std::iterator_traits<std::vector<Ability>::iterator>::difference_type count_in_abilities_if(Comp p) {
    return std::count_if(abilities.begin(), abilities.end(), p);
  }
};

struct Hero_io {
private:
  template<typename T> void Write(std::ostream &, const T *) {
    static_assert(AlwaysFalse<T>::value, "Something not implemented");
  }

  template<typename T> void Write(std::ostream &o, const T &v) {
    o.write(reinterpret_cast<const char *>(&v), sizeof(T));
  }

  template<typename T> void Write(std::ostream &o, const std::vector<T> &v) {
    Write(o, v.size());
    o.write(reinterpret_cast<const char *>(v.data()), sizeof(T) * v.size());
  }

  template<typename T> void Write(std::ostream &, const std::shared_ptr<T> &) {
    static_assert(AlwaysFalse<T>::value, "Something not implemented");
  }

  void Write(std::ostream &o, const std::string &v) {
    Write(o, v.size());
    o.write(v.data(), v.size());
  }

  template<typename T> void Read(std::istream &i, T &v) {
    i.read(reinterpret_cast<char *>(&v), sizeof(T));
  }

  template<typename T> void Read(std::istream &, std::shared_ptr<T> &) {
    static_assert(AlwaysFalse<T>::value, "Something not implemented");
  }

  template<typename T> void Read(std::istream &i, std::vector<T> &v) {
    typename std::vector<T>::size_type s{0};
    Read(i, s);
    v.resize(s);
    i.read(reinterpret_cast<char *>(v.data()), sizeof(T) * s);
  }

  void Read(std::istream &i, std::string &v) {
    std::string::size_type s{0};
    Read(i, s);
    v.resize(s);
    i.read(&v[0], s);
  }

  void Write(std::ostream &o, const Spell &v) {
    Write(o, v.manaCost);
    Write(o, v.cooldown);
  }

  void Read(std::istream &s, Spell &v) {
    Read(s, v.manaCost);
    Read(s, v.cooldown);
  }

  void Write(std::ostream &o, const Technique &v) {
    Write(o, v.damage);
    Write(o, v.strength);
  }

  void Read(std::istream &s, Technique &v) {
    Read(s, v.damage);
    Read(s, v.strength);
  }

  void Write(std::ostream &o, const Ability &v) {
    o.write(reinterpret_cast<const char*>(&v._selection), sizeof(Ability::Selection_t));
    switch(v._selection) {
    case Ability::no_selection: while(false); /* hack for coverage tool */ break;
    case Ability::_Spell_selection: Write(o, v.as_Spell()); break;
    case Ability::_Technique_selection: Write(o, v.as_Technique()); break;
    }
  }

  void Write(std::ostream &o, const std::vector<Ability> &v) {
    Write(o, v.size());
    for (const auto &entry : v)
      Write(o, entry);
  }

  void Read(std::istream &i, Ability &v) {
    i.read(reinterpret_cast<char*>(&v._selection), sizeof(Ability::Selection_t));
    switch(v._selection) {
    case Ability::no_selection: while(false); /* hack for coverage tool */ break;
    case Ability::_Spell_selection: Read(i, v.create_Spell()); break;
    case Ability::_Technique_selection: Read(i, v.create_Technique()); break;
    }
  }

  void Read(std::istream &s, std::vector<Ability> &v) {
    auto size = v.size();
    Read(s, size);
    v.resize(size);
    for (auto &entry : v)
      Read(s, entry);
  }

  void Write(std::ostream &o, const Hero &v) {
    Write(o, v.name);
    Write(o, v.category);
    Write(o, v.health);
    Write(o, v.mana);
    Write(o, v.abilities);
  }

  void Read(std::istream &s, Hero &v) {
    Read(s, v.name);
    Read(s, v.category);
    Read(s, v.health);
    Read(s, v.mana);
    Read(s, v.abilities);
  }

public:
  void WriteHero(std::ostream &o, const Hero &v) {

    o.write("CORE", 4);
    o.write("0.1", 3);
    Write(o, v);
  }

  bool ReadHero(std::istream &i, Hero &v) {

    std::string marker("0000");
    i.read(&marker[0], 4);
    if (marker != "CORE")
      return false;
    std::string version(3, '_');
    i.read(&version[0], 3);
    if (version != "0.1")
      return false;
    Read(i, v);
    return true;
  }

};
}
}
