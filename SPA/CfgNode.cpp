#pragma once
#include <iostream>
#include "CfgNode.h"

using namespace std;


CfgNode::CfgNode(){
}

CfgNode::CfgNode(int prog_line, string node_type) {
	prog_line_num = prog_line;
	CfgNode_type = node_type;
}

int CfgNode::getData(){
	return prog_line_num;
}

pair<CfgNode*, CfgNode*> CfgNode::getFwdPtrs(){
	return fwdPtrs;
}

pair<CfgNode*, CfgNode*> CfgNode::getBkwdPtrs(){
	return bkwdPtrs;
}

void CfgNode::setFwdPtrs(CfgNode* firstPtr, CfgNode* secondPtr){
	fwdPtrs.first = firstPtr;
	fwdPtrs.second = secondPtr;
}

void CfgNode::setBkwdPtrs(CfgNode* firstPtr, CfgNode* secondPtr){
	bkwdPtrs.first = firstPtr;
	bkwdPtrs.second = secondPtr;
}

void CfgNode::setData(int data){
	prog_line_num = data;
}

