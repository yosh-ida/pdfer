#include <stdio.h>
#include <Windows.h>
#include "getopt.h"
#include "pdfer.h"

using namespace std;

int main(int argc, char *argv[])
{
	std::map<const char*, const char*> defined_option;
	std::list<const char*> option;
	const std::string format = "";

	if (getopt(argc, argv, defined_option, option, format) < 0 || option.size() < 2)
	{
		fprintf(stderr, "invalid argument or option.\n");
		return 0;
	}

	pdfer pdf(*(++option.begin()));
	if (pdf.fail()) return 0;

	/*if (strchr(*option.begin(), '*') == NULL)
	{
		FIBITMAP* image = FreeImage_Load(FIF_JPEG, *option.begin(), JPEG_ACCURATE);
		if (!image)	return 0;
		pdf.imgobj(image, false);
		FreeImage_Unload(image);
	}
	else*/
	{
		HANDLE hFind;
		WIN32_FIND_DATA win32fd;
		string dir = string(*option.begin()) + '\\';
		hFind = FindFirstFile((dir + "*.jpg").c_str(), &win32fd);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			FindClose(hFind);
			return 0;
		}
		do
		{
			if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;
			FIBITMAP* image = FreeImage_Load(FIF_JPEG, (dir + win32fd.cFileName).c_str(), JPEG_ACCURATE);
			if (!image)	continue;
			pdf.imgobj(image, false);
			FreeImage_Unload(image);
		} while (FindNextFile(hFind, &win32fd));
		FindClose(hFind);
	}

	pdf.close();
	return 0;
}