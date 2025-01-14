#pragma once

#include <stack>
#include <map>
#include <set>
#include "../PKB/PKBController.h"
#include "TNode.h"
#include "Operator.h"
#include "Lexeme.h"
#include "Lexer.h"

using namespace std;

class Parser {
	string filepath;
	string postfix;
	Lexeme nextToken;
	int line;
	int currentProc;
	int previousStmt;
	TNode* prev;
	int lastVarIndex;
	string procName;
	Lexer lexer;
	stack<Operator> operatorStack;
	stack<TNode*> operandStack;
	stack<int> containerStack;
	stack<TNode*> containerNodeStack;
	vector<string> procedureNames;
	map<int, string> callStatements;
	map<int, set<pair<int, string>>> callees;
	map<int, stack<int>> procAndContainers;

	int totalParents;
	int totalSiblings;

public:
	PKBController* controller;
	Parser();
	Parser(string);
	void parse();
	~Parser();
	void printOut();
	PKBController getController();
	int getTotalStatementNumber();

private:
	bool checkFileExists();
	Lexeme getToken();
	void match(string);
	void program();
	void procedure();
	string procedureName();
	void stmtLst(TNode*);
	void stmt(TNode*);
	void expr(TNode*);
	void exprPrime(TNode*);
	void term(TNode*);
	void termPrime(TNode*);
	void factor(TNode*);
	void variableName();
	void constantValue();
	TNode* createASTNode(string, string, TNode*, int, int);
	int getProcedureIndex(string);
	void debug();

	void populateParent(TNode*, int);
	void populateFollows(int, bool, TNode*, TNode* );
	void populateUses(int, int, bool = false);
	void populateModifies(int, int);
	void populateCallsModifies(int, int);
	void populateRoot(TNode*, int);

	void popOperator(Operator);

	void populateAdditionalModifiesInfo();
	void populateAdditionalUsesInfo();

	void populateProcAndContainers(int, int);
	void constructRelations();
};