/*
 ** Introduction:

                       EasyGoing Configuration

  Created by: Iceyey Chuiskell 2019.3 
  EGConf, or EasyGoing Configuration, is a header-only light-weight 
  library for program configurations.
  An EasyGoing Configuration file may have the following format:
  
    installdir = "~"
    frontcam.serial = "821732900318"
    frontcam.height = 2.3
    frontcam.angle = 15
    frontcam.reverse = 0
    backcam.serial = "760392019306"
    backcam.height = 2.5
    backcam.angle = 0
    backcam.reverse = 1
    tracking.line1.pos = 50
    tracking.line2.pos = 70
    
  After parsing, it will have the following structre
  
    root
    |- installdir
    |- frontcam
    |  |- serial
    |  |- height
    |  |- angle
    |  \- reverse
    |
    |- backcam
    |  |- serial
    |  |- height
    |  |- angle
    |  \- reverse
    |
    \- tracking
       |- line1
       |  \- pos
       |
       \- line2
          \- pos


 ** License

  SAY NO TO SUICIDE PUBLIC LICENSE

  Version 1.0, September 2019

  Copyright (C) 2019 Iceyey Chuiskell

  Everyone is permitted to copy and distribute verbatim copies
  of this license document.

  TERMS AND CONDITIONS FOR USE, REPRODUCTION, MODIFICATION, 
  AND DISTRIBUTION

    1. You can do anything with the original copy, 
    whenever, whatever, no limitation.
  
    2. When you are in despair, just talk to someone you trust, 
    someone you love. Getting help from your family, your friends, 
    the police, the community, the public.
  
    3. Keep yourself alive and say no to suicide.

*/

#ifndef EGCONF_HPP
#define EGCONF_HPP

// redefinable configurations:
#ifndef EGCONF_DUMP_TABSTOP
#define EGCONF_DUMP_TABSTOP 2
#endif

#if __cplusplus >= 201103L
  #define EG_CXX11_CONSTEXPR constexpr
  #define EG_CXX11_NOEXCEPT noexcept
  #define EG_CXX11_OVERRIDE override
  #define EG_CXX11_FINAL final
  #define EG_CXX11_NULLPTR nullptr
  #define EG_CXX11_MOVE_SEMANTICS 1
  
  #include <cinttypes>
  
  namespace egstd {
    using std::int64_t;
  } // namespace egstd
  
#else
  #define EG_CXX11_CONSTEXPR const
  #define EG_CXX11_NOEXCEPT throw()
  #define EG_CXX11_OVERRIDE
  #define EG_CXX11_NULLPTR 0
  #define EG_CXX11_FINAL
  
  #include <inttypes.h>
  namespace egstd {
    using ::int64_t;
  };
  
#endif

#include <cstdio>
#include <cctype>
#include <cassert>
#include <cstddef>
#include <string>
#include <vector>

namespace egconf_impl {

std::pair<std::string, std::string> splitPath(std::string const& path) {
  std::size_t dotPosition = path.find('.');
  if (dotPosition == std::string::npos) {
    return std::make_pair(path, "");
  }
  else {
    return std::make_pair(path.substr(0, dotPosition),
                          path.substr(dotPosition + 1));
  }
}

void printSpace(FILE *fp, int n) {
  for (int i = 0; i < n; i++) {
    fputc(' ', fp);
  }
}

} // egconf_impl

namespace egconf {

typedef std::pair<std::string, std::string> StrPair;

class ConfigValue {
public:
  enum ValueType { IntValue, FloatValue, StringValue };
  
  ConfigValue(egstd::int64_t ivalue) 
    : type(IntValue) { v.ivalue = ivalue; }
  ConfigValue(double fvalue)
    : type(FloatValue) { v.fvalue = fvalue; }
  ConfigValue(std::string const& svalue)
    : type(StringValue), svalue(svalue) {}
    
  ValueType getType() const EG_CXX11_NOEXCEPT { return type; }
  
  egstd::int64_t getAsIntUnsafe() const EG_CXX11_NOEXCEPT {
    assert(getType() == IntValue);
    return v.ivalue;
  }
  
  double getAsFloatUnsafe() const EG_CXX11_NOEXCEPT {
    assert(getType() == FloatValue);
    return v.fvalue;
  }
  
