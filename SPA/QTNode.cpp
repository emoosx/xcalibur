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
}

QTNode::QTNode(STRING newKey, QTNode newParent){
	key = newKey;
	parent = &newParent;
}

QTNode::QTNode(int newKey) {
	value = newKey;
}

/******* Accessors *******/
INDEX QTNode::getNumChild(){
	return children.size();
}

QTNode QTNode::getChild(INDEX index){
	return children.at(index);
}

STRING QTNode::getKey(){
	return key;
}

/******* Mutators *******/
VOID QTNode::addChild(QTNode newChild){
	children.push_back(newChild);
}

VOID QTNode::setParent(QTNode newParent){
	parent = &newParent;
}

VOID QTNode::setKey(STRING newKey){
	key = newKey;
}