#include "utilities.h"



std::vector<int> findLocation(std::string sample, char findIt)
{
	std::vector<int> characterLocations;
	for (int i = 0; i < sample.size(); i++)
		if (sample[i] == findIt)
			characterLocations.push_back(i);

	return characterLocations;
}

void BindStdHandlesToConsole()
{
	//TODO: Add Error checking.

	// Redirect the CRT standard input, output, and error handles to the console
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stderr);
	freopen("CONOUT$", "w", stdout);

	// Note that there is no CONERR$ file
	HANDLE hStdout = CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hStdin = CreateFile("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	SetStdHandle(STD_OUTPUT_HANDLE, hStdout);
	SetStdHandle(STD_ERROR_HANDLE, hStdout);
	SetStdHandle(STD_INPUT_HANDLE, hStdin);

	//Clear the error state for each of the C++ standard stream objects. 
	std::wclog.clear();
	std::clog.clear();
	std::wcout.clear();
	std::cout.clear();
	std::wcerr.clear();
	std::cerr.clear();
	std::wcin.clear();
	std::cin.clear();
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