#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <functional> 
#include <queue>
#include <stack>


#include "func.cpp"




class Parser {
	static int charType(char c) {
		if (isalpha(c)) return 1;
		if (isdigit(c) || c == '{' || c == '}' || c == '.') return 2;
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
					))
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






class Function {
protected:
	int parameters;
public:
	virtual float Evaluate(const std::vector<float>& args) {
		std::cout << "Non-specified function type";
		return 0;
	}
	int ArgCount() {
		return parameters;
	}
};
class CustomFunction : public Function {
	std::vector<std::string> body;
	std::function<float(const std::vector<std::string>&)> evaluator;
public:
	CustomFunction() {
		parameters = 0;
	}
	CustomFunction(std::vector<std::string> bodyTokens, int p,
		std::function<float(const std::vector<std::string>&)> eval) {
		body = bodyTokens;
		parameters = p;
		evaluator = eval;
	}
	float Evaluate(const std::vector<float>& args) {
		std::vector<std::string> substituted = body;
		if (parameters != args.size()) {
			std::cout << "Incorrect number of arguments!\n";
			return 0;
		}
		for (auto& tok : substituted) {
			if (tok.size() >= 3 && tok.front() == '{' && tok.back() == '}') {
				int index = std::stoi(tok.substr(1, tok.size() - 2));
				tok = std::to_string(args[index]);
			}
		}
		return evaluator(substituted);
	}
};
class PredeclaredFunction : public Function {
	std::function<float(const std::vector<float>& args)> value;
public:
	PredeclaredFunction() : value(nullptr) {
		parameters = 0;
	}
	PredeclaredFunction(std::function<float(const std::vector<float>& args)> f, int p) {
		value = f;
		parameters = p;
	}
	float Evaluate(const std::vector<float>& args) {
		return value(args);
	}
};
class IntegrateFunction : public Function {
	std::function<float(const std::vector<float>& args)> value;
public:
	IntegrateFunction() : value(nullptr) {
		parameters = 0;
	}
	IntegrateFunction(std::function<float(const std::vector<float>& args)> f, int p) {
		value = f;
		parameters = p;
	}
	float Evaluate(const std::vector<float>& args) {
		return value(args);
	}
};







class Calculator {

	std::map<std::string, CustomFunction> customFunctions;
	std::map<std::string, PredeclaredFunction> predeclaredFunctions;
	std::map<std::string, PredeclaredFunction> operators;
	//std::map<std::string, Function> functions;

	std::map<std::string, float> variables;

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
		return !s.empty() && ( isdigit((unsigned char)s[0]) || (s.length()>0 && isdigit(s[1])));
	}
	bool isFunction(const std::string& s) {
		return customFunctions.contains(s) || predeclaredFunctions.contains(s);
	}
	int arityOf(const std::string& s) {
		if (predeclaredFunctions.contains(s)) 
			return predeclaredFunctions[s].ArgCount();
		if (customFunctions.contains(s)) 
			return customFunctions[s].ArgCount();
		if (s == "integral") return 3;
		return -1;
	}
	float resolveVar(const std::string& s) {
		if (!variables.contains(s)) 
			throw std::runtime_error("Undefined variable: " + s);
		return variables[s];
	}

	float Integrate(const std::string& f, const std::string& a, const std::string& b) {
		float l = std::stof(a);
		float r = std::stof(b);
		float dif = (r - l) / 100;
		float result = 0.0;
		for (float i = l; i + dif <= r; i += dif) {
			float yl = customFunctions[f].Evaluate({ i });
			float yr = customFunctions[f].Evaluate({ i + dif });
			result += dif * (yl + yr) / 2;
		}
		return result;
	}
	float callFn(const std::string& s, const std::vector<float>& args) {
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

		std::string prevToken = "";

		for (const std::string& token : tokens) {
			if (token == "-" && (prevToken.empty() || prevToken == "(" || prevToken == "," || IsOperator(prevToken))) {
				output.push("0");
				prevToken = "0";
			}
			if (token == ",") {
				while (!ops.empty() && ops.top() != "(") { output.push(ops.top()); ops.pop(); }
				prevToken = token;
				continue;
			}
			if (token == "(") { ops.push(token); prevToken = token; continue; }
			if (token == ")") {
				while (!ops.empty() && ops.top() != "(") { output.push(ops.top()); ops.pop(); }
				if (ops.empty()) throw std::runtime_error("No matching '('");
				ops.pop();
				if (!ops.empty() && isFunction(ops.top())) { output.push(ops.top()); ops.pop(); }
				prevToken = token;
				continue;
			}
			if (IsNumber(token)) { output.push(token); prevToken = token; continue; }
			if (isFunction(token)) { ops.push(token); prevToken = token; continue; }
			if (IsOperator(token)) {
				while (!ops.empty() && ops.top() != "(" && !isFunction(ops.top()) &&
					Priority(ops.top()) >= Priority(token))
				{
					output.push(ops.top());
					ops.pop();
				}
				ops.push(token);
				prevToken = token;
				continue;
			}
			output.push(token);
			prevToken = token;
		}
		while (!ops.empty()) { output.push(ops.top()); ops.pop(); }
		return output;
	}

	float Evaluate(std::queue<std::string> rpn) {
		std::vector<std::string> expr;
		while (!rpn.empty()) { expr.push_back(rpn.front()); rpn.pop(); }

		int i = expr.size() - 1;
		std::function<float()> resolve = [&]() -> float {
			std::string token = expr[i--];
			if (IsOperator(token)) {
				float b = resolve();
				float a = resolve();
				return callFn(token, {a, b});
			}
				
			int n = arityOf(token);
			if (n >= 0) {
				std::vector<float> args(n);
				for (int k = n - 1; k >= 0; k--) args[k] = resolve();
				return callFn(token, args);
			}
			if (IsNumber(token)) return std::stof(token);
			return resolveVar(token);
			};
		return resolve();
	}
	////////////////////////////////


	float Calculate(const std::vector<std::string>& v) {
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
		std::vector<std::string> result(v.begin() + b+2, v.end());
		for (int i = b + 2; i < v.size(); i++) std::cout << v[i]; std::cout << "\n";
		
		//return CustomFunction(result, parameters.size());
		return CustomFunction(result, parameters.size(),
			[this](const std::vector<std::string>& toks) { return Calculate(toks); });
	}

	float Process(const std::string& input) {
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
		else if (tokens[0] == "integrate") {
			if (!customFunctions.contains(tokens[2])) {
				std::cout << "Def " << tokens[2] << " doesn't exist!\n";
				return 0;
			}
			return Integrate(tokens[2], tokens[4], tokens[6]);
		}
		else {
			return Calculate(tokens);
		}
		return 0;
	}

};

