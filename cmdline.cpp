#include "cmdline.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <map>
#include <string>

namespace CmdLine {

enum CommandLineOptType {
  INT64_T = 1,
  BOOL = 2,
  STRING = 3,
};

enum ParseOptResult {
  SUCC = 1,
  FAIL = 2,
};

struct CommandLineOptDefaultValue {
  bool boolValue;
  int64_t int64Value;
  std::string stringValue;
};

class CommandLineOpt {
 public:
  CommandLineOpt() = default;
  CommandLineOpt(bool* value, std::string name, bool defaultValue, bool required) {
    this->type = BOOL;
    this->name = name;
    this->value = (void*)value;
    this->isValueSet = false;
    this->isOptSet = false;
    this->required = required;
    this->defaultValue.boolValue = defaultValue;
  }
  CommandLineOpt(int64_t* value, std::string name, int64_t defaultValue, bool required) {
    this->type = INT64_T;
    this->name = name;
    this->value = (void*)value;
    this->isValueSet = false;
    this->isOptSet = false;
    this->required = required;
    this->defaultValue.int64Value = defaultValue;
  }
  CommandLineOpt(std::string* value, std::string name, std::string defaultValue, bool required) {
    this->type = STRING;
    this->name = name;
    this->value = (void*)value;
    this->isValueSet = false;
    this->isOptSet = false;
    this->required = required;
    this->defaultValue.stringValue = defaultValue;
  }
  bool IsBoolOpt() { return type == BOOL; }
  void SetOptIsSet() { isOptSet = true; }
  void SetBoolValue(bool value) {
    this->isValueSet = true;
    *(bool*)this->value = value;
  }
  void SetValue(std::string value) {
    if (this->type == STRING) {
      *(std::string*)this->value = value;
    }
    if (this->type == INT64_T) {
      *(int64_t*)this->value = atoll(value.c_str());
    }
    this->isValueSet = true;
  }
  bool CheckRequired() {
    if (!required) {
      return true;
    }
    if (required && isValueSet) {
      return true;
    }
    if (not isOptSet) {
      printf("%s is required but not set\n", name.c_str());
      return false;
    }
    printf("required option %s not set argument\n", name.c_str());
    return false;
  }
  void SetDefaultValue() {
    if (isValueSet) {
      return;
    }
    if (type == BOOL) {
      *(bool*)value = defaultValue.boolValue;
    }
    if (type == INT64_T) {
      *(int64_t*)value = defaultValue.int64Value;
    }
    if (type == STRING) {
      *(std::string*)value = defaultValue.stringValue;
    }
  }

