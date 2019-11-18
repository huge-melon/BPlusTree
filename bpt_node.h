#ifndef bpt_node_h
#define bpt_npde_h

#include <iostream>
#include <vector>
using namespace std;

class BPT_Node
{

public:
	vector<long long> keys;
	BPT_Node* parent;
	int keys_num;
	bool is_leaf;
};
// 叶子节点
class Leaf_Node : public BPT_Node
{
public:
	Leaf_Node* next;
	Leaf_Node* pre;
	vector<long long> values;
	Leaf_Node() {
		keys_num = 0;
		is_leaf = true;
		next = NULL;
		pre = NULL;
		parent = NULL;
	}
};
// 内部节点
class Internal_Node : public BPT_Node
{
public:
	vector<BPT_Node*> children;
	Internal_Node() {
		keys_num = 0;
		is_leaf = false;
		parent = NULL;
	}
};

#endif