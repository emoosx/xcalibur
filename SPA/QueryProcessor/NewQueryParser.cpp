#pragma once

#include <stdio.h>
#include <algorithm>
#include <string>
#include <map>
#include <stack>
#include <iostream>
#include <sstream>
#include <string>

#include "NewQueryParser.h"
#include "QueryLexer.h"
#include "QTNode.h"
#include "QueryException.h"
#include "Subquery.h"
#include "ModifiesProcSubquery.cpp"
#include "ModifiesSubquery.cpp"
#include "FollowsSubquery.cpp"
#include "FollowsStarSubquery.cpp"
#include "UsesSubquery.cpp"
#include "UsesProcSubquery.cpp"
#include "ParentSubquery.cpp"
#include "ParentStarSubquery.cpp"
#include "WithSubquery.cpp"
#include "CallsSubquery.cpp"
#include "CallsStarSubquery.cpp"
#include "NextSubquery.cpp"
#include "NextStarSubquery.cpp"
#include "AffectsSubquery.cpp"
#include "AffectsStarSubquery.cpp"
#include "PatternSubquery.cpp"
#include "ContainsSubquery.cpp"
#include "ContainsStarSubquery.cpp"
#include "SiblingSubquery.cpp"


using namespace std;

template <typename T>
T lexical_cast(const std::string& s) {
	std::stringstream ss(s);
	T result;
	if((ss >> result).fail() || !(ss >> std::ws).eof()) {
		throw std::bad_cast();
	}
	return result;
}

template <typename T>
bool lexical_cast(const std::string& s, T& t) {
	try {
		if (s == "0") t = 0;
		else t = lexical_cast<T>(s);
	} catch(const std::bad_cast&) {
		return false;
	}
}

NewQueryParser::NewQueryParser() {
}

NewQueryParser::NewQueryParser(string s, PKBController* controller) {
	this->nextToken = Lexeme();
	this->lexer = QueryLexer(s);
	this->selectVariables = vector<string>(); 
	this->synonyms = map<string, string>();
	this->controller = controller;

	this->withVar = false;
	this->withProc = false;
	this->withVal = false;
	this->withStmtNum = false;
	this->withFirst = "";
	this->withSecond = "";

	parse();
	this->evaluator->setSynonymTable(synonyms);
	// printMap();
}

void NewQueryParser::printMap() {
	cout << "Synonyms: "; 
	for(map<string, string>::iterator it = synonyms.begin(); it != synonyms.end(); ++it) {
		cout << "(" << it->first << "," << it->second << ") ";
	}
	cout << "\nSelect string: ";
	for(vector<string>::iterator it = selectVariables.begin(); it != selectVariables.end(); ++it) {
		cout << *it << "\n";
	}

	cout << "Queries : " << this->evaluator->queries.size() << endl;
}

NewQueryParser::~NewQueryParser() {
}

Lexeme NewQueryParser::getToken() {
	return lexer.lex();
}

void NewQueryParser::parse() {
	try {
		matchDeclaration();
		matchSelect();
	} catch (QueryException& e) {
		cout << e.what() << endl;
	}
}

void NewQueryParser::matchDeclaration() {
	nextToken = getToken();
	if(nextToken.name.compare("Select") != 0) { // start of synonyms
		for(auto i = DESIGN_ENTITIES.begin(); i != DESIGN_ENTITIES.end();) {
			if(nextToken.name == *i) {
				match(*i);
				matchDeclarationVariables(*i); 
				i = DESIGN_ENTITIES.begin();
			} else {
				i++;
			}
		}
	}
}

void NewQueryParser::match(string s) {
	if(nextToken.name == s) {
		nextToken = getToken();
	} else {
		throw(QueryException("QUERY SYNTAX ERROR: Unexpected token " + nextToken.name + ", Expected token: " + s));
	}
}

void NewQueryParser::match(int type) {
	if(nextToken.token = type) {
		nextToken = getToken();
	} else if(type == IDENT && nextToken.token == SIMPLE_IDENT) {
		nextToken = getToken();
	} else {
		throw(QueryException("QUERY GRAMMAR ERROR:[" + nextToken.name + "] Unexpected " + TOKEN_TYPE_STRINGS[type]));
	}
}

