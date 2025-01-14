#pragma once
#include <string>
#include <set>
#include "..\Frontend\TNode.h"
using namespace std;

//public functions
class AST{
public:
	//Constructor
    AST();
	//Accessor for AST
	string getTNodeType(TNode *n);
    string getTNodeValue(TNode *n);
    int getTNodeStmtNum(TNode *n);
    int getTNodeProcedure(TNode *n);
	TNode* getTNodeParent(TNode *n);
	TNode* getTNodeChild(TNode *n, int);
     vector<TNode*> getTNodeChildren(TNode *n);
	TNode* getRightSibling(TNode *n);
	 //Mutator for AST 
	 TNode createNode(string, string, int,int);
	 TNode* assignChild(TNode *n, TNode *c);
     TNode* assignParent(TNode *n, TNode *p); 
	 TNode* assignRightSibling(TNode *n, TNode *r);
     bool isNodeType(TNode *n,string); 
     bool isNodeValue(TNode *n,string ); 
     void insertRoot(TNode *n);
     TNode* getRootNode();
	 //pattern matching
	 static bool matchPattern(string,string);
private:
vector<TNode * > root;
};