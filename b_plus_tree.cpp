#include "b_plus_tree.h"
#include <cmath>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <thread>
using namespace std;

BPlusTree::BPlusTree(void) {
	this->bpt_order = 0;
	this->bpt_root = NULL;
	this->min_order = 0;
	this->leaves_head = NULL;
	this->buffer_size = 0;
}
BPlusTree::BPlusTree(int order) {
	this->bpt_order = order;
	this->bpt_root = NULL;
	this->min_order = ceil(order / 2.0);
	this->leaves_head = NULL;
	this->buffer_size = 0;
}
BPlusTree::BPlusTree(int order, int buffersize) {
	this->bpt_order = order;
	this->bpt_root = NULL;
	this->min_order = ceil(order / 2.0);
	this->leaves_head = NULL;
	this->buffer_size = buffersize;
}

BPlusTree::~BPlusTree() {
	destory_bpt();
}

void BPlusTree::insert_directly(BPT_Node* insert_pos, int key, void* value) {
	// 将key和value 插入到 insert_pos所指向的节点中
	for (int i = 0; i < insert_pos->keys_num; i++) {
		if (insert_pos->keys[i] > key) {
			insert_pos->keys.insert(insert_pos->keys.begin() + i, key);
			if (insert_pos->is_leaf)
				((Leaf_Node*)insert_pos)->values.insert(((Leaf_Node*)insert_pos)->values.begin() + i, *(int*)value);
			else
				((Internal_Node*)insert_pos)->children.insert(((Internal_Node*)insert_pos)->children.begin() + i + 1, (BPT_Node*)value);

			insert_pos->keys_num++;
			return;
		}
		else if (insert_pos->keys[i] == key) {
			// cout <<"线程："<< std::this_thread::get_id()<< " 插入key: "<<key<<" value:"<<value<<" 出现错误（关键码重复）" << endl;
			return;
		}
	}

	insert_pos->keys.push_back(key);
	if (insert_pos->is_leaf)
		((Leaf_Node*)insert_pos)->values.push_back(*(int*)value);
	else
		((Internal_Node*)insert_pos)->children.push_back((BPT_Node*)value);

	insert_pos->keys_num++;

}



void BPlusTree::node_split(BPT_Node* split_pos) {

	BPT_Node* new_node;
	int all_num = split_pos->keys_num;
	int left_num = ceil(all_num / 2.0);
	int right_num = all_num - left_num;

	int middle_key;
	// assign(begin, end) 从begin  (0起始)所指的地方一直复制到end的前一个位置
	// eraser(begin, end) 从begin  (0起始)所指的地方 删除到end的前一个位置
	if (split_pos->is_leaf == true) {

		middle_key = split_pos->keys[left_num];
		new_node = new Leaf_Node();
		Leaf_Node* new_leaf = (Leaf_Node*)new_node;
		Leaf_Node* leaf_split_pos = (Leaf_Node*)split_pos;
		// 分裂后只有一个节点的情况
		new_leaf->keys_num = right_num;
		new_leaf->is_leaf = true;
		new_leaf->keys.assign(leaf_split_pos->keys.begin() + left_num, leaf_split_pos->keys.end());
		new_leaf->values.assign(leaf_split_pos->values.begin() + left_num, leaf_split_pos->values.end());

		new_leaf->next = leaf_split_pos->next;
		new_leaf->pre = leaf_split_pos;
		if (new_leaf->next != NULL) {
			new_leaf->next->pre = new_leaf;
		}

		new_leaf->parent = leaf_split_pos->parent;

		leaf_split_pos->keys_num = left_num;
		leaf_split_pos->keys.erase(leaf_split_pos->keys.begin() + left_num, leaf_split_pos->keys.end());
		leaf_split_pos->values.erase(leaf_split_pos->values.begin() + left_num, leaf_split_pos->values.end());

		leaf_split_pos->next = new_leaf;

	}
	else {
		middle_key = split_pos->keys[left_num - 1];
		new_node = new Internal_Node();
		if (new_node == NULL) {
			cerr <<"线程："<< std::this_thread::get_id()<< " insert操作错误（内存分配失败）" << endl;
			return;
		}
		Internal_Node* new_internal = (Internal_Node*)new_node;
		Internal_Node* internal_split_pos = (Internal_Node*)split_pos;

		new_internal->keys_num = right_num;
		new_internal->is_leaf = false;
		new_internal->keys.assign(internal_split_pos->keys.begin() + left_num, internal_split_pos->keys.end());
		new_internal->children.assign(internal_split_pos->children.begin() + left_num, internal_split_pos->children.end());
		for (auto it = internal_split_pos->children.begin() + left_num; it != internal_split_pos->children.end(); it++) {
			(*it)->parent = new_internal;
		}

		new_internal->parent = internal_split_pos->parent;

		internal_split_pos->keys_num = left_num - 1;
		internal_split_pos->keys.erase(internal_split_pos->keys.begin() + left_num - 1, internal_split_pos->keys.end());
		internal_split_pos->children.erase(internal_split_pos->children.begin() + left_num, internal_split_pos->children.end());
	}
	// middle_key 是应该插入到parent_node的关键码        new_node是新的节点

	BPT_Node* parent_node = split_pos->parent;

	if (parent_node == NULL) {
		parent_node = new Internal_Node();
		if (parent_node == NULL) {
			cerr <<"线程："<< std::this_thread::get_id()<< " insert操作错误（内存分配失败）" << endl;
			return;
		}
		parent_node->is_leaf = false;
		parent_node->keys_num++;
		parent_node->keys.push_back(middle_key);
		((Internal_Node*)parent_node)->children.push_back(split_pos);
		((Internal_Node*)parent_node)->children.push_back(new_node);
		split_pos->parent = parent_node;
		new_node->parent = parent_node;
		bpt_root = parent_node;

	}
	else {
		insert_directly(parent_node, middle_key, new_node);

		if (parent_node->keys_num > bpt_order - 1) {
			node_split(parent_node);
		}
	}


}