void NewQueryParser::matchDeclarationVariables(string entity) {
	string var = nextToken.name;
	// cout << var << "\t" << entity << endl;
	synonyms[var] = entity;
	match(var);
	if(nextToken.name.compare(";") != 0) {
		match(",");
		matchDeclarationVariables(entity);
	} else {
		match(";");
	}
}

void NewQueryParser::matchSelect() {
	match("Select");
	matchResultCL();
	this->evaluator = new QE(selectVariables, controller);
	while(nextToken.name == "such" || nextToken.name == "pattern" || nextToken.name == "with") {
		matchConditions();
	}

}

void NewQueryParser::matchResultCL() {
	if(nextToken.name == "BOOLEAN") {
		selectVariables.push_back("BOOLEAN");
		match("BOOLEAN");
	} else {
		matchTuple();
	}
}

void NewQueryParser::matchTuple() {
	if(nextToken.name == "<") {
		match("<");
		matchTupleElements(true);
		match(">");
	} else {
		matchTupleElements(false);
	}
}

void NewQueryParser::matchTupleElements(bool recursive) {
	string elem = nextToken.name;
	match(elem);
	if(nextToken.name == ".") {
		elem += ".";
		match(".");
		string attribute = nextToken.name;
		elem += attribute;
		match(nextToken.name);
	}
	selectVariables.push_back(elem);

	if(recursive && nextToken.name == ",") {
		match(","); 
		matchTupleElements(true);
	}
}

void NewQueryParser::matchConditions() {
	if(nextToken.name == "such") {
		matchSuchThat();
	} else if(nextToken.name == "pattern") {
		matchPatternCl();
	} else if(nextToken.name == "with") {
		matchWith();
	} else {
		cout << "SYNTAX ERROR -> " << nextToken.name << "\t" << nextToken.token << endl;
		return;
	}
}

void NewQueryParser::matchSuchThat() {
	match("such");
	match("that");
	matchRelCond();
	while(nextToken.name == "and") {
		match("and");
		matchRelCond();
	}
}

void NewQueryParser::matchPatternCl() {
    // pattern-cl: "pattern" patternCond
	match("pattern");
	matchPatternCond();
	while(nextToken.name == "and") {
		match("and");
		matchPatternCond();
	}
}

void NewQueryParser::matchPatternCond() {
    // patternCond: pattern ("and" pattern)*
    matchPattern();
}

void NewQueryParser::matchPattern() {
    // pattern: assign | while | if
	string syn = nextToken.name;
	match(nextToken.name);	// synonym from assign rule
	map<string, string>::iterator it = synonyms.find(syn);
	string synType = (it != synonyms.end()) ? it->second : "";

	if(synType == "assign") {
		matchPatternAssign(syn);
	} else if(synType == "while") {
		matchPatternWhile(syn);
	} else if(synType == "if") {
		matchPatternIf(syn);
	} else {
		throw(QueryException("INVALID SYNONYM: " + syn + " is not a synonym of the type \"if\", \"while\" or \"assign\"")); 
	}
}

void NewQueryParser::matchPatternAssign(string s) {
	// assign : synonym "(" varRef "," expression-spec | "_" ")"
	// expression-spec : """ expr | "_" """ expr """ "_"
	Subquery* patternAssignSq = new PatternSubquery(&synonyms, controller);
	patternAssignSq->isUnderscore = false;
	result = "";
	string value = result;
	match("(");
	string fst = s;
	string snd = matchVarRef();
	match(",");
	if(nextToken.name == "_") {
		match(UNDERSCORE);
		//value += "_";
	}

	if(nextToken.name == "\"") {
		match("\"");
		matchExpression();
		value += result;
		match("\"");
	}
	patternAssignSq->setValue(value, false);
	if(nextToken.name == "_") {
		match(UNDERSCORE);
		if(nextToken.name != ")") {
			cout << "Crash!" << endl;
		}
		patternAssignSq->isUnderscore = true;
		//value += "_";
	}
	match(")");

	
	// cout << "fst: " << fst << "\tsnd: "
	// cout << "POSTFIX : " << result << "\n";
	// result postfix is ready
	setSynonymsHelper(fst, snd, patternAssignSq);
}

