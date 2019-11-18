#ifndef b_plus_tree_h
#define b_plus_tree_h

#include "bpt_node.h"
#include <string>

class BPlusTree
{
public:
	BPlusTree();
	BPlusTree(int order);
	~BPlusTree();
	void insert_data(long long key, long long value); // 插入数据
	void update_data(long long key, long long new_vlaue); // 更新数据
	int search_data(long long key); // 查到key所对应的value
	void remove_data(long long key); // 删除key所对应的数据

	void save_bpt(string filename); // 将B+树保存到磁盘中
	void read_bpt(string filename); // 从磁盘中恢复B+树
	void destory_bpt(); // 销毁B+树

	void print_leaves(); // 打印叶子节点
	void print_tree(); // 打印整个B+树
private:
	BPT_Node* bpt_root;
	int bpt_order;
	int min_order;  // 最小的子树个数
	Leaf_Node* leaves_head;
	void insert_directly(BPT_Node* insert_pos, long long key, void* value);
	void node_split(BPT_Node* insert_pos);

	Leaf_Node* find_node_ptr(long long key);
	long long* find_data_ptr(long long key);

	int find_child_num(BPT_Node* raw_node);

};



#endif