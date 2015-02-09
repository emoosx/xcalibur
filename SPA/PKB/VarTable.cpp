#pragma once
#include <string>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include "VarTable.h"


using namespace std;

/************** Constructors **********************/
// constructor 
VarTable::VarTable(){
}

// insert variable into VarTable
int VarTable::insertVar(string s){
 if (std::find(varVec.begin(), varVec.end(), s) != varVec.end()){
	 return VarTable::getVarIndex(s);
}else{
	varVec.push_back(s);
	return (varVec.size() -1);
 }
}

// get variable name using its index
string VarTable::getVarName(int n){
	if(n>this->getSize())
		throw out_of_range("Index not in varTable");
	return varVec.at(n);
}

vector<string> VarTable::getAllVar(){
	vector<string> var;
	for(size_t i=0;i<varVec.size();i++){
		var.push_back(varVec[i]);
	}
	return var;
}

// get variable index using its string
int VarTable::getVarIndex(string s){

for(size_t i=0; i< varVec.size(); i++){
if(varVec.at(i) == s){
return i;
}
}
throw string ("variable doesn't exist in VarTable");
return -1;
}


void VarTable::printTable(){
	for (vector<string>::iterator i = varVec.begin(); i != varVec.end(); i++) {
		cout<< "here"<<endl;
   cout << *i << endl;
}
}
int VarTable::getSize(){
return varVec.size();
} 

// return a vector of all statement numbers
vector<int> VarTable::getAllVarNum() {
	vector<int> vars(getSize());
	iota(vars.begin(), vars.end(), 0);
	return vars;
}


bool VarTable::containsVar(string s){
	if (std::find(varVec.begin(), varVec.end(), s) != varVec.end()){
		return true;
	}
	return false;
}