#include "b_plus_tree.h"
#include <cmath>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>
// 初始化

BPlusTree::BPlusTree(void) {
	this->bpt_order = 0;
	this->bpt_root = NULL;
	this->min_order = 0;
	this->leaves_head = NULL;
}
BPlusTree::BPlusTree(int order) {
	this->bpt_order = order;
	this->bpt_root = NULL;
	this->min_order = ceil(order / 2.0);
	this->leaves_head = NULL;
}

// 节点未满直接插入 

void BPlusTree::insert_directly(BPT_Node* insert_pos, int key, void* value) {

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
			cout << "关键码重复" << endl;
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

	if (this->bpt_root == NULL) {
		this->bpt_root = new Leaf_Node();

		this->leaves_head = (Leaf_Node*)bpt_root;
		if (this->bpt_root == NULL) {
			cout << "request mem error" << endl;
		}

		insert_directly(bpt_root, key, &value);
		return;
	}



	BPT_Node* bp = bpt_root;

	Leaf_Node* insert_pos = find_node_ptr(key);
	if (insert_pos == NULL) {
		cout << "关键码重复" << endl;
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
		cout << "not found" << endl;
		return;
	}
	*value = new_value;
	return;
}


//Leaf_Node* BPlusTree::find_delete_pos(int key) {
//	Leaf_Node *node_ptr = find_node_ptr(key);
//
//
//	return NULL;
//}

void BPlusTree::delete_directly(Leaf_Node* delete_pos, int key) {
	// 删空的情况??

	for (int i = 0; i < delete_pos->keys_num; i++) {
		if (delete_pos->keys[i] == key) {
			if (i == 0) {
				// 修改父节点
				for (int j = 0; j < delete_pos->parent->keys_num; j++) {
					if (delete_pos->parent->keys[j] == key) {
						if (delete_pos->keys_num > 1) {
							delete_pos->parent->keys[j] == delete_pos->keys[1];
						}
						else {
							delete_pos->parent->keys.erase(delete_pos->parent->keys.begin() + j);
							((Internal_Node*)delete_pos->parent)->children.erase(((Internal_Node*)delete_pos->parent)->children.begin() + j + 1);
						}
						break;
					}
				}
			}
			delete_pos->keys.erase(delete_pos->keys.begin() + i);
			delete_pos->values.erase(delete_pos->values.begin() + i);
			delete_pos->keys_num--;
			break;
		}
	}
}


void BPlusTree::borrow_from_left(BPT_Node* raw_node, BPT_Node* left_node) {
	if (raw_node->is_leaf) {
		Internal_Node* parent = (Internal_Node*)raw_node->parent;
		//delete_directly 删的时候应该连带把父节点中的也替换了
		int raw_internal_node_key = raw_node->keys.front();

		int key_moved = left_node->keys.back();
		left_node->keys.pop_back();
		int value_moved = ((Leaf_Node*)left_node)->values.back();
		((Leaf_Node*)left_node)->values.pop_back();
		left_node->keys_num--;

		raw_node->keys.insert(raw_node->keys.begin(), key_moved);
		((Leaf_Node*)raw_node)->values.insert(((Leaf_Node*)raw_node)->values.begin(), value_moved);
		raw_node->keys_num++;
		// insert_directly(delete_pos, key_moved, &value_moved);

		for (auto it = parent->keys.begin(); it != parent->keys.end(); it++) {
			if (*it == raw_internal_node_key) {
				*it = key_moved;
				break;
			}
		}
	}
	else {

	}

}
void BPlusTree::borrow_from_right(BPT_Node* raw_node, BPT_Node* right_node) {
	if (raw_node->is_leaf) {
		Internal_Node* parent = (Internal_Node*)raw_node->parent;

		int key_moved = right_node->keys.front();
		right_node->keys.erase(right_node->keys.begin());

		int value_moved = ((Leaf_Node*)right_node)->values.front();
		((Leaf_Node*)right_node)->values.erase(((Leaf_Node*)right_node)->values.begin());
		right_node->keys_num--;

		((Leaf_Node*)raw_node)->keys.push_back(key_moved);
		((Leaf_Node*)raw_node)->values.push_back(value_moved);
		((Leaf_Node*)raw_node)->keys_num++;

		for (auto it = parent->keys.begin(); it != parent->keys.end(); it++) {
			if (*it == key_moved) {
				*it = right_node->keys.front();
				break;
			}
		}
	}
	else {

	}

}

