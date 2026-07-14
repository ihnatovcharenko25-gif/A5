#include <vector>
#include <map>
#include <string>
#include <functional> 


#include "func.cpp"

class Function {
protected:
	int parameters;
};
class CustomFunction : public Function {
	std::string value;
public:
	CustomFunction() {
		parameters = 0;
	}
	CustomFunction(std::string f, int p) {
		value = f;
		parameters = p;
	}
};
class PredeclaredFunction : public Function {
	std::function<int(std::vector<int>)> value;
public:
	PredeclaredFunction() : value(nullptr) {
		parameters = 0;
	}
	PredeclaredFunction(std::function<int(std::vector<int>)> f, int p) {
		value = f;
		parameters = p;
	}
};


static class Parser {
public:
	static std::vector<std::string> Parse(const std::string& input) {

	}
};


class Calculator {
	std::map<std::string, CustomFunction> customFunctions;
	std::map<std::string, PredeclaredFunction> predeclaredFunction;
public:
	Calculator() {
		predeclaredFunction["+"] = PredeclaredFunction(plus1, 2);
	}
	std::string Process(const std::string& input) {
		std::vector<std::string> tokens = Parser::Parse(input);

	}

};

