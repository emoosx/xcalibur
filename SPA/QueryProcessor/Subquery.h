#pragma once
#include <string>
#include <vector>
#include <map>
#include "QueryTree.h"
#include "..\PKB\PKBController.h"
#include "ResultTuple.h"

using namespace std;

class Subquery{

private: 
	int priority;

public:
	PKBController* pkb;
	map<string, string>* synonymTable;
	int leftIndex, rightIndex;
	string leftSynonym, rightSynonym;
	map<string, TNODE_TYPE> synToNodeType;
	int isSyn;	// variable to indicate how many synonyms are there,
				// 0: both are int, 1: left int, right string
				// 2: left string, right int, 3: both are string
				// 4: ("_", syn)
				// 5: (syn, "_")
				// 6: ("_", "_")
				// 7: ("_", dig)
				// 8: (dig, "_")

	// constructor
	Subquery(map<string, string>*, PKBController*);

	// Mutators
	void setPriority(int);
	void setSynonyms(string, string);
	void setSynonyms(string, int);
	void setSynonyms(int, string);
	void setSynonyms(int, int);

	// Accessors	
	int getPriority();
	virtual ResultTuple* solve();
	virtual ResultTuple* solve(ResultTuple*);
};