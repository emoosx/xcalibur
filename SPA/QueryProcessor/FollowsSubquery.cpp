#pragma once
#include <string>
#include <vector>
#include <map>
#include "Subquery.h"
#include "Subquery.cpp"

using namespace std;

class FollowsSubquery : public Subquery{
public:
	FollowsSubquery(map<string, string>* m, PKBController* p) : Subquery(m, p){
	
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
		vector<int> followees;
		if (isSyn == 2) {	// Follows(syn, stmt): Get followees of stmt
			followees = pkb->followsTable.getFollowees(rightIndex);
		} else {	// Follows(syn, _): Get all followees stmt
			followees = pkb->followsTable.getAllFolloweeStmt();
		}

		for(int i = 0; i < followees.size(); i++) {
			vector<int> temp = vector<int>();
			// synonym type check here
			if ((synonymTable->at(leftSynonym)=="assign" || synonymTable->at(leftSynonym)=="while" || synonymTable->at(leftSynonym)=="if")
				&& pkb->statementTable.getTNode(followees[i])->getNodeType()!=TNODE_NAMES[synToNodeType.at(synonymTable->at(leftSynonym))]){
				continue;
			}
			temp.push_back(followees.at(i));
			tuple->addResultRow(temp);
		}
		return tuple;
	}

