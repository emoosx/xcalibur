#pragma once
#include <string>
#include <vector>
#include <map>
#include "Subquery.h"

using namespace std;

class NextSubquery : public Subquery {
public:
	NextSubquery(map<string, string>* m, PKBController* p) : Subquery(m, p){
	
	}

	ResultTuple* solve(){
		ResultTuple* ans;
		switch (isSyn) {
			case 1: case 4:
				ans = solveRightSyn();
				break;
			case 2: case 5:
				ans = solveLeftSyn();
				break;
			case 3:
				ans = solveBothSyn();
				break;
			default:
				ans = solveOther();
		}
		return ans;
	}
	
	bool validate() {
		//right synonym
		if(isSyn == 1 || isSyn == 4 || isSyn == 3) {
			string s = synonymTable->at(rightSynonym);
			if (s == "procedure" || s == "constant" || s == "variable") {
				return false;
			}
		}
		//left synonym
		if (isSyn == 2 || isSyn == 5 || isSyn == 3) {
			string s = synonymTable->at(leftSynonym);
			if (s == "procedure" || s == "constant" || s == "variable") {
				return false;
			}
		}
		if (isSyn == 3 && leftSynonym == rightSynonym) {
			return false;
		}
		return true;
	}

	void setPriority() {
		int magnitude = 5;	//calculated during profiling
		bool isLeftSynStmtOrProgLine = true;
		bool isRightSynStmtOrProgLine = true;
		switch (isSyn) {
			case 1: // (int, syn)
				isRightSynStmtOrProgLine = (synonymTable->at(rightSynonym) == "stmt" || synonymTable->at(rightSynonym) == "prog_line");
				if (isRightSynStmtOrProgLine){
					priority = pkb->statementTable->getAllStmtNum().size();
				} else {
					priority = (pkb->statementTable->getStmtNumUsingNodeType(TNODE_NAMES[synToNodeType.at(synonymTable->at(rightSynonym))])).size();
				}
				break;
			case 2: // (syn, int)
				isLeftSynStmtOrProgLine = (synonymTable->at(leftSynonym) == "stmt" || synonymTable->at(leftSynonym) == "prog_line");
				if (isLeftSynStmtOrProgLine){
					priority = pkb->statementTable->getAllStmtNum().size();
				} else {
					priority = (pkb->statementTable->getStmtNumUsingNodeType(TNODE_NAMES[synToNodeType.at(synonymTable->at(leftSynonym))])).size();
				}
				break;
			case 3: // (syn, syn)
				if (leftSynonym == rightSynonym) {
					priority = 0;
					break;
				}

				isLeftSynStmtOrProgLine = (synonymTable->at(leftSynonym) == "stmt" || synonymTable->at(leftSynonym) == "prog_line");
				isRightSynStmtOrProgLine = (synonymTable->at(rightSynonym) == "stmt" || synonymTable->at(rightSynonym) == "prog_line");
				if (isLeftSynStmtOrProgLine && isRightSynStmtOrProgLine){
					priority = pkb->statementTable->getAllStmtNum().size() * pkb->statementTable->getAllStmtNum().size();
				} else if (isLeftSynStmtOrProgLine){
					priority = pkb->statementTable->getAllStmtNum().size() * (pkb->statementTable->getStmtNumUsingNodeType(TNODE_NAMES[synToNodeType.at(synonymTable->at(rightSynonym))])).size();
				} else if (isRightSynStmtOrProgLine){
					priority = (pkb->statementTable->getStmtNumUsingNodeType(TNODE_NAMES[synToNodeType.at(synonymTable->at(leftSynonym))])).size() * pkb->statementTable->getAllStmtNum().size();
				} else {
					priority = (pkb->statementTable->getStmtNumUsingNodeType(TNODE_NAMES[synToNodeType.at(synonymTable->at(leftSynonym))])).size() * (pkb->statementTable->getStmtNumUsingNodeType(TNODE_NAMES[synToNodeType.at(synonymTable->at(rightSynonym))])).size();
				}
				break;
			case 4: // (_, syn)
				isRightSynStmtOrProgLine = (synonymTable->at(rightSynonym) == "stmt" || synonymTable->at(rightSynonym) == "prog_line");
				if (isRightSynStmtOrProgLine){
					priority = pkb->statementTable->getAllStmtNum().size();
				} else {
					priority = (pkb->statementTable->getStmtNumUsingNodeType(TNODE_NAMES[synToNodeType.at(synonymTable->at(rightSynonym))])).size();
				}
				break;
			case 5:	// (syn, _)
				isLeftSynStmtOrProgLine = (synonymTable->at(leftSynonym) == "stmt" || synonymTable->at(leftSynonym) == "prog_line");
				if (isLeftSynStmtOrProgLine){
					priority = pkb->statementTable->getAllStmtNum().size();
				} else {
					priority = (pkb->statementTable->getStmtNumUsingNodeType(TNODE_NAMES[synToNodeType.at(synonymTable->at(leftSynonym))])).size();
				}
				break;
			default: 
				priority = 0;
				break;
		}

		priority *= magnitude;
	}