QTNode* NewQueryParser::matchExpression() {
	stack<string> operatorStack;
	stack<QTNode*> operandStack;
	QTNode* left;
	QTNode* right;
	QTNode* current;

	//shunting yard algorithm
	while(nextToken.name.compare("\"") != 0) {
		string popped;
		if (nextToken.token == OPEN_PARENTHESES) {
			match(OPEN_PARENTHESES);
			operatorStack.push("(");
		} else if (nextToken.token == CLOSE_PARENTHESES) {
			match(CLOSE_PARENTHESES);
			while(!operatorStack.empty() && operatorStack.top() != "(") {
				popped = operatorStack.top(); operatorStack.pop();
				if (popped.compare("(") == 0) {
					
				} else {
					//add node(operandStack, popped);
					right = operandStack.top(); operandStack.pop();
					left = operandStack.top(); operandStack.pop();
					current = new QTNode(popped);
					current->addChild(left);
					current->addChild(right);
					operandStack.push(current);
					result += " " + popped;
				}
			}
			if(operatorStack.top() == "(") operatorStack.pop();
			//throw unbalance parentheses exception
		} else {
			if(nextToken.token == PLUS || nextToken.token == MINUS || nextToken.token == TIMES) {
				string op1 = nextToken.name;
				match(op1);
				string op2;
				while (!operatorStack.empty() && (operatorStack.top() != "(")) {
					op2 = operatorStack.top();
					if(comparePrecedence(op1, op2) <= 0) {
						operatorStack.pop();
						//addNode(operandStack, o2)
						right = operandStack.top();
						operandStack.pop();
						left = operandStack.top();
						operandStack.pop();
						current = new QTNode(op2);
						current->addChild(left);
						current->addChild(right);
						operandStack.push(current);
						if(result.length() > 0) result += " ";
						result += op2;
					} else {
						break;
					}
				}
				operatorStack.push(op1);
			} else {
				string factor = nextToken.name;
				if(nextToken.token == INT_LIT) {
					match(INT_LIT);
				} else {
					match(SIMPLE_IDENT);
				}
				if(result.length() > 0) result += " ";
				result += factor;
				operandStack.push(new QTNode(factor));
			}
		}	
	}
	while(!operatorStack.empty()) {
		string op = operatorStack.top(); operatorStack.pop();
		right = operandStack.top(); operandStack.pop();
		left = operandStack.top(); operandStack.pop();
		current = new QTNode(op);
		current->addChild(left);
		current->addChild(right);
		operandStack.push(current);
		result += " " + op;
	}

	//cout << "postfix : " << result << endl;

	if(result.length() > 0) {
		result = " " + result + " ";
	}

	return operandStack.top();
}

int NewQueryParser::comparePrecedence(string a, string b) {
	if (a.compare(b) == 0) {
		return 0;
	}
	if (a.compare("-") == 0 && b.compare("+") == 0) {
		return 0;
	}
	if (a.compare("+") == 0 && b.compare("-") == 0) {
		return 0;
	}
	if (a.compare("*") == 0) {
		return 1;
	}
	return -1;
}

void NewQueryParser::matchPatternIf(string s) {
	// if : synonym "(" varRef "," "_" "," "_" ")"
	Subquery* patternIfSq = new PatternSubquery(&synonyms, controller);
	match("(");
	string fst = s;
	string snd = matchVarRef();
	match(",");
	match(UNDERSCORE);
	match(",");
	match(UNDERSCORE);
	match(")");
	setSynonymsHelper(fst, snd, patternIfSq);
}

