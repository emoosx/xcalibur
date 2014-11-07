// File name: QueryEvaluator.cpp
#include <string>
#include <map>
#include "QueryTree.h"
#include "QTNode.h"
#include "PKB.h"
#include "PKBController.h"

//For Testing
#include "StatementTable.h"
#include "Follows.h"
#include "Parent.h"
#include "Modifies.h"
#include "Uses.h"
#include "ProcTable.h"
#include "VarTable.h"

#include "QueryEvaluator.h"
#include <set>

using namespace std;

typedef string STRING;

/******* Constructors *******/
QueryEvaluator::QueryEvaluator(){
	pkb = new PKBController();
};

QueryEvaluator::QueryEvaluator(PKBController* newPKB){
	pkb = newPKB;
};
/******* For Testing *******/
bool QueryEvaluator::checkSynonymInSuchThat(string selectSynonym, QueryTree* suchThatTree){
	bool isSynonymInSuchThat = false;
	for (int i=0; i<suchThatTree->getRootNode()->getNumChild(); i++){
		if (selectSynonym == suchThatTree->getRootNode()->getChild(i)->getKey()){
			isSynonymInSuchThat = true;
			break;
		}
	}
	return isSynonymInSuchThat;
}

vector<int> QueryEvaluator::solveForSuchThatFollows(string selectSynonym, map<STRING, STRING>* synonymTable, QueryTree* suchThatTree, StatementTable* statementTable, Follows* follows, ProcTable* procTable, VarTable* varTable){
	vector<int> answer;
	if (synonymTable->find(selectSynonym)==synonymTable->end()){ // if selectSynonym is not defined
		return answer;
	}
	bool isSynonymInSuchThat = checkSynonymInSuchThat(selectSynonym, suchThatTree);
	if (isSynonymInSuchThat){ // Select synonym such that Follows(synonym, *) (Follows(*, synonym))
		if (selectSynonym == suchThatTree->getRootNode()->getChild(0)->getKey()){ // Select synonym such that Follows(synonym, *)
			string stmtSynonym = suchThatTree->getRootNode()->getChild(1)->getKey();
			if (stmtSynonym.empty()){ // Select synonym such that Follows(synonym, 1)
				int stmtIndex = suchThatTree->getRootNode()->getChild(1)->getValue();
				if(stmtIndex<=statementTable->getSize()){
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = follows->getFollowees(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = follows->getFollowees(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = follows->getFollowees(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = follows->getFollowees(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception or return empty
				}
			} else { // Select synonym such that Follows(synonym, another Synonym)
				if (synonymTable->find(stmtSynonym) != synonymTable->end()){
					vector<int> dataFromStmtSynonym;
					if(synonymTable->at(stmtSynonym)=="stmt"){
						dataFromStmtSynonym = follows->getAllFolloweeStmt();
					} else if (synonymTable->at(stmtSynonym)=="prog_line"){
						dataFromStmtSynonym = follows->getAllFolloweeStmt();
					} else if (synonymTable->at(stmtSynonym)=="assign"){
						vector<int> assignList = statementTable->getStmtNumUsingNodeType("assign");
						for (size_t i=0; i<assignList.size(); i++){
							vector<int> result = follows->getFollowees(assignList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="while"){
						vector<int> whileList = statementTable->getStmtNumUsingNodeType("while");
						for (size_t i=0; i<whileList.size(); i++){
							vector<int> result = follows->getFollowees(whileList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="if"){
						// not implemented	
					} else {
						// throw exception
					}
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception
				}
			}
		} else { // Select synonym such that Follows(*, synonym)
			string stmtSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
			if (stmtSynonym.empty()){ // Select synonym such that Follows(1, synonym)
				int stmtIndex = suchThatTree->getRootNode()->getChild(0)->getValue();
				if(stmtIndex<=statementTable->getSize()){
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = follows->getFollowers(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = follows->getFollowers(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = follows->getFollowers(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = follows->getFollowers(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception or return empty
				}
			} else { // Select synonym such that Follows(another Synonym, synonym)
				if (synonymTable->find(stmtSynonym) != synonymTable->end()){
					vector<int> dataFromStmtSynonym;
					if(synonymTable->at(stmtSynonym)=="stmt"){
						dataFromStmtSynonym = follows->getAllFollowerStmt();
					} else if (synonymTable->at(stmtSynonym)=="prog_line"){
						dataFromStmtSynonym = follows->getAllFollowerStmt();
					} else if (synonymTable->at(stmtSynonym)=="assign"){
						vector<int> assignList = statementTable->getStmtNumUsingNodeType("assign");
						for (size_t i=0; i<assignList.size(); i++){
							vector<int> result = follows->getFollowers(assignList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="while"){
						vector<int> whileList = statementTable->getStmtNumUsingNodeType("while");
						for (size_t i=0; i<whileList.size(); i++){
							vector<int> result = follows->getFollowers(whileList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="if"){
						// not implemented	
					} else {
						// throw exception
					}
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception
				}
			}
		}
	} else { // synonym is not in Follows(*, *)
		bool isSuchThatTrue = false;
		string leftSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
		string rightSynonym = suchThatTree->getRootNode()->getChild(1)->getKey();
		if (leftSynonym.empty()&&rightSynonym.empty()){ // Follows(1, 2);
			int leftStmtIndex = suchThatTree->getRootNode()->getChild(0)->getValue();
			int rightStmtIndex = suchThatTree->getRootNode()->getChild(1)->getValue();
			if (leftStmtIndex>statementTable->getSize()||rightStmtIndex>statementTable->getSize()){
				// throw exception
			} else {
				isSuchThatTrue = follows->isFollowsTrue(leftStmtIndex, rightStmtIndex);
			}
		} else if (leftSynonym.empty()){ // Follows(1, synonym)
			int leftStmtIndex = suchThatTree->getRootNode()->getChild(0)->getValue();
			if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (follows->isFollowsTrue(leftStmtIndex, i)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(rightSynonym)=="assign"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="assign"&&follows->isFollowsTrue(leftStmtIndex, i)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(rightSynonym)=="while"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="while"&&follows->isFollowsTrue(leftStmtIndex, i)){
						isSuchThatTrue = true;
						break;
					}
				}		
			} else if (synonymTable->at(rightSynonym)=="if"){
				// not implemented
			} else {
				// throw exception
			}
		} else if (rightSynonym.empty()){ // Follows(synonym, 2)
			int rightStmtIndex = suchThatTree->getRootNode()->getChild(1)->getValue();
			if (synonymTable->at(leftSynonym)=="stmt"||synonymTable->at(leftSynonym)=="prog_line"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (follows->isFollowsTrue(i, rightStmtIndex)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(leftSynonym)=="assign"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="assign"&&follows->isFollowsTrue(i, rightStmtIndex)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(leftSynonym)=="while"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="while"&&follows->isFollowsTrue(i, rightStmtIndex)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(leftSynonym)=="if"){
				// not implemented
			} else {
				// throw exception
			}
		} else { // Follows(synonym1, synonym2)
			if(synonymTable->at(leftSynonym).empty()||synonymTable->at(rightSynonym).empty()){
				// throw exception
			} else if (synonymTable->at(leftSynonym)=="stmt"||synonymTable->at(leftSynonym)=="prog_line"){ // Follows(s1, synonym2)
				if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){ // Follows(s1, s2) 
					for (int i=1; i<=statementTable->getSize(); i++){
						for (int j=1; j<=statementTable->getSize(); j++){
							if (follows->isFollowsTrue(i, j)){
								isSuchThatTrue = true;
								break;
							}		
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="assign"){ // Follows(s1, a2)
					for (int i=1; i<=statementTable->getSize(); i++){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="assign"&&follows->isFollowsTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="while"){ // Follows(s1, w2)
					for (int i=1; i<=statementTable->getSize(); i++){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="while"&&follows->isFollowsTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
					}			
				} else if (synonymTable->at(rightSynonym)=="if"){
					// not implemented
				} else {
					// throw exception
				}
			} else if (synonymTable->at(leftSynonym)=="assign"){ // Follows(a1, synonym2)
				if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){ // Follows(a1, s2) 
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="assign"){
							for (int j=1; j<statementTable->getSize(); j++){
								if (follows->isFollowsTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="assign"){ // Follows(a1, a2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="assign"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="assign"&&follows->isFollowsTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="while"){ // Follows(a1, w2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="assign"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="while"&&follows->isFollowsTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="if"){
					// not implemented
				} else {
					// throw exception
				}
			} else if (synonymTable->at(leftSynonym)=="while"){ // Follows(w1, synonym2)
				if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){ // Follows(w1, s2) 
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="while"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (follows->isFollowsTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="assign"){ // Follows(w1, a2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="while"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="assign"&&follows->isFollowsTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="while"){ // Follows(w1, w2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="while"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="while"&&follows->isFollowsTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="if"){
					// not implemented
				} else {
					// throw exception
				}
			} else if (synonymTable->at(leftSynonym)=="if"){
				// not implemented
			} else {
				// throw exception
			}
		}
		if (isSuchThatTrue){
			// fill answer for different type of selectSynonym such as procedure, statement, while, assign, variable, prog_line
			if (synonymTable->at(selectSynonym)=="stmt"||
				synonymTable->at(selectSynonym)=="assign"||
				synonymTable->at(selectSynonym)=="while"||
				synonymTable->at(selectSynonym)=="if"||
				synonymTable->at(selectSynonym)=="prog_line"){
					if (synonymTable->at(selectSynonym)=="stmt"||synonymTable->at(selectSynonym)=="prog_line"){
						for (int i=1; i<=statementTable->getSize(); i++){
							answer.push_back(i);
						}
					} else {
						answer = statementTable->getStmtNumUsingNodeType(synonymTable->at(selectSynonym));
					}					
			} else if (synonymTable->at(selectSynonym)=="variable"){
				for (int i=1; i<=varTable->getSize(); i++){
					answer.push_back(i);
				}
			} else if (synonymTable->at(selectSynonym)=="procedure"){
				for (int i=1; i<=procTable->getSize(); i++){
					answer.push_back(i);
				}
			} else {
				// throw exception
			}
		} else {
			// nothing in answer
		}
	}
	return answer;
}

vector<int> QueryEvaluator::solveForSuchThatParent(string selectSynonym, map<STRING, STRING>* synonymTable, QueryTree* suchThatTree, StatementTable* statementTable, Parent* parent, ProcTable* procTable, VarTable* varTable){
	vector<int> answer;
	if (synonymTable->find(selectSynonym)==synonymTable->end()){ // if selectSynonym is not defined
		return answer;
	}
	bool isSynonymInSuchThat = checkSynonymInSuchThat(selectSynonym, suchThatTree);
	if (isSynonymInSuchThat){ // Select synonym such that Parent(synonym, *) (Parent(*, synonym))
		if (selectSynonym == suchThatTree->getRootNode()->getChild(0)->getKey()){ // Select synonym such that Parent(synonym, *)
			string stmtSynonym = suchThatTree->getRootNode()->getChild(1)->getKey();
			if (stmtSynonym.empty()){ // Select synonym such that Parent(synonym, 1)
				int stmtIndex = suchThatTree->getRootNode()->getChild(1)->getValue();
				if(stmtIndex<=statementTable->getSize()){
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = parent->getParents(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = parent->getParents(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = parent->getParents(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = parent->getParents(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception or return empty
				}
			} else { // Select synonym such that Parent(synonym, another Synonym)
				if (synonymTable->find(stmtSynonym) != synonymTable->end()){
					vector<int> dataFromStmtSynonym;
					if(synonymTable->at(stmtSynonym)=="stmt"){
						dataFromStmtSynonym = parent->getAllParentStmt();
					} else if (synonymTable->at(stmtSynonym)=="prog_line"){
						dataFromStmtSynonym = parent->getAllParentStmt();
					} else if (synonymTable->at(stmtSynonym)=="assign"){
						vector<int> assignList = statementTable->getStmtNumUsingNodeType("assign");
						for (size_t i=0; i<assignList.size(); i++){
							vector<int> result = parent->getParents(assignList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="while"){
						vector<int> whileList = statementTable->getStmtNumUsingNodeType("while");
						for (size_t i=0; i<whileList.size(); i++){
							vector<int> result = parent->getParents(whileList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="if"){
						// not implemented	
					} else {
						// throw exception
					}
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception
				}
			}
		} else { // Select synonym such that Parent(*, synonym)
			string stmtSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
			if (stmtSynonym.empty()){ // Select synonym such that Parent(1, synonym)
				int stmtIndex = suchThatTree->getRootNode()->getChild(0)->getValue();
				if(stmtIndex<=statementTable->getSize()){
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = parent->getChildren(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = parent->getChildren(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = parent->getChildren(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = parent->getChildren(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception or return empty
				}
			} else { // Select synonym such that Parent(another Synonym, synonym)
				if (synonymTable->find(stmtSynonym) != synonymTable->end()){
					vector<int> dataFromStmtSynonym;
					if(synonymTable->at(stmtSynonym)=="stmt"){
						dataFromStmtSynonym = parent->getAllChildrenStmt();
					} else if (synonymTable->at(stmtSynonym)=="prog_line"){
						dataFromStmtSynonym = parent->getAllChildrenStmt();
					} else if (synonymTable->at(stmtSynonym)=="assign"){
						vector<int> assignList = statementTable->getStmtNumUsingNodeType("assign");
						for (size_t i=0; i<assignList.size(); i++){
							vector<int> result = parent->getChildren(assignList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="while"){
						vector<int> whileList = statementTable->getStmtNumUsingNodeType("while");
						for (size_t i=0; i<whileList.size(); i++){
							vector<int> result = parent->getChildren(whileList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="if"){
						// not implemented	
					} else {
						// throw exception
					}
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception
				}
			}
		}
	} else { // synonym is not in Parent(*, *)
		bool isSuchThatTrue = false;
		string leftSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
		string rightSynonym = suchThatTree->getRootNode()->getChild(1)->getKey();
		if (leftSynonym.empty()&&rightSynonym.empty()){ // Parent(1, 2);
			int leftStmtIndex = suchThatTree->getRootNode()->getChild(0)->getValue();
			int rightStmtIndex = suchThatTree->getRootNode()->getChild(1)->getValue();
			if (leftStmtIndex>statementTable->getSize()||rightStmtIndex>statementTable->getSize()){
				// throw exception
			} else {
				isSuchThatTrue = parent->isParentTrue(leftStmtIndex, rightStmtIndex);
			}
		} else if (leftSynonym.empty()){ // Parent(1, synonym)
			int leftStmtIndex = suchThatTree->getRootNode()->getChild(0)->getValue();
			if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (parent->isParentTrue(leftStmtIndex, i)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(rightSynonym)=="assign"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="assign"&&parent->isParentTrue(leftStmtIndex, i)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(rightSynonym)=="while"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="while"&&parent->isParentTrue(leftStmtIndex, i)){
						isSuchThatTrue = true;
						break;
					}
				}		
			} else if (synonymTable->at(rightSynonym)=="if"){
				// not implemented
			} else {
				// throw exception
			}
		} else if (rightSynonym.empty()){ // Parent(synonym, 2)
			int rightStmtIndex = suchThatTree->getRootNode()->getChild(1)->getValue();
			if (synonymTable->at(leftSynonym)=="stmt"||synonymTable->at(leftSynonym)=="prog_line"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (parent->isParentTrue(i, rightStmtIndex)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(leftSynonym)=="assign"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="assign"&&parent->isParentTrue(i, rightStmtIndex)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(leftSynonym)=="while"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="while"&&parent->isParentTrue(i, rightStmtIndex)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(leftSynonym)=="if"){
				// not implemented
			} else {
				// throw exception
			}
		} else { // Parent(synonym1, synonym2)
			if(synonymTable->at(leftSynonym).empty()||synonymTable->at(rightSynonym).empty()){
				// throw exception
			} else if (synonymTable->at(leftSynonym)=="stmt"||synonymTable->at(leftSynonym)=="prog_line"){ // Parent(s1, synonym2)
				if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){ // Parent(s1, s2) 
					for (int i=1; i<=statementTable->getSize(); i++){
						for (int j=1; j<=statementTable->getSize(); j++){
							if (parent->isParentTrue(i, j)){
								isSuchThatTrue = true;
								break;
							}		
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="assign"){ // Parent(s1, a2)
					for (int i=1; i<=statementTable->getSize(); i++){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="assign"&&parent->isParentTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="while"){ // Parent(s1, w2)
					for (int i=1; i<=statementTable->getSize(); i++){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="while"&&parent->isParentTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
					}			
				} else if (synonymTable->at(rightSynonym)=="if"){
					// not implemented
				} else {
					// throw exception
				}
			} else if (synonymTable->at(leftSynonym)=="assign"){ // Parent(a1, synonym2)
				if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){ // Parent(a1, s2) 
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="assign"){
							for (int j=1; j<statementTable->getSize(); j++){
								if (parent->isParentTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="assign"){ // Parent(a1, a2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="assign"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="assign"&&parent->isParentTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="while"){ // Parent(a1, w2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="assign"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="while"&&parent->isParentTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="if"){
					// not implemented
				} else {
					// throw exception
				}
			} else if (synonymTable->at(leftSynonym)=="while"){ // Parent(w1, synonym2)
				if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){ // Parent(w1, s2) 
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="while"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (parent->isParentTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="assign"){ // Parent(w1, a2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="while"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="assign"&&parent->isParentTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="while"){ // Parent(w1, w2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="while"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="while"&&parent->isParentTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="if"){
					// not implemented
				} else {
					// throw exception
				}
			} else if (synonymTable->at(leftSynonym)=="if"){
				// not implemented
			} else {
				// throw exception
			}
		}
		if (isSuchThatTrue){
			// fill answer for different type of selectSynonym such as procedure, statement, while, assign, variable, prog_line
			if (synonymTable->at(selectSynonym)=="stmt"||
				synonymTable->at(selectSynonym)=="assign"||
				synonymTable->at(selectSynonym)=="while"||
				synonymTable->at(selectSynonym)=="if"||
				synonymTable->at(selectSynonym)=="prog_line"){
					if (synonymTable->at(selectSynonym)=="stmt"||synonymTable->at(selectSynonym)=="prog_line"){
						for (int i=1; i<=statementTable->getSize(); i++){
							answer.push_back(i);
						}
					} else {
						answer = statementTable->getStmtNumUsingNodeType(synonymTable->at(selectSynonym));
					}					
			} else if (synonymTable->at(selectSynonym)=="variable"){
				for (int i=1; i<=varTable->getSize(); i++){
					answer.push_back(i);
				}
			} else if (synonymTable->at(selectSynonym)=="procedure"){
				for (int i=1; i<=procTable->getSize(); i++){
					answer.push_back(i);
				}
			} else {
				// throw exception
			}
		} else {
			// nothing in answer
		}
	}
	return answer;
}

vector<int> QueryEvaluator::solveForSuchThatModifies(string selectSynonym, map<STRING, STRING>* synonymTable, QueryTree* suchThatTree, StatementTable* statementTable, Modifies* modifies, ProcTable* procTable, VarTable* varTable){
	vector<int> answer;
	if (synonymTable->find(selectSynonym)==synonymTable->end()){ // if selectSynonym is not defined
		return answer;
	}
	bool isSynonymInSuchThat = checkSynonymInSuchThat(selectSynonym, suchThatTree);
	if (isSynonymInSuchThat){ // synonym is contained in such that statement
		if (selectSynonym == suchThatTree->getRootNode()->getChild(0)->getKey()){ // Select synonym such that Modifies(synonym, *)
			string rightSynonym = suchThatTree->getRootNode()->getChild(1)->getKey();
			if(!rightSynonym.empty()){ // Select synonym such that Modifies(synonym, *);
				if (synonymTable->find(rightSynonym) == synonymTable->end()){ // Select synonym such that Modifies(synonym, aString);
					if (varTable->containsVar(rightSynonym)){ // if another synonym is a variable
						int varIndex = varTable->getVarIndex(rightSynonym);
						if (synonymTable->at(selectSynonym)=="procedure"){
							answer = modifies->getModifiersProc(varIndex);
						} else if (synonymTable->at(selectSynonym)=="stmt"){
							answer = modifies->getModifiersStmt(varIndex);
						} else if (synonymTable->at(selectSynonym)=="assign"){
							vector<int> temp = modifies->getModifiersStmt(varIndex);
							for (size_t i=0; i<temp.size(); i++){
								if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
									answer.push_back(temp.at(i));
								}
							}
						} else if (synonymTable->at(selectSynonym)=="while"){
							vector<int> temp = modifies->getModifiersStmt(varIndex);
							for (size_t i=0; i<temp.size(); i++){
								if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
									answer.push_back(temp.at(i));
								}
							}
						} else if (synonymTable->at(selectSynonym)=="if"){
							// not implemented
						} else {
							// throw exception
						}
					} else {
						// throw error because another synonym is not found
					}
				} else { // Select synonym such that Modifies(symnonym, another synonym)
					if (synonymTable->at(rightSynonym) == "variable" && varTable->getSize()>=(int)0){
						if (synonymTable->at(selectSynonym) == "procedure"){						
							vector<pair<int, int>> result = modifies->getModifiesProc();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								temp.insert(result.at(i).first);
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym) == "stmt"){
							vector<pair<int, int>> result = modifies->getModifiesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								temp.insert(result.at(i).first);
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym) == "assign"){
							vector<pair<int, int>> result = modifies->getModifiesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								if (statementTable->getTNode(result.at(i).first)->getNodeType()=="assign"){
									temp.insert(result.at(i).first);
								}
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym) == "while"){
							vector<pair<int, int>> result = modifies->getModifiesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								if (statementTable->getTNode(result.at(i).first)->getNodeType()=="while"){
									temp.insert(result.at(i).first);
								}
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym) == "if"){
							// not implemented
						} else {
							// throw exception
						}
					} else {
						// throw exception
					}
				}
			} else {
				// throw exception
			}
		} else if (selectSynonym == suchThatTree->getRootNode()->getChild(1)->getKey()){ // Select synonym such that Modifies(*, synonym)
			if (synonymTable->at(selectSynonym) == "variable"){
				string leftSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
				if (!leftSynonym.empty()){ // Select synonym such that Modifies(anotherSynonym, synonym)
					if (synonymTable->find(leftSynonym) == synonymTable->end()){ // another synonym not in synonym table
						if (procTable->containsProc(leftSynonym)){ // if another synonym is a procedure
							int procIndex = procTable->getProcIndex(leftSynonym);
							answer = modifies->getModifiedVarProc(procIndex);
						} else {
							// throw error because another synonym is not found
						}
					} else { // another synonym is in synonym table
						if (synonymTable->at(leftSynonym)=="procedure"){
							vector<pair<int, int>> result = modifies->getModifiesProc();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								temp.insert(result.at(i).second);
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym)=="stmt"){
							vector<pair<int, int>> result = modifies->getModifiesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								temp.insert(result.at(i).second);
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym)=="assign"){
							vector<pair<int, int>> result = modifies->getModifiesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								if (statementTable->getTNode(result.at(i).first)->getNodeType()=="assign"){
									temp.insert(result.at(i).second);
								}
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym)=="while"){
							vector<pair<int, int>> result = modifies->getModifiesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								if (statementTable->getTNode(result.at(i).first)->getNodeType()=="while"){
									temp.insert(result.at(i).second);
								}
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym)=="if"){
							// not implemented
						} else {
							// throw exception
						}
					}
				} else { // Select synonym such that Modifies(INDEX, synonym)
					int stmtIndex = suchThatTree->getRootNode()->getChild(0)->getValue();
					if (0<stmtIndex&&stmtIndex<=statementTable->getSize()){
						answer = modifies->getModifiedVarStmt(stmtIndex);
					} else {
						// throw exception
					}
				}
			} else {
				// throw exception
			}
		}
	} else { // synonym is not contained in such that statement
		// evaluate such that statement as boolean and solve for it
		bool isSuchThatTrue = false;
		string varSynonym = suchThatTree->getRootNode()->getChild(1)->getKey();
		if (!varSynonym.empty()){
			if (synonymTable->find(varSynonym) == synonymTable->end()){ // varSynonym is a variable name, Modifies(*, "x")
				if (varTable->containsVar(varSynonym)){
					int varIndex = varTable->getVarIndex(varSynonym);
					string leftSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
					if (leftSynonym.empty()) { // Modifies(1, "x")
						int stmtNumber = suchThatTree->getRootNode()->getChild(0)->getValue();
						if (0<stmtNumber&&stmtNumber<=statementTable->getSize()){
							isSuchThatTrue = modifies->isModifiesStmt(stmtNumber, varIndex);
						} else {
							// throw exception, or invalid query
						}
					} else { // Modifies(leftSynonym, "x")
						if (synonymTable->find(leftSynonym) == synonymTable->end()){ // Modifies("Procedure First", "x")
							if (procTable->containsProc(leftSynonym)){
								int procIndex = procTable->getProcIndex(leftSynonym);
								isSuchThatTrue = modifies->isModifiesProc(procIndex, varIndex);
							} else {
								// throw error because procedure not found
							}
						} else { // Modifies(leftSynonym, "x")
							if (synonymTable->at(leftSynonym)=="procedure"){ // Modifies(procedure, "x")
								for (int i=0; i<procTable->getSize(); i++){
									if (modifies->isModifiesProc(i, varIndex)){
										isSuchThatTrue = true;
										break;
									}
								}
							} else if (synonymTable->at(leftSynonym)=="stmt"){ // Modifies(statement/while/assign, "x")
								for (int i=1; i<=statementTable->getSize(); i++){
									if (modifies->isModifiesStmt(i, varIndex)){
										isSuchThatTrue = true;
										break;
									}
								}
							} else if (synonymTable->at(leftSynonym)=="assign"){
								for (int i=1; i<=statementTable->getSize(); i++){
									if (statementTable->getTNode(i)->getNodeType()=="assign"&&modifies->isModifiesStmt(i, varIndex)){
										isSuchThatTrue = true;
										break;
									}
								}
							} else if (synonymTable->at(leftSynonym)=="while"){
								for (int i=1; i<=statementTable->getSize(); i++){
									if (statementTable->getTNode(i)->getNodeType()=="while"&&modifies->isModifiesStmt(i, varIndex)){
										isSuchThatTrue = true;
										break;
									}
								}
							} else if (synonymTable->at(leftSynonym)=="if"){
								// not implemented
							} else {
								// throw exception
							}
						}
					}
				} else {
					// throw exception because varSynonym is not found in program
				}
			} else { // varSynonym is a variable entity, Modifies(*, var)
					string leftSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
					if (leftSynonym.empty()) { // Modifies(1, var)
						int stmtNumber = suchThatTree->getRootNode()->getChild(0)->getValue();
						if (0<stmtNumber&&stmtNumber<=statementTable->getSize()){
							for (int i=0; i<varTable->getSize(); i++){
								if (modifies->isModifiesStmt(stmtNumber, i)){
									isSuchThatTrue = true;
									break;
								}
							}
						} else {
							// throw exception, or invalid query
						}
					} else { // Modifies(leftSynonym, var)
						if (synonymTable->find(leftSynonym) == synonymTable->end()){ // Modifies("Procedure First", var)
							if (procTable->containsProc(leftSynonym)){
								int procIndex = procTable->getProcIndex(leftSynonym);
								for (int i=0; i<varTable->getSize(); i++){
									if (modifies->isModifiesProc(procIndex, i)){
										isSuchThatTrue = true;
										break;
									}
								}
							} else {
								// throw error because procedure not found
							}
						} else { // Modifies(leftSynonym, var)
							if (synonymTable->at(leftSynonym)=="procedure"){ // Modifies(procedure, var)
								for (int i=0; i<procTable->getSize(); i++){
									for (int j=0; j<varTable->getSize(); j++){
										if (modifies->isModifiesProc(i, j)){
											isSuchThatTrue = true;
											break;
										}
									}
									if (isSuchThatTrue) break;
								}
							} else if (synonymTable->at(leftSynonym)=="stmt"){ // Modifies(statement/while/assign/etc., var)
								for (int i=1; i<=statementTable->getSize(); i++){
									for (int j=0; j<varTable->getSize(); j++){
										if (modifies->isModifiesStmt(i, j)){
											isSuchThatTrue = true;
											break;
										}
									}
									if (isSuchThatTrue) break;
								}
							} else if (synonymTable->at(leftSynonym)=="assign"){
								for (int i=1; i<=statementTable->getSize(); i++){
									if (statementTable->getTNode(i)->getNodeType()=="assign"){
										for (int j=0; j<varTable->getSize(); j++){
											if (modifies->isModifiesStmt(i, j)){
												isSuchThatTrue = true;
												break;
											}											
										}
										if (isSuchThatTrue) break;
									}
									if (isSuchThatTrue) break;
								}
							} else if (synonymTable->at(leftSynonym)=="while"){
								for (int i=1; i<=statementTable->getSize(); i++){
									if (statementTable->getTNode(i)->getNodeType()=="while"){
										for (int j=0; j<varTable->getSize(); j++){
											if (modifies->isModifiesStmt(i, j)){
												isSuchThatTrue = true;
												break;
											}											
										}
										if (isSuchThatTrue) break;
									}
									if (isSuchThatTrue) break;
								}
							} else if (synonymTable->at(leftSynonym)=="if"){
								// not implemented
							} else {
								// throw exception							
							}
						}
					}
			} 
		} else {
			// throw exception
		}
		if (isSuchThatTrue){
			// fill answer for different type of selectSynonym such as procedure, statement, while, assign, variable, prog_line
			if (synonymTable->at(selectSynonym)=="stmt"||
				synonymTable->at(selectSynonym)=="assign"||
				synonymTable->at(selectSynonym)=="while"||
				synonymTable->at(selectSynonym)=="if"||
				synonymTable->at(selectSynonym)=="prog_line"){
					if (synonymTable->at(selectSynonym)=="stmt"||synonymTable->at(selectSynonym)=="prog_line"){
						for (int i=1; i<=statementTable->getSize(); i++){
							answer.push_back(i);
						}
					} else {
						answer = statementTable->getStmtNumUsingNodeType(synonymTable->at(selectSynonym));
					}					
			} else if (synonymTable->at(selectSynonym)=="variable"){
				for (int i=0; i<varTable->getSize(); i++){
					answer.push_back(i);
				}
			} else if (synonymTable->at(selectSynonym)=="procedure"){
				for (int i=0; i<procTable->getSize(); i++){
					answer.push_back(i);
				}
			} else {
				// throw exception
			}
		} else {
			// nothing in answer
		}
	}
	return answer;
}

vector<int> QueryEvaluator::solveForSuchThatUses(string selectSynonym, map<STRING, STRING>* synonymTable, QueryTree* suchThatTree, StatementTable* statementTable, Uses* uses, ProcTable* procTable, VarTable* varTable){
	vector<int> answer;
	if (synonymTable->find(selectSynonym)==synonymTable->end()){ // if selectSynonym is not defined
		return answer;
	}
	bool isSynonymInSuchThat = checkSynonymInSuchThat(selectSynonym, suchThatTree);
	if (isSynonymInSuchThat){ // synonym is contained in such that statement
		if (selectSynonym == suchThatTree->getRootNode()->getChild(0)->getKey()){ // Select synonym such that Uses(synonym, *)
			string rightSynonym = suchThatTree->getRootNode()->getChild(1)->getKey();
			if(!rightSynonym.empty()){ // Select synonym such that Uses(synonym, *);
				if (synonymTable->find(rightSynonym) == synonymTable->end()){ // Select synonym such that Uses(synonym, aString);
					if (varTable->containsVar(rightSynonym)){ // if another synonym is a variable
						int varIndex = varTable->getVarIndex(rightSynonym);
						if (synonymTable->at(selectSynonym)=="procedure"){
							answer = uses->getUsersProc(varIndex);
						} else if (synonymTable->at(selectSynonym)=="stmt"){
							answer = uses->getUsersStmt(varIndex);
						} else if (synonymTable->at(selectSynonym)=="assign"){
							vector<int> temp = uses->getUsersStmt(varIndex);
							for (size_t i=0; i<temp.size(); i++){
								if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
									answer.push_back(temp.at(i));
								}
							}
						} else if (synonymTable->at(selectSynonym)=="while"){
							vector<int> temp = uses->getUsersStmt(varIndex);
							for (size_t i=0; i<temp.size(); i++){
								if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
									answer.push_back(temp.at(i));
								}
							}
						} else if (synonymTable->at(selectSynonym)=="if"){
							// not implemented
						} else {
							// throw exception
						}
					} else {
						// throw error because another synonym is not found
					}
				} else { // Select synonym such that Uses(symnonym, another synonym)
					if (synonymTable->at(rightSynonym) == "variable" && varTable->getSize()>=(int)0){
						if (synonymTable->at(selectSynonym) == "procedure"){						
							vector<pair<int, int>> result = uses->getUsesProc();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								temp.insert(result.at(i).first);
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym) == "stmt"){
							vector<pair<int, int>> result = uses->getUsesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								temp.insert(result.at(i).first);
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym) == "assign"){
							vector<pair<int, int>> result = uses->getUsesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								if (statementTable->getTNode(result.at(i).first)->getNodeType()=="assign"){
									temp.insert(result.at(i).first);
								}
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym) == "while"){
							vector<pair<int, int>> result = uses->getUsesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								if (statementTable->getTNode(result.at(i).first)->getNodeType()=="while"){
									temp.insert(result.at(i).first);
								}
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym) == "if"){
							// not implemented
						} else {
							// throw exception
						}
					} else {
						// throw exception
					}
				}
			} else {
				// throw exception
			}
		} else if (selectSynonym == suchThatTree->getRootNode()->getChild(1)->getKey()){ // Select synonym such that Uses(*, synonym)
			if (synonymTable->at(selectSynonym) == "variable"){
				string leftSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
				if (!leftSynonym.empty()){ // Select synonym such that Uses(anotherSynonym, synonym)
					if (synonymTable->find(leftSynonym) == synonymTable->end()){ // another synonym not in synonym table
						if (procTable->containsProc(leftSynonym)){ // if another synonym is a procedure
							int procIndex = procTable->getProcIndex(leftSynonym);
							answer = uses->getUsedVarProc(procIndex);
						} else {
							// throw error because another synonym is not found
						}
					} else { // another synonym is in synonym table
						if (synonymTable->at(leftSynonym)=="procedure"){
							vector<pair<int, int>> result = uses->getUsesProc();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								temp.insert(result.at(i).second);
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym)=="stmt"){
							vector<pair<int, int>> result = uses->getUsesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								temp.insert(result.at(i).second);
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym)=="assign"){
							vector<pair<int, int>> result = uses->getUsesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								if (statementTable->getTNode(result.at(i).first)->getNodeType()=="assign"){
									temp.insert(result.at(i).second);
								}
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym)=="while"){
							vector<pair<int, int>> result = uses->getUsesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								if (statementTable->getTNode(result.at(i).first)->getNodeType()=="while"){
									temp.insert(result.at(i).second);
								}
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym)=="if"){
							// not implemented
						} else {
							// throw exception
						}
					}
				} else { // Select synonym such that Uses(INDEX, synonym)
					int stmtIndex = suchThatTree->getRootNode()->getChild(0)->getValue();
					if (0<stmtIndex&&stmtIndex<=statementTable->getSize()){
						answer = uses->getUsedVarStmt(stmtIndex);
					} else {
						// throw exception
					}
				}
			} else {
				// throw exception
			}
		}
	} else { // synonym is not contained in such that statement
		// evaluate such that statement as boolean and solve for it
		bool isSuchThatTrue = false;
		string varSynonym = suchThatTree->getRootNode()->getChild(1)->getKey();
		if (!varSynonym.empty()){
			if (synonymTable->find(varSynonym) == synonymTable->end()){ // varSynonym is a variable name, Uses(*, "x")
				if (varTable->containsVar(varSynonym)){
					int varIndex = varTable->getVarIndex(varSynonym);
					string leftSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
					if (leftSynonym.empty()) { // Uses(1, "x")
						int stmtNumber = suchThatTree->getRootNode()->getChild(0)->getValue();
						if (0<stmtNumber&&stmtNumber<=statementTable->getSize()){
							isSuchThatTrue = uses->isUsesStmt(stmtNumber, varIndex);
						} else {
							// throw exception, or invalid query
						}
					} else { // Uses(leftSynonym, "x")
						if (synonymTable->find(leftSynonym) == synonymTable->end()){ // Uses("Procedure First", "x")
							if (procTable->containsProc(leftSynonym)){
								int procIndex = procTable->getProcIndex(leftSynonym);
								isSuchThatTrue = uses->isUsesProc(procIndex, varIndex);
							} else {
								// throw error because procedure not found
							}
						} else { // Uses(leftSynonym, "x")
							if (synonymTable->at(leftSynonym)=="procedure"){ // Uses(procedure, "x")
								for (int i=0; i<procTable->getSize(); i++){
									if (uses->isUsesProc(i, varIndex)){
										isSuchThatTrue = true;
										break;
									}
								}
							} else if (synonymTable->at(leftSynonym)=="stmt"){ // Uses(statement/while/assign, "x")
								for (int i=1; i<=statementTable->getSize(); i++){
									if (uses->isUsesStmt(i, varIndex)){
										isSuchThatTrue = true;
										break;
									}
								}
							} else if (synonymTable->at(leftSynonym)=="assign"){
								for (int i=1; i<=statementTable->getSize(); i++){
									if (statementTable->getTNode(i)->getNodeType()=="assign"&&uses->isUsesStmt(i, varIndex)){
										isSuchThatTrue = true;
										break;
									}
								}
							} else if (synonymTable->at(leftSynonym)=="while"){
								for (int i=1; i<=statementTable->getSize(); i++){
									if (statementTable->getTNode(i)->getNodeType()=="while"&&uses->isUsesStmt(i, varIndex)){
										isSuchThatTrue = true;
										break;
									}
								}
							} else if (synonymTable->at(leftSynonym)=="if"){
								// not implemented
							} else {
								// throw exception
							}
						}
					}
				} else {
					// throw exception because varSynonym is not found in program
				}
			} else { // varSynonym is a variable entity, Uses(*, var)
					string leftSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
					if (leftSynonym.empty()) { // Uses(1, var)
						int stmtNumber = suchThatTree->getRootNode()->getChild(0)->getValue();
						if (0<stmtNumber&&stmtNumber<=statementTable->getSize()){
							for (int i=0; i<varTable->getSize(); i++){
								if (uses->isUsesStmt(stmtNumber, i)){
									isSuchThatTrue = true;
									break;
								}
							}
						} else {
							// throw exception, or invalid query
						}
					} else { // Uses(leftSynonym, var)
						if (synonymTable->find(leftSynonym) == synonymTable->end()){ // Uses("Procedure First", var)
							if (procTable->containsProc(leftSynonym)){
								int procIndex = procTable->getProcIndex(leftSynonym);
								for (int i=0; i<varTable->getSize(); i++){
									if (uses->isUsesProc(procIndex, i)){
										isSuchThatTrue = true;
										break;
									}
								}
							} else {
								// throw error because procedure not found
							}
						} else { // Uses(leftSynonym, var)
							if (synonymTable->at(leftSynonym)=="procedure"){ // Uses(procedure, var)
								for (int i=0; i<procTable->getSize(); i++){
									for (int j=0; j<varTable->getSize(); j++){
										if (uses->isUsesProc(i, j)){
											isSuchThatTrue = true;
											break;
										}
									}
									if (isSuchThatTrue) break;
								}
							} else if (synonymTable->at(leftSynonym)=="stmt"){ // Uses(statement/while/assign/etc., var)
								for (int i=1; i<=statementTable->getSize(); i++){
									for (int j=0; j<varTable->getSize(); j++){
										if (uses->isUsesStmt(i, j)){
											isSuchThatTrue = true;
											break;
										}
									}
									if (isSuchThatTrue) break;
								}
							} else if (synonymTable->at(leftSynonym)=="assign"){
								for (int i=1; i<=statementTable->getSize(); i++){
									if (statementTable->getTNode(i)->getNodeType()=="assign"){
										for (int j=0; j<varTable->getSize(); j++){
											if (uses->isUsesStmt(i, j)){
												isSuchThatTrue = true;
												break;
											}											
										}
										if (isSuchThatTrue) break;
									}
									if (isSuchThatTrue) break;
								}
							} else if (synonymTable->at(leftSynonym)=="while"){
								for (int i=1; i<=statementTable->getSize(); i++){
									if (statementTable->getTNode(i)->getNodeType()=="while"){
										for (int j=0; j<varTable->getSize(); j++){
											if (uses->isUsesStmt(i, j)){
												isSuchThatTrue = true;
												break;
											}											
										}
										if (isSuchThatTrue) break;
									}
									if (isSuchThatTrue) break;
								}
							} else if (synonymTable->at(leftSynonym)=="if"){
								// not implemented
							} else {
								// throw exception							
							}
						}
					}
			} 
		} else {
			// throw exception
		}
		if (isSuchThatTrue){
			// fill answer for different type of selectSynonym such as procedure, statement, while, assign, variable, prog_line
			if (synonymTable->at(selectSynonym)=="stmt"||
				synonymTable->at(selectSynonym)=="assign"||
				synonymTable->at(selectSynonym)=="while"||
				synonymTable->at(selectSynonym)=="if"||
				synonymTable->at(selectSynonym)=="prog_line"){
					if (synonymTable->at(selectSynonym)=="stmt"||synonymTable->at(selectSynonym)=="prog_line"){
						for (int i=1; i<=statementTable->getSize(); i++){
							answer.push_back(i);
						}
					} else {
						answer = statementTable->getStmtNumUsingNodeType(synonymTable->at(selectSynonym));
					}					
			} else if (synonymTable->at(selectSynonym)=="variable"){
				for (int i=0; i<varTable->getSize(); i++){
					answer.push_back(i);
				}
			} else if (synonymTable->at(selectSynonym)=="procedure"){
				for (int i=0; i<procTable->getSize(); i++){
					answer.push_back(i);
				}
			} else {
				// throw exception
			}
		} else {
			// nothing in answer
		}
	}
	return answer;
}

/******* Helper Level 4 *******/
bool checkSynonymInSuchThat(string selectSynonym, QueryTree* suchThatTree){
	bool isSynonymInSuchThat = false;
	for (int i=0; i<suchThatTree->getRootNode()->getNumChild(); i++){
		if (selectSynonym == suchThatTree->getRootNode()->getChild(i)->getKey()){
			isSynonymInSuchThat = true;
			break;
		}
	}
	return isSynonymInSuchThat;
}

/******* Helper Level 3 *******/
vector<int> solveForSuchThatModifies(string selectSynonym, map<STRING, STRING>* synonymTable, QueryTree* suchThatTree, StatementTable* statementTable, Modifies* modifies, ProcTable* procTable, VarTable* varTable){
	vector<int> answer;
	if (synonymTable->find(selectSynonym)==synonymTable->end()){ // if selectSynonym is not defined
		return answer;
	}
	bool isSynonymInSuchThat = checkSynonymInSuchThat(selectSynonym, suchThatTree);
	if (isSynonymInSuchThat){ // synonym is contained in such that statement
		if (selectSynonym == suchThatTree->getRootNode()->getChild(0)->getKey()){ // Select synonym such that Modifies(synonym, *)
			string rightSynonym = suchThatTree->getRootNode()->getChild(1)->getKey();
			if(!rightSynonym.empty()){ // Select synonym such that Modifies(synonym, *);
				if (synonymTable->find(rightSynonym) == synonymTable->end()){ // Select synonym such that Modifies(synonym, aString);
					if (varTable->containsVar(rightSynonym)){ // if another synonym is a variable
						int varIndex = varTable->getVarIndex(rightSynonym);
						if (synonymTable->at(selectSynonym)=="procedure"){
							answer = modifies->getModifiersProc(varIndex);
						} else if (synonymTable->at(selectSynonym)=="stmt"){
							answer = modifies->getModifiersStmt(varIndex);
						} else if (synonymTable->at(selectSynonym)=="assign"){
							vector<int> temp = modifies->getModifiersStmt(varIndex);
							for (size_t i=0; i<temp.size(); i++){
								if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
									answer.push_back(temp.at(i));
								}
							}
						} else if (synonymTable->at(selectSynonym)=="while"){
							vector<int> temp = modifies->getModifiersStmt(varIndex);
							for (size_t i=0; i<temp.size(); i++){
								if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
									answer.push_back(temp.at(i));
								}
							}
						} else if (synonymTable->at(selectSynonym)=="if"){
							// not implemented
						} else {
							// throw exception
						}
					} else {
						// throw error because another synonym is not found
					}
				} else { // Select synonym such that Modifies(symnonym, another synonym)
					if (synonymTable->at(rightSynonym) == "variable" && varTable->getSize()>=(int)0){
						if (synonymTable->at(selectSynonym) == "procedure"){						
							vector<pair<int, int>> result = modifies->getModifiesProc();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								temp.insert(result.at(i).first);
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym) == "stmt"){
							vector<pair<int, int>> result = modifies->getModifiesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								temp.insert(result.at(i).first);
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym) == "assign"){
							vector<pair<int, int>> result = modifies->getModifiesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								if (statementTable->getTNode(result.at(i).first)->getNodeType()=="assign"){
									temp.insert(result.at(i).first);
								}
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym) == "while"){
							vector<pair<int, int>> result = modifies->getModifiesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								if (statementTable->getTNode(result.at(i).first)->getNodeType()=="while"){
									temp.insert(result.at(i).first);
								}
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym) == "if"){
							// not implemented
						} else {
							// throw exception
						}
					} else {
						// throw exception
					}
				}
			} else {
				// throw exception
			}
		} else if (selectSynonym == suchThatTree->getRootNode()->getChild(1)->getKey()){ // Select synonym such that Modifies(*, synonym)
			if (synonymTable->at(selectSynonym) == "variable"){
				string leftSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
				if (!leftSynonym.empty()){ // Select synonym such that Modifies(anotherSynonym, synonym)
					if (synonymTable->find(leftSynonym) == synonymTable->end()){ // another synonym not in synonym table
						if (procTable->containsProc(leftSynonym)){ // if another synonym is a procedure
							int procIndex = procTable->getProcIndex(leftSynonym);
							answer = modifies->getModifiedVarProc(procIndex);
						} else {
							// throw error because another synonym is not found
						}
					} else { // another synonym is in synonym table
						if (synonymTable->at(leftSynonym)=="procedure"){
							vector<pair<int, int>> result = modifies->getModifiesProc();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								temp.insert(result.at(i).second);
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym)=="stmt"){
							vector<pair<int, int>> result = modifies->getModifiesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								temp.insert(result.at(i).second);
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym)=="assign"){
							vector<pair<int, int>> result = modifies->getModifiesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								if (statementTable->getTNode(result.at(i).first)->getNodeType()=="assign"){
									temp.insert(result.at(i).second);
								}
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym)=="while"){
							vector<pair<int, int>> result = modifies->getModifiesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								if (statementTable->getTNode(result.at(i).first)->getNodeType()=="while"){
									temp.insert(result.at(i).second);
								}
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym)=="if"){
							// not implemented
						} else {
							// throw exception
						}
					}
				} else { // Select synonym such that Modifies(INDEX, synonym)
					int stmtIndex = suchThatTree->getRootNode()->getChild(0)->getValue();
					if (0<stmtIndex&&stmtIndex<=statementTable->getSize()){
						answer = modifies->getModifiedVarStmt(stmtIndex);
					} else {
						// throw exception
					}
				}
			} else {
				// throw exception
			}
		}
	} else { // synonym is not contained in such that statement
		// evaluate such that statement as boolean and solve for it
		bool isSuchThatTrue = false;
		string varSynonym = suchThatTree->getRootNode()->getChild(1)->getKey();
		if (!varSynonym.empty()){
			if (synonymTable->find(varSynonym) == synonymTable->end()){ // varSynonym is a variable name, Modifies(*, "x")
				if (varTable->containsVar(varSynonym)){
					int varIndex = varTable->getVarIndex(varSynonym);
					string leftSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
					if (leftSynonym.empty()) { // Modifies(1, "x")
						int stmtNumber = suchThatTree->getRootNode()->getChild(0)->getValue();
						if (0<stmtNumber&&stmtNumber<=statementTable->getSize()){
							isSuchThatTrue = modifies->isModifiesStmt(stmtNumber, varIndex);
						} else {
							// throw exception, or invalid query
						}
					} else { // Modifies(leftSynonym, "x")
						if (synonymTable->find(leftSynonym) == synonymTable->end()){ // Modifies("Procedure First", "x")
							if (procTable->containsProc(leftSynonym)){
								int procIndex = procTable->getProcIndex(leftSynonym);
								isSuchThatTrue = modifies->isModifiesProc(procIndex, varIndex);
							} else {
								// throw error because procedure not found
							}
						} else { // Modifies(leftSynonym, "x")
							if (synonymTable->at(leftSynonym)=="procedure"){ // Modifies(procedure, "x")
								for (int i=0; i<procTable->getSize(); i++){
									if (modifies->isModifiesProc(i, varIndex)){
										isSuchThatTrue = true;
										break;
									}
								}
							} else if (synonymTable->at(leftSynonym)=="stmt"){ // Modifies(statement/while/assign, "x")
								for (int i=1; i<=statementTable->getSize(); i++){
									if (modifies->isModifiesStmt(i, varIndex)){
										isSuchThatTrue = true;
										break;
									}
								}
							} else if (synonymTable->at(leftSynonym)=="assign"){
								for (int i=1; i<=statementTable->getSize(); i++){
									if (statementTable->getTNode(i)->getNodeType()=="assign"&&modifies->isModifiesStmt(i, varIndex)){
										isSuchThatTrue = true;
										break;
									}
								}
							} else if (synonymTable->at(leftSynonym)=="while"){
								for (int i=1; i<=statementTable->getSize(); i++){
									if (statementTable->getTNode(i)->getNodeType()=="while"&&modifies->isModifiesStmt(i, varIndex)){
										isSuchThatTrue = true;
										break;
									}
								}
							} else if (synonymTable->at(leftSynonym)=="if"){
								// not implemented
							} else {
								// throw exception
							}
						}
					}
				} else {
					// throw exception because varSynonym is not found in program
				}
			} else { // varSynonym is a variable entity, Modifies(*, var)
					string leftSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
					if (leftSynonym.empty()) { // Modifies(1, var)
						int stmtNumber = suchThatTree->getRootNode()->getChild(0)->getValue();
						if (0<stmtNumber&&stmtNumber<=statementTable->getSize()){
							for (int i=0; i<varTable->getSize(); i++){
								if (modifies->isModifiesStmt(stmtNumber, i)){
									isSuchThatTrue = true;
									break;
								}
							}
						} else {
							// throw exception, or invalid query
						}
					} else { // Modifies(leftSynonym, var)
						if (synonymTable->find(leftSynonym) == synonymTable->end()){ // Modifies("Procedure First", var)
							if (procTable->containsProc(leftSynonym)){
								int procIndex = procTable->getProcIndex(leftSynonym);
								for (int i=0; i<varTable->getSize(); i++){
									if (modifies->isModifiesProc(procIndex, i)){
										isSuchThatTrue = true;
										break;
									}
								}
							} else {
								// throw error because procedure not found
							}
						} else { // Modifies(leftSynonym, var)
							if (synonymTable->at(leftSynonym)=="procedure"){ // Modifies(procedure, var)
								for (int i=0; i<procTable->getSize(); i++){
									for (int j=0; j<varTable->getSize(); j++){
										if (modifies->isModifiesProc(i, j)){
											isSuchThatTrue = true;
											break;
										}
									}
									if (isSuchThatTrue) break;
								}
							} else if (synonymTable->at(leftSynonym)=="stmt"){ // Modifies(statement/while/assign/etc., var)
								for (int i=1; i<=statementTable->getSize(); i++){
									for (int j=0; j<varTable->getSize(); j++){
										if (modifies->isModifiesStmt(i, j)){
											isSuchThatTrue = true;
											break;
										}
									}
									if (isSuchThatTrue) break;
								}
							} else if (synonymTable->at(leftSynonym)=="assign"){
								for (int i=1; i<=statementTable->getSize(); i++){
									if (statementTable->getTNode(i)->getNodeType()=="assign"){
										for (int j=0; j<varTable->getSize(); j++){
											if (modifies->isModifiesStmt(i, j)){
												isSuchThatTrue = true;
												break;
											}											
										}
										if (isSuchThatTrue) break;
									}
									if (isSuchThatTrue) break;
								}
							} else if (synonymTable->at(leftSynonym)=="while"){
								for (int i=1; i<=statementTable->getSize(); i++){
									if (statementTable->getTNode(i)->getNodeType()=="while"){
										for (int j=0; j<varTable->getSize(); j++){
											if (modifies->isModifiesStmt(i, j)){
												isSuchThatTrue = true;
												break;
											}											
										}
										if (isSuchThatTrue) break;
									}
									if (isSuchThatTrue) break;
								}
							} else if (synonymTable->at(leftSynonym)=="if"){
								// not implemented
							} else {
								// throw exception							
							}
						}
					}
			} 
		} else {
			// throw exception
		}
		if (isSuchThatTrue){
			// fill answer for different type of selectSynonym such as procedure, statement, while, assign, variable, prog_line
			if (synonymTable->at(selectSynonym)=="stmt"||
				synonymTable->at(selectSynonym)=="assign"||
				synonymTable->at(selectSynonym)=="while"||
				synonymTable->at(selectSynonym)=="if"||
				synonymTable->at(selectSynonym)=="prog_line"){
					if (synonymTable->at(selectSynonym)=="stmt"||synonymTable->at(selectSynonym)=="prog_line"){
						for (int i=1; i<=statementTable->getSize(); i++){
							answer.push_back(i);
						}
					} else {
						answer = statementTable->getStmtNumUsingNodeType(synonymTable->at(selectSynonym));
					}					
			} else if (synonymTable->at(selectSynonym)=="variable"){
				for (int i=0; i<varTable->getSize(); i++){
					answer.push_back(i);
				}
			} else if (synonymTable->at(selectSynonym)=="procedure"){
				for (int i=0; i<procTable->getSize(); i++){
					answer.push_back(i);
				}
			} else {
				// throw exception
			}
		} else {
			// nothing in answer
		}
	}
	return answer;
}

vector<int> solveForSuchThatUses(string selectSynonym, map<STRING, STRING>* synonymTable, QueryTree* suchThatTree, StatementTable* statementTable, Uses* uses, ProcTable* procTable, VarTable* varTable){
	vector<int> answer;
	if (synonymTable->find(selectSynonym)==synonymTable->end()){ // if selectSynonym is not defined
		return answer;
	}
	bool isSynonymInSuchThat = checkSynonymInSuchThat(selectSynonym, suchThatTree);
	if (isSynonymInSuchThat){ // synonym is contained in such that statement
		if (selectSynonym == suchThatTree->getRootNode()->getChild(0)->getKey()){ // Select synonym such that Uses(synonym, *)
			string rightSynonym = suchThatTree->getRootNode()->getChild(1)->getKey();
			if(!rightSynonym.empty()){ // Select synonym such that Uses(synonym, *);
				if (synonymTable->find(rightSynonym) == synonymTable->end()){ // Select synonym such that Uses(synonym, aString);
					if (varTable->containsVar(rightSynonym)){ // if another synonym is a variable
						int varIndex = varTable->getVarIndex(rightSynonym);
						if (synonymTable->at(selectSynonym)=="procedure"){
							answer = uses->getUsersProc(varIndex);
						} else if (synonymTable->at(selectSynonym)=="stmt"){
							answer = uses->getUsersStmt(varIndex);
						} else if (synonymTable->at(selectSynonym)=="assign"){
							vector<int> temp = uses->getUsersStmt(varIndex);
							for (size_t i=0; i<temp.size(); i++){
								if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
									answer.push_back(temp.at(i));
								}
							}
						} else if (synonymTable->at(selectSynonym)=="while"){
							vector<int> temp = uses->getUsersStmt(varIndex);
							for (size_t i=0; i<temp.size(); i++){
								if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
									answer.push_back(temp.at(i));
								}
							}
						} else if (synonymTable->at(selectSynonym)=="if"){
							// not implemented
						} else {
							// throw exception
						}
					} else {
						// throw error because another synonym is not found
					}
				} else { // Select synonym such that Uses(symnonym, another synonym)
					if (synonymTable->at(rightSynonym) == "variable" && varTable->getSize()>=(int)0){
						if (synonymTable->at(selectSynonym) == "procedure"){						
							vector<pair<int, int>> result = uses->getUsesProc();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								temp.insert(result.at(i).first);
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym) == "stmt"){
							vector<pair<int, int>> result = uses->getUsesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								temp.insert(result.at(i).first);
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym) == "assign"){
							vector<pair<int, int>> result = uses->getUsesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								if (statementTable->getTNode(result.at(i).first)->getNodeType()=="assign"){
									temp.insert(result.at(i).first);
								}
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym) == "while"){
							vector<pair<int, int>> result = uses->getUsesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								if (statementTable->getTNode(result.at(i).first)->getNodeType()=="while"){
									temp.insert(result.at(i).first);
								}
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym) == "if"){
							// not implemented
						} else {
							// throw exception
						}
					} else {
						// throw exception
					}
				}
			} else {
				// throw exception
			}
		} else if (selectSynonym == suchThatTree->getRootNode()->getChild(1)->getKey()){ // Select synonym such that Uses(*, synonym)
			if (synonymTable->at(selectSynonym) == "variable"){
				string leftSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
				if (!leftSynonym.empty()){ // Select synonym such that Uses(anotherSynonym, synonym)
					if (synonymTable->find(leftSynonym) == synonymTable->end()){ // another synonym not in synonym table
						if (procTable->containsProc(leftSynonym)){ // if another synonym is a procedure
							int procIndex = procTable->getProcIndex(leftSynonym);
							answer = uses->getUsedVarProc(procIndex);
						} else {
							// throw error because another synonym is not found
						}
					} else { // another synonym is in synonym table
						if (synonymTable->at(leftSynonym)=="procedure"){
							vector<pair<int, int>> result = uses->getUsesProc();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								temp.insert(result.at(i).second);
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym)=="stmt"){
							vector<pair<int, int>> result = uses->getUsesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								temp.insert(result.at(i).second);
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym)=="assign"){
							vector<pair<int, int>> result = uses->getUsesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								if (statementTable->getTNode(result.at(i).first)->getNodeType()=="assign"){
									temp.insert(result.at(i).second);
								}
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym)=="while"){
							vector<pair<int, int>> result = uses->getUsesStmt();
							set<int> temp;
							for (size_t i=0; i<result.size(); i++){
								if (statementTable->getTNode(result.at(i).first)->getNodeType()=="while"){
									temp.insert(result.at(i).second);
								}
							}
							vector<int> output(temp.begin(), temp.end());
							answer = output;
						} else if (synonymTable->at(selectSynonym)=="if"){
							// not implemented
						} else {
							// throw exception
						}
					}
				} else { // Select synonym such that Uses(INDEX, synonym)
					int stmtIndex = suchThatTree->getRootNode()->getChild(0)->getValue();
					if (0<stmtIndex&&stmtIndex<=statementTable->getSize()){
						answer = uses->getUsedVarStmt(stmtIndex);
					} else {
						// throw exception
					}
				}
			} else {
				// throw exception
			}
		}
	} else { // synonym is not contained in such that statement
		// evaluate such that statement as boolean and solve for it
		bool isSuchThatTrue = false;
		string varSynonym = suchThatTree->getRootNode()->getChild(1)->getKey();
		if (!varSynonym.empty()){
			if (synonymTable->find(varSynonym) == synonymTable->end()){ // varSynonym is a variable name, Uses(*, "x")
				if (varTable->containsVar(varSynonym)){
					int varIndex = varTable->getVarIndex(varSynonym);
					string leftSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
					if (leftSynonym.empty()) { // Uses(1, "x")
						int stmtNumber = suchThatTree->getRootNode()->getChild(0)->getValue();
						if (0<stmtNumber&&stmtNumber<=statementTable->getSize()){
							isSuchThatTrue = uses->isUsesStmt(stmtNumber, varIndex);
						} else {
							// throw exception, or invalid query
						}
					} else { // Uses(leftSynonym, "x")
						if (synonymTable->find(leftSynonym) == synonymTable->end()){ // Uses("Procedure First", "x")
							if (procTable->containsProc(leftSynonym)){
								int procIndex = procTable->getProcIndex(leftSynonym);
								isSuchThatTrue = uses->isUsesProc(procIndex, varIndex);
							} else {
								// throw error because procedure not found
							}
						} else { // Uses(leftSynonym, "x")
							if (synonymTable->at(leftSynonym)=="procedure"){ // Uses(procedure, "x")
								for (int i=0; i<procTable->getSize(); i++){
									if (uses->isUsesProc(i, varIndex)){
										isSuchThatTrue = true;
										break;
									}
								}
							} else if (synonymTable->at(leftSynonym)=="stmt"){ // Uses(statement/while/assign, "x")
								for (int i=1; i<=statementTable->getSize(); i++){
									if (uses->isUsesStmt(i, varIndex)){
										isSuchThatTrue = true;
										break;
									}
								}
							} else if (synonymTable->at(leftSynonym)=="assign"){
								for (int i=1; i<=statementTable->getSize(); i++){
									if (statementTable->getTNode(i)->getNodeType()=="assign"&&uses->isUsesStmt(i, varIndex)){
										isSuchThatTrue = true;
										break;
									}
								}
							} else if (synonymTable->at(leftSynonym)=="while"){
								for (int i=1; i<=statementTable->getSize(); i++){
									if (statementTable->getTNode(i)->getNodeType()=="while"&&uses->isUsesStmt(i, varIndex)){
										isSuchThatTrue = true;
										break;
									}
								}
							} else if (synonymTable->at(leftSynonym)=="if"){
								// not implemented
							} else {
								// throw exception
							}
						}
					}
				} else {
					// throw exception because varSynonym is not found in program
				}
			} else { // varSynonym is a variable entity, Uses(*, var)
					string leftSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
					if (leftSynonym.empty()) { // Uses(1, var)
						int stmtNumber = suchThatTree->getRootNode()->getChild(0)->getValue();
						if (0<stmtNumber&&stmtNumber<=statementTable->getSize()){
							for (int i=0; i<varTable->getSize(); i++){
								if (uses->isUsesStmt(stmtNumber, i)){
									isSuchThatTrue = true;
									break;
								}
							}
						} else {
							// throw exception, or invalid query
						}
					} else { // Uses(leftSynonym, var)
						if (synonymTable->find(leftSynonym) == synonymTable->end()){ // Uses("Procedure First", var)
							if (procTable->containsProc(leftSynonym)){
								int procIndex = procTable->getProcIndex(leftSynonym);
								for (int i=0; i<varTable->getSize(); i++){
									if (uses->isUsesProc(procIndex, i)){
										isSuchThatTrue = true;
										break;
									}
								}
							} else {
								// throw error because procedure not found
							}
						} else { // Uses(leftSynonym, var)
							if (synonymTable->at(leftSynonym)=="procedure"){ // Uses(procedure, var)
								for (int i=0; i<procTable->getSize(); i++){
									for (int j=0; j<varTable->getSize(); j++){
										if (uses->isUsesProc(i, j)){
											isSuchThatTrue = true;
											break;
										}
									}
									if (isSuchThatTrue) break;
								}
							} else if (synonymTable->at(leftSynonym)=="stmt"){ // Uses(statement/while/assign/etc., var)
								for (int i=1; i<=statementTable->getSize(); i++){
									for (int j=0; j<varTable->getSize(); j++){
										if (uses->isUsesStmt(i, j)){
											isSuchThatTrue = true;
											break;
										}
									}
									if (isSuchThatTrue) break;
								}
							} else if (synonymTable->at(leftSynonym)=="assign"){
								for (int i=1; i<=statementTable->getSize(); i++){
									if (statementTable->getTNode(i)->getNodeType()=="assign"){
										for (int j=0; j<varTable->getSize(); j++){
											if (uses->isUsesStmt(i, j)){
												isSuchThatTrue = true;
												break;
											}											
										}
										if (isSuchThatTrue) break;
									}
									if (isSuchThatTrue) break;
								}
							} else if (synonymTable->at(leftSynonym)=="while"){
								for (int i=1; i<=statementTable->getSize(); i++){
									if (statementTable->getTNode(i)->getNodeType()=="while"){
										for (int j=0; j<varTable->getSize(); j++){
											if (uses->isUsesStmt(i, j)){
												isSuchThatTrue = true;
												break;
											}											
										}
										if (isSuchThatTrue) break;
									}
									if (isSuchThatTrue) break;
								}
							} else if (synonymTable->at(leftSynonym)=="if"){
								// not implemented
							} else {
								// throw exception							
							}
						}
					}
			} 
		} else {
			// throw exception
		}
		if (isSuchThatTrue){
			// fill answer for different type of selectSynonym such as procedure, statement, while, assign, variable, prog_line
			if (synonymTable->at(selectSynonym)=="stmt"||
				synonymTable->at(selectSynonym)=="assign"||
				synonymTable->at(selectSynonym)=="while"||
				synonymTable->at(selectSynonym)=="if"||
				synonymTable->at(selectSynonym)=="prog_line"){
					if (synonymTable->at(selectSynonym)=="stmt"||synonymTable->at(selectSynonym)=="prog_line"){
						for (int i=1; i<=statementTable->getSize(); i++){
							answer.push_back(i);
						}
					} else {
						answer = statementTable->getStmtNumUsingNodeType(synonymTable->at(selectSynonym));
					}					
			} else if (synonymTable->at(selectSynonym)=="variable"){
				for (int i=0; i<varTable->getSize(); i++){
					answer.push_back(i);
				}
			} else if (synonymTable->at(selectSynonym)=="procedure"){
				for (int i=0; i<procTable->getSize(); i++){
					answer.push_back(i);
				}
			} else {
				// throw exception
			}
		} else {
			// nothing in answer
		}
	}
	return answer;
}

