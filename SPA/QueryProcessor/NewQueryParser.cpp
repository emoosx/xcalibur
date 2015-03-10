#pragma once

#include <stdio.h>
#include <algorithm>
#include <string>
#include <map>
#include <stack>
#include <iostream>
#include <sstream>

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
#include "ParentSubquery.cpp"
#include "WithSubquery.cpp"
#include "CallsSubquery.cpp"
#include "CallsStarSubquery.cpp"
#include "NextSubquery.cpp"


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
		t = lexical_cast<T>(s);
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
	parse();
	printMap();
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
		for(size_t i = 0; i < DESIGN_ENTITIES.size(); ++i) {
			if(nextToken.name == DESIGN_ENTITIES[i]) {
				match(DESIGN_ENTITIES[i]);
				matchDeclarationVariables(DESIGN_ENTITIES[i]); 
				i = 0;
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
	matchConditions();
	if(nextToken.name == "such" || nextToken.name == "pattern" || nextToken.name == "with") {
		matchConditions();
	}

	evaluator = new QE(selectVariables);
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
	selectVariables.push_back(elem);
	match(elem);

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
	result = "";
	match("(");
	string fst = matchVarRef();
	match(",");
	if(nextToken.name == "_") {
		match(UNDERSCORE);
	}

	if(nextToken.name == "\"") {
		match("\"");
		matchExpression();
		match("\"");
	}

	if(nextToken.name == "_") {
		match(UNDERSCORE);
	}
	match(")");
	
	cout << "POSTFIX : " << result << "\n";
	// result postfix is ready
}

QTNode* NewQueryParser::matchExpression() {
	stack<string> operatorStack;
	stack<QTNode*> operandStack;
	QTNode* left;
	QTNode* right;
	QTNode* current;

	//shunting yard algorithm
	next:
	while(nextToken.name.compare("\"") != 0) {
		string popped;
		if (nextToken.token == OPEN_PARENTHESES) {
			match(OPEN_PARENTHESES);
			operatorStack.push("(");
		} else if (nextToken.token == CLOSE_PARENTHESES) {
			match(CLOSE_PARENTHESES);
			while(!operatorStack.empty()) {
				popped = operatorStack.top();
				operatorStack.pop();
				if (popped.compare("(") == 0) {
					goto next;
				} else {
					//add node(operandStack, popped);
					right = operandStack.top();
					operandStack.pop();
					left = operandStack.top();
					operandStack.pop();
					current = new QTNode(popped);
					current->addChild(left);
					current->addChild(right);
					operandStack.push(current);
				}
			}
			//throw unbalance parentheses exception
		} else {
			if(nextToken.token == PLUS || nextToken.token == MINUS || nextToken.token == TIMES) {
				string op1 = nextToken.name;
				match(op1);
				string op2;
				while (!operatorStack.empty() && !(op2 = operatorStack.top()).empty()) {
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
		string op = operatorStack.top();
		operatorStack.pop();
		right = operandStack.top();
		operandStack.pop();
		left = operandStack.top();
		operandStack.pop();
		current = new QTNode(op);
		current->addChild(left);
		current->addChild(right);
		operandStack.push(current);
		result += " " + op;
	}

	//cout << "postfix : " << result << endl;

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
	match("(");
	string fst = matchVarRef();
	match(",");
	match(UNDERSCORE);
	match(",");
	match(UNDERSCORE);
	match(")");
}

void NewQueryParser::matchPatternWhile(string s) {
	// while : synonym "(" varRef "," "_" ")"
	match("(");
	string fst = matchVarRef();
	match(",");
	match(UNDERSCORE);
	match(")");
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
	string fst = matchRef();
	match("=");
	string snd = matchRef();
	cout << "With: fst -> " << fst << "\tsnd -> " << snd;
}

string NewQueryParser::matchRef() {
	// ref: attrRef | synonym | '"' IDENT '"' | INTEGER
	// in the above synonym must be a synonym of prog_line
	// attRef: synonym '.' attrname
	return "";

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
	} else {
		cout << "SYNTAX ERROR" << nextToken.name << endl;
		return;
	}
}

void NewQueryParser::matchModifies() {
	// ModifiesP: "Modifies" "(" entRef "," varRef ")"
	// ModifiesC: "Modifies" "(" stmtRef "," varRef ")"
	ModifiesSubquery modifiesSq = ModifiesSubquery(&synonyms, &controller);
	match("(");
	string fst = matchEntRef(true);
	match(",");
	string snd = matchVarRef();
	match(")");
	setSynonymsHelper(fst, snd, &modifiesSq);
	//cout << "fst : " << fst << endl;
	//cout << "snd : " << snd << endl;
}

string NewQueryParser::matchEntRef(bool excludeUnderScore) {
	// entRef: synonym | "_" | """ IDENT """ | INTEGER
	string fst = "";
	if(nextToken.name.compare("_") == 0) {
		if(!excludeUnderScore) {
			fst = "_";
			match(UNDERSCORE);
		} else {
			throw(QueryException("VAGUE QUERY ERROR: The first argument of Uses and Modifies cannot be a \"_\""));
		}
	} else if(nextToken.token == INT_LIT) {
		fst = nextToken.name;
		match(nextToken.name);
	} else if(nextToken.token == IDENT || nextToken.token == SIMPLE_IDENT) {
		fst = nextToken.name;
		match(nextToken.name);
	} else if(nextToken.name.compare("\"") == 0) {
		match("\"");
		fst = nextToken.name;
		match(nextToken.name);
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
		match("\"");
	}
	return snd;
}

void NewQueryParser::matchUses() {
	// UsesP: "Uses" "(" entRef "," varRef ")"
	// UsesC: "Uses" "(" entRef "," varRef ")"
	match("(");
	string fst = matchEntRef(true);
	match(",");
	string snd = matchVarRef();
	match(")");
	cout << "Uses fst : " << fst << "\tUses snd : " << snd;
}

void NewQueryParser::matchCalls() {
	// Calls : "Calls" "(" entRef "," varRef ")"
	CallsSubquery callsSq = CallsSubquery(&synonyms, controller);
	match("(");
	string fst = matchEntRef(false);
	match(",");
	string snd = matchVarRef();
	match(")");
	setSynonymsHelper(fst, snd, &callsSq);
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
	evaluator->addQuery(&query);

}

void NewQueryParser::matchCallsStar() {
	// CallsT : "Calls*" "(" entRef "," varRef ")"
	CallsStarSubquery callsStarSq = CallsStarSubquery(&synonyms, controller);
	match("(");
	string fst = matchEntRef(false);
	match(",");
	string snd = matchVarRef();
	match(")");
	setSynonymsHelper(fst, snd, &callsStarSq);
	//cout << "Calls*: fst -> " << fst << "\tsnd -> " << snd;
}

void NewQueryParser::matchParent() {
	ParentSubquery parentSq = ParentSubquery(&synonyms, controller);
	match("(");
	string fst = matchStmtRef();
	match(",");
	string snd = matchStmtRef();
	match(")");
	setSynonymsHelper(fst, snd, &parentSq);
	//cout << "Parent: fst -> " << fst << "\tsnd -> " << snd;
}

void NewQueryParser::matchParentStar() {
	ParentStarSubquery parentStarSq = ParentStarSubquery(&synonyms, controller);
	match("(");
	string fst = matchStmtRef();
	match(",");
	string snd = matchStmtRef();
	match(")");
	setSynonymsHelper(fst, snd, &parentStarSq);
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
	FollowsSubquery followsSq = FollowsSubquery(&synonyms, controller);
	match("(");
	string fst = matchStmtRef();
	match(",");
	string snd = matchStmtRef();
	match(")");
	setSynonymsHelper(fst, snd, &followsSq);
	//cout << "Follows: fst -> " << fst << "\tsnd -> " << snd;
}

void NewQueryParser::matchFollowsStar() {
	FollowsStarSubquery followsStarSq = FollowsStarSubquery(&synonyms, controller);
	match("(");
	string fst = matchStmtRef();
	match(",");
	string snd = matchStmtRef();
	match(")");
	setSynonymsHelper(fst, snd, &followsStarSq);
	//cout << "Follows*: fst -> " << fst << "\tsnd -> " << snd;
}

void NewQueryParser::matchNext() {
	NextSubquery nextSq = NextSubquery(&synonyms, controller);
    match("(");
    string fst = matchLineRef();
    match(",");
    string snd = matchLineRef();
    match(")");
	setSynonymsHelper(fst, snd &nextSq);
	//cout << "Next: fst -> " << fst << "\tsnd -> " << snd;
}

void NewQueryParser::matchNextStar() {
    match("(");
    string fst = matchLineRef();
    match(",");
    string snd = matchLineRef();
    match(")");

	cout << "Next*: fst -> " << fst << "\tsnd -> " << snd;
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
    match("(");
    string fst = matchStmtRef();
    match(",");
    string snd = matchStmtRef();
    match(")");
	cout << "Affects: fst -> " << fst << "\tsnd -> " << snd;
}

void NewQueryParser::matchAffectsStar() {
    match("(");
    string fst = matchStmtRef();
    match(",");
    string snd = matchStmtRef();
    match(")");
	cout << "Affects: fst -> " << fst << "\tsnd -> " << snd;
}