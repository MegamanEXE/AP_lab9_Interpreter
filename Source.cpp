#include<iostream>
#include<string>
#include<fstream>
#include<sstream>
#include<vector>

using namespace std;

class Variable;
vector<Variable> varTable;	//global table maintaining variables

class Variable {
private:
	string name;
	int value;
public:
	Variable() : name("_uninitialized"), value(0) {}
	Variable(string argName) : name(argName), value(0) {}
	Variable(string argName, int argValue) : name(argName), value(argValue) {}

	int getValue() { return value; }
	void set(int argValue) { value = argValue; }
	string getName() { return name; }

};

bool isNumeric(string argString) {
	string acceptable = "-.1234567890";
	return argString.find_first_not_of(acceptable) == string::npos;
}

vector<string> tokenize(string argLine, char delim) {
	vector<string> result;
	stringstream ss(argLine);
	string token;
	char delimiter = delim;
	while (getline(ss, token, delim)) {
		//cout << "Current token: " << token;
		result.push_back(token);
	}

	return result;
}


string removeSemicolon(string argString) {
	//return tokenize(argString, ';')[0];

	int strLength = argString.size();
	if (argString[strLength - 1] == ';')
	{
		return argString.substr(0, strLength - 1);
	}
	else
	{
		cout << "Error: semi-colon missing";
		return "";
	}
}

string trim(const string &s)
{
	string::const_iterator it = s.begin();
	while (it != s.end() && isspace(*it))
		it++;

	string::const_reverse_iterator rit = s.rbegin();
	while (rit.base() != it && isspace(*rit))
		rit++;

	return string(it, rit.base());
}

float evaluateExpression(string argString) {
	string operations = "+-/*";
	int opIndex = argString.find_first_of(operations);
	char op = argString[opIndex];	//actual operation

	if (opIndex == string::npos) {
		cout << "No operation found";
	}
	else {
		//Evaluate

		vector<string> tokens = tokenize(argString, op);
		string operandL = trim(tokens[0]);
		string operandR = trim(tokens[1]);

		//get their values first
		//if both are variables
		int lValue = -1, rValue = -1;	//-1 means they never got the value


		for (int i = 0; i < varTable.size(); i++) {
			Variable var = varTable[i];

			if (operandL == trim(var.getName())) {
				lValue = var.getValue();
			}

			if (operandR == trim(var.getName())) {
				rValue = var.getValue();
			}
		}

		//Handle constants
		if (isNumeric(operandL)) { lValue = stoi(operandL); }
		if (isNumeric(operandR)) { rValue = stoi(operandR); }

		switch (op) {
		case '+': return (lValue + rValue); break;
		case '-': return (lValue - rValue); break;
		case '/': return (lValue / rValue); break;
		case '*': return (lValue * rValue); break;
		}

		return 0.0;
	}

}

bool checkIfExists(string varName) {
	for (int i = 0; i < varTable.size(); i++) {
		if (varTable[i].getName() == varName)
			return true;
	}
	return false;
}

int main()
{
	ifstream codeFile("code.txt");


	if (codeFile) {
		string buffer;
		while (getline(codeFile, buffer)) {
			//cout << "Current line: " <<buffer <<endl;
			string cleaned = removeSemicolon(buffer);

			//Declaration of variables
			if ((cleaned.find("let") != string::npos) && (cleaned.find_first_of("+-/*") == string::npos)) {
				vector<string> tokens = tokenize(cleaned, ' ');
				//cout << tokens[0] <<" " << tokens[1] <<endl;
				string varName = tokens[1];
				int varValue = stoi(tokens[3]);
				Variable tempVar(varName, varValue);
				if(!checkIfExists(varName))
					varTable.push_back(tempVar);
				continue;
			}

			//Assignments
			//float m = evaluateExpression("x+2");
			vector<string> isAnAssignmentTokens = tokenize(cleaned, ' ');

			if (isAnAssignmentTokens.size() == 6) {

				vector<string> exprTokens = tokenize(cleaned, '=');
				int rhs = -1;

				if (exprTokens[1].find_first_of("+-/*") != string::npos) {	//an expression on rhs
					rhs = evaluateExpression(trim(exprTokens[1]));
					//cout << rhs << endl;
				}

				//Handle lhs
				if (exprTokens[0].find("let") != string::npos) {
					vector<string> lhsTokens = tokenize(exprTokens[0], ' ');
					string varName = lhsTokens[1];
					//Add to table
					Variable temp(varName, rhs);

					if (!checkIfExists(varName))
						varTable.push_back(temp);	//For new variables
					else {
						for (int i = 0; i < varTable.size(); i++) {
							if (varTable[i].getName() == varName)
								varTable[i].set(rhs);	//Reassign existing variable
						}
					}
				}
			}

			//Handle printing
			if (cleaned.find("print") != string::npos) {
				vector<string> newtokens = tokenize(cleaned, ' ');
				string toPrint = newtokens[1];

				if (isNumeric(toPrint)) {	//just print if constant
					cout << toPrint << endl;
				}
				else {
					for (int i = 0; i < varTable.size(); i++) {
						if (varTable[i].getName() == toPrint) {
							cout << varTable[i].getValue() <<endl;
						}
					}
				}
			}
		}

	}
	else
	{
		cout << "File could not be opened...\n";
	}

	return 0;
}