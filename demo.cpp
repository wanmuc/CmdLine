#include <stdio.h>

#include <iostream>

#include "cmdline.h"

using namespace std;

std::string type;
std::string name;
int64_t len;
int64_t age;
bool flag;

void usage() {
  cout << "demo -type product -name wanmuc -age 18 -flag" << endl;
  cout << "options:" << endl;
  cout << "    -h,--help      print usage" << endl;
  cout << "    -type          just type" << endl;
  cout << "    -name          just name" << endl;
  cout << "    -len           just len" << endl;
  cout << "    -age           just age" << endl;
  cout << "    -flag          just flag" << endl;
  cout << endl;
}

int main(int argc, char* argv[]) {
  CmdLine::BoolOpt(&flag, "flag");
  CmdLine::Int64Opt(&len, "len", 10);
  CmdLine::StrOpt(&type, "type", "test");
  CmdLine::StrOptRequired(&name, "name");
  CmdLine::Int64OptRequired(&age, "age");
  CmdLine::SetUsage(usage);
  CmdLine::Parse(argc, argv);
  std::cout << "type = " << type << std::endl;
  std::cout << "len = " << len << std::endl;
  std::cout << "flag = " << flag << std::endl;
  std::cout << "name = " << name << std::endl;
  std::cout << "age = " << age << std::endl;
  return 0;
}
