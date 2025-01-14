#pragma once
#include <utility>
#include <vector>
#include <iostream>
#include <set>

using namespace std;

class Sibling {

private :

	vector<pair<int,int>> procProc;
	vector<pair<int,int>> stmtLstStmtLst; // then-else
	vector<pair<int,int>> stmtStmt;
	vector<pair<int,int>> varStmtLst;  // var-then, var-else
	vector<pair<int,int>> varConst; //assign 
	vector<pair<int,int>> varPlus;
	vector<pair<int,int>> varMinus;
	vector<pair<int,int>> varTimes;
	vector<pair<int,int>> constStmtLst;
	vector<pair<int,int>> constPlus;
	vector<pair<int,int>> constMinus;
	vector<pair<int,int>> constTimes;
	vector<pair<int,int>> plusPlus;
	vector<pair<int,int>> plusMinus;
	vector<pair<int,int>> plusTimes;
	vector<pair<int,int>> minusMinus;
	vector<pair<int,int>> minusTimes;
	vector<pair<int,int>> timesTimes;
	vector<pair<int,int>> constConst;
	vector<pair<int,int>> varVar;

public :

	Sibling();
	
	//functions to insert into the siblings table
	void insertProcProc(int,int);
	void insertStmtLstStmtLst(int,int);
	void insertStmtStmt(int,int);
	void inserVarStmtLst(int,int);
	void insertVarConst(int,int);
	void insertVarPlus(int,int);
	void insertVarMinus(int,int);
	void insertVarTimes(int,int);
	void insertConstStmtLst(int,int);
	void insertConstConst(int,int);
	void insertConstPlus(int,int);
	void insertConstMinus(int,int);
	void insertConstTimes(int,int);
	void insertPlusPlus(int,int);
	void insertPlusMinus(int,int);
	void insertPlusTimes(int,int);
	void insertMinusMinus(int,int);
	void insertMinusTimes(int,int);
	void insertTimesTimes(int,int);
	void insertVarVar(int,int);

	//accessors
	vector<pair<int,int>> getProcProcSibling();
	vector<pair<int,int>> getStmtLstStmtLstSibling();
	vector<pair<int,int>> getStmtStmtSibling();
	vector<pair<int,int>> getVarStmtLstSibling();  // var-then, var-else
	vector<pair<int,int>> getVarConstSibling(); //assign 
	vector<pair<int,int>> getVarPlusSibling();
	vector<pair<int,int>> getVarMinusSibling();
	vector<pair<int,int>> getVarTimesSibling();
	vector<pair<int,int>> getConstPlusSibling();
	vector<pair<int,int>> getConstMinusSibling();
	vector<pair<int,int>> getConstTimesSibling();
	vector<pair<int,int>> getPlusPlusSibling();
	vector<pair<int,int>> getPlusMinusSibling();
	vector<pair<int,int>> getPlusTimesSibling();
	vector<pair<int,int>> getMinusMinusSibling();
	vector<pair<int,int>> getMinusTimesSibling();
	vector<pair<int,int>> getTimesTimesSibling();
	vector<pair<int,int>> getVarVarSibling();
	vector<pair<int,int>> getConstConstSibling();
	vector<pair<int,int>> getConstStmtLstSibling();

	set<int> getProcsSiblingOfProc(int);
	set<int> getStmtLstSiblingOfStmtLst(int);
	set<int> getStmtSiblingOfStmt(int);
	set<int> getVarSiblingOfConst(int);
	set<int> getConstSiblingOfVar(int);
	set<int> getVarSiblingOfVar(int);
	set<int> getConstSiblingOfConst(int);

	// MISSING
	set<int> getVarSiblingOfStmtLst(int);
	set<int> getStmtLstSiblingOfVar(int);
	set<int> getPlusSiblingOfVar(int);
	set<int> getMinusSiblingOfVar(int);
	set<int> getTimesSiblingOfVar(int);
	set<int> getPlusSiblingOfConst(int);
	set<int> getMinusSiblingOfConst(int);
	set<int> getTimesSiblingOfConst(int);
	set<int> getVarSiblingOfPlus(int);
	set<int> getConstSiblingOfPlus(int);
	set<int> getPlusSiblingOfPlus(int);
	set<int> getMinusSiblingOfPlus(int);
	set<int> getTimesSiblingOfPlus(int);
	set<int> getVarSiblingOfMinus(int);
	set<int> getConstSiblingOfMinus(int);
	set<int> getPlusSiblingOfMinus(int);
	set<int> getMinusSiblingOfMinus(int);
	set<int> getTimesSiblingOfMinus(int);
	set<int> getVarSiblingOfTimes(int);
	set<int> getConstSiblingOfTimes(int);
	set<int> getPlusSiblingOfTimes(int);
	set<int> getMinusSiblingOfTimes(int);
	set<int> getTimesSiblingOfTimes(int);

	set<int> getAllProcsSiblingOfProc();
	set<int> getAllStmtLstSiblingOfStmtLst();
	set<int> getAllStmtSiblingOfStmt();
	set<int> getAllVarSiblingOfConst();
	set<int> getAllConstSiblingOfVar();
	set<int> getAllVarSiblingOfVar();
	set<int> getAllVarSiblingOfMinus();
	set<int> getAllVarSiblingOfPlus();
	set<int> getAllVarSiblingOfTimes();
	set<int> getAllConstSiblingOfPlus();
	set<int> getAllConstSiblingOfMinus();
	set<int> getAllConstSiblingOfTimes();
	set<int> getAllConstSiblingOfConst();
	set<int> getAllVarSiblingOfStmtLst();
	set<int> getAllStmtLstSiblingOfVar();

	//boolean functions
	bool isSiblingProcProc(int,int);
	bool isSiblingVarVar(int,int);
	bool isSiblingStmtLstStmtLst(int,int);
	bool isSiblingStmtStmt(int,int);
	bool isSiblingVarStmtLst(int,int);
	bool isSiblingVarConst(int,int);
	bool isSiblingVarPlus(int,int);
	bool isSiblingVarMinus(int,int);
	bool isSiblingVarTimes(int,int);
	bool isSiblingConstConst(int,int);
	bool isSiblingConstPlus(int,int);
	bool isSiblingConstMinus(int,int);
	bool isSiblingConstStmtLst(int,int);
	bool isSiblingConstTimes(int,int);
	bool isSiblingPlusPlus(int,int);
	bool isSiblingPlusMinus(int,int);
	bool isSiblingPlusTimes(int,int);
	bool isSiblingMinusMinus(int,int);
	bool isSiblingMinusTimes(int,int);
	bool isSiblingTimesTimes(int,int);

};