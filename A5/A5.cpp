#include <iostream>
#include <string> 

#include "classes.cpp"


int main()
{
	std::string input;

	Calculator calc;

	while (std::getline(std::cin, input)) {
		if (input == "exit" || input == "quit") {
			break;
		}

		if (input.empty()) {
			continue;
		}
		std::cout << "Echo: " << input << "\n" << calc.Process(input);
	}
	return 0;
}
