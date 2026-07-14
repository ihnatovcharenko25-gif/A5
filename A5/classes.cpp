#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <functional> 
#include <queue>
#include <stack>


#include "func.cpp"

class Function {
protected:
	int parameters;
public:
	virtual int Evaluate(const std::vector<int>& args) {
		std::cout << "Non-specified function type";
		return 0;
	}
	int ArgCount() {
		return parameters;
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
	int Evaluate(const std::vector<int>& args) {
		return -1;
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
	int Evaluate(const std::vector<int>& args) {
		return value(args);
	}
};


class Parser {
	static int charType(char c) {
		if (isalpha(c)) return 1;
		if (isdigit(c)) return 2;
		if (c == '(' || c == ')') return 3;
		std::string operators = "+-*/";
		if (operators.find(c) != std::string::npos) return 4;
		if (c == '=') return 5;
		return 0;
	}
public:
	static std::vector<std::string> Parse(const std::string& input) {
		std::vector<std::string> result;
		int i = 0;
		std::string currentToken = "";
		while (i < input.length()) {
			if (input[i] == ',') {
				if (currentToken.length() > 0) {
					result.push_back(currentToken);
					currentToken = "";
				}
				result.push_back(",");
				i++;
				continue;
			}
			if (input[i] == ' ') {
				if (currentToken.length() > 0) {
					result.push_back(currentToken);
					currentToken = "";
				}
				i++;
				continue;
			}
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
	std::map<std::string, PredeclaredFunction> operators;
	std::map<std::string, Function> functions;

	std::map<std::string, int> variables;

	std::vector<std::string> bannedFunctionsList = {
		"min", "max", "=",
		"+", "-", "*", "/",
		"pow", "abs"
	};

	bool IsOperator(const std::string& s) {
		return s.size() == 1 && std::string("+-*/").find(s[0]) != std::string::npos;
	}
	int Priority(const std::string& s) {
		return (s == "*" || s == "/") ? 1 : 0;
	}
	bool IsNumber(const std::string& s) {
		return !s.empty() && isdigit((unsigned char)s[0]);
	}
	bool isFunction(const std::string& s) {
		return customFunctions.contains(s) || predeclaredFunctions.contains(s);
	}
	int arityOf(const std::string& s) {
		if (predeclaredFunctions.contains(s)) 
			return predeclaredFunctions[s].ArgCount();
		if (customFunctions.contains(s)) 
			return customFunctions[s].ArgCount();
		return -1;
	}
	int resolveVar(const std::string& s) {
		if (!variables.contains(s)) 
			throw std::runtime_error("Undefined variable: " + s);
		return variables[s];
	}
	int callFn(const std::string& s, const std::vector<int>& args) {
		if (predeclaredFunctions.contains(s))
			return predeclaredFunctions[s].Evaluate(args);
		else if (customFunctions.contains(s))
			return customFunctions[s].Evaluate(args);
		else 
			return operators[s].Evaluate(args);
	}

public:
	Calculator() {
		operators["+"] = PredeclaredFunction(plus1, 2);
		operators["-"] = PredeclaredFunction(minus1, 2);
		operators["*"] = PredeclaredFunction(multiply1, 2);
		operators["/"] = PredeclaredFunction(divide1, 2);
		predeclaredFunctions["max"] = PredeclaredFunction(max1, 2);
		predeclaredFunctions["min"] = PredeclaredFunction(min1, 2);
		predeclaredFunctions["pow"] = PredeclaredFunction(pow1, 2);
		predeclaredFunctions["abs"] = PredeclaredFunction(abs1, 1);
	}


	///////////////////////////
	std::queue<std::string> ToRPN(const std::vector<std::string>& tokens)
	{
		std::queue<std::string> output;
		std::stack<std::string> ops;

		for (const std::string& token : tokens) {
			if (token == ",") {
				while (!ops.empty() && ops.top() != "(") { output.push(ops.top()); ops.pop(); }
				continue;
			}
			if (token == "(") { ops.push(token); continue; }
			if (token == ")") {
				while (!ops.empty() && ops.top() != "(") { output.push(ops.top()); ops.pop(); }
				if (ops.empty()) throw std::runtime_error("No matching '('");
				ops.pop();
				if (!ops.empty() && isFunction(ops.top())) { output.push(ops.top()); ops.pop(); }
				continue;
			}
			if (IsNumber(token)) { output.push(token); continue; }
			if (isFunction(token)) { ops.push(token); continue; }
			if (IsOperator(token)) {
				while (!ops.empty() && ops.top() != "(" && !isFunction(ops.top()) &&
					Priority(ops.top()) >= Priority(token))
				{
					output.push(ops.top());
					ops.pop();
				}
				ops.push(token);
				continue;
			}
			output.push(token);
		}
		while (!ops.empty()) { output.push(ops.top()); ops.pop(); }
		return output;
	}

	int Evaluate(std::queue<std::string> rpn) {
		std::vector<std::string> expr;
		while (!rpn.empty()) { expr.push_back(rpn.front()); rpn.pop(); }

		int i = expr.size() - 1;
		std::function<int()> resolve = [&]() -> int {
			std::string token = expr[i--];
			if (IsOperator(token)) {
				int b = resolve();
				int a = resolve();
				return callFn(token, {a, b});
			}
				
			int n = arityOf(token);
			if (n >= 0) {
				std::vector<int> args(n);
				for (int k = n - 1; k >= 0; k--) args[k] = resolve();
				return callFn(token, args);
			}
			if (IsNumber(token)) return std::stoi(token);
			return resolveVar(token);
			};
		return resolve();
	}
	////////////////////////////////


	int Calculate(const std::vector<std::string>& v) {
		//for (auto s : v) std::cout << s; std::cout << "\n";

		std::queue rpn = ToRPN(v);
		return Evaluate(rpn);
	}
	
	CustomFunction CreateFunction(std::vector<std::string> v) {
		int a = std::find(v.begin(), v.end(), "(") - v.begin();
		int b = std::find(v.begin(), v.end(), ")") - v.begin();
		std::vector<std::string> parameters;
		for (int i = a + 1; i < b; i++)
			if (v[i] != ",")
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

	int Process(const std::string& input) {
		std::vector<std::string> tokens = Parser::Parse(input);
		//for (auto s : tokens) std::cout << s << "\n";
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
			return Calculate(tokens);
		}
		return 0;
	}

};