void BPlusTree::merge_with_left(BPT_Node* raw_node, BPT_Node* left_node) {
	if (raw_node->is_leaf) {
		((Leaf_Node*)left_node)->keys.insert(((Leaf_Node*)left_node)->keys.end(), ((Leaf_Node*)raw_node)->keys.begin(), ((Leaf_Node*)raw_node)->keys.end());
		((Leaf_Node*)left_node)->values.insert(((Leaf_Node*)left_node)->values.end(), ((Leaf_Node*)raw_node)->values.begin(), ((Leaf_Node*)raw_node)->values.end());
		((Leaf_Node*)left_node)->next = ((Leaf_Node*)raw_node)->next;
		if (((Leaf_Node*)raw_node)->next != NULL) {
			((Leaf_Node*)raw_node)->next->pre = ((Leaf_Node*)left_node);
		}
		left_node->keys_num += raw_node->keys_num;

		for (int i = 0; i < raw_node->parent->keys_num + 1; i++) {
			if (((Internal_Node*)raw_node->parent)->children[i] == raw_node) {
				((Internal_Node*)raw_node->parent)->children.erase(((Internal_Node*)raw_node->parent)->children.begin() + i);
				raw_node->parent->keys.erase(raw_node->parent->keys.begin() + i - 1);
				raw_node->parent->keys_num--;
				break;
			}
		}

		delete(raw_node);
		// raw_node->parent->keys.erase

		// for(int i=0; i < raw_node->keys_num; i++){
		// 	((Leaf_Node*)left_node)->keys.push_back(((Leaf_Node*)raw_node)->keys[i]);
		// 	((Leaf_Node*)left_node)->values.push_back(((Leaf_Node*)raw_node)->values[i]);
		// 	((Leaf_Node*)left_node)->keys_num++;
		// }
	}
	else {

	}
}

void BPlusTree::merge_with_right(BPT_Node* raw_node, BPT_Node* right_node) {
	if (raw_node->is_leaf) {
		raw_node->keys.insert(raw_node->keys.end(), right_node->keys.begin(), right_node->keys.end());
		((Leaf_Node*)raw_node)->values.insert(((Leaf_Node*)raw_node)->values.end(), ((Leaf_Node*)right_node)->values.begin(), ((Leaf_Node*)right_node)->values.end());

		raw_node->keys_num += right_node->keys_num;

		((Leaf_Node*)raw_node)->next = ((Leaf_Node*)right_node)->next;
		if (((Leaf_Node*)raw_node)->next != NULL) {
			((Leaf_Node*)right_node)->next->pre = (Leaf_Node*)raw_node;
		}

		for (int i = 0; i < right_node->parent->keys_num + 1; i++) {
			if (((Internal_Node*)right_node->parent)->children[i] == right_node) {
				((Internal_Node*)right_node->parent)->children.erase(((Internal_Node*)right_node->parent)->children.begin() + i);
				right_node->parent->keys.erase(right_node->parent->keys.begin() + i - 1);
				right_node->parent->keys_num--;
				break;
			}
		}


		delete(right_node);

	}
	else {

	}
}



pair<BPT_Node*, BPT_Node*> find_brothers(BPT_Node* raw_node) {
	int child_num = -1;
	Internal_Node* parent = (Internal_Node*)raw_node->parent;
	for (int i = 0; i < parent->keys_num + 1; i++) {
		if (parent->children[i] == raw_node) {
			child_num = i;
			break;
		}
	}
	BPT_Node* left_bro = NULL;
	BPT_Node* right_bro = NULL;
	if (child_num - 1 >= 0) {
		left_bro = parent->children[child_num - 1];
	}
	if (child_num + 1 < parent->keys_num + 1) {
		right_bro = parent->children[child_num + 1];
	}
	return std::pair<BPT_Node*, BPT_Node*>(left_bro, right_bro);
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
		cout << "没找到" << endl;
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
				left_node->keys.insert(left_node->keys.end(), delete_pos->keys.begin(), delete_pos->keys.end());
				left_node->values.insert(left_node->values.begin(), ((Leaf_Node*)delete_pos)->values.begin(), ((Leaf_Node*)delete_pos)->values.end());

				left_node->next = ((Leaf_Node*)delete_pos)->next;
				if (left_node->next != NULL) {
					left_node->next->pre = left_node;
				}
				left_node->keys_num += delete_pos->keys_num;

				delete_pos->parent->keys.erase(delete_pos->parent->keys.begin() + child_num - 1);
				((Internal_Node*)delete_pos->parent)->children.erase(((Internal_Node*)delete_pos->parent)->children.begin() + child_num);
				delete_pos->parent->keys_num--;
				delete_pos = left_node;




			}
			else if (right_node != NULL && right_node->keys_num < this->min_order) {
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
			}
			Internal_Node* del_inter_pos = (Internal_Node*)delete_pos;
			
			// 如果把root节点也清空了
			while (del_inter_pos->parent != this->bpt_root) {
				Internal_Node *left_inter_node = NULL;
				Internal_Node *right_inter_node = NULL;
				del_inter_pos = (Internal_Node*)del_inter_pos->parent;

				if (del_inter_pos->keys_num >= this->min_order - 1) {
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
						del_inter_pos = left_inter_node;


					}
					else if (right_inter_node != NULL && right_inter_node->keys_num < this->min_order) {
						right_inter_node->children.insert(right_inter_node->children.begin(), del_inter_pos->children.begin(), del_inter_pos->children.end());
						right_inter_node->keys.insert(right_inter_node->keys.begin(), del_inter_pos->parent->keys[child_inter_num]);
						right_inter_node->keys.insert(right_inter_node->keys.begin(), del_inter_pos->keys.begin(), del_inter_pos->keys.end());
						right_inter_node->keys_num += (del_inter_pos->keys_num + 1);
						for (int i = 0; i < del_inter_pos->keys_num + 1; i++) {
							del_inter_pos->children[i]->parent = right_inter_node;
						}

						del_inter_pos->parent->keys.erase(del_inter_pos->parent->keys.begin() + child_inter_num);
						((Internal_Node*)del_inter_pos->parent)->children.erase(((Internal_Node*)del_inter_pos->parent)->children.begin() + child_inter_num);
						del_inter_pos->parent->keys_num--;
						del_inter_pos = right_inter_node;
					}

				}


			}
			if (del_inter_pos->parent == this->bpt_root && this->bpt_root->keys_num==0) {
				del_inter_pos->parent = NULL;
				this->bpt_root = del_inter_pos;
				return;
			}

		}

		


	}



}


