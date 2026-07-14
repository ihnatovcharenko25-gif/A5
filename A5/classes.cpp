#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <functional> 


#include "func.cpp"

class Function {
protected:
	int parameters;
public:
	virtual int Evaluate(const std::vector<int>& args) {
		std::cout << "Non-specified function type";
		return 0;
	}
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
	std::function<int(const std::vector<int>& args)> value;
public:
	PredeclaredFunction() : value(nullptr) {
		parameters = 0;
	}
	PredeclaredFunction(std::function<int(const std::vector<int>& args)> f, int p) {
		value = f;
		parameters = p;
	}
};


class Parser {
	static int charType(char c) {
		if (isalpha(c)) return 1;
		if (isdigit(c)) return 2;
		if (c == '(' || c == ')') return 3;
		std::string operators = "+-*/";
		if (operators.find(c) != std::string::npos) return 4;
		return 0;
	}
public:
	static std::vector<std::string> Parse(const std::string& input) {
		std::vector<std::string> result;
		int i = 0;
		std::string currentToken = "";
		while (i < input.length()) {
			if (input[i] == ' ' || input[i] == ',') {
				if (currentToken.length() > 0) {
					result.push_back(currentToken);
					currentToken = "";
				}
				i++;
				continue;
			}
			if (charType(input[i]) == 0) std::cout << "BYPASS" << input[i];
			if (currentToken.length() == 0 ||
				(charType(currentToken[currentToken.length() - 1]) == charType(input[i]) &&
				 charType(input[i]) != 3
				) )
				currentToken += input[i];
			else {
				result.push_back(currentToken);
				currentToken = input[i];
			}
			i++;
		}
		result.push_back(currentToken);
		return result;
	}
};



class Calculator {

	std::map<std::string, CustomFunction> customFunctions;
	std::map<std::string, PredeclaredFunction> predeclaredFunctions;

	std::map<std::string, int> variables;

	std::vector<std::string> bannedFunctionsList = {
		"min", "max", "=",
		"+", "-", "*", "/",
		"pow", "abs"
	};

public:

	Calculator() {
		predeclaredFunctions["+"] = PredeclaredFunction(plus1, 2);
		predeclaredFunctions["-"] = PredeclaredFunction(minus1, 2);
		predeclaredFunctions["*"] = PredeclaredFunction(multiply1, 2);
		predeclaredFunctions["/"] = PredeclaredFunction(divide1, 2);
		predeclaredFunctions["max"] = PredeclaredFunction(max1, 2);
		predeclaredFunctions["min"] = PredeclaredFunction(min1, 2);
		predeclaredFunctions["pow"] = PredeclaredFunction(pow1, 2);
		predeclaredFunctions["abs"] = PredeclaredFunction(abs1, 1);
	}

	int Calculate(const std::vector<std::string>& v) {
		for (auto s : v) std::cout << s; std::cout << "\n";

		return 0;
	}

	CustomFunction CreateFunction(std::vector<std::string> v) {
		int a = std::find(v.begin(), v.end(), "(") - v.begin();
		int b = std::find(v.begin(), v.end(), ")") - v.begin();
		std::vector<std::string> parameters;
		for (int i = a + 1; i < b; i++)
			parameters.push_back(v[i]);
		for (int p = 0; p < parameters.size(); p++)
			for (int i = 0; i < v.size(); i++)
				if (v[i] == parameters[p])
					v[i] = "{" + std::to_string(p) + "}";

		std::cout << "Created def: ";
		std::string result = "";
		for (int i = b + 2; i < v.size(); i++) result += v[i];
		std::cout << result << "\n";

		return CustomFunction(result, parameters.size());
	}

	std::string Process(const std::string& input) {
		std::vector<std::string> tokens = Parser::Parse(input);
		for (auto s : tokens) std::cout << s << "\n";
		if (tokens[0] == "var") {
			if (variables.contains(tokens[1]))
				std::cout << "Var " << tokens[1] << " already exists!\n";
			else {
				std::vector<std::string> subset(tokens.begin() + 3, tokens.end());
				variables[tokens[1]] = Calculate(subset);
				std::cout << "Created variable " << tokens[1] << " = " << variables[tokens[1]] << "\n";
			}
		}
		else if (tokens[0] == "def") {
			if (customFunctions.contains(tokens[1]) ||
				std::find(bannedFunctionsList.begin(), bannedFunctionsList.end(), tokens[1]) != bannedFunctionsList.end() )
				std::cout << "Def " << tokens[1] << " already exists!\n";
			else {
				std::vector<std::string> subset(tokens.begin() + 2, tokens.end());
				customFunctions[tokens[1]] = CreateFunction(subset);
			}
		}
		else {
			Calculate(tokens);
		}
		return "NNN";
	}

};

