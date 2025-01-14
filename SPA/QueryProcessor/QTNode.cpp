// File name: QTNode.cpp
#include <string>
#include <vector>
#include "QTNode.h"

using namespace std;

typedef int INDEX;
typedef string STRING;
typedef void VOID;

/******* Constructors *******/
QTNode::QTNode(){

}

QTNode::QTNode(STRING newKey){
	key=newKey;
	data="";
}

QTNode::QTNode(STRING newKey, STRING newData) {
	key = newKey;
	data = data;
}

QTNode::QTNode(STRING newKey, QTNode* newParent){
	key = newKey;
	data = "";
	parent = newParent;
}

QTNode::QTNode(int newKey) {
	value = newKey;
}

QTNode::QTNode(int newKey, QTNode* newParent){
	value = newKey;
	parent = newParent;
}


/******* Accessors *******/
INDEX QTNode::getNumChild(){
	return children.size();
}

QTNode* QTNode::getChild(INDEX index){
	return children.at(index);
}

STRING QTNode::getKey(){
	return key;
}

STRING QTNode::getData() {
	return data;
}

int QTNode::getValue(){
	return value;
}

/******* Mutators *******/
VOID QTNode::addChild(QTNode* newChild){
	children.push_back(newChild);
}

VOID QTNode::setParent(QTNode* newParent){
	parent = newParent;
}

VOID QTNode::setKey(STRING newKey){
	key = newKey;
}

VOID QTNode::setData(STRING newData){
	data = newData;
}