void BPlusTree::insert_data(int key, int value) {

	Leaf_Node* insert_pos = find_node_ptr(key);

	if (insert_pos == NULL) {
		this->bpt_root = new Leaf_Node();

		this->leaves_head = (Leaf_Node*)bpt_root;
		if (this->bpt_root == NULL) {
			cerr <<"线程："<< std::this_thread::get_id()<< " insert操作错误（内存分配失败）" << endl;
			return;
		}

		insert_directly(bpt_root, key, &value);
		return;
	}
	insert_directly(insert_pos, key, &value);

	// 先插入再检查是否，超过order的限制
	if (insert_pos->keys_num > bpt_order - 1) {
		node_split(insert_pos);
	}
}




Leaf_Node* BPlusTree::find_node_ptr(int key) {

	BPT_Node* node_ptr = this->bpt_root;

	if (this->bpt_root == NULL) {
		return NULL;
	}
	while (!node_ptr->is_leaf) {
		bool flag = false;
		for (int i = 0; i < node_ptr->keys_num; i++) {
			if (node_ptr->keys[i] > key) {
				node_ptr = ((Internal_Node*)node_ptr)->children[i];
				flag = true;
				break;
			}
		}
		if (!flag) {
			node_ptr = ((Internal_Node*)node_ptr)->children[node_ptr->keys_num];
		}
	}

	return (Leaf_Node*)node_ptr;

}

int* BPlusTree::find_data_ptr(int key) {
	Leaf_Node* bp = find_node_ptr(key);
	if (bp == NULL)
		return NULL;
	for (int i = 0; i < bp->keys_num; i++) {
		if (bp->keys[i] == key) {
			return &(bp->values[i]);
		}
	}
	return NULL;

}

int BPlusTree::search_data(int key) {
	int* temp = find_data_ptr(key);
	if (temp != NULL)
		return *temp;
	return -1;
}


void BPlusTree::update_data(int key, int new_value) {
	int *value = find_data_ptr(key);
	if (value == NULL) {
		// cout <<"线程："<< std::this_thread::get_id()<< " update 操作错误 （key: "<<key<<" 不存在）" << endl;
		return;
	}
	*value = new_value;
	return;
}

int BPlusTree::find_child_num(BPT_Node* raw_node) {
	int child_num = -1;
	Internal_Node* parent = (Internal_Node*)raw_node->parent;
	for (int i = 0; i < parent->keys_num + 1; i++) {
		if (parent->children[i] == raw_node) {
			child_num = i;
			return i;
		}
	}
}

