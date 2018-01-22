/**
the first two lines may be disappeared in linux
name: Shijie Ma
date: 01/14/2017
**/

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
}

#include<iostream>
#include<vector>
#include<string>
#include<fstream>
using namespace std;

class Scanner {
private:
	int n_LA;
	int n_NA;
	int sum_NA;
	int n_OP;
	int n_CP;
	vector<string> LA;
	string in;
public:
	Scanner();
	void Scan();
	string getNextToken(int& pos, string& token);
};

string Scanner::getNextToken(int& pos, string& token) {
	while (pos < in.length()) {
		if (in[pos] == '(') {
			pos++;
			return "OpenParenthesis";
		}
		else if (in[pos] == ')') {
			pos++;
			return "ClosingParenthesis";
		}
		else if (in[pos] >= 'A' && in[pos] <= 'Z') {
			while (pos < in.length() && (in[pos] >= 'A' && in[pos] <= 'Z') || (in[pos] >= '0' && in[pos] <= '9')) {
				token.push_back(in[pos]);
				pos++;
			}
			return "Literal";
		}
		else if (in[pos] >= '0' && in[pos] <= '9') {
			int flag = 0;
			while (pos < in.length() && (in[pos] >= 'A' && in[pos] <= 'Z') || (in[pos] >= '0' && in[pos] <= '9')) {
				token.push_back(in[pos]);
				if (in[pos] >= 'A' && in[pos] <= 'Z')
					flag = 1;
				pos++;
			}
			if (flag)
				return "ERROR";
			else
				return "Numeric";
		}
		else
			pos++;
	}
	return "EOF";
}

void Scanner::Scan() {
	int pos = 0;
	while (pos < in.length()) {
		string token;
		string res = getNextToken(pos, token);
		if (res == "EOF")	break;
		else if (res == "OpenParenthesis")
			n_OP++;
		else if (res == "ClosingParenthesis")
			n_CP++;
		else if (res == "Literal") {
			n_LA++;
			LA.push_back(token);
		}
		else if (res == "Numeric") {
			n_NA++;
			int num = atoi(token.c_str());
			sum_NA += num;
		}
		else if (res == "ERROR") {
			cout << "ERROR: Invalid token " << token << endl;
			return;
		}
	}
	cout << "LITERAL ATOMS: " << n_LA;
	for (int i = 0; i < LA.size(); i++)
		cout << ", " << LA[i];
	cout << endl;
	cout << "NUMERIC ATOMS: " << n_NA << ", " << sum_NA << endl;
	cout << "OPEN PARENTHESES: " << n_OP << endl;
	cout << "CLOSING PARENTHESES: " << n_CP << endl;
}

Scanner::Scanner() {
	n_OP = 0;
	n_CP = 0;
	n_LA = 0;
	n_NA = 0;
	sum_NA = 0;
	char buf[1024];
	string s;
	while (getline(cin, s))
		in += s;
}

int main(int argc, char **argv) {
	Scanner *sc=new Scanner();
	sc->Scan();
	delete sc;
	return 0;
}