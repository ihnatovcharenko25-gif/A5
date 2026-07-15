#include <iostream>
#include <string> 

#include "classes.cpp"


#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	std::string input;

	Calculator calc;

	while (std::getline(std::cin, input)) {
		if (input == "exit" || input == "quit") {
			break;
		}

		if (input.empty()) {
			continue;
		}
		std::cout << "Result (or exitcode): " << calc.Process(input) << "\n";
	}
	//int* leakedArray = new int[10];
	return 0;
}