void BPlusTree::remove_data(int key) {
	Leaf_Node* delete_pos = find_node_ptr(key);
	if (delete_pos == NULL) {
		// cout <<"线程："<< std::this_thread::get_id()<< " delete操作错误 （key: "<<key<<" 不存在）" << endl;
		return;
	}
	bool is_exist = false;

	for (int i = 0; i < delete_pos->keys_num; i++) {
		if (delete_pos->keys[i] == key) {
			delete_pos->keys.erase(delete_pos->keys.begin() + i);
			((Leaf_Node*)delete_pos)->values.erase(((Leaf_Node*)delete_pos)->values.begin() + i);
			delete_pos->keys_num--;
			is_exist = true;
			break;
		}
	}
	if (!is_exist) {
		// cout <<"线程："<< std::this_thread::get_id()<< " delete操作错误 （key: "<<key<<" 不存在）" << endl;
		return;
	}

	if (delete_pos->keys_num < min_order - 1 && delete_pos->parent != NULL) {
		Leaf_Node* right_node = NULL;
		Leaf_Node* left_node = NULL;

		int child_num = find_child_num(delete_pos);

		if (child_num - 1 >= 0) {
			left_node = (Leaf_Node*)((Internal_Node*)delete_pos->parent)->children[child_num - 1];
		}
		if (child_num + 1 < delete_pos->parent->keys_num + 1) {
			right_node = (Leaf_Node*)((Internal_Node*)delete_pos->parent)->children[child_num + 1];
		}

		if (left_node != NULL && left_node->keys_num >= this->min_order) {
			// 从左兄弟节点借一个key
			delete_pos->keys.insert(delete_pos->keys.begin(), left_node->keys.back());
			((Leaf_Node*)delete_pos)->values.insert(((Leaf_Node*)delete_pos)->values.begin(), left_node->values.back());
			delete_pos->keys_num++;

			left_node->keys.erase(left_node->keys.end() - 1);
			left_node->values.erase(left_node->values.end() - 1);
			left_node->keys_num++;

			delete_pos->parent->keys[child_num - 1] = delete_pos->keys.front();
			return;

		}
		else if (right_node != NULL && right_node->keys_num >= this->min_order) {
			// 从右兄弟节点借一个key
			delete_pos->keys.push_back(right_node->keys.front());
			((Leaf_Node*)delete_pos)->values.push_back(right_node->values.front());
			delete_pos->keys_num++;

			right_node->keys.erase(right_node->keys.begin());
			right_node->values.erase(right_node->values.begin());
			right_node->keys_num--;

			delete_pos->parent->keys[child_num] = right_node->keys.front();
			return;

		}
		else {
			if (left_node != NULL && left_node->keys_num < this->min_order) {
				// 与左兄弟进行合并, 将当前节点插入到左兄弟节点的后边
				left_node->keys.insert(left_node->keys.end(), delete_pos->keys.begin(), delete_pos->keys.end());
				left_node->values.insert(left_node->values.begin(), ((Leaf_Node*)delete_pos)->values.begin(), ((Leaf_Node*)delete_pos)->values.end());

				left_node->next = ((Leaf_Node*)delete_pos)->next;
				if (left_node->next != NULL) {
					left_node->next->pre = left_node;
				}
				left_node->keys_num += delete_pos->keys_num;

				delete_pos->parent->keys.erase(delete_pos->parent->keys.begin() + child_num - 1); // 从父节点中删除该节点的信息
				((Internal_Node*)delete_pos->parent)->children.erase(((Internal_Node*)delete_pos->parent)->children.begin() + child_num);
				delete_pos->parent->keys_num--;

				delete delete_pos; // 释放内存
				delete_pos = left_node;


			}
			else if (right_node != NULL && right_node->keys_num < this->min_order) {
				// 与右兄弟进行合并, 将右兄弟节点的信息插入到当前节点的后边
				delete_pos->keys.insert(delete_pos->keys.end(), right_node->keys.begin(), right_node->keys.end());
				((Leaf_Node*)delete_pos)->values.insert(((Leaf_Node*)delete_pos)->values.end(), right_node->values.begin(), right_node->values.end());

				((Leaf_Node*)delete_pos)->next = right_node->next;
				if (right_node->next != NULL) {
					right_node->next->pre = ((Leaf_Node*)delete_pos);
				}

				delete_pos->keys_num += right_node->keys_num;
				delete_pos->parent->keys.erase(delete_pos->parent->keys.begin() + child_num);
				delete_pos->parent->keys_num--;
				((Internal_Node*)delete_pos->parent)->children.erase(((Internal_Node*)delete_pos->parent)->children.begin() + child_num + 1);

				delete right_node;
			}
			Internal_Node* del_inter_pos = (Internal_Node*)delete_pos; // 对内部节点进行合并

			while (del_inter_pos->parent != this->bpt_root) {
				Internal_Node *left_inter_node = NULL;
				Internal_Node *right_inter_node = NULL;
				del_inter_pos = (Internal_Node*)del_inter_pos->parent;

				if (del_inter_pos->keys_num >= this->min_order - 1) { // 父节点key的个数大于最低限制，不需要操作
					return;
				}
				int child_inter_num = find_child_num(del_inter_pos);
				if (child_num - 1 >= 0) {
					left_inter_node = (Internal_Node*)((Internal_Node*)del_inter_pos->parent)->children[child_inter_num - 1];
				}

				if (child_num + 1 < del_inter_pos->parent->keys_num + 1) {
					right_inter_node = (Internal_Node*)((Internal_Node*)del_inter_pos->parent)->children[child_inter_num + 1];
				}

				if (right_inter_node != NULL && right_inter_node->keys_num >= this->min_order) {
					// 从右兄弟借
					del_inter_pos->keys.push_back(del_inter_pos->parent->keys[child_inter_num]);
					del_inter_pos->children.push_back(right_inter_node->children.front());
					del_inter_pos->parent->keys[child_inter_num] = right_inter_node->keys.front();

					del_inter_pos->keys_num++;

					right_inter_node->children.erase(right_inter_node->children.begin());
					right_inter_node->keys.erase(right_inter_node->keys.begin());

					right_inter_node->keys_num--;
					del_inter_pos->children.back()->parent = del_inter_pos;
					return;
				}
				else if (left_inter_node != NULL && left_inter_node->keys_num >= this->min_order) {
					// 从左兄弟借
					del_inter_pos->children.insert(del_inter_pos->children.begin(), left_inter_node->children.back());
					del_inter_pos->keys.insert(del_inter_pos->keys.begin(), del_inter_pos->parent->keys[child_inter_num - 1]);
					del_inter_pos->parent->keys[child_inter_num] = left_inter_node->keys.back();

					del_inter_pos->keys_num++;
					left_inter_node->children.erase(left_inter_node->children.end() - 1);
					left_inter_node->keys.erase(left_inter_node->keys.end() - 1);
					left_inter_node->keys_num--;

					del_inter_pos->children.front()->parent = del_inter_pos;
					return;
				}
				else {
					if (left_inter_node != NULL && left_inter_node->keys_num < this->min_order) {
						// 和左兄弟合并
						left_inter_node->children.insert(left_inter_node->children.end(), del_inter_pos->children.begin(), del_inter_pos->children.end());
						left_inter_node->keys.insert(left_inter_node->keys.end(), del_inter_pos->parent->keys[child_inter_num - 1]);
						left_inter_node->keys.insert(left_inter_node->keys.end(), del_inter_pos->keys.begin(), del_inter_pos->keys.end());
						left_inter_node->keys_num += (del_inter_pos->keys_num + 1);
						for (int i = 0; i < del_inter_pos->keys_num + 1; i++) {
							del_inter_pos->children[i]->parent = left_inter_node;
						}

						del_inter_pos->parent->keys.erase(del_inter_pos->parent->keys.begin() + child_inter_num - 1);
						((Internal_Node*)del_inter_pos->parent)->children.erase(((Internal_Node*)del_inter_pos->parent)->children.begin() + child_inter_num);
						del_inter_pos->parent->keys_num--;

						delete del_inter_pos; // 释放内存
						del_inter_pos = left_inter_node;



					}
					else if (right_inter_node != NULL && right_inter_node->keys_num < this->min_order) {
						// 和右兄弟合并，将当前节点信息插入到右兄弟的头部
						right_inter_node->children.insert(right_inter_node->children.begin(), del_inter_pos->children.begin(), del_inter_pos->children.end());
						right_inter_node->keys.insert(right_inter_node->keys.begin(), del_inter_pos->parent->keys[child_inter_num]);
						right_inter_node->keys.insert(right_inter_node->keys.begin(), del_inter_pos->keys.begin(), del_inter_pos->keys.end());
						right_inter_node->keys_num += (del_inter_pos->keys_num + 1);
						for (int i = 0; i < del_inter_pos->keys_num + 1; i++) {
							del_inter_pos->children[i]->parent = right_inter_node;
						}
						// 从该节点的父节点中删除该节点的信息
						del_inter_pos->parent->keys.erase(del_inter_pos->parent->keys.begin() + child_inter_num);
						((Internal_Node*)del_inter_pos->parent)->children.erase(((Internal_Node*)del_inter_pos->parent)->children.begin() + child_inter_num);
						del_inter_pos->parent->keys_num--;

						delete del_inter_pos; // 释放内存
						del_inter_pos = right_inter_node;
					}
				}
			}
			if (del_inter_pos->parent == this->bpt_root && this->bpt_root->keys_num == 0) {
				del_inter_pos->parent = NULL;
				this->bpt_root = del_inter_pos;
				return;
			}

		}
	}
}

