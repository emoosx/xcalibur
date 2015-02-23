#pragma once

using namespace std;

enum CFGNODE_TYPE {
	WHILE_NODE,
	IF_NODE,
	STMTLST_NODE
};

static string CFGNODE_NAMES[] = {
	"WHILE_NODE",
	"IF_NODE",
	"STMTLST_NODE"
};

using namespace std ;

class CfgNode {

private:
	int prog_line_num;
	string CfgNode_type;
	pair<CfgNode*,CfgNode*> fwdPtrs;
	pair<CfgNode*, CfgNode*> bkwdPtrs;

public:
	CfgNode();
	CfgNode(int, string );
	int getData();
	pair<CfgNode*,CfgNode*> getFwdPtrs();
	pair<CfgNode*, CfgNode*> getBkwdPtrs();
	void setFwdPtrs(CfgNode*, CfgNode*);
	void setBkwdPtrs(CfgNode*, CfgNode*);
	void setData(int);
};