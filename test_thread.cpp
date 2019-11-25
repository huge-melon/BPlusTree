
#include <iostream>
#include <unistd.h>
#include <thread>
#include <ctime>
#include <malloc.h>
#include "b_plus_tree.h"
using namespace std;
#define BUF_SIZE 100
#define ORDER 8
#define RANDNUM 20
int main()
{
	srand((unsigned int)time(NULL));
	BPlusTree bpt(ORDER,BUF_SIZE);
	cout << "B+树初始化：序数 "<<ORDER<<" 任务缓冲区大小 "<<BUF_SIZE<<endl;
	int data_size = 1e8;
	cout << "开始插入1亿条数据"<<endl;
	for(int i=0;i<data_size;i++){
		bpt.insert_data(rand()/RANDNUM, rand()/RANDNUM);
	}
	cout <<"1亿条数据插入完成"<<endl << "开始多线程执行："<<endl;
	std::thread productor_threads[BUF_SIZE];
	std::thread consumer_threads[BUF_SIZE];
	srand((unsigned int)time(NULL));
	while (1) {
		int threads_half_num = rand()%((BUF_SIZE>>3)*5); // 随机生成生产者的数量,线程数量可能超过缓冲区大小
		for(int i=0;i<threads_half_num;i++){ // 各个操作之间的比例   插入：删除：查找：更新 = 4:4:1:1
			switch (rand()%10)
			{
			case 0:
			case 1:
			case 2:
			case 3:
				productor_threads[i] = std::thread(&BPlusTree::multi_insert, std::ref(bpt), rand()/RANDNUM, rand()/RANDNUM);
				break;
			case 4:
			case 5:
			case 6:
			case 7:
				productor_threads[i] = std::thread(&BPlusTree::multi_remove, std::ref(bpt), rand()/RANDNUM);
				break;
			case 8:
				productor_threads[i] = std::thread(&BPlusTree::multi_search, std::ref(bpt), rand()/RANDNUM);
				break;
			case 9:
				productor_threads[i] = std::thread(&BPlusTree::multi_update, std::ref(bpt), rand()/RANDNUM, rand()/RANDNUM);
				break;
			}
		}
		for(int i=0;i<threads_half_num;i++){ // 创建消费者线程
			consumer_threads[i] = std::thread(&BPlusTree::consume_task, std::ref(bpt));
		}
		for(int i=0;i<threads_half_num;i++){
			productor_threads[i].detach();
			consumer_threads[i].detach();
		}
		malloc_trim(0);
		usleep(50);
	}
	return 0;
}