vector<int> solveForSuchThatFollows(string selectSynonym, map<STRING, STRING>* synonymTable, QueryTree* suchThatTree, StatementTable* statementTable, Follows* follows, ProcTable* procTable, VarTable* varTable){
	vector<int> answer;
	if (synonymTable->find(selectSynonym)==synonymTable->end()){ // if selectSynonym is not defined
		return answer;
	}
	bool isSynonymInSuchThat = checkSynonymInSuchThat(selectSynonym, suchThatTree);
	if (isSynonymInSuchThat){ // Select synonym such that Follows(synonym, *) (Follows(*, synonym))
		if (selectSynonym == suchThatTree->getRootNode()->getChild(0)->getKey()){ // Select synonym such that Follows(synonym, *)
			string stmtSynonym = suchThatTree->getRootNode()->getChild(1)->getKey();
			if (stmtSynonym.empty()){ // Select synonym such that Follows(synonym, 1)
				int stmtIndex = suchThatTree->getRootNode()->getChild(1)->getValue();
				if(stmtIndex<=statementTable->getSize()){
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = follows->getFollowees(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = follows->getFollowees(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = follows->getFollowees(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = follows->getFollowees(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception or return empty
				}
			} else { // Select synonym such that Follows(synonym, another Synonym)
				if (synonymTable->find(stmtSynonym) != synonymTable->end()){
					vector<int> dataFromStmtSynonym;
					if(synonymTable->at(stmtSynonym)=="stmt"){
						dataFromStmtSynonym = follows->getAllFolloweeStmt();
					} else if (synonymTable->at(stmtSynonym)=="prog_line"){
						dataFromStmtSynonym = follows->getAllFolloweeStmt();
					} else if (synonymTable->at(stmtSynonym)=="assign"){
						vector<int> assignList = statementTable->getStmtNumUsingNodeType("assign");
						for (size_t i=0; i<assignList.size(); i++){
							vector<int> result = follows->getFollowees(assignList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="while"){
						vector<int> whileList = statementTable->getStmtNumUsingNodeType("while");
						for (size_t i=0; i<whileList.size(); i++){
							vector<int> result = follows->getFollowees(whileList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="if"){
						// not implemented	
					} else {
						// throw exception
					}
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception
				}
			}
		} else { // Select synonym such that Follows(*, synonym)
			string stmtSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
			if (stmtSynonym.empty()){ // Select synonym such that Follows(1, synonym)
				int stmtIndex = suchThatTree->getRootNode()->getChild(0)->getValue();
				if(stmtIndex<=statementTable->getSize()){
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = follows->getFollowers(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = follows->getFollowers(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = follows->getFollowers(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = follows->getFollowers(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception or return empty
				}
			} else { // Select synonym such that Follows(another Synonym, synonym)
				if (synonymTable->find(stmtSynonym) != synonymTable->end()){
					vector<int> dataFromStmtSynonym;
					if(synonymTable->at(stmtSynonym)=="stmt"){
						dataFromStmtSynonym = follows->getAllFollowerStmt();
					} else if (synonymTable->at(stmtSynonym)=="prog_line"){
						dataFromStmtSynonym = follows->getAllFollowerStmt();
					} else if (synonymTable->at(stmtSynonym)=="assign"){
						vector<int> assignList = statementTable->getStmtNumUsingNodeType("assign");
						for (size_t i=0; i<assignList.size(); i++){
							vector<int> result = follows->getFollowers(assignList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="while"){
						vector<int> whileList = statementTable->getStmtNumUsingNodeType("while");
						for (size_t i=0; i<whileList.size(); i++){
							vector<int> result = follows->getFollowers(whileList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="if"){
						// not implemented	
					} else {
						// throw exception
					}
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception
				}
			}
		}
	} else { // synonym is not in Follows(*, *)
		bool isSuchThatTrue = false;
		string leftSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
		string rightSynonym = suchThatTree->getRootNode()->getChild(1)->getKey();
		if (leftSynonym.empty()&&rightSynonym.empty()){ // Follows(1, 2);
			int leftStmtIndex = suchThatTree->getRootNode()->getChild(0)->getValue();
			int rightStmtIndex = suchThatTree->getRootNode()->getChild(1)->getValue();
			if (leftStmtIndex>statementTable->getSize()||rightStmtIndex>statementTable->getSize()){
				// throw exception
			} else {
				isSuchThatTrue = follows->isFollowsTrue(leftStmtIndex, rightStmtIndex);
			}
		} else if (leftSynonym.empty()){ // Follows(1, synonym)
			int leftStmtIndex = suchThatTree->getRootNode()->getChild(0)->getValue();
			if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (follows->isFollowsTrue(leftStmtIndex, i)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(rightSynonym)=="assign"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="assign"&&follows->isFollowsTrue(leftStmtIndex, i)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(rightSynonym)=="while"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="while"&&follows->isFollowsTrue(leftStmtIndex, i)){
						isSuchThatTrue = true;
						break;
					}
				}		
			} else if (synonymTable->at(rightSynonym)=="if"){
				// not implemented
			} else {
				// throw exception
			}
		} else if (rightSynonym.empty()){ // Follows(synonym, 2)
			int rightStmtIndex = suchThatTree->getRootNode()->getChild(1)->getValue();
			if (synonymTable->at(leftSynonym)=="stmt"||synonymTable->at(leftSynonym)=="prog_line"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (follows->isFollowsTrue(i, rightStmtIndex)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(leftSynonym)=="assign"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="assign"&&follows->isFollowsTrue(i, rightStmtIndex)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(leftSynonym)=="while"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="while"&&follows->isFollowsTrue(i, rightStmtIndex)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(leftSynonym)=="if"){
				// not implemented
			} else {
				// throw exception
			}
		} else { // Follows(synonym1, synonym2)
			if(synonymTable->at(leftSynonym).empty()||synonymTable->at(rightSynonym).empty()){
				// throw exception
			} else if (synonymTable->at(leftSynonym)=="stmt"||synonymTable->at(leftSynonym)=="prog_line"){ // Follows(s1, synonym2)
				if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){ // Follows(s1, s2) 
					for (int i=1; i<=statementTable->getSize(); i++){
						for (int j=1; j<=statementTable->getSize(); j++){
							if (follows->isFollowsTrue(i, j)){
								isSuchThatTrue = true;
								break;
							}		
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="assign"){ // Follows(s1, a2)
					for (int i=1; i<=statementTable->getSize(); i++){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="assign"&&follows->isFollowsTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="while"){ // Follows(s1, w2)
					for (int i=1; i<=statementTable->getSize(); i++){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="while"&&follows->isFollowsTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
					}			
				} else if (synonymTable->at(rightSynonym)=="if"){
					// not implemented
				} else {
					// throw exception
				}
			} else if (synonymTable->at(leftSynonym)=="assign"){ // Follows(a1, synonym2)
				if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){ // Follows(a1, s2) 
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="assign"){
							for (int j=1; j<statementTable->getSize(); j++){
								if (follows->isFollowsTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="assign"){ // Follows(a1, a2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="assign"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="assign"&&follows->isFollowsTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="while"){ // Follows(a1, w2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="assign"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="while"&&follows->isFollowsTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="if"){
					// not implemented
				} else {
					// throw exception
				}
			} else if (synonymTable->at(leftSynonym)=="while"){ // Follows(w1, synonym2)
				if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){ // Follows(w1, s2) 
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="while"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (follows->isFollowsTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="assign"){ // Follows(w1, a2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="while"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="assign"&&follows->isFollowsTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="while"){ // Follows(w1, w2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="while"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="while"&&follows->isFollowsTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="if"){
					// not implemented
				} else {
					// throw exception
				}
			} else if (synonymTable->at(leftSynonym)=="if"){
				// not implemented
			} else {
				// throw exception
			}
		}
		if (isSuchThatTrue){
			// fill answer for different type of selectSynonym such as procedure, statement, while, assign, variable, prog_line
			if (synonymTable->at(selectSynonym)=="stmt"||
				synonymTable->at(selectSynonym)=="assign"||
				synonymTable->at(selectSynonym)=="while"||
				synonymTable->at(selectSynonym)=="if"||
				synonymTable->at(selectSynonym)=="prog_line"){
					if (synonymTable->at(selectSynonym)=="stmt"||synonymTable->at(selectSynonym)=="prog_line"){
						for (int i=1; i<=statementTable->getSize(); i++){
							answer.push_back(i);
						}
					} else {
						answer = statementTable->getStmtNumUsingNodeType(synonymTable->at(selectSynonym));
					}					
			} else if (synonymTable->at(selectSynonym)=="variable"){
				for (int i=1; i<=varTable->getSize(); i++){
					answer.push_back(i);
				}
			} else if (synonymTable->at(selectSynonym)=="procedure"){
				for (int i=1; i<=procTable->getSize(); i++){
					answer.push_back(i);
				}
			} else {
				// throw exception
			}
		} else {
			// nothing in answer
		}
	}
	return answer;
}

vector<int> solveForSuchThatFollowsStar(string selectSynonym, map<STRING, STRING>* synonymTable, QueryTree* suchThatTree, StatementTable* statementTable, Follows* follows, ProcTable* procTable, VarTable* varTable){
	vector<int> answer;
	if (synonymTable->find(selectSynonym)==synonymTable->end()){ // if selectSynonym is not defined
		return answer;
	}
	bool isSynonymInSuchThat = checkSynonymInSuchThat(selectSynonym, suchThatTree);
	if (isSynonymInSuchThat){ // Select synonym such that Follows*(synonym, *) (Follows*(*, synonym))
		if (selectSynonym == suchThatTree->getRootNode()->getChild(0)->getKey()){ // Select synonym such that Follows*(synonym, *)
			string stmtSynonym = suchThatTree->getRootNode()->getChild(1)->getKey();
			if (stmtSynonym.empty()){ // Select synonym such that Follows*(synonym, 1)
				int stmtIndex = suchThatTree->getRootNode()->getChild(1)->getValue();
				if(stmtIndex<=statementTable->getSize()){
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = follows->getFolloweesStar(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = follows->getFolloweesStar(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = follows->getFolloweesStar(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = follows->getFolloweesStar(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception or return empty
				}
			} else { // Select synonym such that Follows*(synonym, another Synonym)
				if (synonymTable->find(stmtSynonym) != synonymTable->end()){
					vector<int> dataFromStmtSynonym;
					if(synonymTable->at(stmtSynonym)=="stmt"){
						dataFromStmtSynonym = follows->getAllFolloweeStmt();
					} else if (synonymTable->at(stmtSynonym)=="prog_line"){
						dataFromStmtSynonym = follows->getAllFolloweeStmt();
					} else if (synonymTable->at(stmtSynonym)=="assign"){
						vector<int> assignList = statementTable->getStmtNumUsingNodeType("assign");
						for (size_t i=0; i<assignList.size(); i++){
							vector<int> result = follows->getFolloweesStar(assignList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="while"){
						vector<int> whileList = statementTable->getStmtNumUsingNodeType("while");
						for (size_t i=0; i<whileList.size(); i++){
							vector<int> result = follows->getFolloweesStar(whileList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="if"){
						// not implemented	
					} else {
						// throw exception
					}
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception
				}
			}
		} else { // Select synonym such that Follows*(*, synonym)
			string stmtSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
			if (stmtSynonym.empty()){ // Select synonym such that Follows*(1, synonym)
				int stmtIndex = suchThatTree->getRootNode()->getChild(0)->getValue();
				if(stmtIndex<=statementTable->getSize()){
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = follows->getFollowersStar(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = follows->getFollowersStar(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = follows->getFollowersStar(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = follows->getFollowersStar(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception or return empty
				}
			} else { // Select synonym such that Follows*(another Synonym, synonym)
				if (synonymTable->find(stmtSynonym) != synonymTable->end()){
					vector<int> dataFromStmtSynonym;
					if(synonymTable->at(stmtSynonym)=="stmt"){
						dataFromStmtSynonym = follows->getAllFollowerStmt();
					} else if (synonymTable->at(stmtSynonym)=="prog_line"){
						dataFromStmtSynonym = follows->getAllFollowerStmt();
					} else if (synonymTable->at(stmtSynonym)=="assign"){
						vector<int> assignList = statementTable->getStmtNumUsingNodeType("assign");
						for (size_t i=0; i<assignList.size(); i++){
							vector<int> result = follows->getFollowersStar(assignList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="while"){
						vector<int> whileList = statementTable->getStmtNumUsingNodeType("while");
						for (size_t i=0; i<whileList.size(); i++){
							vector<int> result = follows->getFollowersStar(whileList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="if"){
						// not implemented	
					} else {
						// throw exception
					}
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception
				}
			}
		}
	} else { // synonym is not in Follows*(*, *)
		bool isSuchThatTrue = false;
		string leftSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
		string rightSynonym = suchThatTree->getRootNode()->getChild(1)->getKey();
		if (leftSynonym.empty()&&rightSynonym.empty()){ // Follows*(1, 2);
			int leftStmtIndex = suchThatTree->getRootNode()->getChild(0)->getValue();
			int rightStmtIndex = suchThatTree->getRootNode()->getChild(1)->getValue();
			if (leftStmtIndex>statementTable->getSize()||rightStmtIndex>statementTable->getSize()){
				// throw exception
			} else {
				isSuchThatTrue = follows->isFollowsStarTrue(leftStmtIndex, rightStmtIndex);
			}
		} else if (leftSynonym.empty()){ // Follows*(1, synonym)
			int leftStmtIndex = suchThatTree->getRootNode()->getChild(0)->getValue();
			if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (follows->isFollowsStarTrue(leftStmtIndex, i)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(rightSynonym)=="assign"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="assign"&&follows->isFollowsStarTrue(leftStmtIndex, i)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(rightSynonym)=="while"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="while"&&follows->isFollowsStarTrue(leftStmtIndex, i)){
						isSuchThatTrue = true;
						break;
					}
				}		
			} else if (synonymTable->at(rightSynonym)=="if"){
				// not implemented
			} else {
				// throw exception
			}
		} else if (rightSynonym.empty()){ // Follows*(synonym, 2)
			int rightStmtIndex = suchThatTree->getRootNode()->getChild(1)->getValue();
			if (synonymTable->at(leftSynonym)=="stmt"||synonymTable->at(leftSynonym)=="prog_line"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (follows->isFollowsStarTrue(i, rightStmtIndex)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(leftSynonym)=="assign"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="assign"&&follows->isFollowsStarTrue(i, rightStmtIndex)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(leftSynonym)=="while"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="while"&&follows->isFollowsStarTrue(i, rightStmtIndex)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(leftSynonym)=="if"){
				// not implemented
			} else {
				// throw exception
			}
		} else { // Follows*(synonym1, synonym2)
			if(synonymTable->at(leftSynonym).empty()||synonymTable->at(rightSynonym).empty()){
				// throw exception
			} else if (synonymTable->at(leftSynonym)=="stmt"||synonymTable->at(leftSynonym)=="prog_line"){ // Follows*(s1, synonym2)
				if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){ // Follows*(s1, s2) 
					for (int i=1; i<=statementTable->getSize(); i++){
						for (int j=1; j<=statementTable->getSize(); j++){
							if (follows->isFollowsStarTrue(i, j)){
								isSuchThatTrue = true;
								break;
							}		
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="assign"){ // Follows*(s1, a2)
					for (int i=1; i<=statementTable->getSize(); i++){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="assign"&&follows->isFollowsStarTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="while"){ // Follows*(s1, w2)
					for (int i=1; i<=statementTable->getSize(); i++){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="while"&&follows->isFollowsStarTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
					}			
				} else if (synonymTable->at(rightSynonym)=="if"){
					// not implemented
				} else {
					// throw exception
				}
			} else if (synonymTable->at(leftSynonym)=="assign"){ // Follows*(a1, synonym2)
				if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){ // Follows*(a1, s2) 
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="assign"){
							for (int j=1; j<statementTable->getSize(); j++){
								if (follows->isFollowsStarTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="assign"){ // Follows*(a1, a2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="assign"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="assign"&&follows->isFollowsStarTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="while"){ // Follows*(a1, w2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="assign"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="while"&&follows->isFollowsStarTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="if"){
					// not implemented
				} else {
					// throw exception
				}
			} else if (synonymTable->at(leftSynonym)=="while"){ // Follows*(w1, synonym2)
				if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){ // Follows*(w1, s2) 
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="while"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (follows->isFollowsStarTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="assign"){ // Follows*(w1, a2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="while"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="assign"&&follows->isFollowsStarTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="while"){ // Follows*(w1, w2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="while"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="while"&&follows->isFollowsStarTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="if"){
					// not implemented
				} else {
					// throw exception
				}
			} else if (synonymTable->at(leftSynonym)=="if"){
				// not implemented
			} else {
				// throw exception
			}
		}
		if (isSuchThatTrue){
			// fill answer for different type of selectSynonym such as procedure, statement, while, assign, variable, prog_line
			if (synonymTable->at(selectSynonym)=="stmt"||
				synonymTable->at(selectSynonym)=="assign"||
				synonymTable->at(selectSynonym)=="while"||
				synonymTable->at(selectSynonym)=="if"||
				synonymTable->at(selectSynonym)=="prog_line"){
					if (synonymTable->at(selectSynonym)=="stmt"||synonymTable->at(selectSynonym)=="prog_line"){
						for (int i=1; i<=statementTable->getSize(); i++){
							answer.push_back(i);
						}
					} else {
						answer = statementTable->getStmtNumUsingNodeType(synonymTable->at(selectSynonym));
					}					
			} else if (synonymTable->at(selectSynonym)=="variable"){
				for (int i=1; i<=varTable->getSize(); i++){
					answer.push_back(i);
				}
			} else if (synonymTable->at(selectSynonym)=="procedure"){
				for (int i=1; i<=procTable->getSize(); i++){
					answer.push_back(i);
				}
			} else {
				// throw exception
			}
		} else {
			// nothing in answer
		}
	}
	return answer;
}

vector<int> solveForSuchThatParent(string selectSynonym, map<STRING, STRING>* synonymTable, QueryTree* suchThatTree, StatementTable* statementTable, Parent* parent, ProcTable* procTable, VarTable* varTable){
	vector<int> answer;
	if (synonymTable->find(selectSynonym)==synonymTable->end()){ // if selectSynonym is not defined
		return answer;
	}
	bool isSynonymInSuchThat = checkSynonymInSuchThat(selectSynonym, suchThatTree);
	if (isSynonymInSuchThat){ // Select synonym such that Parent(synonym, *) (Parent(*, synonym))
		if (selectSynonym == suchThatTree->getRootNode()->getChild(0)->getKey()){ // Select synonym such that Parent(synonym, *)
			string stmtSynonym = suchThatTree->getRootNode()->getChild(1)->getKey();
			if (stmtSynonym.empty()){ // Select synonym such that Parent(synonym, 1)
				int stmtIndex = suchThatTree->getRootNode()->getChild(1)->getValue();
				if(stmtIndex<=statementTable->getSize()){
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = parent->getParents(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = parent->getParents(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = parent->getParents(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = parent->getParents(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception or return empty
				}
			} else { // Select synonym such that Parent(synonym, another Synonym)
				if (synonymTable->find(stmtSynonym) != synonymTable->end()){
					vector<int> dataFromStmtSynonym;
					if(synonymTable->at(stmtSynonym)=="stmt"){
						dataFromStmtSynonym = parent->getAllParentStmt();
					} else if (synonymTable->at(stmtSynonym)=="prog_line"){
						dataFromStmtSynonym = parent->getAllParentStmt();
					} else if (synonymTable->at(stmtSynonym)=="assign"){
						vector<int> assignList = statementTable->getStmtNumUsingNodeType("assign");
						for (size_t i=0; i<assignList.size(); i++){
							vector<int> result = parent->getParents(assignList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="while"){
						vector<int> whileList = statementTable->getStmtNumUsingNodeType("while");
						for (size_t i=0; i<whileList.size(); i++){
							vector<int> result = parent->getParents(whileList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="if"){
						// not implemented	
					} else {
						// throw exception
					}
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception
				}
			}
		} else { // Select synonym such that Parent(*, synonym)
			string stmtSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
			if (stmtSynonym.empty()){ // Select synonym such that Parent(1, synonym)
				int stmtIndex = suchThatTree->getRootNode()->getChild(0)->getValue();
				if(stmtIndex<=statementTable->getSize()){
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = parent->getChildren(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = parent->getChildren(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = parent->getChildren(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = parent->getChildren(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception or return empty
				}
			} else { // Select synonym such that Parent(another Synonym, synonym)
				if (synonymTable->find(stmtSynonym) != synonymTable->end()){
					vector<int> dataFromStmtSynonym;
					if(synonymTable->at(stmtSynonym)=="stmt"){
						dataFromStmtSynonym = parent->getAllChildrenStmt();
					} else if (synonymTable->at(stmtSynonym)=="prog_line"){
						dataFromStmtSynonym = parent->getAllChildrenStmt();
					} else if (synonymTable->at(stmtSynonym)=="assign"){
						vector<int> assignList = statementTable->getStmtNumUsingNodeType("assign");
						for (size_t i=0; i<assignList.size(); i++){
							vector<int> result = parent->getChildren(assignList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="while"){
						vector<int> whileList = statementTable->getStmtNumUsingNodeType("while");
						for (size_t i=0; i<whileList.size(); i++){
							vector<int> result = parent->getChildren(whileList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="if"){
						// not implemented	
					} else {
						// throw exception
					}
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception
				}
			}
		}
	} else { // synonym is not in Parent(*, *)
		bool isSuchThatTrue = false;
		string leftSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
		string rightSynonym = suchThatTree->getRootNode()->getChild(1)->getKey();
		if (leftSynonym.empty()&&rightSynonym.empty()){ // Parent(1, 2);
			int leftStmtIndex = suchThatTree->getRootNode()->getChild(0)->getValue();
			int rightStmtIndex = suchThatTree->getRootNode()->getChild(1)->getValue();
			if (leftStmtIndex>statementTable->getSize()||rightStmtIndex>statementTable->getSize()){
				// throw exception
			} else {
				isSuchThatTrue = parent->isParentTrue(leftStmtIndex, rightStmtIndex);
			}
		} else if (leftSynonym.empty()){ // Parent(1, synonym)
			int leftStmtIndex = suchThatTree->getRootNode()->getChild(0)->getValue();
			if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (parent->isParentTrue(leftStmtIndex, i)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(rightSynonym)=="assign"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="assign"&&parent->isParentTrue(leftStmtIndex, i)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(rightSynonym)=="while"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="while"&&parent->isParentTrue(leftStmtIndex, i)){
						isSuchThatTrue = true;
						break;
					}
				}		
			} else if (synonymTable->at(rightSynonym)=="if"){
				// not implemented
			} else {
				// throw exception
			}
		} else if (rightSynonym.empty()){ // Parent(synonym, 2)
			int rightStmtIndex = suchThatTree->getRootNode()->getChild(1)->getValue();
			if (synonymTable->at(leftSynonym)=="stmt"||synonymTable->at(leftSynonym)=="prog_line"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (parent->isParentTrue(i, rightStmtIndex)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(leftSynonym)=="assign"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="assign"&&parent->isParentTrue(i, rightStmtIndex)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(leftSynonym)=="while"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="while"&&parent->isParentTrue(i, rightStmtIndex)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(leftSynonym)=="if"){
				// not implemented
			} else {
				// throw exception
			}
		} else { // Parent(synonym1, synonym2)
			if(synonymTable->at(leftSynonym).empty()||synonymTable->at(rightSynonym).empty()){
				// throw exception
			} else if (synonymTable->at(leftSynonym)=="stmt"||synonymTable->at(leftSynonym)=="prog_line"){ // Parent(s1, synonym2)
				if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){ // Parent(s1, s2) 
					for (int i=1; i<=statementTable->getSize(); i++){
						for (int j=1; j<=statementTable->getSize(); j++){
							if (parent->isParentTrue(i, j)){
								isSuchThatTrue = true;
								break;
							}		
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="assign"){ // Parent(s1, a2)
					for (int i=1; i<=statementTable->getSize(); i++){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="assign"&&parent->isParentTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="while"){ // Parent(s1, w2)
					for (int i=1; i<=statementTable->getSize(); i++){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="while"&&parent->isParentTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
					}			
				} else if (synonymTable->at(rightSynonym)=="if"){
					// not implemented
				} else {
					// throw exception
				}
			} else if (synonymTable->at(leftSynonym)=="assign"){ // Parent(a1, synonym2)
				if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){ // Parent(a1, s2) 
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="assign"){
							for (int j=1; j<statementTable->getSize(); j++){
								if (parent->isParentTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="assign"){ // Parent(a1, a2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="assign"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="assign"&&parent->isParentTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="while"){ // Parent(a1, w2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="assign"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="while"&&parent->isParentTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="if"){
					// not implemented
				} else {
					// throw exception
				}
			} else if (synonymTable->at(leftSynonym)=="while"){ // Parent(w1, synonym2)
				if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){ // Parent(w1, s2) 
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="while"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (parent->isParentTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="assign"){ // Parent(w1, a2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="while"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="assign"&&parent->isParentTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="while"){ // Parent(w1, w2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="while"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="while"&&parent->isParentTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="if"){
					// not implemented
				} else {
					// throw exception
				}
			} else if (synonymTable->at(leftSynonym)=="if"){
				// not implemented
			} else {
				// throw exception
			}
		}
		if (isSuchThatTrue){
			// fill answer for different type of selectSynonym such as procedure, statement, while, assign, variable, prog_line
			if (synonymTable->at(selectSynonym)=="stmt"||
				synonymTable->at(selectSynonym)=="assign"||
				synonymTable->at(selectSynonym)=="while"||
				synonymTable->at(selectSynonym)=="if"||
				synonymTable->at(selectSynonym)=="prog_line"){
					if (synonymTable->at(selectSynonym)=="stmt"||synonymTable->at(selectSynonym)=="prog_line"){
						for (int i=1; i<=statementTable->getSize(); i++){
							answer.push_back(i);
						}
					} else {
						answer = statementTable->getStmtNumUsingNodeType(synonymTable->at(selectSynonym));
					}					
			} else if (synonymTable->at(selectSynonym)=="variable"){
				for (int i=1; i<=varTable->getSize(); i++){
					answer.push_back(i);
				}
			} else if (synonymTable->at(selectSynonym)=="procedure"){
				for (int i=1; i<=procTable->getSize(); i++){
					answer.push_back(i);
				}
			} else {
				// throw exception
			}
		} else {
			// nothing in answer
		}
	}
	return answer;
}

vector<int> solveForSuchThatParentStar(string selectSynonym, map<STRING, STRING>* synonymTable, QueryTree* suchThatTree, StatementTable* statementTable, Parent* parent, ProcTable* procTable, VarTable* varTable){
	vector<int> answer;
	if (synonymTable->find(selectSynonym)==synonymTable->end()){ // if selectSynonym is not defined
		return answer;
	}
	bool isSynonymInSuchThat = checkSynonymInSuchThat(selectSynonym, suchThatTree);
	if (isSynonymInSuchThat){ // Select synonym such that Parent*(synonym, *) (Parent*(*, synonym))
		if (selectSynonym == suchThatTree->getRootNode()->getChild(0)->getKey()){ // Select synonym such that Parent*(synonym, *)
			string stmtSynonym = suchThatTree->getRootNode()->getChild(1)->getKey();
			if (stmtSynonym.empty()){ // Select synonym such that Parent*(synonym, 1)
				int stmtIndex = suchThatTree->getRootNode()->getChild(1)->getValue();
				if(stmtIndex<=statementTable->getSize()){
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = parent->getParentStar(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = parent->getParentStar(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = parent->getParentStar(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = parent->getParentStar(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception or return empty
				}
			} else { // Select synonym such that Parent*(synonym, another Synonym)
				if (synonymTable->find(stmtSynonym) != synonymTable->end()){
					vector<int> dataFromStmtSynonym;
					if(synonymTable->at(stmtSynonym)=="stmt"){
						dataFromStmtSynonym = parent->getAllParentStmt();
					} else if (synonymTable->at(stmtSynonym)=="prog_line"){
						dataFromStmtSynonym = parent->getAllParentStmt();
					} else if (synonymTable->at(stmtSynonym)=="assign"){
						vector<int> assignList = statementTable->getStmtNumUsingNodeType("assign");
						for (size_t i=0; i<assignList.size(); i++){
							vector<int> result = parent->getParentStar(assignList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="while"){
						vector<int> whileList = statementTable->getStmtNumUsingNodeType("while");
						for (size_t i=0; i<whileList.size(); i++){
							vector<int> result = parent->getParentStar(whileList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="if"){
						// not implemented	
					} else {
						// throw exception
					}
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception
				}
			}
		} else { // Select synonym such that Parent*(*, synonym)
			string stmtSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
			if (stmtSynonym.empty()){ // Select synonym such that Parent*(1, synonym)
				int stmtIndex = suchThatTree->getRootNode()->getChild(0)->getValue();
				if(stmtIndex<=statementTable->getSize()){
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = parent->getChildrenStar(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = parent->getChildrenStar(stmtIndex);
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = parent->getChildrenStar(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = parent->getChildrenStar(stmtIndex);
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception or return empty
				}
			} else { // Select synonym such that Parent*(another Synonym, synonym)
				if (synonymTable->find(stmtSynonym) != synonymTable->end()){
					vector<int> dataFromStmtSynonym;
					if(synonymTable->at(stmtSynonym)=="stmt"){
						dataFromStmtSynonym = parent->getAllChildrenStmt();
					} else if (synonymTable->at(stmtSynonym)=="prog_line"){
						dataFromStmtSynonym = parent->getAllChildrenStmt();
					} else if (synonymTable->at(stmtSynonym)=="assign"){
						vector<int> assignList = statementTable->getStmtNumUsingNodeType("assign");
						for (size_t i=0; i<assignList.size(); i++){
							vector<int> result = parent->getChildrenStar(assignList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="while"){
						vector<int> whileList = statementTable->getStmtNumUsingNodeType("while");
						for (size_t i=0; i<whileList.size(); i++){
							vector<int> result = parent->getChildrenStar(whileList.at(i));
							dataFromStmtSynonym.insert(dataFromStmtSynonym.end(), result.begin(), result.end());
						}
					} else if (synonymTable->at(stmtSynonym)=="if"){
						// not implemented	
					} else {
						// throw exception
					}
					if (synonymTable->at(selectSynonym)=="stmt"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="prog_line"){
						answer = dataFromStmtSynonym;
					} else if (synonymTable->at(selectSynonym)=="assign"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="assign"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="while"){
						vector<int> temp = dataFromStmtSynonym;
						for (size_t i=0; i<temp.size(); i++){
							if (statementTable->getTNode(temp.at(i))->getNodeType()=="while"){
								answer.push_back(temp.at(i));
							}
						}
					} else if (synonymTable->at(selectSynonym)=="if"){
						// not implemented
					} else {
						// throw exception or return empty
					}
				} else {
					// throw exception
				}
			}
		}
	} else { // synonym is not in Parent*(*, *)
		bool isSuchThatTrue = false;
		string leftSynonym = suchThatTree->getRootNode()->getChild(0)->getKey();
		string rightSynonym = suchThatTree->getRootNode()->getChild(1)->getKey();
		if (leftSynonym.empty()&&rightSynonym.empty()){ // Parent*(1, 2);
			int leftStmtIndex = suchThatTree->getRootNode()->getChild(0)->getValue();
			int rightStmtIndex = suchThatTree->getRootNode()->getChild(1)->getValue();
			if (leftStmtIndex>statementTable->getSize()||rightStmtIndex>statementTable->getSize()){
				// throw exception
			} else {
				isSuchThatTrue = parent->isParentStarTrue(leftStmtIndex, rightStmtIndex);
			}
		} else if (leftSynonym.empty()){ // Parent*(1, synonym)
			int leftStmtIndex = suchThatTree->getRootNode()->getChild(0)->getValue();
			if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (parent->isParentStarTrue(leftStmtIndex, i)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(rightSynonym)=="assign"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="assign"&&parent->isParentStarTrue(leftStmtIndex, i)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(rightSynonym)=="while"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="while"&&parent->isParentStarTrue(leftStmtIndex, i)){
						isSuchThatTrue = true;
						break;
					}
				}		
			} else if (synonymTable->at(rightSynonym)=="if"){
				// not implemented
			} else {
				// throw exception
			}
		} else if (rightSynonym.empty()){ // Parent*(synonym, 2)
			int rightStmtIndex = suchThatTree->getRootNode()->getChild(1)->getValue();
			if (synonymTable->at(leftSynonym)=="stmt"||synonymTable->at(leftSynonym)=="prog_line"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (parent->isParentStarTrue(i, rightStmtIndex)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(leftSynonym)=="assign"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="assign"&&parent->isParentStarTrue(i, rightStmtIndex)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(leftSynonym)=="while"){
				for (int i=1; i<=statementTable->getSize(); i++){
					if (statementTable->getTNode(i)->getNodeType()=="while"&&parent->isParentStarTrue(i, rightStmtIndex)){
						isSuchThatTrue = true;
						break;
					}
				}
			} else if (synonymTable->at(leftSynonym)=="if"){
				// not implemented
			} else {
				// throw exception
			}
		} else { // Parent*(synonym1, synonym2)
			if(synonymTable->at(leftSynonym).empty()||synonymTable->at(rightSynonym).empty()){
				// throw exception
			} else if (synonymTable->at(leftSynonym)=="stmt"||synonymTable->at(leftSynonym)=="prog_line"){ // Parent*(s1, synonym2)
				if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){ // Parent*(s1, s2) 
					for (int i=1; i<=statementTable->getSize(); i++){
						for (int j=1; j<=statementTable->getSize(); j++){
							if (parent->isParentStarTrue(i, j)){
								isSuchThatTrue = true;
								break;
							}		
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="assign"){ // Parent*(s1, a2)
					for (int i=1; i<=statementTable->getSize(); i++){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="assign"&&parent->isParentStarTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="while"){ // Parent*(s1, w2)
					for (int i=1; i<=statementTable->getSize(); i++){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="while"&&parent->isParentStarTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
					}			
				} else if (synonymTable->at(rightSynonym)=="if"){
					// not implemented
				} else {
					// throw exception
				}
			} else if (synonymTable->at(leftSynonym)=="assign"){ // Parent*(a1, synonym2)
				if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){ // Parent*(a1, s2) 
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="assign"){
							for (int j=1; j<statementTable->getSize(); j++){
								if (parent->isParentStarTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="assign"){ // Parent*(a1, a2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="assign"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="assign"&&parent->isParentStarTrue(i, j)){
									isSuchThatTrue = true;
									break;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="while"){ // Parent*(a1, w2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="assign"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="while"&&parent->isParentStarTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="if"){
					// not implemented
				} else {
					// throw exception
				}
			} else if (synonymTable->at(leftSynonym)=="while"){ // Parent*(w1, synonym2)
				if (synonymTable->at(rightSynonym)=="stmt"||synonymTable->at(rightSynonym)=="prog_line"){ // Parent*(w1, s2) 
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="while"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (parent->isParentStarTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="assign"){ // Parent*(w1, a2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="while"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="assign"&&parent->isParentStarTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="while"){ // Parent*(w1, w2)
					for (int i=1; i<=statementTable->getSize(); i++){
						if (statementTable->getTNode(i)->getNodeType()=="while"){
							for (int j=1; j<=statementTable->getSize(); j++){
								if (statementTable->getTNode(j)->getNodeType()=="while"&&parent->isParentStarTrue(i, j)){
									isSuchThatTrue = true;
								}			
							}
							if (isSuchThatTrue) break;
						}
						if (isSuchThatTrue) break;
					}
				} else if (synonymTable->at(rightSynonym)=="if"){
					// not implemented
				} else {
					// throw exception
				}
			} else if (synonymTable->at(leftSynonym)=="if"){
				// not implemented
			} else {
				// throw exception
			}
		}
		if (isSuchThatTrue){
			// fill answer for different type of selectSynonym such as procedure, statement, while, assign, variable, prog_line
			if (synonymTable->at(selectSynonym)=="stmt"||
				synonymTable->at(selectSynonym)=="assign"||
				synonymTable->at(selectSynonym)=="while"||
				synonymTable->at(selectSynonym)=="if"||
				synonymTable->at(selectSynonym)=="prog_line"){
					if (synonymTable->at(selectSynonym)=="stmt"||synonymTable->at(selectSynonym)=="prog_line"){
						for (int i=1; i<=statementTable->getSize(); i++){
							answer.push_back(i);
						}
					} else {
						answer = statementTable->getStmtNumUsingNodeType(synonymTable->at(selectSynonym));
					}					
			} else if (synonymTable->at(selectSynonym)=="variable"){
				for (int i=1; i<=varTable->getSize(); i++){
					answer.push_back(i);
				}
			} else if (synonymTable->at(selectSynonym)=="procedure"){
				for (int i=1; i<=procTable->getSize(); i++){
					answer.push_back(i);
				}
			} else {
				// throw exception
			}
		} else {
			// nothing in answer
		}
	}
	return answer;
}

/******* Helper Level 2 *******/

vector<int> solveForSuchThat(string selectSynonym, map<STRING, STRING>* synonymTable, QueryTree* tree, PKBController* pkb){
	vector<int> answer;
	// only one such that, can make into a loop later
	// for future development with many such that, need to check dependency and start with the lowest first
	// for example, "Select w such that Parent(w, a) and Follows(a, a1) and Modifies(a1, var1) and Uses(if, var1)",
	// we need to evaluate such that from right to left to find w
	ProcTable* procTable = &(pkb->procTable);
	VarTable* varTable = &(pkb->varTable);
	StatementTable* statementTable = &(pkb->statementTable);
	Modifies* modifies = &(pkb->modifiesTable);
	Uses* uses = &(pkb->usesTable);
	Follows* follows = &(pkb->followsTable);
	Parent* parent = &(pkb->parentTable);
	QueryTree* suchThatSubtree = tree->getSubtreeFromNode(tree->getRootNode()->getChild(1)->getChild(0));
	string suchThatType = suchThatSubtree->getRootNode()->getKey();
	if (suchThatType == "Modifies"){
		answer = solveForSuchThatModifies(selectSynonym, synonymTable, suchThatSubtree, statementTable, modifies, procTable, varTable);
	} else if (suchThatType == "Uses"){
		answer = solveForSuchThatUses(selectSynonym, synonymTable, suchThatSubtree, statementTable, uses, procTable, varTable);
	} else if (suchThatType == "Follows"){
		answer = solveForSuchThatFollows(selectSynonym, synonymTable, suchThatSubtree, statementTable, follows, procTable, varTable);
	} else if (suchThatType == "Follows*"){
		answer = solveForSuchThatFollowsStar(selectSynonym, synonymTable, suchThatSubtree, statementTable, follows, procTable, varTable);
	} else if (suchThatType == "Parent"){
		answer = solveForSuchThatParent(selectSynonym, synonymTable, suchThatSubtree, statementTable, parent, procTable, varTable);
	} else if (suchThatType == "Parent*"){
		answer = solveForSuchThatParentStar(selectSynonym, synonymTable, suchThatSubtree, statementTable, parent, procTable, varTable);
	}

	return answer;
}

vector<int> solveForPattern(string selectSynonym, map<STRING, STRING>* synonymTable, QueryTree* tree){
	vector<int> answer;
	return answer;
}

/******* Helper Level 1 *******/
// Method to collect solution from different conditional clauses (such that, with, pattern, etc.) and combine
vector<int> solve(string selectSynonym, map<STRING, STRING>* synonymTable, QueryTree* tree, PKBController* pkb){
	vector<int> resultFromSuchThat = solveForSuchThat(selectSynonym, synonymTable, tree, pkb);
	vector<int> resultFromPattern = solveForPattern(selectSynonym, synonymTable, tree);
	set<int> temp;
	temp.insert(resultFromSuchThat.begin(), resultFromSuchThat.end());
	temp.insert(resultFromPattern.begin(), resultFromPattern.end());
	vector<int> answer(temp.begin(), temp.end());
	return answer;
}

/******* Methods *******/
// Method to get and printout final result
list<string> QueryEvaluator::evaluate(map<STRING, STRING>* synonymTable, QueryTree* tree){

	string output = "Hello World!";
	list<string> outputlist;
	PKBController* pkb = this->pkb;
	string selectSynonym = tree->getRootNode()->getChild(0)->getChild(0)->getKey();
	// Need to check if selectSynonym is inside synonymTable
	// TODO: throws exception
	vector<int> answer = solve(selectSynonym, synonymTable, tree, pkb);
	string temp = "";
	if (synonymTable->at(selectSynonym)=="variable"||
		synonymTable->at(selectSynonym)=="procedure"){
		if (synonymTable->at(selectSynonym)=="variable"){
			for (size_t i=0; i<answer.size(); i++){
				temp = temp + ("" + pkb->varTable.getVarName(answer.at(i)));
				outputlist.push_back(pkb->varTable.getVarName(answer.at(i)));
				if (i!=answer.size()-1){
					temp = temp+",";
				}
			}
		} else {
			for (size_t i=0; i<answer.size(); i++){
				temp = temp + ("" + pkb->procTable.getProcName(answer.at(i)));
				if (i!=answer.size()-1){
					temp = temp+",";
				}
				outputlist.push_back(pkb->procTable.getProcName(answer.at(i)));
			}
		}
	} else {
		for (size_t i=0; i<answer.size(); i++){
			temp = temp + to_string((long long)answer[i]);
			if (i!=answer.size()-1){
				temp = temp+",";
			}
			outputlist.push_back(to_string((long long)answer[i]));
	}
	output = temp;
	return outputlist;
	}
};