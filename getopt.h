#pragma once

#include <map>
#include <string>
#include <list>

int getopt(int argc, char *const argv[], std::map<const char*, const char*>& defined_option, std::list<const char*>& option,const std::string &format);

extern char *optarg;
extern int optind;

int getopt(int argc, char *const argv[], const char *optstring);

