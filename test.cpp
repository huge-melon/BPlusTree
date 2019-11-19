#include <iostream>
#include <unistd.h>
#include <thread>
#include <ctime>
#include "b_plus_tree.h"

using namespace std;

int main()
{
	int order = 8;
	int buf_size =50;
	srand((unsigned int)time(NULL));
	BPlusTree bpt(order,buf_size);
	cout << "B+树初始化：序数 "<<order<<" 任务缓冲区大小 "<<buf_size<<endl;
	int data_size = 1e8;
	cout << "开始插入1亿条数据"<<endl;
	for(int i=0;i<data_size;i++){
		bpt.insert_data(rand(), rand());
	}
	cout <<"1亿条数据插入完成"<<endl << "开始多线程执行："<<endl;
	
	std::thread productor_threads[buf_size*2];
	std::thread consumer_threads[buf_size*2];

	while (1) {
		int productor_num = rand()%(buf_size); // 随机生成生产者的数量，其大小在缓冲区大小以内
		for(int i=0;i<productor_num;i++){ // 各个操作之间的比例   插入：删除：查找：更新 = 4:4:1:1
			switch (rand()%10)
			{
			case 0:
			case 1:
			case 2:
			case 3:
				productor_threads[i] = std::thread(&BPlusTree::multi_insert, std::ref(bpt), rand(), rand());
				break;
			case 4:
			case 5:
			case 6:
			case 7:
				productor_threads[i] = std::thread(&BPlusTree::multi_remove, std::ref(bpt), rand());
				break;
			case 8:
				productor_threads[i] = std::thread(&BPlusTree::multi_search, std::ref(bpt), rand());
				break;
			case 9:
				productor_threads[i] = std::thread(&BPlusTree::multi_update, std::ref(bpt), rand(), rand());
				break;
			}
		}

		int consumer_num = rand()%(buf_size); // 随机生成消费者的数量，其大小在缓冲区大小以内
		for(int i=0;i<consumer_num;i++){
			consumer_threads[i] = std::thread(&BPlusTree::consume_task, std::ref(bpt));
		}


		for(int i=0;i<productor_num;i++){
			productor_threads[i].detach();
		}
		for(int i=0;i<consumer_num;i++){
			consumer_threads[i].detach();
		}
		// sleep(1);
		usleep(100);
		
	}
	//bpt.print_tree();
	return 0;
}