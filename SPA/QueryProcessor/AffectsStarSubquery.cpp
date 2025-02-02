#pragma once
#include <string>
#include <vector>
#include <map>
#include "Subquery.h"
#include "CacheTable.h";

using namespace std;
// TODO: 77, 119, 236?
class AffectsStarSubquery : public Subquery {
public:
	bool isConcurrent;
	AffectsStarSubquery(map<string, string>* m, PKBController* p) : Subquery(m, p){
	
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
		type = 3;
		return ans;
	}
	
	bool validate() {
		//right synonym
		if(isSyn == 1 || isSyn == 4 || isSyn == 3) {
			string s = synonymTable->at(rightSynonym);
			if (!(s == "assign" || s == "stmt" || s == "prog_line")) {
				return false;
			}
		}
		//left synonym
		if (isSyn == 2 || isSyn == 5 || isSyn == 3) {
			string s = synonymTable->at(leftSynonym);
			if (!(s == "assign" || s == "stmt" || s == "prog_line")) {
				return false;
			}
		}
		return true;
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

	void setPriority() {
		int magnitude = 50;	//calculated during profiling
		bool isLeftSynStmtOrProgLine = true;
		bool isRightSynStmtOrProgLine = true;
		switch (isSyn) {
			case 1: // (int, syn)
				priority = (pkb->statementTable->getStmtNumUsingNodeType("ASSIGN_NODE")).size();
				break;
			case 2: // (syn, int)
				priority = (pkb->statementTable->getStmtNumUsingNodeType("ASSIGN_NODE")).size();
				break;
			case 3: // (syn, syn)
				priority = (pkb->statementTable->getStmtNumUsingNodeType("ASSIGN_NODE")).size() * (pkb->statementTable->getStmtNumUsingNodeType("ASSIGN_NODE")).size();
				break;
			case 4: // (_, syn)
				priority = (pkb->statementTable->getStmtNumUsingNodeType("ASSIGN_NODE")).size();
				break;
			case 5:	// (syn, _)
				priority = (pkb->statementTable->getStmtNumUsingNodeType("ASSIGN_NODE")).size();
				break;
			default: 
				priority = 1;
				break;
		}

		priority *= magnitude;
	}

	ResultTuple* solveLeftSyn() {
		ResultTuple* tuple = new ResultTuple();
		int index = tuple->addSynonym(leftSynonym);
		tuple->addSynonymToMap(leftSynonym, index);
		vector<int> previous = vector<int>();
		vector<int> assStmt = pkb->statementTable->getStmtNumUsingNodeType("ASSIGN_NODE");

		set<int> tempAffectedStar = set<int>();
		vector<int> AffectedStar;

		if (isSyn == 2) {	// AffectsStar(syn, stmt)
			if (isConcurrent) {
				try {
					AffectedStar = CacheTable::instance()->affectedStarCache.at(rightIndex);
				} catch (exception& e) {
					for (size_t i = 0; i < assStmt.size(); i++) {
						if (pkb->affectsExtractor->isAffectsStar(assStmt[i], rightIndex)) {
							tempAffectedStar.insert(assStmt[i]);
						} 
					}
					AffectedStar.assign(tempAffectedStar.begin(), tempAffectedStar.end());
				}
			} else {
				for (size_t i = 0; i < assStmt.size(); i++) {
					if (pkb->affectsExtractor->isAffectsStar(assStmt[i], rightIndex)) {
						tempAffectedStar.insert(assStmt[i]);
					} 
				}
				AffectedStar.assign(tempAffectedStar.begin(), tempAffectedStar.end());
			}
		} else {	// AffectsStar(syn, _): Get all AffectedStar stmt
			if (isConcurrent) {
				for (size_t i = 0; i < assStmt.size(); i++) {
					for (size_t j = 0; j < assStmt.size(); j++) {
						try {
							if (CacheTable::instance()->isAffectsStarCache.at(make_pair(assStmt[i], assStmt[j]))) {
								tempAffectedStar.insert(assStmt[i]);
								break;
							}
						} catch (exception& e) {
							if (pkb->affectsExtractor->isAffectsStar(assStmt[i], assStmt[j])) {
								tempAffectedStar.insert(assStmt[i]);
								break;
							}
						}
					}
				}
			} else {
				for (size_t i = 0; i < assStmt.size(); i++) {
					for (size_t j = 0; j < assStmt.size(); j++) {
						if (pkb->affectsExtractor->isAffectsStar(assStmt[i], assStmt[j])) {
							tempAffectedStar.insert(assStmt[i]);
							break;
						}
					}
				}
			}
			AffectedStar.assign(tempAffectedStar.begin(), tempAffectedStar.end());
		}

		pair<int, int> p = pair<int, int>();
		p.second = rightIndex;
		for(size_t i = 0; i < AffectedStar.size(); i++) {
			if (isConcurrent && isSyn == 1) {
				p.second = AffectedStar[i];
				CacheTable::instance()->isAffectsStarCache[p] = true;
			}

			vector<int> temp = vector<int>();	
			temp.push_back(AffectedStar.at(i));
			tuple->addResultRow(temp);
		}
		return tuple;
	}

	ResultTuple* solveLeftSyn(ResultTuple* tuple) {
		ResultTuple* result = new ResultTuple();
		result->setSynonym(tuple->getSynonyms());
		result->setSynonymMap(tuple->getSynonymMap());
		pair<int, int> p = pair<int, int>();
		p.second = rightIndex;

		int index = tuple->getSynonymIndex(leftSynonym);
		for (size_t i = 0; i < tuple->getAllResults().size(); i++) {
			vector<int> temp = tuple->getAllResults().at(i);
			if (isSyn == 2) {	// AffectsStar(syn, stmt)
				p.first = temp.at(index);

				if (isConcurrent) {
					try {
						if (CacheTable::instance()->isAffectsStarCache.at(p)) {
							result->addResultRow(temp);
						}
					} catch (exception& e) {
						if (pkb->affectsExtractor->isAffectsStar(temp.at(index), rightIndex)) {
							result->addResultRow(temp);
							CacheTable::instance()->isAffectsStarCache[p] = true;
						} else {
							CacheTable::instance()->isAffectsStarCache[p] = false;	
						}
					}
				} else {
					if (pkb->affectsExtractor->isAffectsStar(temp.at(index), rightIndex)) {
						result->addResultRow(temp);
					}
				}
			} else {	// AffectsStar(syn, _)
				if (!pkb->affectsExtractor->getAffectsStar(temp.at(index)).empty()) {
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
		set<int> tempAffectsStar;
		vector<int> AffectsStar;

		if (isSyn == 1) {	// AffectsStar(stmt, syn): Get AffectsStar of stmt
			if (isConcurrent) {
				try {
					AffectsStar = CacheTable::instance()->affectsStarCache.at(leftIndex);
				} catch (exception& e) {
					tempAffectsStar = pkb->affectsExtractor->getAffectsStar(leftIndex);
					AffectsStar.assign(tempAffectsStar.begin(), tempAffectsStar.end());
					CacheTable::instance()->affectsStarCache.insert(make_pair(leftIndex, AffectsStar));
				}
			} else {
				tempAffectsStar = pkb->affectsExtractor->getAffectsStar(leftIndex);
				AffectsStar.assign(tempAffectsStar.begin(), tempAffectsStar.end());
				CacheTable::instance()->affectsStarCache.insert(make_pair(leftIndex, AffectsStar));
			}
		} else {	// AffectsStar(_, syn): Get all AffectedStar stmt
			vector<int> assStmt = pkb->statementTable->getStmtNumUsingNodeType("ASSIGN_NODE");
			if (isConcurrent) {
				for (size_t i = 0; i < assStmt.size(); i++) {
					for (size_t j = 0; j < assStmt.size(); j++) {
						try {
							if (CacheTable::instance()->isAffectsStarCache.at(make_pair(assStmt[j], assStmt[i]))) {
								tempAffectsStar.insert(assStmt[i]);
								break;
							}
						} catch (exception& e) {
							if (pkb->affectsExtractor->isAffectsStar(assStmt[j], assStmt[i])) {
								tempAffectsStar.insert(assStmt[i]);
								break;
							}
						}
					}
				}
			} else {
				for (size_t i = 0; i < assStmt.size(); i++) {
					for (size_t j = 0; j < assStmt.size(); j++) {
						if (pkb->affectsExtractor->isAffectsStar(assStmt[j], assStmt[i])) {
							tempAffectsStar.insert(assStmt[i]);
							break;
						}
					}
				}
			}
			AffectsStar.assign(tempAffectsStar.begin(), tempAffectsStar.end());
		}

		pair<int, int> p = pair<int, int>();
		p.first = leftIndex;
		for(size_t i = 0; i < AffectsStar.size(); i++) {
			if (isConcurrent && isSyn == 1) {
				p.second = AffectsStar[i];
				CacheTable::instance()->isAffectsStarCache[p] = true;
			}
			
			vector<int> temp = vector<int>();
			temp.push_back(AffectsStar.at(i));
			tuple->addResultRow(temp);
		}
		return tuple;
	}

	ResultTuple* solveRightSyn(ResultTuple* tuple) {
		ResultTuple* result = new ResultTuple();
		result->setSynonym(tuple->getSynonyms());
		result->setSynonymMap(tuple->getSynonymMap());
		int size = tuple->getAllResults().size();
		vector<int> assStmt = pkb->statementTable->getStmtNumUsingNodeType("ASSIGN_NODE");
		int index = tuple->getSynonymIndex(rightSynonym);
		pair<int, int> p = pair<int, int>();

		if (isSyn == 1) {	// AffectsStar(stmt, syn)
			p.first = leftIndex;
			for (size_t i = 0; i < size; i++) {
				vector<int> temp = tuple->getAllResults().at(i);
				p.second = temp.at(index);
				if (isConcurrent) {
					try {
						if (CacheTable::instance()->isAffectsStarCache.at(make_pair(leftIndex, temp[index]))) {
							result->addResultRow(temp);
						}
					} catch (exception& e) {
						if (pkb->affectsExtractor->isAffectsStar(leftIndex, temp[index])) {
							result->addResultRow(temp);
							CacheTable::instance()->isAffectsStarCache[p] = true;
						} else {
							CacheTable::instance()->isAffectsStarCache[p] = false;
						}
					}
				} else {
					if (pkb->affectsExtractor->isAffectsStar(leftIndex, temp[index])) {
						result->addResultRow(temp);
					}
				}
			}
		} else {	// AffectsStar(_, syn)
			for (size_t i = 0; i < size; i++) {
				vector<int> temp = tuple->getAllResults().at(i);
				p.second = temp[index];
				for (size_t j = 0; j < assStmt.size(); j++) {
					p.first = assStmt[j];
					if (isConcurrent) {
						try {
							if (CacheTable::instance()->isAffectsStarCache.at(make_pair(assStmt[j], temp[index]))) {
								result->addResultRow(temp);
								break;
							}
						} catch (exception& e) {
							if (pkb->affectsExtractor->isAffectsStar(assStmt[j], temp[index])) {
								result->addResultRow(temp);
								CacheTable::instance()->isAffectsStarCache[p] = true;
								break;
							} else {
								CacheTable::instance()->isAffectsStarCache[p] = false;
							}
						}
					} else {
						if (pkb->affectsExtractor->isAffectsStar(assStmt[j], temp[index])) {
							result->addResultRow(temp);
							break;
						}
					}
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
		pair<int, int> p = pair<int, int>();

		vector<int> assStmt = pkb->statementTable->getStmtNumUsingNodeType("ASSIGN_NODE");
		for (size_t i = 0; i < assStmt.size(); i++) {
			p.first = assStmt[i];
			if (leftSynonym != rightSynonym) {
				for (size_t j = 0; j < assStmt.size(); j++) {
					p.second = assStmt[j];
					if (isConcurrent) {
						try {
							if (CacheTable::instance()->isAffectsStarCache.at(p)) {
								vector<int> row = vector<int>();
								row.push_back(assStmt.at(i));
								row.push_back(assStmt.at(j));
								tuple->addResultRow(row);
							}
						} catch (exception& e) {
							if (pkb->affectsExtractor->isAffectsStar(assStmt[i], assStmt[j])) {
								vector<int> row = vector<int>();
								row.push_back(assStmt.at(i));
								row.push_back(assStmt.at(j));
								tuple->addResultRow(row);
								CacheTable::instance()->isAffectsStarCache[p] = true;
							} else {
								CacheTable::instance()->isAffectsStarCache[p] = false;
							}
						}
					} else {				
						if (pkb->affectsExtractor->isAffectsStar(assStmt[i], assStmt[j])) {
							vector<int> row = vector<int>();
							row.push_back(assStmt.at(i));
							row.push_back(assStmt.at(j));
							tuple->addResultRow(row);
						}
					}
				}
			} else {
				p.second = assStmt[i];
				if (isConcurrent) {
					try {
						if (CacheTable::instance()->isAffectsStarCache.at(p)) {
							vector<int> row = vector<int>();
							row.push_back(assStmt.at(i));
							row.push_back(assStmt.at(i));
							tuple->addResultRow(row);
						}
					} catch (exception& e) {
						if (pkb->affectsExtractor->isAffectsStar(assStmt[i], assStmt[i])) {
							vector<int> row = vector<int>();
							row.push_back(assStmt.at(i));
							row.push_back(assStmt.at(i));
							tuple->addResultRow(row);
							CacheTable::instance()->isAffectsStarCache[p] = true;
						} else {
							CacheTable::instance()->isAffectsStarCache[p] = false;
						}
					}
				} else {				
					if (pkb->affectsExtractor->isAffectsStar(assStmt[i], assStmt[i])) {
						vector<int> row = vector<int>();
						row.push_back(assStmt.at(i));
						row.push_back(assStmt.at(i));
						tuple->addResultRow(row);
					}
				}
			}
		}
		return tuple;
	}

	ResultTuple* solveBothSyn(ResultTuple* tuple) {
		ResultTuple* result = new ResultTuple();
		result->setSynonym(tuple->getSynonyms());
		result->setSynonymMap(tuple->getSynonymMap());
		pair<int, int> p = pair<int, int>();
		int lIndex = tuple->getSynonymIndex(leftSynonym);
		int rIndex = tuple->getSynonymIndex(rightSynonym);
		if (lIndex != -1 && rIndex != -1){ //case 1: both are inside
			for (size_t i = 0; i < tuple->getAllResults().size(); i++){
				if (isConcurrent) {
					p.first = tuple->getResultAt(i, lIndex);
					p.second = tuple->getResultAt(i, rIndex);
					try {
						if (CacheTable::instance()->isAffectsStarCache.at(p)) {
							result->addResultRow(tuple->getResultRow(i));
						}
					} catch (exception& e) {
						if (pkb->affectsExtractor->isAffectsStar(tuple->getResultAt(i, lIndex), tuple->getResultAt(i, rIndex))){
							result->addResultRow(tuple->getResultRow(i));
							CacheTable::instance()->isAffectsStarCache[p] = true;
						} else {
							CacheTable::instance()->isAffectsStarCache[p] = false;
						}
					}
				} else {
					if (pkb->affectsExtractor->isAffectsStar(tuple->getResultAt(i, lIndex), tuple->getResultAt(i, rIndex))){
						result->addResultRow(tuple->getResultRow(i));
					}
				}
			}
		} else if (rIndex == -1) { //case 2: only left is inside
			int index = result->addSynonym(rightSynonym);
			result->addSynonymToMap(rightSynonym, index);
			map<int, vector<int>> prevSolution = map<int, vector<int>>();
			
			for (size_t i = 0; i < tuple->getAllResults().size(); i++) {
				vector<int> AffectsStar;
				set<int> tempAffectsStar;
				int leftValue = tuple->getResultAt(i, lIndex);

				if (isConcurrent) {
					try {
						AffectsStar = CacheTable::instance()->affectsStarCache.at(leftValue);
					} catch (exception& e) {
						tempAffectsStar = pkb->affectsExtractor->getAffectsStar(leftValue);
						AffectsStar.assign(tempAffectsStar.begin(), tempAffectsStar.end());
						CacheTable::instance()->affectsStarCache.insert(make_pair(leftValue, AffectsStar));
					}
				} else {
					try {
						AffectsStar = prevSolution.at(leftValue);
					} catch (exception& e) {
						tempAffectsStar = pkb->affectsExtractor->getAffectsStar(leftValue);
						AffectsStar.assign(tempAffectsStar.begin(), tempAffectsStar.end());
						prevSolution.insert(make_pair(leftValue, AffectsStar));
					}
				}
				
				p.first = leftValue;
				for (size_t j = 0; j < AffectsStar.size(); j++){
					if (isConcurrent) {
						p.second = AffectsStar[j];
						CacheTable::instance()->isAffectsStarCache[p] = true;
					}
					
					vector<int> newRow(tuple->getResultRow(i));
					newRow.push_back(AffectsStar[j]);
					result->addResultRow(newRow);
				}
			}
		} else if (lIndex == -1) { //case 3: only right is inside
			int index = result->addSynonym(leftSynonym);
			int size = tuple->getAllResults().size();
			result->addSynonymToMap(leftSynonym, index);
			map<int, vector<int>> prevSolution = map<int, vector<int>>();
			vector<int> pre = pkb->statementTable->getStmtNumUsingNodeType("ASSIGN_NODE");
						
			for (size_t i = 0; i < size; i++) {
				int rightValue = tuple->getResultAt(i, rIndex);
				set<int> tempAffectedStar;
				vector<int> AffectedStar;

				if (isConcurrent) {
					try {
						AffectedStar = CacheTable::instance()->affectedStarCache.at(rightValue);
					} catch (exception& e) {
						for (size_t x = 0; x < pre.size(); x++) {
							if (pkb->affectsExtractor->isAffectsStar(pre[x], rightValue)) {
								AffectedStar.push_back(pre[x]);
							}
						}
						CacheTable::instance()->affectedStarCache.insert(make_pair(rightValue, AffectedStar));
					}
				} else {
					try {
						AffectedStar = prevSolution.at(rightValue);
					} catch (exception& e) {
						for (size_t x = 0; x < pre.size(); x++) {
							if (pkb->affectsExtractor->isAffectsStar(pre[x], rightValue)) {
								AffectedStar.push_back(pre[x]);
							}
						}
						prevSolution.insert(make_pair(rightValue, AffectedStar));
					}
				}

				p.second = rightValue;
				for (size_t j = 0; j < AffectedStar.size(); j++){
					if (isConcurrent) {
						p.first = AffectedStar[j];
						CacheTable::instance()->isAffectsStarCache[p] = true;
					}

					vector<int> newRow(tuple->getResultRow(i));
					newRow.push_back(AffectedStar[j]);
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
			tuple->setEmpty(!pkb->affectsExtractor->isAffectsStar(leftIndex, rightIndex));
		} else if (isSyn == 7) {	//(_, digit)
			tuple->setEmpty(pkb->affectsExtractor->getAffectsStarBy(rightIndex).empty());
		} else if (isSyn == 8) {	//(digit, _)
			tuple->setEmpty(pkb->affectsExtractor->getAffectsStar(leftIndex).empty());
		} else {	//(_, _)
			vector<int> assStmt = pkb->statementTable->getStmtNumUsingNodeType("ASSIGN_NODE");
			for (size_t i = 0; i < assStmt.size(); i++) {
				for (size_t j = 0; j < assStmt.size(); j++) {
					if (pkb->affectsExtractor->isAffectsStar(assStmt[i], assStmt[j])) {
						tuple->setEmpty(false);
						return tuple;
					}
				}
			}
			tuple->setEmpty(true);
		}
		return tuple;
	}
};