  std::string const& getAsStrUnsafe() const EG_CXX11_NOEXCEPT {
    assert(getType() == StringValue); 
    return svalue;
  }
  
  void dump() const EG_CXX11_NOEXCEPT {
    switch (getType()) {
    case IntValue: 
      std::fprintf(stderr, "{Int value %" PRId64 "}", v.ivalue); break;
    case FloatValue:
      std::fprintf(stderr, "{Float value %f}", v.fvalue); break;
    case StringValue:
      std::fprintf(stderr, "{Str value %s}", svalue.c_str()); break;
    }
  }
  
private:
  ValueType type;
  union {
    egstd::int64_t ivalue;
    double fvalue;
  } v;
  std::string svalue;
};

struct ConfigLookupResult {
  enum ResultType { Success, NotFound, NotValueNode };
  
  ConfigLookupResult(ConfigValue const* value) 
    : value(value), type(Success) { }
  ConfigLookupResult(ResultType type) 
    : value(EG_CXX11_NULLPTR), type(type) { }
  
  ConfigValue const* const value;
  ResultType const type;
};

struct ConfigInsertResult {
  enum Ty { Success, Duplicate, IllFormed }; 
};

class ConfigTreeNode {
public:
  ConfigTreeNode(std::string const& name) : name(name) {}
  std::string const& getName() const EG_CXX11_NOEXCEPT { return name; }
  virtual ~ConfigTreeNode() {}
  virtual ConfigLookupResult
  findValue(std::string const& name) const = 0;
  virtual ConfigInsertResult::Ty
  insertValue(std::string const& path, ConfigValue const& value) = 0;
  virtual void dump(int depth) const = 0;
  
private:
  std::string name;
};

class ConfigTreeValueNode EG_CXX11_FINAL : public ConfigTreeNode {
public:
  ConfigTreeValueNode(std::string const& name,
                      ConfigValue const& value)
    : ConfigTreeNode(name), value(value) {}
  
  ConfigLookupResult 
  findValue(std::string const& name) const EG_CXX11_OVERRIDE {
    if (getName() == name) {
      return ConfigLookupResult(&value);  
    }
    return ConfigLookupResult(ConfigLookupResult::NotFound);
  }

  ConfigInsertResult::Ty
  insertValue(std::string const&,
              ConfigValue const&) EG_CXX11_OVERRIDE {
    return ConfigInsertResult::IllFormed;
  }
  
  void dump(int depth) const EG_CXX11_OVERRIDE {
    egconf_impl::printSpace(stderr, depth * EGCONF_DUMP_TABSTOP);
    std::fprintf(stderr, "%s = ", getName().c_str());
    value.dump();
    std::fputc('\n', stderr);
  }

private:
  ConfigValue value;
};

class ConfigTreeInternNode EG_CXX11_FINAL : public ConfigTreeNode {
public:
  ConfigTreeInternNode(std::string const& name) : ConfigTreeNode(name) {}

  ~ConfigTreeInternNode() EG_CXX11_OVERRIDE {
    for (iter_type it = children.begin(); it != children.end(); ++it) {
      delete *it;
    }
  }

  ConfigLookupResult 
  findValue(std::string const& name) const EG_CXX11_OVERRIDE {
    StrPair splittedPath = egconf_impl::splitPath(name);
    if (splittedPath.first == getName()) {
      if (splittedPath.second == "") {
        return ConfigLookupResult(ConfigLookupResult::NotValueNode);
      }
      for (const_iter_type it = children.begin();
           it != children.end(); ++it) {
        ConfigLookupResult r = (*it)->findValue(splittedPath.second);
        if (r.type == ConfigLookupResult::Success) {
          return r;
        }
      }
    }
    return ConfigLookupResult(ConfigLookupResult::NotFound);
  }
  
