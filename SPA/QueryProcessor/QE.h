#pragma once
#include <vector>
#include <string>
#include <map>
#include <queue>

#include "Subquery.h"
#include "ResultTuple.h"

using namespace std;

class QE {
private:
	bool useOptimizedSolver;
	PKBController* pkb;

public:
	map<string, string> synonymTable;
	vector<string> synonyms;
	ResultTuple* solution;
	vector<ResultTuple*> answers;
	vector<Subquery*> queries;
	bool validateQueries();
	QE(vector<string>, PKBController*);
	void setSynonymTable(map<string, string>);
	void addQuery(Subquery*);
	map<int, list<string>> memo;

	list<string> getAnswers(vector<int>, vector<int>, int, int);
	list<string> solve();
	void trimSolution();
	void basicSolve();
	vector<string> convert();
	list<string> convertSolutionToString();
};