void NewQueryParser::matchPatternWhile(string s) {
	// while : synonym "(" varRef "," "_" ")"
	Subquery* patternWhileSq = new PatternSubquery(&synonyms, controller);
	match("(");
	string fst = s;
	string snd = matchVarRef();
	match(",");
	match(UNDERSCORE);
	match(")");
	setSynonymsHelper(fst, snd, patternWhileSq);
	
}

void NewQueryParser::matchWith() {
	match("with");
	matchAttrCond();
	while(nextToken.name == "and") {
		match("and");
		matchAttrCond();
	}
}

void NewQueryParser::matchAttrCond() {
	//attrCond:  attrCompare ("and" attCompare )*
	matchAttrCompare();
}

void NewQueryParser::matchAttrCompare() {
	// attrCompare : ref '=' ref
	// lhs and rhs 'ref' msut be of teh same type (INTEGER or character string)	

	WithSubquery* withSq = new WithSubquery(&synonyms, controller);
	string fst = matchRef(true, withSq);
	match("=");
	string snd = matchRef(false, withSq);

	if(withSecond == "procName" && withFirst == "") {
		fst = to_string((long long)controller->procTable->getProcIndex(fst));
	}
	if(withSecond == "varName" && withFirst == "") {
		fst = to_string((long long)controller->varTable->getVarIndex(fst));
	}

	// cout << "With: fst -> " << fst << "\tsnd -> " << snd << endl;
	setSynonymsHelper(fst, snd, withSq);
}

string NewQueryParser::matchRef(bool lhs, WithSubquery* withsq) {
	// ref: attrRef | synonym | '"' IDENT '"' | INTEGER
	// in the above synonym must be a synonym of prog_line
	// attRef: synonym '.' attrname

	string result = "";
	if(nextToken.name.compare("\"") == 0) {
		match("\"");
		if(!lhs) {
			if(withVar) result = to_string((long long)controller->varTable->getVarIndex(nextToken.name));
			if(withProc) result = to_string((long long)controller->procTable->getProcIndex(nextToken.name));
		}

		match(nextToken.name);
		match("\"");
		return result;
	}

	if(nextToken.token == IDENT || nextToken.token == SIMPLE_IDENT) { // synonyms 
		result = nextToken.name; // synonym
		match(nextToken.name);
		if(nextToken.name.compare(".") == 0) {
			match(".");
			string attrname = nextToken.name;
			if(attrname == "procName")  {withProc = true; withsq->setAttr("procName", lhs);}
			if(attrname == "varName") {withVar = true; withsq->setAttr("varName", lhs);}
			if(attrname == "value") {withVal = true; withsq->setAttr("value", lhs);}
			if(attrname == "stmt#") {withStmtNum = true; withsq->setAttr("stmt#", lhs);}
			if(!lhs) {
				withSecond = attrname;
			} else {
				withFirst = attrname;
			}
			match(nextToken.name);
			return result;
		} 
	} else if(nextToken.token == INT_LIT) {
			result = nextToken.name;
			match(nextToken.name);
	}
	return result;
}

void NewQueryParser::matchRelCond() {
	matchRelRef();
}

void NewQueryParser::matchRelRef() {
	// relRef: ModifiesP | ModifiesS | UsesP | UsesS | Calls | CallsT |
	//         Parent | ParentT | Follows | FollowsT | Next | NextT | 
	//         Affects | AffectsT
	string relation = nextToken.name;
	nextToken = getToken();
	if(nextToken.token == TIMES) {
		relation += "*";
		nextToken = getToken();
	}

	if(relation == "Modifies") {
		matchModifies();
	} else if(relation == "Uses") {
		matchUses();
	} else if(relation == "Calls") {
		matchCalls();
	} else if(relation == "Calls*") {
		matchCallsStar();
	} else if(relation == "Parent") {
		matchParent();
	} else if(relation == "Parent*") {
		matchParentStar();
	} else if(relation == "Follows") {
		matchFollows();
	} else if(relation == "Follows*") {
		matchFollowsStar();
	} else if(relation == "Next") {
		matchNext();
	} else if(relation == "Next*") {
		matchNextStar();
	} else if(relation == "Affects") {
		matchAffects();
	} else if(relation == "Affects*") {
		matchAffectsStar();
	} else if(relation == "Contains") {
		matchContains();
	} else if(relation == "Contains*") {
		matchContainsStar();
	} else if(relation == "Sibling") {
		matchSiblings();
	} else {
		cout << "SYNTAX ERROR" << nextToken.name << endl;
		return;
	}
}