	ResultTuple* solveLeftSyn(ResultTuple* tuple) {
		ResultTuple* result = new ResultTuple();
		result->setSynonym(tuple->getSynonyms());
		result->setSynonymMap(tuple->getSynonymMap());

		int index = tuple->getSynonymIndex(leftSynonym);
		for (int i = 0; i < tuple->getAllResults().size(); i++) {
			vector<int> temp = tuple->getAllResults().at(i);
			if (isSyn == 1) {	// Follows(syn, stmt)
				if (pkb->followsTable.isFollowsTrue(temp.at(index), rightIndex)) {
					result->addResultRow(temp);
				}
			} else {	// Follows(syn, _)
				if (!pkb->followsTable.getFollowers(temp.at(index)).empty()) {
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
		
		vector<int> followers;
		if (isSyn == 1) {	// Follows(stmt, syn): Get followers of stmt
			followers = pkb->followsTable.getFollowers(leftIndex);
		} else {	// Follows(_, syn): Get all followers stmt
			followers = pkb->followsTable.getAllFollowerStmt();
		}

		for(int i = 0; i < followers.size(); i++) {
			vector<int> temp = vector<int>();
			// synonym type check here
			if ((synonymTable->at(rightSynonym)=="assign" || synonymTable->at(rightSynonym)=="while" || synonymTable->at(rightSynonym)=="if")
				&& pkb->statementTable.getTNode(followers[i])->getNodeType()!=TNODE_NAMES[synToNodeType.at(synonymTable->at(rightSynonym))]){
				continue;
			}
			temp.push_back(followers.at(i));
			tuple->addResultRow(temp);
		}
		return tuple;
	}

	ResultTuple* solveRightSyn(ResultTuple* tuple) {
		ResultTuple* result = new ResultTuple();
		result->setSynonym(tuple->getSynonyms());
		result->setSynonymMap(tuple->getSynonymMap());

		int index = tuple->getSynonymIndex(rightSynonym);
		for (int i = 0; i < tuple->getAllResults().size(); i++) {
			vector<int> temp = tuple->getAllResults().at(i);
			if (isSyn == 1) {	// Follows(stmt, syn)
				if (pkb->followsTable.isFollowsTrue(leftIndex, temp.at(index))) {
					result->addResultRow(temp);
				}
			} else {	// Follows(_, syn)
				if (!pkb->followsTable.getFollowees(temp.at(index)).empty()) {
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

		// get all followees statement
		// for each followee statement, get its followers
		vector<int> followees = pkb->followsTable.getAllFolloweeStmt();
		for (int i = 0; i < followees.size(); i++) {
			// synonym type check
			if ((synonymTable->at(leftSynonym)=="assign" || synonymTable->at(leftSynonym)=="while" || synonymTable->at(leftSynonym)=="if")
				&& pkb->statementTable.getTNode(followees[i])->getNodeType()!=TNODE_NAMES[synToNodeType.at(synonymTable->at(leftSynonym))]){
				continue;
			}
			vector<int> followers = pkb->followsTable.getFollowers(followees[i]);
			for (int j = 0; j < followers.size(); j++) {
				// synonym type check
				if ((synonymTable->at(rightSynonym)=="assign" || synonymTable->at(rightSynonym)=="while" || synonymTable->at(rightSynonym)=="if")
				&& pkb->statementTable.getTNode(followers[i])->getNodeType()!=TNODE_NAMES[synToNodeType.at(synonymTable->at(rightSynonym))]){
					continue;
				}
				vector<int> row = vector<int>();
				row.push_back(followees.at(i));
				row.push_back(followers.at(i));
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
			for (int i = 0; i < tuple->getAllResults().size(); i++){
				if (pkb->followsTable.isFollowsTrue(tuple->getAllResults()[i][lIndex], tuple->getAllResults()[i][rIndex])){
					result->addResultRow(tuple->getResultRow(i));
				}
			}
		} else if (rIndex == -1) { //case 2: only left is inside
			int index = result->addSynonym(rightSynonym);
			result->addSynonymToMap(rightSynonym, index);
			map<int, vector<int>> prevSolution = map<int, vector<int>>();
			for (int i = 0; i < tuple->getAllResults().size(); i++) {
				int leftValue = tuple->getResultAt(i, lIndex);
				if (prevSolution.find(leftValue) == prevSolution.end()){
					vector<int> tempValues = pkb->followsTable.getFollowers(leftValue);
					prevSolution.insert(make_pair(leftValue, tempValues));
				}
				vector<int> vals = prevSolution.at(leftValue);
				for (int j = 0; j < vals.size(); j++){
					vector<int> newRow(tuple->getResultRow(i).begin(), tuple->getResultRow(i).end());
					newRow.push_back(vals[j]);
					result->addResultRow(newRow);
				}
			}
		} else if (lIndex == -1) { //case 3: only right is inside
			int index = result->addSynonym(leftSynonym);
			result->addSynonymToMap(leftSynonym, index);
			map<int, vector<int>> prevSolution = map<int, vector<int>>();
			for (int i = 0; i < tuple->getAllResults().size(); i++) {
				int rightValue = tuple->getResultAt(i, rIndex);
				if (prevSolution.find(rightValue) == prevSolution.end()){
					vector<int> tempValues = pkb->followsTable.getFollowees(rightValue);
					prevSolution.insert(make_pair(rightValue, tempValues));
				}
				vector<int> vals = prevSolution.at(rightValue);
				for (int j = 0; j < vals.size(); j++){
					vector<int> newRow(tuple->getResultRow(i).begin(), tuple->getResultRow(i).end());
					newRow.push_back(vals[j]);
					result->addResultRow(newRow);
				}
			}
		}
		return new ResultTuple();
	}

	// BOOLEAN Result
	ResultTuple* solveOther() {
		ResultTuple* tuple = new ResultTuple();
		tuple->setBool(true);
		if(isSyn == 0) {	//(digit, digit)
			tuple->setEmpty(!pkb->followsTable.isFollowsTrue(leftIndex, rightIndex));
		} else if (isSyn == 7) {	//(_, digit)
			tuple->setEmpty(pkb->followsTable.getFollowees(rightIndex).empty());
		} else if (isSyn == 8) {	//(digit, _)
			tuple->setEmpty(pkb->followsTable.getFollowers(leftIndex).empty());
		} else {	//(_, _)
			tuple->setEmpty(pkb->followsTable.getAllFollowerStmt().empty());
		}
		return tuple;
	}
};