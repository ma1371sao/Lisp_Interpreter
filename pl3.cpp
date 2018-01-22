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
#include<queue>
using namespace std;

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
	Node* Car(Node** root);
	Node* Cdr(Node** root);
	void Print(Node* root);
};

void Scanner::Print(Node* root) {
	if (root == NULL)
		return;
	if (root->left == NULL && root->right == NULL) {
		if (root->tk.cate == 'L')
			cout << root->tk.atom;
		else if (root->tk.cate == 'N')
			cout << root->tk.val;
		return;
	}
	int flag = 1;
	while (root != NULL) {
		if (root->left != NULL || root->right != NULL) {
			if (flag) {
				cout << "(";
				flag = 0;
			}
			Print(root->left);
			if (root->right != NULL && (root->right->left != NULL || root->right->right != NULL))
				cout << " ";
		}
		else if (root->tk.atom == "NIL") {
			cout << ")";
			return;
		}
		else {
			cout << " . " << root->tk.atom << ")";
			return;
		}
		root = root->right;
	}
}

Node* Scanner::Car(Node** root) {
	Node* node1 = *root;
	if (node1 != NULL &&  node1->left != NULL && node1->right != NULL)
		return node1->left;
	cout << "Error CAR: undefined expression" << endl;
	exit(0);
}

Node* Scanner::Cdr(Node** root) {
	Node* node1 = *root;
	if (node1 != NULL && node1->left != NULL && node1->right != NULL)
		return node1->right;
	cout << "Error CDR: undefined expression" << endl;
	exit(0);
}

