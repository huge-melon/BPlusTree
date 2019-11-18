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
	void insert_data(long long key, long long value); // done
	//void delete_data(int key); // no done
	void update_data(long long key, long long new_vlaue); // done
	int search_data(long long key); // done
	void remove_data(long long key);

	void save_bpt(string filename);
	void read_bpt(string filename);
	void destory_bpt();

	void print_leaves();
	void print_tree();
private:
	BPT_Node* bpt_root;
	int bpt_order;
	int min_order;  // 最小的子树个数
	Leaf_Node* leaves_head;
	void insert_directly(BPT_Node* insert_pos, long long key, void* value);
	void node_split(BPT_Node* insert_pos);

	Leaf_Node* find_node_ptr(long long key);
	long long* find_data_ptr(long long key);

	//void delete_directly(Leaf_Node* delete_pos, int key);
	//// Leaf_Node* find_delete_pos(int key);
	//void merge_with_left(BPT_Node* raw_node, BPT_Node* left_node);
	//void merge_with_right(BPT_Node* raw_node, BPT_Node* right_node);

	//void borrow_from_right(BPT_Node* raw_node, BPT_Node* right_node);
	//void borrow_from_left(BPT_Node* raw_node, BPT_Node* left_node);
	int find_child_num(BPT_Node* raw_node);

};



#endif