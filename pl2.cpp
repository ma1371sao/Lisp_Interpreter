/**
the first two lines may be disappeared in linux
name: Shijie Ma
date: 01/21/2017
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
#include<stack>
using namespace std;

struct Token {
	string atom;
	char cate;	//N: Numeric Atoms; L: Literal Atoms O: '('; C: ')'; E: EOF; R: ERROR
	Token() {
		cate = 'E';
	}
};

class Scanner {
private:
	int n_LA;
	int n_NA;
	int sum_NA;
	int n_OP;
	int n_CP;
	int pos;
	vector<string> LA;
	string in;
	Token current;
	stack<char> pth;
public:
	Scanner();
	Token GetNextToken();
	void Init();
	void MoveToNext();
	void ParseStart();
	bool ParseExpr(string& s, int num);
};

bool Scanner::ParseExpr(string& s, int num) {
	if (current.cate == 'L' || current.cate == 'N') {
		s += current.atom;
		MoveToNext();
	}
	else if (current.cate == 'O') {
		int count = 0;
		MoveToNext();
		if (current.cate != 'C') {
			s += "(";
			pth.push('(');
		}
		else {
			s += "NIL";
			MoveToNext();
			return true;
		}
		do
		{
			char c = current.cate;
			if (!ParseExpr(s, count))
				return false;
			if (current.cate == 'E') {
				if (!pth.empty()) {
					cout << "ERROR: invalid parentheses input" << endl;
					return false;
				}
				for (int i = 0; i < count; i++)
					s += ")";
				break;
			}
			if (current.cate != 'C') {
				s += " . (";
				count++;
			}
			else
				s += " . ";
		} while (current.cate != 'C');
		if (current.cate == 'C') {
			if (pth.empty()) {
				cout << "ERROR: invalid parentheses input" << endl;
				return false;
			}
			pth.pop();
			s += "NIL)";
			for (int i = 0; i < count; i++)
				s += ")";
		}
		MoveToNext();
	}
	else {
		if(current.cate=='R')
			cout << "ERROR: error token" << endl;
		if (current.cate == 'C')
			cout << "ERROR: syntactically-invalid input" << endl;
		return false;
	}
	return true;
}

void Scanner::ParseStart() {
	do {
		string s;
		if (!ParseExpr(s, 0))
			return;
		cout << s << endl;
	} while (current.cate!='E' && current.cate!='R');
}

void Scanner::MoveToNext() {
	current = GetNextToken();
}

void Scanner::Init() {
	current = GetNextToken();
}

Token Scanner::GetNextToken() {
	Token t;
	while (pos < in.length()) {
		if (in[pos] == '(') {
			pos++;
			t.cate = 'O';
			return t;
		}
		else if (in[pos] == ')') {
			pos++;
			t.cate = 'C';
			return t;
		}
		else if (in[pos] >= 'A' && in[pos] <= 'Z') {
			t.cate = 'L';
			while (pos < in.length() && (in[pos] >= 'A' && in[pos] <= 'Z') || (in[pos] >= '0' && in[pos] <= '9')) {
				t.atom.push_back(in[pos]);
				pos++;
			}
			return t;
		}
		else if (in[pos] >= '0' && in[pos] <= '9') {
			int flag = 0;
			string num;
			while (pos < in.length() && (in[pos] >= 'A' && in[pos] <= 'Z') || (in[pos] >= '0' && in[pos] <= '9')) {
				num.push_back(in[pos]);
				if (in[pos] >= 'A' && in[pos] <= 'Z')
					flag = 1;
				pos++;
			}
			if (flag) {
				t.cate = 'R';
				return t;
			}
			else {
				t.cate = 'N';
				t.atom = num;
				return t;
			}
		}
		else
			pos++;
	}
	return t;
}

Scanner::Scanner() {
	n_OP = 0;
	n_CP = 0;
	n_LA = 0;
	n_NA = 0;
	sum_NA = 0;
	pos = 0;
	char buf[1024];
	string s;
	//while (getline(cin, s))
	//	in += s;
	getline(cin, in);
}

int main(int argc, char **argv) {
	Scanner *sc = new Scanner();
	sc->Init();
	sc->ParseStart();
	delete sc;
	system("pause");
	return 0;
}