 private:
  CommandLineOptType type;
  std::string name;
  void* value;
  bool isValueSet;
  bool isOptSet;
  bool required;
  CommandLineOptDefaultValue defaultValue;
};

static Usage _usage = nullptr;
static std::map<std::string, CommandLineOpt> _opts;

static bool isInvalidName(std::string name) {
  if (name == "") {
    return false;
  }
  if (name[0] == '-') {
    printf("option %s begins with -\n", name.c_str());
    return true;
  }
  if (name.find("=") != name.npos) {
    printf("option %s contains =\n", name.c_str());
    return true;
  }
  return false;
}

static ParseOptResult ParseOpt(int argc, char* argv[], int& parseIndex) {
  char* opt = argv[parseIndex];
  int optLen = strlen(opt);
  if (optLen <= 1) {  // 选项的长度必须>=2
    printf("option's len must greater than or equal to 2\n");
    return FAIL;
  }
  if (opt[0] != '-') {  // 选项必须以'-'开头
    printf("option must begins with '-', %s is invalid option\n", opt);
    return FAIL;
  }
  opt++;  // 过滤第一个'-'
  optLen--;
  if (*opt == '-') {
    opt++;  // 过滤第二个'-'
    optLen--;
  }
  // 过滤完有效的'-'之后还要再check一下后面的内容和长度
  if (optLen == 0 || *opt == '-' || *opt == '=') {
    printf("bad opt syntax:%s\n", argv[parseIndex]);
    return FAIL;
  }

  // 执行到这里说明是一个选项，接下来判断这个选项是否有参数
  bool hasArgument = false;
  std::string argument = "";
  for (int i = 1; i < optLen; i++) {
    if (opt[i] == '=') {
      hasArgument = true;
      argument = std::string(opt + i + 1);  // 取等号之后的内容赋值为argument
      opt[i] = 0;                           // 这样opt执行的字符串就是'='之前的内容。
      break;
    }
  }

  std::string optName = std::string(opt);
  if (optName == "help" || optName == "h") {  // 有help选项，则直接调用_usage函数，并退出
    _usage();
    exit(0);
  }

  std::map<std::string, CommandLineOpt>::iterator iter = _opts.find(optName);
  // 选项不存在
  if (iter == _opts.end()) {
    printf("option provided but not defined: -%s\n", optName.c_str());
    return FAIL;
  }
  iter->second.SetOptIsSet();
  if (iter->second.IsBoolOpt()) {  // 不需要参数的bool类型选项
    iter->second.SetBoolValue(true);
    parseIndex++;  // parseIndex跳到下一个选项
  } else {         // 需要参数的选项，参数可能在下一个命令行参数中
    if (hasArgument) {
      parseIndex++;
    } else {
      if (parseIndex + 1 < argc) {  // 选项的值在下一个命令行参数
        hasArgument = true;
        argument = std::string(argv[parseIndex + 1]);
        parseIndex += 2;  // parseIndex跳到下一个选项
      }
    }
    if (not hasArgument) {
      printf("option needs an argument: -%s\n", optName.c_str());
      return FAIL;
    }
    iter->second.SetValue(argument);
  }
  return SUCC;
}

static void CheckRequiredAndSetDefault() {
  std::map<std::string, CommandLineOpt>::iterator iter = _opts.begin();
  while (iter != _opts.end()) {
    if (!iter->second.CheckRequired()) {
      exit(-1);
    }
    iter->second.SetDefaultValue();
    iter++;
  }
}

void BoolOpt(bool* value, std::string name) {
  if (_opts.find(name) != _opts.end()) {
    printf("%s opt already set\n", name.c_str());
    exit(-1);
  }
  if (isInvalidName(name)) {
    printf("%s is invalid name\n", name.c_str());
    exit(-2);
  }
  _opts[name] = CommandLineOpt(value, name, false, false);
}

void Int64Opt(int64_t* value, std::string name, int64_t defaultValue) {
  if (_opts.find(name) != _opts.end()) {
    printf("%s opt already set\n", name.c_str());
    exit(-1);
  }
  if (isInvalidName(name)) {
    printf("%s is invalid name\n", name.c_str());
    exit(-2);
  }
  _opts[name] = CommandLineOpt(value, name, defaultValue, false);
}

void StrOpt(std::string* value, std::string name, std::string defaultValue) {
  if (_opts.find(name) != _opts.end()) {
    printf("%s opt already set\n", name.c_str());
    exit(-1);
  }
  if (isInvalidName(name)) {
    printf("%s is invalid name\n", name.c_str());
    exit(-2);
  }
  _opts[name] = CommandLineOpt(value, name, defaultValue, false);
}

void Int64OptRequired(int64_t* value, std::string name) {
  if (_opts.find(name) != _opts.end()) {
    printf("%s opt already set\n", name.c_str());
    exit(-1);
  }
  if (isInvalidName(name)) {
    printf("%s is invalid name\n", name.c_str());
    exit(-2);
  }
  _opts[name] = CommandLineOpt(value, name, 0, true);
}

void StrOptRequired(std::string* value, std::string name) {
  if (_opts.find(name) != _opts.end()) {
    printf("%s opt already set\n", name.c_str());
    exit(-1);
  }
  if (isInvalidName(name)) {
    printf("%s is invalid name\n", name.c_str());
    exit(-2);
  }
  _opts[name] = CommandLineOpt(value, name, "", true);
}

void SetUsage(Usage usage) { _usage = usage; }

void Parse(int argc, char* argv[]) {
  if (nullptr == _usage) {
    printf("usage function not set\n");
    exit(-1);
  }
  // 这里跳过命令名不解析，所以parseIndex从1开始
  int parseIndex = 1;
  while (parseIndex < argc) {
    int ret = ParseOpt(argc, argv, parseIndex);
    if (SUCC == ret) {
      continue;
    }
    exit(-2);
  }
  CheckRequiredAndSetDefault();
}

}  // namespace CmdLine
