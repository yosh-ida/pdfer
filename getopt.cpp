#include "getopt.h"

int getopt(int argc, char *const argv[], std::map<const char*, const char*>& defined_option, std::list<const char*>& option, const std::string &format)
{
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{ 
			size_t pos = format.find(argv[i]);
			if (pos == std::string::npos)
				continue;
			pos += strlen(argv[i]);
			bool have_arg = pos < format.length() && format[pos] == ':';
			if (have_arg && i + 1 >= argc)
				return -1;
			defined_option[argv[i]] = have_arg ? argv[i + 1] : NULL;		
			if (have_arg) i++;
			continue;
		}
		if (argv[i][0] == 0)
			return -1;
		option.push_back(argv[i]);
	}
	return 0;
}