void BPlusTree::delete_data(int key) {
	Leaf_Node* delete_pos = find_node_ptr(key);
	bool is_exist = false;
	for (auto it = delete_pos->keys.begin(); it != delete_pos->keys.end(); it++) {
		if (*it == key) {
			is_exist = true;
		}
	}
	if (!is_exist) {
		cout << "没找到" << endl;
		return;
	}
	delete_directly(delete_pos, key);
	// 这里删了后   有可能，已经是空节点，

	if (delete_pos->keys_num >= min_order - 1) {
		return;
	}
	// 先处理叶子节点

	pair<BPT_Node*, BPT_Node*> node_brothers = find_brothers(delete_pos);
	Leaf_Node* left_node = (Leaf_Node*)node_brothers.first;
	Leaf_Node* right_node = (Leaf_Node*)node_brothers.second;
	if (left_node != NULL && left_node->keys_num >= this->min_order) {
		borrow_from_left(delete_pos, delete_pos->pre);
	}
	else if (right_node != NULL && right_node->keys_num >= this->min_order) {
		borrow_from_right(delete_pos, delete_pos->next);
	}
	else {
		if (left_node != NULL && left_node->keys_num < this->min_order) {
			merge_with_left(delete_pos, left_node);
		}
		else if (right_node != NULL && right_node->keys_num < this->min_order) {
			merge_with_right(delete_pos, right_node);
		}
	}
}

void BPlusTree::print_tree() {
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
			if(!que.front()->is_leaf)
				que.push(NULL);
			continue;
		}

		if (head->is_leaf == false) {
			bool flag = true;
			for (auto it = head->keys.begin(); it != head->keys.end(); it++) {
				flag = false;
				cout <<head->keys_num<<':'<< *it << ' ';
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

void BPlusTree::print_leaves() {
	Leaf_Node* p = this->leaves_head;
	while (p != NULL) {
		for (int i = 0; i < p->keys_num; i++) {
			cout << p->keys[i] << ":" << p->values[i] << "  ";
		}
		cout << " | " ;
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
		cout << "error tree is empty" << endl;
		return;
	}

	ofstream outfile;
	outfile.open(filename, ios::out | ios::trunc);
	if (!outfile.is_open()) {
		cout << "error" << endl;
		return;
	}
	BPT_Node* node_ptr = this->bpt_root;
	outfile << this->bpt_order << ' ' << this->min_order << endl;


	queue<BPT_Node* > node_que;
	node_que.push(node_ptr);
	while (!node_que.empty()) {

		node_ptr = node_que.front();
		node_que.pop();
		if (node_ptr->is_leaf) {
			outfile << 1 << ' ' << node_ptr->keys_num << ' ';
			for (int i = 0; i < node_ptr->keys_num; i++) {
				outfile << node_ptr->keys[i] << ' ' << ((Leaf_Node*)node_ptr)->values[i] << ' ';
			}
			outfile << endl;
		}
		else {
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
		cout << "error, please destory current tree, then try again" << endl;
		// 只有空时才能建立新的树
		return;
	}
	
	
	ifstream inputfile;
	inputfile.open(filename, ios::in);
	if (!inputfile.is_open()) {
		cout << "open file error" << endl;
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
	while (getline(inputfile, line)) {
		//由于保存b+树时，节点按行在文件中保存，因此读取时按行进行读取
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

BPlusTree::~BPlusTree() {
	delete this->bpt_root;
}