	ResultTuple* solve(ResultTuple* tuple) {
		ResultTuple* ans;
		switch (isSyn) {
			case 1: case 4:
				ans = solveRightSyn(tuple);
				break;
			case 2: case 5:
				ans = solveLeftSyn(tuple);
				break;
			case 3:
				ans = solveBothSyn(tuple);
				break;
			default: break;
		}
		return ans;
	}

	ResultTuple* solveLeftSyn() {
		ResultTuple* tuple = new ResultTuple();
		int index = tuple->addSynonym(leftSynonym);
		tuple->addSynonymToMap(leftSynonym, index);
		set<int> tempPrevious;
		if (isSyn == 2) {	// Next(syn, stmt): Get Previous of stmt
			tempPrevious = pkb->nextExtractor->getPrev(rightIndex);
		} else {	// Next(syn, _): Get all Previous stmt
			// getAllPrevious Statements
			tempPrevious = pkb->nextExtractor->getAllPrev();
		}
		vector<int> Previous(tempPrevious.begin(), tempPrevious.end());
		for(size_t i = 0; i < Previous.size(); i++) {
			vector<int> temp = vector<int>();
			// synonym type check here
			if ((synonymTable->at(leftSynonym)=="assign" || synonymTable->at(leftSynonym)=="while" || synonymTable->at(leftSynonym)=="if" || synonymTable->at(leftSynonym)=="call")
				&& pkb->statementTable->getTNode(Previous[i])->getNodeType()!=TNODE_NAMES[synToNodeType.at(synonymTable->at(leftSynonym))]){
				continue;
			}
			temp.push_back(Previous.at(i));
			tuple->addResultRow(temp);
		}
		return tuple;
	}

	ResultTuple* solveLeftSyn(ResultTuple* tuple) {
		ResultTuple* result = new ResultTuple();
		result->setSynonym(tuple->getSynonyms());
		result->setSynonymMap(tuple->getSynonymMap());

		int index = tuple->getSynonymIndex(leftSynonym);
		for (size_t i = 0; i < tuple->getAllResults().size(); i++) {
			vector<int> temp = tuple->getAllResults().at(i);
			if (isSyn == 2) {	// Next(syn, stmt)
				if (temp.at(index) == rightIndex) {
					continue;
				}
				if (pkb->nextExtractor->isNext(temp.at(index), rightIndex)) {
					result->addResultRow(temp);
				}
			} else {	// Next(syn, _)
				if (!pkb->nextExtractor->getNext(temp.at(index)).empty()) {
					result->addResultRow(temp);
				}
			}
		}
		return result;
	}