void NewQueryParser::matchModifies() {
	// ModifiesP: "Modifies" "(" entRef "," varRef ")"
	// ModifiesC: "Modifies" "(" stmtRef "," varRef ")"
	Subquery* subquery = new Subquery(&synonyms, controller);
	//ModifiesSubquery* modifiesSq = new ModifiesSubquery(&synonyms, controller);
	match("(");
	string fst = matchEntRef(true, *&subquery, true);
	match(",");
	string snd = matchVarRef();
	match(")");
	setSynonymsHelper(fst, snd, subquery);
	//cout << "Modifies fst : " << fst << "\tModifies snd : " << snd << "\t Queries:" << this->evaluator->queries.size() << endl;
}

string NewQueryParser::matchEntRef(bool excludeUnderScore, Subquery*& subquery, bool modifies, bool uses) {
	// entRef: synonym | "_" | """ IDENT """ | INTEGER
	string fst = "";
	if(nextToken.name.compare("_") == 0) {
		fst = "_";
		match(UNDERSCORE);
	} else if(nextToken.token == INT_LIT) {
		fst = nextToken.name;
		match(nextToken.name);
		if(modifies || uses) {
			if(modifies) subquery = new ModifiesSubquery(&synonyms, controller);
			if(uses) subquery = new UsesSubquery(&synonyms, controller);
		}
	} else if(nextToken.token == IDENT || nextToken.token == SIMPLE_IDENT) { // synonym
		fst = nextToken.name;
		match(nextToken.name);
		if(modifies || uses) {
			if(synonyms.at(fst) == "procedure") {
				if(modifies) subquery = new ModifiesProcSubquery(&synonyms, controller);
				if(uses) subquery = new UsesProcSubquery(&synonyms, controller);
			} else {
				if(modifies) subquery = new ModifiesSubquery(&synonyms, controller);
				if(uses) subquery = new UsesSubquery(&synonyms, controller);
			}
		}
	} else if(nextToken.name.compare("\"") == 0) {
		match("\"");
		fst = nextToken.name;
		match(nextToken.name);
		if(controller->procTable->getProcIndex(fst) != -1) {
			fst = to_string((long long)controller->procTable->getProcIndex(fst));
			if(modifies || uses) {
				if(modifies) {
					fst = "_";
					subquery = new ModifiesProcSubquery(&synonyms, controller);
				}
				if(uses) { 
					fst = "_";
					subquery = new UsesProcSubquery(&synonyms, controller);
				}
			}
		}
		if(controller->varTable->getVarIndex(fst) != -1) {
			fst = to_string((long long)controller->varTable->getVarIndex(fst));
			if(modifies || uses) {
				if(modifies)  {
					fst = "_";
					subquery = new ModifiesSubquery(&synonyms, controller);
				}
				if(uses) {
					fst = "_";
					subquery = new UsesSubquery(&synonyms, controller);
				}
			}
		}
		// check with synoyntable
		// try to convert into integer first -> if yes, 
		match("\"");
	}
	return fst;
}

string NewQueryParser::matchVarRef() {
	// varRef: synonym | "_" | """ IDENT """
	string snd = "";
	if(nextToken.name == "_") {
		match(UNDERSCORE);
		snd = "_";
	} else if(nextToken.token == IDENT || nextToken.token == SIMPLE_IDENT) {
		snd = nextToken.name;
		match(nextToken.name); // synonym
	} else if(nextToken.name == "\"") {
		match("\"");
		snd = nextToken.name;
		match(nextToken.name);
		snd = to_string((long long)controller->varTable->getVarIndex(snd));
		match("\"");
	}
	return snd;
}

