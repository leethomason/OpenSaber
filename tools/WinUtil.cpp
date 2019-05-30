#include "../src/saber/Grinliz_Util.h"
#include "../src/saber/rgb.h"
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


void dprintf(const char* format, ...)
{
	va_list     va;
	char		buffer[1024];

	//
	//  format and output the message..
	//
	va_start(va, format);
	vsnprintf_s(buffer, 1024, _TRUNCATE, format, va);	
	va_end(va);

	OutputDebugStringA(buffer);

	fflush(0);
}


void SPLog::attachSerial(Stream* stream)
{
}


void SPLog::attachLog(Stream* stream)
{
}


const SPLog& SPLog::p(const char v[], int width) const
{
    if (width)
        dprintf("%*s", width, v);
    else
        dprintf("%s", v);
	return *this;
}


const SPLog& SPLog::p(char v) const
{
	dprintf("%c", v);
	return *this;
}


const SPLog& SPLog::p(unsigned char v, int p) const
{
	if (p == DEC)
		dprintf("%d", p);
	else if (p == HEX)
		dprintf("%0xd", p);
	else
		ASSERT(false);
	return *this;
}


const SPLog& SPLog::p(int v, int p) const
{
	if (p == DEC)
		dprintf("%d", v);
	else if (p == HEX)
		dprintf("%0xd", v);
	else
		ASSERT(false);
	return *this;
}


const SPLog& SPLog::p(unsigned int v, int p) const
{
	if (p == DEC)
		dprintf("%d", v);
	else if (p == HEX)
		dprintf("%0xd", v);
	else
		ASSERT(false);
	return *this;
}


const SPLog& SPLog::p(long v, int p) const
{
	if (p == DEC)
		dprintf("%d", v);
	else if (p == HEX)
		dprintf("%0xd", v);
	else
		ASSERT(false);
	return *this;
}


const SPLog& SPLog::p(unsigned long v, int p) const
{
	if (p == DEC)
		dprintf("%d", v);
	else if (p == HEX)
		dprintf("%0xd", v);
	else
		ASSERT(false);
	return *this;
}


const SPLog& SPLog::p(double v, int p) const
{
	dprintf("%f", v);
	return *this;
}


void SPLog::eol() const
{
	dprintf("\n");
}


