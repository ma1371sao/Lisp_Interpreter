/**
name: Shijie Ma
date: 04/18/2017
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
#include<queue>
using namespace std;

struct AbstractV {
	string t;
	int k;
	AbstractV() {
		k = 0;
	}
};

struct Token {
	string atom;
	int val;
	char cate;	//N: Numeric Atoms; L: Literal Atoms O: '('; C: ')'; E: EOF; R: ERROR; C: NIL;
	Token() {
		cate = 'E';
	}
};

struct Node {
	Node* left;
	Node* right;
	Token tk;
	int length;
	Node() : left(NULL), right(NULL) {
		tk.cate = 'O';
	}
};

class Scanner {
private:
	int pos;
	Node* root;
	Node* sroot;
	vector<string> LA;
	string in;
	Token current;
	stack<char> pth;
public:
	Scanner();
	Token GetNextToken();
	void Init();
	void MoveToNext();
	int BuildTree(Node** root);
	void testBFS(Node* root);
	void Start();
	void listr(Node** root);
	int GetLength(Node* root);
	void DeleteTree(Node* root);
	void Eval(Node** root, Node** sroot);
	void Print(Node* root);
	bool IsListofNumbers(Node** sroot);
	string Typechecker(Node** root);
	AbstractV EmpListchecker(Node** root);
};

AbstractV Scanner::EmpListchecker(Node** root) {
	Node* node1 = *root;
	if (node1->left == NULL && node1->right == NULL) {
		if (node1->tk.atom == "T") {
			AbstractV ab;
			ab.t = "T";
			return ab;
		}
		else if (node1->tk.atom == "F") {
			AbstractV ab;
			ab.t = "F";
			return ab;
		}
		else if (node1->tk.atom == "NIL") {
			AbstractV ab;
			ab.t = "L";
			ab.k = 0;
			return ab;
		}
		else if (node1->tk.cate == 'N') {
			AbstractV ab;
			ab.t = "AN";
			return ab;
		}
		else {
			cout << "EMPTY LIST ERROR: Undefined" << endl;
			//system("pause");
			exit(0);
		}
	}
	if (node1->left != NULL && node1->right != NULL && node1->length >= 2) {
		Node* cla = node1->left;
		if (cla->tk.atom == "COND") {
			Node* cur = node1->right;
			Node* s;
			vector<AbstractV> ab;
			while (!(cur->left == NULL && cur->right == NULL && cur->tk.atom == "NIL")) {
				s = cur->left;
				ab.push_back(EmpListchecker(&(s->right->left)));
				cur = cur->right;
			}
			int flag = 1;
			for (int i = 0; i < ab.size(); i++) {
				if (ab[i].t != "AN") {
					flag = 0;
					break;
				}
			}
			if (flag) {
				AbstractV ab;
				ab.t = "AN";
				return ab;
			}
			flag = 1;
			for (int i = 0; i < ab.size(); i++) {
				if (ab[i].t != "T" && ab[i].t != "F" && ab[i].t != "AB") {
					flag = 0;
					break;
				}
			}
			if (flag) {
				AbstractV ab;
				ab.t = "AB";
				return ab;
			}
			flag = 1;
			int min = 10000;
			for (int i = 0; i < ab.size(); i++) {
				if (min > ab[i].k)
					min = ab[i].k;
			}
			AbstractV res;
			res.t = "L";
			res.k = min;
			return res;
		}
		else if (cla->tk.atom == "PLUS") {
			Node* cur = node1->right;
			if (EmpListchecker(&(cur->left)).t == "AN" && EmpListchecker(&(cur->right->left)).t == "AN") {
				AbstractV ab;
				ab.t = "AN";
				return ab;
			}
		}
		else if (cla->tk.atom == "LESS") {
			Node* cur = node1->right;
			if (EmpListchecker(&(cur->left)).t == "AN" && EmpListchecker(&(cur->right->left)).t == "AN") {
				AbstractV ab;
				ab.t = "AB";
				return ab;
			}
		}
		else if (cla->tk.atom == "EQ") {
			Node* cur = node1->right;
			if (EmpListchecker(&(cur->left)).t == "AN" && EmpListchecker(&(cur->right->left)).t == "AN") {
				AbstractV ab;
				ab.t = "AB";
				return ab;
			}
		}
		else if (cla->tk.atom == "ATOM") {
			Node* cur = node1->right;
			AbstractV s = EmpListchecker(&(cur->left));
			if (s.t == "T" || s.t == "F" || s.t == "AB" || s.t == "AN") {
				AbstractV ab;
				ab.t = "T";
				return ab;
			}
			else if (s.t == "L") {
				AbstractV ab;
				ab.t = "F";
				return ab;
			}
		}
		else if (cla->tk.atom == "INT") {
			Node* cur = node1->right;
			AbstractV s = EmpListchecker(&(cur->left));
			if (s.t == "T" || s.t == "F" || s.t == "AB" || s.t == "L") {
				AbstractV ab;
				ab.t = "F";
				return ab;
			}
			else if (s.t == "AN") {
				AbstractV ab;
				ab.t = "T";
				return ab;
			}
		}
		else if (cla->tk.atom == "NULL") {
			Node* cur = node1->right;
			AbstractV s = EmpListchecker(&(cur->left));
			if (s.t == "L" && s.k == 0) {
				AbstractV ab;
				ab.t = "AB";
				return ab;
			}
			else if (s.t == "L" && s.k >= 1) {
				AbstractV ab;
				ab.t = "F";
				return ab;
			}
		}
		else if (cla->tk.atom == "CAR") {
			Node* cur = node1->right;
			AbstractV s = EmpListchecker(&(cur->left));
			if (s.k == 0) {
				cout << "EMPTY LIST ERROR: CAR" << endl;
				//system("pause");
				exit(0);
			}
			else if (s.k >= 1) {
				AbstractV ab;
				ab.t = "AN";
				return ab;
			}
		}
		else if (cla->tk.atom == "CDR") {
			Node* cur = node1->right;
			AbstractV s = EmpListchecker(&(cur->left));
			if (s.k == 0) {
				cout << "EMPTY LIST ERROR: CDR" << endl;
				exit(0);
			}
			else if (s.k >= 1) {
				s.k--;
				return s;
			}
		}
		else if (cla->tk.atom == "CONS") {
			Node* cur = node1->right;
			AbstractV s = EmpListchecker(&(cur->right->left));
			s.k++;
			return s;
		}
		else {
			cout << "EMPTY LIST ERROR" << endl;
			exit(0);
		}
	}
	else {
		cout << "EMPTY LIST ERROR" << endl;
		exit(0);
	}
}

string Scanner::Typechecker(Node** root) {
	Node* node1 = *root;
	if (node1->left == NULL && node1->right == NULL) {
		if (node1->tk.atom == "T" && node1->left == NULL && node1->right == NULL) {
			return "B";
		}
		else if (node1->tk.atom == "F" && node1->left == NULL && node1->right == NULL) {
			return "B";
		}
		else if (node1->tk.atom == "NIL" && node1->left == NULL && node1->right == NULL) {
			return "LN";
		}
		else if (node1->tk.cate == 'N' && node1->left == NULL && node1->right == NULL) {
			return "N";
		}
		else {
			cout << "TYPE ERROR: Undefined atom type" << endl;
			exit(0);
		}
	}
	if (node1->left != NULL && node1->right != NULL && node1->length >= 2) {
		Node* cla = node1->left;
		if (cla->tk.atom == "COND") {
			Node* cur = node1->right;
			Node* s;
			string T = "";
			while (!(cur->left == NULL && cur->right == NULL && cur->tk.atom == "NIL")) {
				s = cur->left;
				if (s == NULL) {
					cout << "TYPE ERROR: COND" << endl;
					//system("pause");
					exit(0);
				}
				if (s->tk.cate != 'O') {
					cout << "TYPE ERROR: COND" << endl;
					//system("pause");
					exit(0);
				}
				if (s->length != 2) {
					cout << "TYPE ERROR: COND" << endl;
					//system("pause");
					exit(0);
				}
				string b = Typechecker(&(s->left));
				string e = Typechecker(&(s->right->left));
				if (b != "B") {
					cout << "TYPE ERROR: COND" << endl;
					//system("pause");
					exit(0);
				}
				if (T == "")
					T = e;
				else if (T != e) {
					cout << "TYPE ERROR: COND" << endl;
					//system("pause");
					exit(0);
				}
				cur = cur->right;
			}
			return T;
		}
		else if (cla->tk.atom == "PLUS") {
			if (node1->length != 3) {
				cout << "TYPE ERROR: PLUS" << endl;
				exit(0);
			}
			Node* cur = node1->right;
			if (Typechecker(&(cur->left)) == "N" && Typechecker(&(cur->right->left)) == "N")
				return "N";
			else {
				cout << "TYPE ERROR: PLUS" << endl;
				exit(0);
			}
		}
		else if (cla->tk.atom == "LESS") {
			if (node1->length != 3) {
				cout << "TYPE ERROR: LESS" << endl;
				exit(0);
			}
			Node* cur = node1->right;
			if (Typechecker(&(cur->left)) == "N" && Typechecker(&(cur->right->left)) == "N")
				return "B";
			else {
				cout << "TYPE ERROR: LESS" << endl;
				exit(0);
			}
		}
		else if (cla->tk.atom == "EQ") {
			if (node1->length != 3) {
				cout << "TYPE ERROR: EQ" << endl;
				exit(0);
			}
			Node* cur = node1->right;
			if (Typechecker(&(cur->left)) == "N" && Typechecker(&(cur->right->left)) == "N")
				return "B";
			else {
				cout << "TYPE ERROR: EQ" << endl;
				exit(0);
			}
		}
		else if (cla->tk.atom == "ATOM") {
			if (node1->length != 2) {
				cout << "TYPE ERROR: ATOM" << endl;
				exit(0);
			}
			Node* cur = node1->right;
			Typechecker(&(cur->left));
			return "B";
		}
		else if (cla->tk.atom == "INT") {
			if (node1->length != 2) {
				cout << "TYPE ERROR: INT" << endl;
				exit(0);
			}
			Node* cur = node1->right;
			Typechecker(&(cur->left));
			return "B";
		}
		else if (cla->tk.atom == "NULL") {
			if (node1->length != 2) {
				cout << "TYPE ERROR: NULL" << endl;
				exit(0);
			}
			Node* cur = node1->right;
			if (Typechecker(&(cur->left)) == "LN")
				return "B";
			else {
				cout << "TYPE ERROR: NULL" << endl;
				exit(0);
			}
		}
		else if (cla->tk.atom == "CAR") {
			if (node1->length != 2) {
				cout << "TYPE ERROR: CAR" << endl;
				exit(0);
			}
			Node* cur = node1->right;
			if (Typechecker(&(cur->left)) == "LN")
				return "N";
			else {
				cout << "TYPE ERROR: CAR" << endl;
				exit(0);
			}
		}
		else if (cla->tk.atom == "CDR") {
			if (node1->length != 2) {
				cout << "TYPE ERROR: CDR" << endl;
				exit(0);
			}
			Node* cur = node1->right;
			if (Typechecker(&(cur->left)) == "LN")
				return "LN";
			else {
				cout << "TYPE ERROR: CDR" << endl;
				exit(0);
			}
		}
		else if (cla->tk.atom == "CONS") {
			if (node1->length != 3) {
				cout << "TYPE ERROR: CONS" << endl;
				exit(0);
			}
			Node* cur = node1->right;
			if (Typechecker(&(cur->left)) == "N" && Typechecker(&(cur->right->left)) == "LN")
				return "LN";
			else {
				cout << "TYPE ERROR: CONS" << endl;
				exit(0);
			}
		}
		else {
			cout << "TYPE ERROR: invalid function name" << endl;
			exit(0);
		}
	}
	else {
		cout << "TYPE ERROR: not well typed expression" << endl;
		exit(0);
	}
}

void Scanner::Print(Node* root) {
	if (root == NULL)
		return;
	if (root->left == NULL && root->right == NULL) {
		if (root->tk.cate == 'L')
			cout << root->tk.atom;
		else if (root->tk.cate == 'N')
			cout << root->tk.val;
		else if (root->tk.atom == "NIL")
			cout << "NIL";
		return;
	}
	cout << "(";
	while (root != NULL) {
		if (root->left) {
			Print(root->left);
			if (root->right->right != NULL && root->right->left != NULL)
				cout << " ";
		}
		if (root->right == NULL && root->left == NULL) {
			if (root->tk.atom == "NIL")
				cout << ")";
			else {
				cout << " . ";
				if (root->tk.cate == 'L')
					cout << root->tk.atom;
				else if (root->tk.cate == 'N')
					cout << root->tk.val;
			}
		}
		root = root->right;
	}
}

bool Scanner::IsListofNumbers(Node** sroot) {
	Node* p = *sroot;
	if (p->left == NULL || p->right == NULL) {
		if (p->tk.atom == "NIL")
			return true;
		return false;
	}
	while (!(p->left == NULL && p->right == NULL && p->tk.atom == "NIL")) {
		if (p->left->tk.cate != 'N')
			return false;
		p = p->right;
	}
	return true;
}

void Scanner::Eval(Node** root, Node** sroot) {
	Node* node1 = *root;
	if (node1->left == NULL && node1->right == NULL) {
		if (node1->tk.atom == "T" && node1->left == NULL && node1->right == NULL) {
			(*sroot) = new Node();
			(*sroot)->tk = node1->tk;
			return;
		}
		else if (node1->tk.atom == "F" && node1->left == NULL && node1->right == NULL) {
			(*sroot) = new Node();
			(*sroot)->tk = node1->tk;
			return;
		}
		else if (node1->tk.atom == "NIL" && node1->left == NULL && node1->right == NULL) {
			(*sroot) = new Node();
			(*sroot)->tk = node1->tk;
			return;
		}
		else if (node1->tk.cate == 'N' && node1->left == NULL && node1->right == NULL) {
			(*sroot) = new Node();
			(*sroot)->tk = node1->tk;
			return;
		}
		else {
			cout << "Error: Undefined" << endl;
			//system("pause");
			exit(0);
		}
	}
	if (node1->left != NULL && node1->right != NULL && node1->length >= 2) {
		Node* cla = node1->left;
		if (cla->tk.atom == "COND") {
			Node* cur = node1->right;
			Node* s;
			while (cur != NULL) {
				s = cur->left;
				if (s == NULL) {
					cout << "Error COND: expression undefined" << endl;
					//system("pause");
					exit(0);
				}
				if (s->tk.cate != 'O') {
					cout << "Error COND: si is not a list" << endl;
					//system("pause");
					exit(0);
				}
				if (s->length != 2) {
					cout << "Error COND: length(si)!=2" << endl;
					//system("pause");
					exit(0);
				}
				Node* sroot1 = NULL;
				Eval(&(s->left), &sroot1);
				if (sroot1 == NULL) {
					cout << "Error COND: si undefined" << endl;
					//system("pause");
					exit(0);
				}
				if (sroot1->tk.atom != "F" && sroot1->tk.atom != "T") {
					cout << "Error COND: si is not a boolean value" << endl;
					//system("pause");
					exit(0);
				}
				if (sroot1->tk.atom == "T") {
					Node* sroot2 = NULL;
					Eval(&(s->right->left), &sroot2);
					(*sroot) = sroot2;
					return;
				}
				cur = cur->right;
			}
			cout << "Error COND: undefined" << endl;
			//system("pause");
			exit(0);
		}
		else if (cla->tk.atom == "PLUS") {
			if (node1->length != 3) {
				cout << "Error PLUS: length(s)!=3" << endl;
				exit(0);
			}
			Node* cur = node1->right;
			Node* sroot1 = NULL;
			Node* sroot2 = NULL;
			Eval(&(cur->left), &sroot1);
			Eval(&(cur->right->left), &sroot2);
			if (sroot1 == NULL || sroot2 == NULL) {
				cout << "Error PLUS: s1 or s2 is undefined" << endl;
				exit(0);
			}
			if (sroot1->tk.cate != 'N' || sroot2->tk.cate != 'N') {
				cout << "Error PLUS: s1 or s2 is not a numeric atom" << endl;
				//system("pause");
				exit(0);
			}
			(*sroot) = new Node();
			(*sroot)->tk.cate = 'N';
			(*sroot)->tk.val = sroot1->tk.val + sroot2->tk.val;
			return;
		}
		else if (cla->tk.atom == "LESS") {
			if (node1->length != 3) {
				cout << "Error LESS: length(s)!=3" << endl;
				exit(0);
			}
			Node* cur = node1->right;
			Node* sroot1 = NULL;
			Node* sroot2 = NULL;
			Eval(&(cur->left), &sroot1);
			Eval(&(cur->right->left), &sroot2);
			if (sroot1 == NULL || sroot2 == NULL) {
				cout << "Error LESS: s1 or s2 is undefined" << endl;
				exit(0);
			}
			if (sroot1->tk.cate != 'N' || sroot2->tk.cate != 'N') {
				cout << "Error LESS: s1 or s2 is not a numeric atom" << endl;
				//system("pause");
				exit(0);
			}
			(*sroot) = new Node();
			(*sroot)->tk.cate = 'L';
			if (sroot1->tk.val < sroot2->tk.val)
				(*sroot)->tk.atom = "T";
			else
				(*sroot)->tk.atom = "F";
			return;
		}
		else if (cla->tk.atom == "EQ") {
			if (node1->length != 3) {
				cout << "Error EQ: length(s)!=3" << endl;
				exit(0);
			}
			Node* cur = node1->right;
			Node* sroot1 = NULL;
			Node* sroot2 = NULL;
			Eval(&(cur->left), &sroot1);
			Eval(&(cur->right->left), &sroot2);
			if (sroot1 == NULL || sroot2 == NULL) {
				cout << "Error EQ: s1 or s2 is undefined" << endl;
				exit(0);
			}
			if (sroot1->tk.cate != 'N' || sroot2->tk.cate != 'N') {
				cout << "Error EQ: s1 or s2 is not a numeric atom" << endl;
				//system("pause");
				exit(0);
			}
			(*sroot) = new Node();
			(*sroot)->tk.cate = 'L';
			if (sroot1->tk.val == sroot2->tk.val)
				(*sroot)->tk.atom = "T";
			else
				(*sroot)->tk.atom = "F";
			return;
		}
		else if (cla->tk.atom == "ATOM") {
			if (node1->length != 2) {
				cout << "Error ATOM: length(s)!=2" << endl;
				exit(0);
			}
			Node* cur = node1->right;
			Node* sroot1 = NULL;
			Eval(&(cur->left), &sroot1);
			if (sroot1 == NULL) {
				cout << "Error ATOM: s1 is undefined" << endl;
				exit(0);
			}
			(*sroot) = new Node();
			(*sroot)->tk.cate = 'L';
			if (sroot1->left == NULL && sroot1->right == NULL && (sroot1->tk.atom == "T" || sroot1->tk.atom == "F" || sroot1->tk.cate == 'N'))
				(*sroot)->tk.atom = "T";
			else if (IsListofNumbers(&sroot1))
				(*sroot)->tk.atom = "F";
			else {
				cout << "Error ATOM: function is undefined" << endl;
				exit(0);
			}
			return;
		}
		else if (cla->tk.atom == "INT") {
			if (node1->length != 2) {
				cout << "Error INT: length(s)!=2" << endl;
				exit(0);
			}
			Node* cur = node1->right;
			Node* sroot1 = NULL;
			Eval(&(cur->left), &sroot1);
			if (sroot1 == NULL) {
				cout << "Error INT: s1 is undefined" << endl;
				exit(0);
			}
			(*sroot) = new Node();
			(*sroot)->tk.cate = 'L';
			if (sroot1->left == NULL && sroot1->right == NULL && sroot1->tk.cate == 'N')
				(*sroot)->tk.atom = "T";
			else if ((sroot1->left == NULL && sroot1->right == NULL && (sroot1->tk.atom == "T" || sroot1->tk.atom == "F")) || IsListofNumbers(&sroot1))
				(*sroot)->tk.atom = "F";
			else {
				cout << "Error INT: function is undefined" << endl;
				exit(0);
			}
			return;
		}
		else if (cla->tk.atom == "NULL") {
			if (node1->length != 2) {
				cout << "Error NULL: length(s)!=2" << endl;
				exit(0);
			}
			Node* cur = node1->right;
			Node* sroot1 = NULL;
			Eval(&(cur->left), &sroot1);
			if (sroot1 == NULL) {
				cout << "Error NULL: s1 is undefined" << endl;
				exit(0);
			}
			(*sroot) = new Node();
			(*sroot)->tk.cate = 'L';
			if (sroot1->left == NULL && sroot1->right == NULL && sroot1->tk.atom == "NIL")
				(*sroot)->tk.atom = "T";
			else if (IsListofNumbers(&sroot1))
				(*sroot)->tk.atom = "F";
			else {
				cout << "Error NULL: the input is not a list of numbers" << endl;
				exit(0);
			}
			return;
		}
		else if (cla->tk.atom == "CAR") {
			if (node1->length != 2) {
				cout << "Error CAR: length(s)!=2" << endl;
				exit(0);
			}
			Node* cur = node1->right;
			Node* sroot1 = NULL;
			Eval(&(cur->left), &sroot1);
			if (sroot1 == NULL) {
				cout << "Error CAR: s1 is undefined" << endl;
				exit(0);
			}
			if (sroot1->tk.atom == "NIL") {
				cout << "Error CAR: the input list is empty" << endl;
				//system("pause");
				exit(0);
			}
			if (!IsListofNumbers(&sroot1)) {
				cout << "Error CAR: the input is not a list of numbers" << endl;
				//system("pause");
				exit(0);
			}
			(*sroot) = sroot1->left;
			return;
		}
		else if (cla->tk.atom == "CDR") {
			if (node1->length != 2) {
				cout << "Error CDR: length(s)!=2" << endl;
				exit(0);
			}
			Node* cur = node1->right;
			Node* sroot1 = NULL;
			Eval(&(cur->left), &sroot1);
			if (sroot1 == NULL) {
				cout << "Error CDR: s1 is undefined" << endl;
				exit(0);
			}
			if (sroot1->tk.atom == "NIL") {
				cout << "Error CDR: the input list is empty" << endl;
				//system("pause");
				exit(0);
			}
			if (!IsListofNumbers(&sroot1)) {
				cout << "Error CDR: the input is not a list of numbers" << endl;
				//system("pause");
				exit(0);
			}
			(*sroot) = sroot1->right;
			return;
		}
		else if (cla->tk.atom == "CONS") {
			if (node1->length != 3) {
				cout << "Error CONS: length(s)!=3" << endl;
				exit(0);
			}
			Node* cur = node1->right;
			Node* sroot1 = NULL;
			Node* sroot2 = NULL;
			Eval(&(cur->left), &sroot1);
			Eval(&(cur->right->left), &sroot2);
			if (sroot1 == NULL || sroot2 == NULL) {
				cout << "Error CONS: s1 or s2 is undefined" << endl;
				exit(0);
			}
			if (sroot1->tk.cate != 'N' || !IsListofNumbers(&sroot2)) {
				cout << "Error CONS: s1 is not a numeric atom or s2 is not a list of numbers" << endl;
				exit(0);
			}
			(*sroot) = new Node();
			(*sroot)->left = sroot1;
			(*sroot)->right = sroot2;
			return;
		}
		else {
			cout << "Error: undefined eval" << endl;
			exit(0);
		}
	}
	else {
		cout << "Error: undefined eval(s)" << endl;
		//system("pause");
		exit(0);
	}
}

void Scanner::DeleteTree(Node* root) {
	if (root->left)
		DeleteTree(root->left);
	if (root->right)
		DeleteTree(root->right);
	delete root;
}

int Scanner::GetLength(Node* root) {
	if (root->left == NULL && root->right == NULL) {
		if (root->tk.cate == 'C')
			root->length = 0;
		else
			root->length = -1;
		return root->length;
	}
	if (root->right) {
		int nxtlen = GetLength(root->right);
		if (nxtlen == -1)
			root->length = -1;
		else
			root->length = nxtlen + 1;
	}
	if (root->left)
		GetLength(root->left);
	return root->length;
}

void Scanner::Start() {
	do {
		BuildTree(&root);
		GetLength(root);
		//testBFS(root);
		//system("pause");
		Typechecker(&root);
		//cout << Typechecker(&root) << endl;
		//system("pause");
		EmpListchecker(&root);
		//Eval(&root, &sroot);
		//cout << "pass" << endl;
		//testBFS(root);
		//cout << endl;
		//cout << in << endl;
		Print(root);
		//cout << "passpass" << endl;
		cout << endl;
		if (root)
			DeleteTree(root);
		//cout << "pass1" << endl;
		//if (sroot)
		//	DeleteTree(sroot);
		root = NULL;
		//sroot = NULL;
		if (current.cate == 'E')
			return;
	} while (current.cate != 'E' && current.cate != 'R');
}


void Scanner::testBFS(Node* root) {
	if (root == NULL)
		return;
	queue<Node*> q;
	q.push(root);
	while (!q.empty()) {
		Node* p = q.front();
		if (p->tk.cate == 'O')
			cout << "( ";
		else if (p->tk.cate == 'N')
			cout << p->tk.val << " ";
		else
			cout << p->tk.atom << " ";
		q.pop();
		if (p->left)	q.push(p->left);
		if (p->right)	q.push(p->right);
	}
	cout << endl;
}


int Scanner::BuildTree(Node** root) {
	if (current.cate == 'L' || current.cate == 'N') {
		Node* tmp = new Node();
		tmp->tk = current;
		*root = tmp;
		MoveToNext();
		if (current.cate == 'C')
			return 1;
		else
			return 0;
	}
	else if (current.cate == 'O') {
		int flag = 0;
		MoveToNext();
		while (current.cate != 'C' && current.cate != 'E') {
			flag = 1;
			if (current.cate == 'E') {
				cout << "ERROR: NO CLOSE PARENTHESE FOUND" << endl;
				exit(0);
			}
			listr(&(*root));
		}
		if (flag == 0) {
			Node* tmp = new Node();
			tmp->tk.atom = "NIL";
			tmp->tk.cate = 'C';
			*root = tmp;
		}
		if (current.cate == 'E') {
			cout << "ERROR: OPEN PARENTHESE MISMATCH" << endl;
			exit(0);
		}
		MoveToNext();
	}
	else {
		if (current.cate == 'E') {
			cout << "ERROR: OPEN PARENTHESE MISMATCH" << endl;
		}
		else
			cout << "ERROR TOKEN" << endl;
		exit(0);
	}
	return 0;
}

void Scanner::listr(Node** root) {
	int count = 0;
	int a;
	Node* tmp = new Node();
	tmp->tk.cate = 'O';
	*root = tmp;
	a = BuildTree(&(*root)->left);
	if (current.cate == 'C') {
		(*root)->right = new Node();
		(*root)->right->tk.atom = "NIL";
		(*root)->right->tk.cate = 'C';
		return;
	}
	listr(&(*root)->right);
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
				t.val = atoi(num.c_str());
				return t;
			}
		}
		else
			pos++;
	}
	return t;
}

Scanner::Scanner() {
	root = NULL;
	sroot = NULL;
	pos = 0;
	char buf[1024];
	string s;
	while (getline(cin, s))
		in += s;
	//getline(cin, in);
}

int main(int argc, char **argv) {
	Scanner *sc = new Scanner();
	sc->Init();
	sc->Start();
	delete sc;
	//system("pause");
	return 0;
}