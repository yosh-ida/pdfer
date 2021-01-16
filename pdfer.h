#pragma once
#include "../FreeImage/x64/FreeImage.h"
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <map>

using namespace std;

class pdfer : ofstream
{
private:
	uint32_t id = 3;
	uint32_t xref = 0;

	const string xrefheader = "xref\n%u %u\n";
	const string xrefbody = "%010u %05u n\n";
	const string startxref = "startxref\n%u\n%%%%EOF";
	const string trailer = "trailer\n<<\n\/Root %u %u R\n\/Info %u %u R\n\/Size %u\n>>\n";
	const string header = "%PDF-1.2\n";
	const string objstart = "%u %u obj\n<<\n";
	const string objend = ">>\nendobj\n";
	const string imgresources = "/Resources\n<<\n/XObject <</Im%u %u 0 R>>\n/ProcSet [/PDF /ImageC]\n>>\n";
	const string imgobjmedia = "/MediaBox [0 0 %u %u]\n";

	static const size_t N = 512;
	char buf[N];
	bool closed = true;
	list<uint32_t> index;
	map<uint32_t, uint32_t> pages;
	uint32_t img_count = 0;

	void PagesObj();
	void CatalogObj();
	inline void write(const char* s);
	inline void write(const char* s, const size_t len);
public:
	pdfer(const char *path);
	~pdfer();
	void imgobj(FIBITMAP* const img, const bool png = false);
	void obj(const vector<string> const &element);
	void close();
	bool fail();
};