void NewQueryParser::matchUses() {
	// UsesP: "Uses" "(" entRef "," varRef ")"
	// UsesC: "Uses" "(" entRef "," varRef ")"
	Subquery* subquery = new Subquery(&synonyms, controller);
	match("(");
	string fst = matchEntRef(true, *&subquery, false, true);
	match(",");
	string snd = matchVarRef();
	match(")");
	setSynonymsHelper(fst, snd, subquery);
	//cout << "Uses fst : " << fst << "\tUses snd : " << snd << endl;
}

void NewQueryParser::matchCalls() {
	// Calls : "Calls" "(" entRef "," entRef ")"
	Subquery* callsSq = new CallsSubquery(&synonyms, controller);
	match("(");
	string fst = matchEntRef(false, *&callsSq);
	match(",");
	string snd = matchEntRef(false, *&callsSq);
	match(")");
	setSynonymsHelper(fst, snd, callsSq);
	//cout << "Calls: fst -> " << fst << "\tsnd -> " << snd;
}

void NewQueryParser::setSynonymsHelper(string fst, string snd, Subquery* query) {
	int i, j;
	if(lexical_cast(fst, i) && lexical_cast(snd, j)) {
		query->setSynonyms(i, j);
	} else if(lexical_cast(fst, i) && !(lexical_cast(snd, j))) { 
		query->setSynonyms(i, snd);
	} else if(!(lexical_cast(fst, i)) && lexical_cast(snd, j)) {
		query->setSynonyms(fst, j);
	} else {
		query->setSynonyms(fst, snd);
	}
	evaluator->addQuery(query);
}

void NewQueryParser::matchCallsStar() {
	// CallsT : "Calls*" "(" entRef "," entRef ")"
	Subquery* callsStarSq = new CallsStarSubquery(&synonyms, controller);
	match("(");
	string fst = matchEntRef(false, *&callsStarSq);
	match(",");
	string snd = matchEntRef(false, *&callsStarSq);
	match(")");
	setSynonymsHelper(fst, snd, callsStarSq);
	//cout << "Calls*: fst -> " << fst << "\tsnd -> " << snd;
}

void NewQueryParser::matchParent() {
	ParentSubquery* parentSq = new ParentSubquery(&synonyms, controller);
	match("(");
	string fst = matchStmtRef();
	match(",");
	string snd = matchStmtRef();
	match(")");
	setSynonymsHelper(fst, snd, parentSq);
	//cout << "Parent: fst -> " << fst << "\tsnd -> " << snd;
}

void NewQueryParser::matchParentStar() {
	ParentStarSubquery* parentStarSq = new ParentStarSubquery(&synonyms, controller);
	match("(");
	string fst = matchStmtRef();
	match(",");
	string snd = matchStmtRef();
	match(")");
	setSynonymsHelper(fst, snd, parentStarSq);
	//cout << "Parent*: fst -> " << fst << "\tsnd -> " << snd;
}

string NewQueryParser::matchStmtRef() {
	// stmtRef: synonym | "_" | INTEGER
	string param;
	if(nextToken.name == "_") {
		param = "_";
		match(UNDERSCORE);
	} else if(nextToken.token == INT_LIT) {
		param = nextToken.name;
		match(nextToken.name);
	} else if(nextToken.token == IDENT || nextToken.token == SIMPLE_IDENT) {
		param = nextToken.name;
		match(nextToken.name);
	} else {
		cout << "SYNTAX ERROR -> " << nextToken.name << "\t" << nextToken.token << endl;
	}
	return param;
}

void NewQueryParser::matchFollows() {
	// Follows: "Follows" "(" stmtRef "," stmtRef ")";
	FollowsSubquery* followsSq = new FollowsSubquery(&synonyms, controller);
	match("(");
	string fst = matchStmtRef();
	match(",");
	string snd = matchStmtRef();
	match(")");
	setSynonymsHelper(fst, snd, followsSq);
	//cout << "Follows: fst -> " << fst << "\tsnd -> " << snd << endl;
}

