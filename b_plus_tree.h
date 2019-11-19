#ifndef b_plus_tree_h
#define b_plus_tree_h
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

class BPT_Node
{
public:
	std::vector<int> keys; // 保存节点中的key
	BPT_Node* parent; // 节点的父亲节点指针
	int keys_num; // 节点中key的数量
	bool is_leaf; // 是否为叶子节点
};

class Leaf_Node : public BPT_Node // 叶子节点
{
public:
	Leaf_Node* next; // 后一个叶子节点指针
	Leaf_Node* pre; // 前一个叶子节点指针
	std::vector<int> values; // key对应的值
	Leaf_Node() {
		keys_num = 0;
		is_leaf = true;
		next = NULL;
		pre = NULL;
		parent = NULL;
	}
};
// 内部节点
class Internal_Node : public BPT_Node // 内部节点，即非叶子节点
{
public:
	std::vector<BPT_Node*> children; // 保存儿子节点的指针
	Internal_Node() {
		keys_num = 0;
		is_leaf = false;
		parent = NULL;
	}
};


struct MyCommand // 保存操作类型
{
	std::string operation;
	int key;
	int value;
};



class BPlusTree // B+树
{
public:
	BPlusTree();
	BPlusTree(int order);
	BPlusTree(int order, int buffer_size);
	~BPlusTree();
	void set_order(int num) {
		this->bpt_order = num;
	}
	int get_order() {
		return this->bpt_order;
	}
	void insert_data(int key, int value); // 插入数据
	void update_data(int key, int new_vlaue); // 更新数据
	int search_data(int key); // 查到key所对应的value
	void remove_data(int key); // 删除key所对应的数据
	void destory_bpt(); // 销毁B+树

	void save_bpt(std::string filename); // 将B+树保存到磁盘中
	void read_bpt(std::string filename); // 从磁盘中恢复B+树

	void print_leaves(); // 打印叶子节点
	void print_tree(); // 打印整个B+树

	// 并发操作
	void set_buffersize(int bsize) {
		this->buffer_size = bsize;
	}
	void multi_insert(int key, int value);
	void multi_update(int key, int new_value);
	void multi_search(int key);
	void multi_remove(int key);
	void consume_task();
private:
	BPT_Node* bpt_root; // 指向根节点
	int bpt_order; // 子树最大个数
	int min_order;  // 子树最小个数
	Leaf_Node* leaves_head; // 指向最左端的叶子节点

	void insert_directly(BPT_Node* insert_pos, int key, void* value); // 直接向指向的节点中插入数据
	void node_split(BPT_Node* insert_pos); // 分裂指向的节点
	Leaf_Node* find_node_ptr(int key); // 找到key应该插入节点的指针
	int* find_data_ptr(int key); // 找到key所对应的value的执行，用于update操作
	int find_child_num(BPT_Node* raw_node); // 找到该节点是其父节点的第几个儿子节点

	// 用于并发
	int buffer_size; // 资源信号量
	std::queue<MyCommand> TaskBuffer; // 缓存区

	std::mutex produced_mtx; // 生产者之间互斥
	std::mutex consumed_mtx; // 消费者之间互斥
	std::mutex pv_mtx; // 生产者消费者之间互斥
	std::condition_variable buffer_not_full; // 缓存区是否满
	std::condition_variable buffer_not_empty; // 缓存区是否空
	void add_command(MyCommand com); // 生产者线程：向缓存区中添加task
	void take_command(); // 消费者线程：从缓存区中取出task并执行


};

#endif