  ConfigInsertResult::Ty
  insertValue(std::string const& path,
              ConfigValue const& value) EG_CXX11_OVERRIDE {
    StrPair splittedPath = egconf_impl::splitPath(path);
    if (splittedPath.second == "") {
      for (iter_type it = children.begin();
           it != children.end(); ++it) {
        if ((*it)->getName() == splittedPath.first) {
          return ConfigInsertResult::Duplicate;
        }
      }
      children.push_back(new ConfigTreeValueNode(splittedPath.first,
                                                 value));
      return ConfigInsertResult::Success;
    }
    else {
      for (iter_type it = children.begin();
           it != children.end(); ++it) {
        if ((*it)->getName() == splittedPath.first) {
          return (*it)->insertValue(splittedPath.second, value);
        }
      }
      ConfigTreeInternNode *node = 
        new ConfigTreeInternNode(splittedPath.first);
      children.push_back(node);
      return node->insertValue(splittedPath.second, value);
    }
  }

  void dump(int depth) const EG_CXX11_OVERRIDE {
    egconf_impl::printSpace(stderr, depth * EGCONF_DUMP_TABSTOP);
    std::fprintf(stderr, "%s {\n", getName().c_str());
    for (const_iter_type it = children.begin();
         it != children.end(); ++it) {
      (*it)->dump(depth + 1);
    }
    egconf_impl::printSpace(stderr, depth * EGCONF_DUMP_TABSTOP);
    std::fprintf(stderr, "}\n");
  }

private:
  typedef typename std::vector<ConfigTreeNode*>::iterator iter_type;
  typedef typename std::vector<ConfigTreeNode*>::const_iterator 
           const_iter_type;
  std::vector<ConfigTreeNode*> children;
};

class ConfigParser {
public:
  enum ParseResult { Success, SyntaxErr, Duplicate, IllFormed, Failed };

  ConfigParser() : node(new ConfigTreeInternNode("_")) {}
  ~ConfigParser() { delete node; }
  
  ParseResult parseConfigLine(std::string const& str) {
    std::size_t idx = 0;
    skipWhitespace(str, idx);
    std::string path = parsePath(str, idx);
    skipWhitespace(str, idx);
    if (str[idx] != '=') {
      return SyntaxErr;
    }
    ++idx;
    skipWhitespace(str, idx);
    std::pair<ConfigValue, ParseResult> value = parseValue(str, idx);
    if (value.second != Success) {
      return value.second;
    }
    
    ConfigInsertResult::Ty result = node->insertValue(path, value.first);
    switch (result) {
    case ConfigInsertResult::Success: return Success;
    case ConfigInsertResult::Duplicate: return Duplicate;
    case ConfigInsertResult::IllFormed: return IllFormed;
    }
    return Failed;
  }

  ConfigTreeNode* getAndReset() EG_CXX11_NOEXCEPT {
    ConfigTreeNode *ret = node;
    node = EG_CXX11_NULLPTR;
    return ret;
  }

private:
  void skipWhitespace(std::string const& str,
                      std::size_t &idx) EG_CXX11_NOEXCEPT {
    while (isblank(str[idx])) {
      ++idx;
    }
  }

  std::string parsePath(std::string const& str, std::size_t &idx) {
    std::string ret;
    while (std::isalnum(str[idx]) || str[idx] == '.') {
      ret.push_back(str[idx]);
      ++idx;
    }
    return ret;
  }
  
  std::pair<ConfigValue, ParseResult>
  parseValue(std::string const& str, std::size_t &idx) {
    if (std::isdigit(str[idx])) {
      egstd::int64_t ivalue = 0;
      while (std::isdigit(str[idx])) {
        ivalue *= 10;
        ivalue += str[idx] - '0';
        ++idx;
      }
      if (str[idx] != '.') {
        return std::make_pair(ConfigValue(ivalue), Success);
      }
      
      ++idx;
      double frac = 0;
      while (std::isdigit(str[idx])) {
        frac *= 10;
        frac += str[idx] - '0';
        ++idx;  
      }
      while (frac >= 1) {
        frac = frac / 10;
      }
      return std::make_pair(ConfigValue(ivalue + frac), Success);
    }
    else if (str[idx] == '"') {
      ++idx;
      std::string svalue;
      while (str[idx] != '\0' && str[idx] != '"') {
        svalue.push_back(str[idx]);
        ++idx;
      }
      return std::make_pair(ConfigValue(svalue), Success);
    }
    else {
      return std::make_pair(ConfigValue((egstd::int64_t)0), SyntaxErr);
    }
  }

  ConfigTreeNode *node;
};

} // namespace egconf

#endif