	ResultTuple* solveRightSyn() {
		ResultTuple* tuple = new ResultTuple();
		int index = tuple->addSynonym(rightSynonym);
		tuple->addSynonymToMap(rightSynonym, index);
		
		set<int> tempNext;
		if (isSyn == 1) {	// Next(stmt, syn): Get Next of stmt
			tempNext = pkb->nextExtractor->getNext(leftIndex);
		} else {	// Next(_, syn): Get all Next stmt
			tempNext = pkb->nextExtractor->getAllNext();
		}
		vector<int> Next(tempNext.begin(), tempNext.end());
		for(size_t i = 0; i < Next.size(); i++) {
			vector<int> temp = vector<int>();
			// synonym type check here
			if ((synonymTable->at(rightSynonym)=="assign" || synonymTable->at(rightSynonym)=="while" || synonymTable->at(rightSynonym)=="if" || synonymTable->at(rightSynonym)=="call")
				&& pkb->statementTable->getTNode(Next[i])->getNodeType()!=TNODE_NAMES[synToNodeType.at(synonymTable->at(rightSynonym))]){
				continue;
			}
			temp.push_back(Next.at(i));
			tuple->addResultRow(temp);
		}
		return tuple;
	}

	ResultTuple* solveRightSyn(ResultTuple* tuple) {
		ResultTuple* result = new ResultTuple();
		result->setSynonym(tuple->getSynonyms());
		result->setSynonymMap(tuple->getSynonymMap());

		int index = tuple->getSynonymIndex(rightSynonym);
		for (size_t i = 0; i < tuple->getAllResults().size(); i++) {
			vector<int> temp = tuple->getAllResults().at(i);
			if (isSyn == 1) {	// Next(stmt, syn)
				if (leftIndex == temp.at(index)) {
					continue;
				}
				if (pkb->nextExtractor->isNext(leftIndex, temp.at(index))) {
					result->addResultRow(temp);
				}
			} else {	// Next(_, syn)
				if (!pkb->nextExtractor->getPrev(temp.at(index)).empty()) {
					result->addResultRow(temp);
				}
			}
		}
		return result;
	}

	ResultTuple* solveBothSyn() {
		ResultTuple* tuple = new ResultTuple();
		int index = tuple->addSynonym(leftSynonym);
		tuple->addSynonymToMap(leftSynonym, index);
		index = tuple->addSynonym(rightSynonym);
		tuple->addSynonymToMap(rightSynonym, index);

		set<int> tempPrevious = pkb->nextExtractor->getAllPrev();
		vector<int> Previous(tempPrevious.begin(), tempPrevious.end());
		for (size_t i = 0; i < Previous.size(); i++) {
			// synonym type check
			if ((synonymTable->at(leftSynonym)=="assign" || synonymTable->at(leftSynonym)=="while" || synonymTable->at(leftSynonym)=="if" || synonymTable->at(leftSynonym)=="call")
				&& pkb->statementTable->getTNode(Previous[i])->getNodeType()!=TNODE_NAMES[synToNodeType.at(synonymTable->at(leftSynonym))]){
				continue;
			}
			set<int> tempNext = pkb->nextExtractor->getNext(Previous[i]);
			vector<int> Next(tempNext.begin(), tempNext.end());
			for (size_t j = 0; j < Next.size(); j++) {
				// synonym type check
				if ((synonymTable->at(rightSynonym)=="assign" || synonymTable->at(rightSynonym)=="while" || synonymTable->at(rightSynonym)=="if" || synonymTable->at(rightSynonym)=="call")
				&& pkb->statementTable->getTNode(Next[j])->getNodeType()!=TNODE_NAMES[synToNodeType.at(synonymTable->at(rightSynonym))]){
					continue;
				}

				if (Previous.at(i) == Next.at(j)) {
					continue;
				}

				vector<int> row = vector<int>();
				row.push_back(Previous.at(i));
				row.push_back(Next.at(j));
				tuple->addResultRow(row);
			}
		}
		return tuple;
	}