void BPlusTree::print_tree() { // 打印整个B+树
	int level = 1;
	BPT_Node* p = this->bpt_root;
	queue<BPT_Node*> que;
	que.push(NULL);
	que.push(p);
	while (!que.empty()) {
		BPT_Node* head = que.front();

		if (head == NULL) {
			cout << endl;
			que.pop();
			if (!que.front()->is_leaf)
				que.push(NULL);
			continue;
		}

		if (head->is_leaf == false) {
			bool flag = true;
			for (auto it = head->keys.begin(); it != head->keys.end(); it++) {
				flag = false;
				cout << head->keys_num << ':' << *it << ' ';
			}
			if (flag) {
				cout << head->keys_num << endl;
			}
			cout << "$ ";

			for (auto it = ((Internal_Node*)head)->children.begin(); it != ((Internal_Node*)head)->children.end(); it++) {
				que.push(*it);
			}
			que.pop();
		}
		else {
			for (int i = 0; i < head->keys_num; i++) {
				cout << ((Leaf_Node*)head)->keys[i] << ':' << ((Leaf_Node*)head)->values[i] << ' ';
			}
			if (head->keys_num > 0)
				cout << "| ";
			que.pop();
		}
		cout << endl;
	}
}

void BPlusTree::print_leaves() { // 打印所有叶子节点
	Leaf_Node* p = this->leaves_head;
	while (p != NULL) {
		for (int i = 0; i < p->keys_num; i++) {
			cout << p->keys[i] << ":" << p->values[i] << "  ";
		}
		cout << " | ";
		p = p->next;
	}
	cout << endl;
}


