#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <vector>
#include <string>
#include <windows.h>
#include <cstdlib>
#include <iostream>

std::vector<int> findLocation(std::string sample, char findIt);

void BindStdHandlesToConsole();