#include <iostream>
#include <unistd.h>
#include <thread>
#include <ctime>
#include <malloc.h>
#include "b_plus_tree.h"

using namespace std;
#define ORDER 4
int main()
{
	BPlusTree bpt(ORDER); // 初始化一个4阶B+树
	for(int i=0;i<10;i++){
		bpt.insert_data(i*5,rand());
	}
	bpt.print_tree();
	cout<< "*****************************************************************************"<<endl<<endl;
	for(int i=0;i<10;i++){
		bpt.insert_data(i*3,rand());
	}
	bpt.print_tree();
	cout<< "*****************************************************************************"<<endl<<endl;

	bpt.search_data(3);
	bpt.search_data(4);
	cout<< "*****************************************************************************"<<endl<<endl;

	bpt.update_data(3,333333);
	bpt.search_data(3);
	cout<< "*****************************************************************************"<<endl<<endl;

	bpt.save_bpt("/home/shixin/BPlusTree/tree_instance");
	for(int i=0;i<5;i++){
		bpt.remove_data(i*5);
		bpt.remove_data(i*3);
	}
	bpt.print_tree();
	cout<< "*****************************************************************************"<<endl<<endl;

	bpt.destory_bpt();

	bpt.print_tree();
	cout<< "*****************************************************************************"<<endl<<endl;

	bpt.read_bpt("/home/shixin/BPlusTree/tree_instance");
	bpt.print_tree();
	return 0;
}