void BPlusTree::save_bpt(string filename) {
	/*
		将B+树持久化
		存储格式：B+树的最大order数，B+数的最小order数
		从根节点起，从左向右逐层保存每个节点。
		每个节点的保存格式为：是否为叶子节点、节点内关键码的个数、（若为叶子节点：关键码对应的value）、使用'\n'当作结束符，每项之间通过空格分隔。
	*/
	if (this->bpt_root == NULL) {
		cerr << "线程：" << std::this_thread::get_id() << " B+树序列化 出现错误（B+树为空）" << endl;

		return;
	}

	ofstream outfile;
	outfile.open(filename, ios::out | ios::trunc);
	if (!outfile.is_open()) {
		cerr << "线程：" << std::this_thread::get_id() << " B+树序列化 出现错误（文件打开失败）" << endl;
		return;
	}
	BPT_Node* node_ptr = this->bpt_root;
	outfile << this->bpt_order << ' ' << this->min_order << endl;


	queue<BPT_Node* > node_que;
	node_que.push(node_ptr);
	while (!node_que.empty()) {

		node_ptr = node_que.front();
		node_que.pop();
		if (node_ptr->is_leaf) { // 从左到有遍历叶子节点，保存其key和value
			outfile << 1 << ' ' << node_ptr->keys_num << ' ';
			for (int i = 0; i < node_ptr->keys_num; i++) {
				outfile << node_ptr->keys[i] << ' ' << ((Leaf_Node*)node_ptr)->values[i] << ' ';
			}
			outfile << endl;
		}
		else { // 从左到右遍历该层的内部节点，保存其key值，并将其children加入到队列中
			outfile << 0 << ' ' << node_ptr->keys_num << ' ';
			for (auto it = node_ptr->keys.begin(); it != node_ptr->keys.end(); it++) {
				outfile << *it << ' ';
			}
			outfile << endl;
			for (auto it = ((Internal_Node*)node_ptr)->children.begin(); it != ((Internal_Node*)node_ptr)->children.end(); it++) {
				node_que.push(*it);
			}
		}
	}
}

