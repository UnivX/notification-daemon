#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

#ifndef SFML_SYSTEM_LINUX
#include <windows.h>
#else
#include <locale> 
#include <codecvt> 
#endif

#include <cstdlib>
#include <iostream>
#include <stdexcept>


std::vector<int> findLocation(std::string sample, char findIt);

void replaceAll(std::string& str, const std::string& from, const std::string& to);

char* bin2hex(const unsigned char* bin, size_t len);

int hexchr2bin(const char hex, char* out);

size_t hexs2bin(const char* hex, unsigned char** out);
#ifndef SFML_SYSTEM_LINUX
std::wstring utf8toUtf16(const std::string& str);
std::wstring Utf8ToUtf16_2(const std::string& utf8);
#endif