	ResultTuple* solveBothSyn(ResultTuple* tuple) {
		ResultTuple* result = new ResultTuple();
		result->setSynonym(tuple->getSynonyms());
		result->setSynonymMap(tuple->getSynonymMap());

		int lIndex = tuple->getSynonymIndex(leftSynonym);
		int rIndex = tuple->getSynonymIndex(rightSynonym);
		if (lIndex != -1 && rIndex != -1){ //case 1: both are inside
			for (size_t i = 0; i < tuple->getAllResults().size(); i++){
				if (pkb->nextExtractor->isNext(tuple->getAllResults()[i][lIndex], tuple->getAllResults()[i][rIndex])){
					result->addResultRow(tuple->getResultRow(i));
				}
			}
		} else if (rIndex == -1) { //case 2: only left is inside
			int index = result->addSynonym(rightSynonym);
			result->addSynonymToMap(rightSynonym, index);
			map<int, vector<int>> prevSolution = map<int, vector<int>>();
			for (size_t i = 0; i < tuple->getAllResults().size(); i++) {
				int leftValue = tuple->getResultAt(i, lIndex);
				if (prevSolution.find(leftValue) == prevSolution.end()){
					set<int> tV = pkb->nextExtractor->getNext(leftValue);
					vector<int> tempValues(tV.begin(), tV.end());
					prevSolution.insert(make_pair(leftValue, tempValues));
				}
				vector<int> vals = prevSolution.at(leftValue);
				for (size_t j = 0; j < vals.size(); j++){
					if ((synonymTable->at(rightSynonym)=="assign" || synonymTable->at(rightSynonym)=="while" || synonymTable->at(rightSynonym)=="if" || synonymTable->at(rightSynonym)=="call")
						&& pkb->statementTable->getTNode(vals[j])->getNodeType()!=TNODE_NAMES[synToNodeType.at(synonymTable->at(rightSynonym))]){
						continue;
					}
					vector<int> newRow(tuple->getResultRow(i));
					newRow.push_back(vals[j]);
					result->addResultRow(newRow);
				}
			}
		} else if (lIndex == -1) { //case 3: only right is inside
			int index = result->addSynonym(leftSynonym);
			result->addSynonymToMap(leftSynonym, index);
			map<int, vector<int>> prevSolution = map<int, vector<int>>();
			for (size_t i = 0; i < tuple->getAllResults().size(); i++) {
				int rightValue = tuple->getResultAt(i, rIndex);
				if (prevSolution.find(rightValue) == prevSolution.end()){
					set<int> tV = pkb->nextExtractor->getPrev(rightValue);
					vector<int> tempValues(tV.begin(), tV.end());
					prevSolution.insert(make_pair(rightValue, tempValues));
				}
				vector<int> vals = prevSolution.at(rightValue);
				for (size_t j = 0; j < vals.size(); j++){
					if ((synonymTable->at(leftSynonym)=="assign" || synonymTable->at(leftSynonym)=="while" || synonymTable->at(leftSynonym)=="if" || synonymTable->at(leftSynonym)=="call")
						&& pkb->statementTable->getTNode(vals[j])->getNodeType()!=TNODE_NAMES[synToNodeType.at(synonymTable->at(leftSynonym))]){
						continue;
					}
					vector<int> newRow(tuple->getResultRow(i));
					newRow.push_back(vals[j]);
					result->addResultRow(newRow);
				}
			}
		}
		return result;
	}

	// BOOLEAN Result
	ResultTuple* solveOther() {
		ResultTuple* tuple = new ResultTuple();
		tuple->setBool(true);
		if(isSyn == 0) {	//(digit, digit)
			tuple->setEmpty(!pkb->nextExtractor->isNext(leftIndex, rightIndex));
		} else if (isSyn == 7) {	//(_, digit)
			tuple->setEmpty(pkb->nextExtractor->getPrev(rightIndex).empty());
		} else if (isSyn == 8) {	//(digit, _)
			tuple->setEmpty(pkb->nextExtractor->getNext(leftIndex).empty());
		} else {	//(_, _)
			tuple->setEmpty(pkb->nextExtractor->getAllNext().empty());
		}
		return tuple;
	}
};