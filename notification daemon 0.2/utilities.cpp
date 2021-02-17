#include "utilities.h"
#include <stdint.h>   // for uint32_t
#include <stdexcept>  // for std::runtime_error
#include <cstring>


std::vector<int> findLocation(std::string sample, char findIt)
{
	std::vector<int> characterLocations;
	for (int i = 0; i < sample.size(); i++)
		if (sample[i] == findIt)
			characterLocations.push_back(i);

	return characterLocations;
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

char* bin2hex(const unsigned char* bin, size_t len)
{
	char* out;
	size_t  i;

	if (bin == NULL || len == 0)
		return NULL;

	out = (char*)malloc(len * 2 + 1);
	for (i = 0; i < len; i++) {
		out[i * 2] = "0123456789ABCDEF"[bin[i] >> 4];
		out[i * 2 + 1] = "0123456789ABCDEF"[bin[i] & 0x0F];
	}
	out[len * 2] = '\0';

	return out;
}

int hexchr2bin(const char hex, char* out)
{
	if (out == NULL)
		return 0;

	if (hex >= '0' && hex <= '9') {
		*out = hex - '0';
	}
	else if (hex >= 'A' && hex <= 'F') {
		*out = hex - 'A' + 10;
	}
	else if (hex >= 'a' && hex <= 'f') {
		*out = hex - 'a' + 10;
	}
	else {
		return 0;
	}

	return 1;
}

size_t hexs2bin(const char* hex, unsigned char** out)
{
	size_t len;
	char   b1;
	char   b2;
	size_t i;

	if (hex == NULL || *hex == '\0' || out == NULL)
		return 0;

	len = strlen(hex);
	if (len % 2 != 0)
		return 0;
	len /= 2;

	*out = (unsigned char*)malloc(len);
	memset(*out, 'A', len);
	for (i = 0; i < len; i++) {
		if (!hexchr2bin(hex[i * 2], &b1) || !hexchr2bin(hex[i * 2 + 1], &b2)) {
			return 0;
		}
		(*out)[i] = (b1 << 4) | b2;
	}
	return len;
}
#ifndef SFML_SYSTEM_LINUX
std::wstring utf8toUtf16(const std::string& str)
{
	if (str.empty())
		return std::wstring();
	size_t charsNeeded = ::MultiByteToWideChar(CP_UTF8, 0,
		str.data(), (int)str.size(), NULL, 0);
	if (charsNeeded == 0)
		throw std::runtime_error("Failed converting UTF-8 string to UTF-16");

	std::vector<wchar_t> buffer(charsNeeded);
	int charsConverted = ::MultiByteToWideChar(CP_UTF8, 0,
		str.data(), (int)str.size(), &buffer[0], buffer.size());
	if (charsConverted == 0)
		throw std::runtime_error("Failed converting UTF-8 string to UTF-16");

	return std::wstring(&buffer[0], charsConverted);
}

std::wstring Utf8ToUtf16_2(const std::string& utf8)
{
	std::wstring utf16; // Result
	if (utf8.empty())
	{
		return utf16;
	}
	// Safely fails if an invalid UTF-8 character
	// is encountered in the input string
	constexpr DWORD kFlags = MB_ERR_INVALID_CHARS;

	if (utf8.length() > static_cast<size_t>((std::numeric_limits<int>::max)()))
	{
		throw std::overflow_error(
			"Input string too long: size_t-length doesn't fit into int.");
	}
	// Safely convert from size_t (STL string's length)
	// to int (for Win32 APIs)
	const int utf8Length = static_cast<int>(utf8.length());
	const int utf16Length = ::MultiByteToWideChar(
		CP_UTF8,       // Source string is in UTF-8
		kFlags,        // Conversion flags
		utf8.data(),   // Source UTF-8 string pointer
		utf8Length,    // Length of the source UTF-8 string, in chars
		nullptr,       // Unused - no conversion done in this step
		0              // Request size of destination buffer, in wchar_ts
	);
	if (utf16Length == 0)
	{
		// Conversion error: capture error code and throw
		const DWORD error = ::GetLastError();
		throw std::exception(
			"Cannot get result string length when converting " \
			"from UTF-8 to UTF-16 (MultiByteToWideChar failed).",
			error);
	}
	utf16.resize(utf16Length);
	// Convert from UTF-8 to UTF-16
	int result = ::MultiByteToWideChar(
		CP_UTF8,       // Source string is in UTF-8
		kFlags,        // Conversion flags
		utf8.data(),   // Source UTF-8 string pointer
		utf8Length,    // Length of source UTF-8 string, in chars
		&utf16[0],     // Pointer to destination buffer
		utf16Length    // Size of destination buffer, in wchar_ts          
	);
	if (result == 0)
	{
		// Conversion error: capture error code and throw
		const DWORD error = ::GetLastError();
		throw std::exception(
			"Cannot convert from UTF-8 to UTF-16 "\
			"(MultiByteToWideChar failed).",
			error);
	}
	return utf16;
}
#else

#endif