void BPlusTree::read_bpt(string filename) {
	if (this->bpt_root != NULL) {
		destory_bpt(); // 不为空时则销毁当前的B+树
	}

	ifstream inputfile;
	inputfile.open(filename, ios::in);
	if (!inputfile.is_open()) {
		cerr << "线程：" << std::this_thread::get_id() << " B+树反序列化 出现错误（文件打开失败）" << endl;
		return;
	}

	string line;
	getline(inputfile, line);
	istringstream node_message(line);

	node_message >> this->bpt_order >> this->min_order;
	// 文件首行保存的是b+树的orders
	cout << this->bpt_order << " " << this->min_order << endl;

	queue<BPT_Node* > node_queue; //保存上层节点信息，用来建立子节点和父节点的联系
	Leaf_Node* pre_leaf = NULL; //保存当前叶子节点的前一个叶子节点信息，用来建立叶子节点之间的双向链表
	while (getline(inputfile, line)) { //保存b+树时，节点数据按行在文件中保存，因此读取时按行进行读取
		bool is_leaf;
		int keys_num;

		node_message.clear();
		node_message.str(line);
		node_message >> is_leaf >> keys_num;

		if (is_leaf) { // 读取到的是叶子节点
			Leaf_Node* new_leaf_node = new Leaf_Node();
			new_leaf_node->is_leaf = true;
			new_leaf_node->keys_num = keys_num;
			for (int i = 0; i < keys_num; i++) {
				int key, value;
				node_message >> key >> value;
				new_leaf_node->keys.push_back(key);
				new_leaf_node->values.push_back(value);
				//将文件中叶子节点的关键码和键值读取到Leaf_Node中
			}
			Internal_Node* parent_node = (Internal_Node*)node_queue.front(); // 寻找其父节点
			if (parent_node->keys_num + 1 == parent_node->children.size()) {
				node_queue.pop(); // 队首节点的childern已满，所以队首下一个节点才是该节点的父节点
				parent_node = (Internal_Node*)node_queue.front();
			}
			new_leaf_node->parent = parent_node;
			parent_node->children.push_back(new_leaf_node);
			// 叶子节点不需要添加到node_queue中，因为其没有儿子节点

			if (pre_leaf == NULL) { // 该节点是第一个叶子节点，处于队首
				this->leaves_head = new_leaf_node;
				new_leaf_node->pre = NULL;
				new_leaf_node->next = NULL;
				pre_leaf = new_leaf_node; // pre_leaf指向当前叶子节点的前一个叶子节点
			}
			else {
				new_leaf_node->next = NULL;
				new_leaf_node->pre = pre_leaf;
				pre_leaf->next = new_leaf_node; // 建立双向链表
				pre_leaf = new_leaf_node; // pre_leaf指向当前叶子节点的前一个叶子节点
			}
		}
		else { // 读取到的是内部节点
			Internal_Node* new_inter_node = new Internal_Node();
			new_inter_node->is_leaf = false;
			new_inter_node->keys_num = keys_num;
			for (int i = 0; i < keys_num; i++) { //从文件中读取内部节点的关键码，插入到Internal_Node中
				int key;
				node_message >> key;
				new_inter_node->keys.push_back(key);
			}

			if (node_queue.empty()) {
				this->bpt_root = new_inter_node; // 队列为空，说明该节点是根节点
				new_inter_node->parent = NULL;
				node_queue.push(new_inter_node);
			}
			else {
				Internal_Node* parent_node = (Internal_Node*)node_queue.front(); // 队列不为空，则处于队首的节点是该节点的父节点
				if (parent_node->keys_num + 1 == parent_node->children.size()) {
					node_queue.pop(); // 队首节点的childern已满，所以队首下一个节点才是该节点的父节点
					parent_node = (Internal_Node*)node_queue.front();
				}
				new_inter_node->parent = parent_node;
				parent_node->children.push_back(new_inter_node); // 将该节点插入到父节点的children中
				node_queue.push(new_inter_node); // 将该节点插入到队列中，等待与之后的节点建立父子联系
			}
		}
	}
}