void Scanner::Eval(Node** root, Node** sroot) {
	Node* node1 = *root;
	if (node1->tk.atom == "T" && node1->left == NULL && node1->right == NULL) {
		(*sroot) = new Node();
		(*sroot)->tk = node1->tk;
		return;
	}
	/*
	if (node1->tk.cate == 'L' && node1->left == NULL && node1->right == NULL) {
		(*sroot) = new Node();
		(*sroot)->tk = node1->tk;
		return;
	}
	*/
	if (node1->tk.atom == "NIL" && node1->left == NULL && node1->right == NULL) {
		(*sroot) = new Node();
		(*sroot)->tk = node1->tk;
		return;
	}
	if (node1->tk.cate == 'N' && node1->left == NULL && node1->right == NULL) {
		(*sroot) = new Node();
		(*sroot)->tk = node1->tk;
		return;
	}
	if (node1->left != NULL && node1->right != NULL) {
		Node* cla = node1->left;
		if (cla->tk.atom == "QUOTE") {
			if (node1->length != 2) {
				cout << "Error QUOTE: undefined with length!=2" << endl;
				(*sroot) = NULL;
				exit(0);
				return;
			}

			Node* tmp3 = node1->right->left;
			//(*sroot) = tmp3;

			(*sroot) = new Node();
			//(*sroot)->tk = tmp3->tk;
			queue<Node*> ori;
			queue<Node*> cpy;
			ori.push(tmp3);
			cpy.push(*sroot);
			while (!ori.empty()) {
				Node* fr = ori.front();
				Node* cfr = cpy.front();
				cfr->tk = fr->tk;
				ori.pop();
				cpy.pop();
				if (fr->left) {
					ori.push(fr->left);
					cfr->left = new Node();
					cpy.push(cfr->left);
				}
				if (fr->right) {
					ori.push(fr->right);
					cfr->right = new Node();
					cpy.push(cfr->right);
				}
			}

			return;
		}
		if (cla->tk.atom == "COND") {
			Node* cur = node1->right;
			Node* s;
			while (cur != NULL) {
				s = Car(&(cur));
				if (s == NULL) {
					cout << "Error COND: expression undefined" << endl;
					(*sroot) = NULL;
					exit(0);
					return;
				}
				if (s->tk.cate != 'O') {
					cout << "Error COND: si is not a list" << endl;
					(*sroot) = NULL;
					exit(0);
					return;
				}
				if (s->length != 2) {
					cout << "Error COND: length(si)!=2" << endl;
					(*sroot) = NULL;
					exit(0);
					return;
				}
				Node* sroot1 = NULL;
				Eval(&(s->left), &sroot1);

				//Eval(&(s->left), sroot);
				if (sroot1 == NULL) {
					cout << "Error COND: s1 undefined" << endl;
					exit(0);
					return;
				}
				if (sroot1->tk.atom != "NIL") {
					Node* sroot2 = NULL;
					Eval(&(s->right->left), &sroot2);
					(*sroot) = sroot2;
					return;
				}
				cur = cur->right;
			}
			cout << "Error COND: undefined" << endl;
			(*sroot) = NULL;
			exit(0);
			return;
		}
		if (cla->tk.atom == "MINUS") {
			if (node1->length != 3) {
				cout << "Error MINUS: length(s)!=3" << endl;
				(*sroot) = NULL;
				exit(0);
				return;
			}
			Node* cur = node1->right;
			Node* sroot1 = NULL;
			Node* sroot2 = NULL;
			Eval(&(cur->left), &sroot1);
			Eval(&(cur->right->left), &sroot2);
			if (sroot1 == NULL || sroot2 == NULL) {
				cout << "Error MINUS: s1 or s2 is undefined" << endl;
				*(sroot) = NULL;
				exit(0);
				return;
			}
			if (sroot1->tk.cate != 'N' || sroot2->tk.cate != 'N') {
				cout << "Error MINUS: s1 or s2 is not numeric atom" << endl;
				*(sroot) = NULL;
				exit(0);
				return;
			}
			(*sroot) = new Node();
			(*sroot)->tk.cate = 'N';
			(*sroot)->tk.val = sroot1->tk.val - sroot2->tk.val;
			return;
		}
		if (cla->tk.atom == "PLUS") {
			if (node1->length != 3) {
				cout << "Error PLUS: length(s)!=3" << endl;
				(*sroot) = NULL;
				exit(0);
				return;
			}
			Node* cur = node1->right;
			Node* sroot1 = NULL;
			Node* sroot2 = NULL;
			Eval(&(cur->left), &sroot1);
			Eval(&(cur->right->left), &sroot2);
			if (sroot1 == NULL || sroot2 == NULL) {
				cout << "Error PLUS: s1 or s2 is undefined" << endl;
				*(sroot) = NULL;
				exit(0);
				return;
			}
			if (sroot1->tk.cate != 'N' || sroot2->tk.cate != 'N') {
				cout << "Error PLUS: s1 or s2 is not numeric atom" << endl;
				*(sroot) = NULL;
				exit(0);
				return;
			}
			(*sroot) = new Node();
			(*sroot)->tk.cate = 'N';
			(*sroot)->tk.val = sroot1->tk.val + sroot2->tk.val;
			return;
		}
		if (cla->tk.atom == "TIMES") {
			if (node1->length != 3) {
				cout << "Error TIMES: length(s)!=3" << endl;
				(*sroot) = NULL;
				exit(0);
				return;
			}
			Node* cur = node1->right;
			Node* sroot1 = NULL;
			Node* sroot2 = NULL;
			Eval(&(cur->left), &sroot1);
			Eval(&(cur->right->left), &sroot2);
			if (sroot1 == NULL || sroot2 == NULL) {
				cout << "Error TIMES: s1 or s2 is undefined" << endl;
				*(sroot) = NULL;
				exit(0);
				return;
			}
			if (sroot1->tk.cate != 'N' || sroot2->tk.cate != 'N') {
				cout << "Error TIMES: s1 or s2 is not numeric atom" << endl;
				*(sroot) = NULL;
				exit(0);
				return;
			}
			(*sroot) = new Node();
			(*sroot)->tk.cate = 'N';
			(*sroot)->tk.val = sroot1->tk.val * sroot2->tk.val;
			return;
		}
		if (cla->tk.atom == "LESS") {
			if (node1->length != 3) {
				cout << "Error LESS: length(s)!=3" << endl;
				(*sroot) = NULL;
				exit(0);
				return;
			}
			Node* cur = node1->right;
			Node* sroot1 = NULL;
			Node* sroot2 = NULL;
			Eval(&(cur->left), &sroot1);
			Eval(&(cur->right->left), &sroot2);
			if (sroot1 == NULL || sroot2 == NULL) {
				cout << "Error LESS: s1 or s2 is undefined" << endl;
				*(sroot) = NULL;
				exit(0);
				return;
			}
			if (sroot1->tk.cate != 'N' || sroot2->tk.cate != 'N') {
				cout << "Error LESS: s1 or s2 is not numeric atom" << endl;
				*(sroot) = NULL;
				exit(0);
				return;
			}
			(*sroot) = new Node();
			(*sroot)->tk.cate = 'L';
			if (sroot1->tk.val < sroot2->tk.val)
				(*sroot)->tk.atom = "T";
			else
				(*sroot)->tk.atom = "NIL";
			return;
		}
		if (cla->tk.atom == "GREATER") {
			if (node1->length != 3) {
				cout << "Error GREATER: length(s)!=3" << endl;
				(*sroot) = NULL;
				exit(0);
				return;
			}
			Node* cur = node1->right;
			Node* sroot1 = NULL;
			Node* sroot2 = NULL;
			Eval(&(cur->left), &sroot1);
			Eval(&(cur->right->left), &sroot2);
			if (sroot1 == NULL || sroot2 == NULL) {
				cout << "Error GREATER: s1 or s2 is undefined" << endl;
				*(sroot) = NULL;
				exit(0);
				return;
			}
			if (sroot1->tk.cate != 'N' || sroot2->tk.cate != 'N') {
				cout << "Error GREATER: s1 or s2 is not numeric atom" << endl;
				*(sroot) = NULL;
				exit(0);
				return;
			}
			(*sroot) = new Node();
			(*sroot)->tk.cate = 'L';
			if (sroot1->tk.val > sroot2->tk.val)
				(*sroot)->tk.atom = "T";
			else
				(*sroot)->tk.atom = "NIL";
			return;
		}
		if (cla->tk.atom == "EQ") {
			if (node1->length != 3) {
				cout << "Error EQ: length(s)!=3" << endl;
				(*sroot) = NULL;
				exit(0);
				return;
			}
			Node* cur = node1->right;
			Node* sroot1 = NULL;
			Node* sroot2 = NULL;
			Eval(&(cur->left), &sroot1);
			Eval(&(cur->right->left), &sroot2);
			if (sroot1 == NULL || sroot2 == NULL) {
				cout << "Error EQ: s1 or s2 is undefined" << endl;
				*(sroot) = NULL;
				exit(0);
				return;
			}
			if ((sroot1->tk.cate != 'N' && sroot1->tk.cate != 'L') || (sroot2->tk.cate != 'N' && sroot2->tk.cate != 'L')) {
				cout << "Error EQ: s1 or s2 is something other than an atom" << endl;
				*(sroot) = NULL;
				exit(0);
				return;
			}
			if (sroot1->left == NULL && sroot1->right == NULL && sroot2->left == NULL && sroot2->right == NULL) {
				(*sroot) = new Node();
				(*sroot)->tk.cate = 'L';
				if (sroot1->tk.cate == 'L' && sroot2->tk.cate == 'L' && sroot1->tk.atom == sroot2->tk.atom)
					(*sroot)->tk.atom = "T";
				else if (sroot1->tk.cate == 'N' && sroot2->tk.cate == 'N' && sroot1->tk.val == sroot2->tk.val)
					(*sroot)->tk.atom = "T";
				else
					(*sroot)->tk.atom = "NIL";
			}
			else {
				cout << "Error EQ: s1 or s2 has more than one node" << endl;
				*(sroot) = NULL;
				exit(0);
			}
			return;
		}
		if (cla->tk.atom == "ATOM") {
			if (node1->length != 2) {
				cout << "Error ATOM: length(s)!=2" << endl;
				(*sroot) = NULL;
				exit(0);
				return;
			}
			Node* cur = node1->right;
			Node* sroot1 = NULL;
			Eval(&(cur->left), &sroot1);
			if (sroot1 == NULL) {
				cout << "Error ATOM: s1 is undefined" << endl;
				(*sroot) = NULL;
				exit(0);
				return;
			}
			(*sroot) = new Node();
			(*sroot)->tk.cate = 'L';
			if (sroot1->left == NULL && sroot1->right == NULL)
				(*sroot)->tk.atom = "T";
			else
				(*sroot)->tk.atom = "NIL";
			return;
		}
		if (cla->tk.atom == "INT") {
			if (node1->length != 2) {
				cout << "Error INT: length(s)!=2" << endl;
				(*sroot) = NULL;
				exit(0);
				return;
			}
			Node* cur = node1->right;
			Node* sroot1 = NULL;
			Eval(&(cur->left), &sroot1);
			if (sroot1 == NULL) {
				cout << "Error INT: s1 is undefined" << endl;
				(*sroot) = NULL;
				exit(0);
				return;
			}
			(*sroot) = new Node();
			(*sroot)->tk.cate = 'L';
			if (cur->left->left == NULL && cur->left->right == NULL && cur->left->tk.cate == 'N')
				(*sroot)->tk.atom = "T";
			else
				(*sroot)->tk.atom = "NIL";
			return;
		}
		if (cla->tk.atom == "NULL") {
			if (node1->length != 2) {
				cout << "Error NULL: length(s)!=2" << endl;
				(*sroot) = NULL;
				exit(0);
				return;
			}
			Node* cur = node1->right;
			Node* sroot1 = NULL;
			Eval(&(cur->left), &sroot1);
			if (sroot1 == NULL) {
				cout << "Error NULL: s1 is undefined" << endl;
				(*sroot) = NULL;
				exit(0);
				return;
			}
			(*sroot) = new Node();
			(*sroot)->tk.cate = 'L';
			if (cur->left->left == NULL && cur->left->right == NULL && cur->left->tk.atom == "NIL")
				(*sroot)->tk.atom = "T";
			else
				(*sroot)->tk.atom = "NIL";
			return;
		}
		if (cla->tk.atom == "CAR") {
			if (node1->length != 2) {
				cout << "Error CAR: length(s)!=2" << endl;
				(*sroot) = NULL;
				exit(0);
				return;
			}
			Node* cur = node1->right;
			Node* sroot1 = NULL;
			Eval(&(cur->left), &sroot1);
			if (sroot1 == NULL) {
				cout << "Error CAR: s1 is undefined" << endl;
				(*sroot) = NULL;
				exit(0);
				return;
			}
			if (sroot1->tk.cate == 'N' || sroot1->tk.cate == 'L') {
				cout << "Error CAR: s1 is an atom" << endl;
				(*sroot) = NULL;
				exit(0);
				return;
			}
			if (sroot1->left == NULL && sroot1->right == NULL) {
				cout << "Error CAR: input binary tree contains only one node" << endl;
				(*sroot) = NULL;
				exit(0);
				return;
			}
			(*sroot) = sroot1->left;
			return;
		}
		if (cla->tk.atom == "CDR") {
			if (node1->length != 2) {
				cout << "Error CDR: length(s)!=2" << endl;
				(*sroot) = NULL;
				exit(0);
				return;
			}
			Node* cur = node1->right;
			Node* sroot1 = NULL;
			Eval(&(cur->left), &sroot1);
			if (sroot1 == NULL) {
				cout << "Error CDR: s1 is undefined" << endl;
				(*sroot) = NULL;
				exit(0);
				return;
			}
			if (sroot1->tk.cate == 'N' || sroot1->tk.cate == 'L') {
				cout << "Error CDR: s1 is an atom" << endl;
				(*sroot) = NULL;
				exit(0);
				return;
			}
			if (sroot1->left == NULL && sroot1->right == NULL) {
				cout << "Error CDR: input binary tree contains only one node" << endl;
				(*sroot) = NULL;
				exit(0);
				return;
			}
			(*sroot) = sroot1->right;
			return;
		}
		if (cla->tk.atom == "CONS") {
			if (node1->length != 3) {
				cout << "Error CONS: length(s)!=3" << endl;
				(*sroot) = NULL;
				exit(0);
				return;
			}
			Node* cur = node1->right;
			Node* sroot1 = NULL;
			Node* sroot2 = NULL;
			Eval(&(cur->left), &sroot1);
			Eval(&(cur->right->left), &sroot2);
			if (sroot1 == NULL || sroot2 == NULL) {
				cout << "Error CONS: s1 or s2 is undefined" << endl;
				*(sroot) = NULL;
				exit(0);
				return;
			}
			(*sroot) = new Node();
			(*sroot)->left = sroot1;
			(*sroot)->right = sroot2;
			return;
		}
		else {
			cout << "Error: undefined eval" << endl;
			(*sroot) = NULL;
			exit(0);
			return;
		}
	}
	else {
		cout << "Error: undefined eval" << endl;
		(*sroot) = NULL;
		//system("pause");
		exit(0);
		return;
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
		Eval(&root, &sroot);
		//cout << "pass" << endl;
		//testBFS(sroot);
		//cout << endl;
		Print(sroot);
		//cout << "passpass" << endl;
		cout << endl;
		if (root)
			DeleteTree(root);
		//cout << "pass1" << endl;
		if (sroot)
			DeleteTree(sroot);
		root = NULL;
		sroot = NULL;
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
	//while (getline(cin, s))
	//	in += s;
	getline(cin, in);
}

int main(int argc, char **argv) {
	Scanner *sc = new Scanner();
	sc->Init();
	sc->Start();
	delete sc;
	system("pause");
	return 0;
}