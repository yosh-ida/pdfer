#include <string>
#include <stdlib.h>
#include <list>
#include <map>
#include <vector>
#include "pdfer.h"

using namespace std;

unsigned DLL_CALLCONV writeToMemory(void *buffer, unsigned size, unsigned count, fi_handle handle)
{
	((ofstream*)handle)->write((char*)buffer, size*count);
	return count;
}


void pdfer::PagesObj()
{
	index.push_front(tellp());
	write("2 0 obj\n<<\n");
	write("/Type /Pages\n");
	write("/Kids [");
	bool space = false;
	for (auto i : pages)
	{
		if (space) write(" ", 1);
		size_t len = _snprintf_s(buf, N, _TRUNCATE, "%u %u R", i.first, i.second);
		write(buf, len);
		space = true;
	}
	write("]\n");
	size_t len = _snprintf_s(buf, N, _TRUNCATE, "/Count %u\n", pages.size());
	write(buf, len);
	write(objend.c_str());
}

void pdfer::CatalogObj()
{
	index.push_front(tellp());
	write("1 0 obj\n<<\n");
	write("/Type /Catalog\n");
	write("/Pages 2 0 R\n");
	write(objend.c_str());
}

inline void pdfer::write(const char* s)
{
	const size_t len = strlen(s);
	write(s, len);
}

inline void pdfer::write(const char* s, const size_t len)
{
	ofstream::write(s, len);
	//if (!closed) xref += len;
}

pdfer::pdfer(const char *path) : ofstream(path, ios::out | ios::binary | ios::trunc)
{
	if (fail()) return;
	closed = false;
	write(header.c_str(), header.length());
}

pdfer::~pdfer()
{
	if (!closed) close();
}

void pdfer::imgobj(FIBITMAP* const img, bool png)
{
	const uint32_t page_index = id++;
	const uint32_t content_index = id++;
	const uint32_t img_index = id++;
	const uint32_t img_length_index = id++;
	pages[page_index] = 0;
	size_t len;

	//	page
	/*	-----------------------------------	*/
	//index.push_back(xref);
	index.push_back(tellp());
	len = _snprintf_s(buf, N, _TRUNCATE, objstart.c_str(), page_index, 0);
	write(buf, len);
	write("/Type /Page\n");
	write("/Parent 2 0 R\n");

	const uint32_t w = FreeImage_GetWidth(img);
	const uint32_t h = FreeImage_GetHeight(img);
	const uint32_t bpp = FreeImage_GetBPP(img);

	len = _snprintf_s(buf, N, _TRUNCATE, imgobjmedia.c_str(), w, h);
	write(buf, len);

	len = _snprintf_s(buf, N, _TRUNCATE, imgresources.c_str(), img_count, img_index);
	write(buf, len);

	len = _snprintf_s(buf, N, _TRUNCATE, "/Contents %u %u R\n", content_index, 0);
	write(buf, len);
	write(objend.c_str(), objend.length());
	/*	-----------------------------------	*/

	//	content
	/*	-----------------------------------	*/
	//index.push_back(xref);
	index.push_back(tellp());

	static const string content_length_str = "/Length %u\n>>\nstream\n";
	static const string stream_str = "q\n%u 0 0 %u 0 0 cm\n/Im%u Do\nQ";
	string stream;
	_snprintf_s(buf, N, _TRUNCATE, stream_str.c_str(), w, h, img_count);
	stream = buf;

	len = _snprintf_s(buf, N, _TRUNCATE, objstart.c_str(), content_index, 0);
	write(buf, len);
	len = _snprintf_s(buf, N, _TRUNCATE, content_length_str.c_str(), stream.length());
	write(buf, len);
	write(stream.c_str(), stream.length());
	write("\nendstream\nendobj\n");
	/*	-----------------------------------	*/

	//	image
	/*	-----------------------------------	*/
	//index.push_back(xref);
	index.push_back(tellp());
	len = _snprintf_s(buf, N, _TRUNCATE, objstart.c_str(), img_index, 0);
	write(buf, len);
	write("/Type /XObject\n/Subtype /Image\n");
	len = _snprintf_s(buf, N, _TRUNCATE, "/Name /Im%u\n", img_count);
	write(buf, len);
	if (png)
		write("/Filter[/FlateDecode]\n");
	else
		write("/Filter[/DCTDecode]\n");
	len = _snprintf_s(buf, N, _TRUNCATE, "/Width %u\n", w);
	write(buf, len);
	len = _snprintf_s(buf, N, _TRUNCATE, "/Height %u\n", h);
	write(buf, len);
	write("/ColorSpace /DeviceRGB\n");
	len = _snprintf_s(buf, N, _TRUNCATE, "/BitsPerComponent %u\n", 8);
	write(buf, len);
	len = _snprintf_s(buf, N, _TRUNCATE, "/Length %u %u R\n", img_length_index, 0);
	write(buf, len);
	write(">>\nstream\n");

	auto img_start = tellp();
	FreeImageIO io;
	io.write_proc = writeToMemory;
	if (png)
		FreeImage_SaveToHandle(FIF_PNG, img, &io, this, PNG_DEFAULT);
	else
		FreeImage_SaveToHandle(FIF_JPEG, img, &io, this, JPEG_OPTIMIZE);
	uint32_t img_length = tellp() - img_start;
	write("\nendstream\nendobj\n");

	//	image length
	/*	-----------------------------------	*/
	//index.push_back(xref);
	index.push_back(tellp());
	len = _snprintf_s(buf, N, _TRUNCATE, "%u %u obj\n%u\nendobj\n", img_length_index, 0, img_length);
	write(buf, len);
	/*	-----------------------------------	*/

	img_count++;
}

void pdfer::obj(const vector<string> const &element)
{
	//index.push_back(xref);
	index.push_back(tellp());
	size_t len = _snprintf_s(buf, N, _TRUNCATE, objstart.c_str(), id++, 0);
	write(buf, len);

	//	do anything
	for (auto elem : element)
		write((elem + "\n").c_str(), (elem + "\n").length());

	write(objend.c_str(), objend.length());
}

void pdfer::close()
{
	size_t len;

	PagesObj();
	CatalogObj();

	closed = true;

	//	xref
	len = _snprintf_s(buf, N, _TRUNCATE, xrefheader.c_str(), 0, id);
	write(buf, len);
	//	xrefÇÕçsññÇ™(0x20 0x0A)
	write("0000000000 65535 f \n");
	for (auto i : index)
	{
		len = _snprintf_s(buf, N, _TRUNCATE, xrefbody.c_str(), i, 0);
		write(buf, len);
	}

	//	trailer
	len = _snprintf_s(buf, N, _TRUNCATE, trailer.c_str(), 1, 0, 0, 0, id);
	write(buf, len);

	//	startxref
	len = _snprintf_s(buf, N, _TRUNCATE, startxref.c_str(), tellp()); //xref);
	write(buf, len);
	ofstream::close();
}

bool pdfer::fail() { return ofstream::fail(); }