void BPlusTree::destory_bpt() {
	queue<BPT_Node* > nodelist;
	if (this->bpt_root == NULL) {
		this->leaves_head = NULL;
		this->min_order = 0;
		this->bpt_order = 0;
		return;
	}
	nodelist.push(bpt_root);
	while (!nodelist.empty()) { // 当队列为空时说明所有节点都已经delete完毕
		BPT_Node *front_node = nodelist.front();
		nodelist.pop();
		if (!front_node->is_leaf) {
			for (auto it = ((Internal_Node*)front_node)->children.begin(); it != ((Internal_Node*)front_node)->children.end(); it++) {
				nodelist.push(*it); // 将内部节点的children节点加入到节点队列中，等待删除
			}
		}
		delete front_node;
	}
	this->bpt_root = NULL;
	this->leaves_head = NULL;
	this->min_order = 0;
	this->bpt_order = 0;
	return;
}


void BPlusTree::add_command(MyCommand com) {

	unique_lock<mutex> pvlock(this->pv_mtx); // 获得互斥锁
	while ((this->TaskBuffer).size() == this->buffer_size) {
		// 命令缓存区已满
		// cout << "生产者线程：" << std::this_thread::get_id() << ' ' << com.operation << " 操作，被阻塞（缓存区已满）" << endl;
		(this->buffer_not_full).wait(pvlock);
	}
	// cout << "生产者线程：" << com.operation << ": " << com.key << endl;
	TaskBuffer.push(com);
	(this->buffer_not_empty).notify_all();
	// 缓存区不为空，唤醒被阻塞的消费者线程
	//pvlock.unlock();

}

void BPlusTree::take_command() {
	unique_lock<mutex> pvlock(this->pv_mtx); // 获得互斥锁
	while ((this->TaskBuffer).size() == 0) {
		// cout << "消费者线程：" << std::this_thread::get_id() << " 被阻塞（缓存区为空）" << endl;
		(this->buffer_not_empty).wait(pvlock);
	}

	MyCommand com = (this->TaskBuffer).front();
	// cout << "消费者线程：" << com.operation << ": " << com.key << endl;
	(this->TaskBuffer).pop();

	if (com.operation == "insert") {
		this->insert_data(com.key, com.value);
	}
	else if (com.operation == "remove") {
		this->remove_data(com.key);
	}
	else if (com.operation == "update") {
		this->update_data(com.key, com.value);
	}
	else if (com.operation == "search") {
		this->search_data(com.key);

	}
	else {
		cerr << "消费者线程：" << std::this_thread::get_id() << " 读取到非法命令:"<< com.operation << endl;
	}

	(this->buffer_not_full).notify_all();
	//pvlock.unlock();
}

// 并发操作
void BPlusTree::multi_insert(int key, int value)
{
	unique_lock<mutex> product_lock(this->produced_mtx);
	MyCommand com;
	com.operation = "insert";
	com.key = key;
	com.value = value;
	add_command(com);

}
void BPlusTree::multi_update(int key, int new_value)
{
	unique_lock<mutex> product_lock(this->produced_mtx);
	MyCommand com;
	com.operation = "update";
	com.key = key;
	com.value = new_value;
	add_command(com);
}
void BPlusTree::multi_search(int key)
{
	unique_lock<mutex> product_lock(this->produced_mtx);
	MyCommand com;
	com.operation = "search";
	com.key = key;
	com.value = -1;
	add_command(com);
}
void BPlusTree::multi_remove(int key)
{
	unique_lock<mutex> product_lock(this->produced_mtx);
	MyCommand com;
	com.operation = "remove";
	com.key = key;
	com.value = -1;
	add_command(com);
}

void BPlusTree::consume_task() {
	unique_lock<mutex> consumer_lock(this->consumed_mtx);
	take_command();
}