void NewQueryParser::matchFollowsStar() {
	FollowsStarSubquery* followsStarSq = new FollowsStarSubquery(&synonyms, controller);
	match("(");
	string fst = matchStmtRef();
	match(",");
	string snd = matchStmtRef();
	match(")");
	setSynonymsHelper(fst, snd, followsStarSq);
	//cout << "Follows*: fst -> " << fst << "\tsnd -> " << snd << endl;
}

void NewQueryParser::matchNext() {
	NextSubquery* nextSq = new NextSubquery(&synonyms, controller);
    match("(");
    string fst = matchLineRef();
    match(",");
    string snd = matchLineRef();
    match(")");
	setSynonymsHelper(fst, snd, nextSq);
	//cout << "Next: fst -> " << fst << "\tsnd -> " << snd << endl;
}

void NewQueryParser::matchNextStar() {
	NextStarSubquery* nextStarSq = new NextStarSubquery(&synonyms, controller);
    match("(");
    string fst = matchLineRef();
    match(",");
    string snd = matchLineRef();
    match(")");
	setSynonymsHelper(fst, snd, nextStarSq);
	//cout << "Next*: fst -> " << fst << "\tsnd -> " << snd << endl;
}

string NewQueryParser::matchLineRef() {
    // lineRef:  synonym | "_" | INTEGER
	string param;
	if(nextToken.name == "_") {
		param = "_";
		match(UNDERSCORE);
	} else if(nextToken.token == INT_LIT) {
		param = nextToken.name;
		match(INT_LIT);
	} else if(nextToken.token == IDENT || nextToken.token == SIMPLE_IDENT) {
		param = nextToken.name;
		match(nextToken.name);
	} else {
		cout << "SYNTAX ERROR -> " << nextToken.name << "\t" << nextToken.token << endl;
	}
	return param;
}

void NewQueryParser::matchAffects() {
	AffectsSubquery* affectsSq = new AffectsSubquery(&synonyms, controller);
    match("(");
    string fst = matchStmtRef();
    match(",");
    string snd = matchStmtRef();
    match(")");
	setSynonymsHelper(fst, snd, affectsSq);

	// cout << "Affects: fst -> " << fst << "\tsnd -> " << snd;
}

void NewQueryParser::matchAffectsStar() {
	AffectsStarSubquery* affectsStarSq = new AffectsStarSubquery(&synonyms, controller);
    match("(");
    string fst = matchStmtRef();
    match(",");
    string snd = matchStmtRef();
    match(")");
	setSynonymsHelper(fst, snd, affectsStarSq);
	// cout << "Affects: fst -> " << fst << "\tsnd -> " << snd;
}

void NewQueryParser::matchContains() {
	ContainsSubquery* containsSq = new ContainsSubquery(&synonyms, controller);
	match("(");
	string fst = matchNodeRef();
	match(",");
	string snd = matchNodeRef();
	match(")");
	setSynonymsHelper(fst, snd, containsSq);
}

void NewQueryParser::matchContainsStar() {
	ContainsStarSubquery* containsStarSq = new ContainsStarSubquery(&synonyms, controller);
	match("(");
	string fst = matchNodeRef();
	match(",");
	string snd = matchNodeRef();
	match(")");
	setSynonymsHelper(fst, snd, containsStarSq);
}

void NewQueryParser::matchSiblings() {
	SiblingSubquery* siblingSq = new SiblingSubquery(&synonyms, controller);
	match("(");
	string fst = matchNodeRef();
	match(",");
	string snd = matchNodeRef();
	match(")");
	setSynonymsHelper(fst, snd, siblingSq);
}


string NewQueryParser::matchNodeRef() {
	string param;
	if(nextToken.token == INT_LIT) {
		param = nextToken.name;
		match(INT_LIT);
	} else if(nextToken.token == IDENT || nextToken.token == SIMPLE_IDENT) {
		param = nextToken.name;
		match(nextToken.name);
	} else {
		cout << "SYNTAX ERROR -> " << nextToken.name << "\t" << nextToken.token << endl;
	}
	return param;
}
