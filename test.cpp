#include <iostream>
#include <unistd.h>
#include <thread>
#include <ctime>
#include "b_plus_tree.h"

using namespace std;
#define BUF_SIZE 100
#define ORDER 8
int main()
{
	srand((unsigned int)time(NULL));
	BPlusTree bpt(ORDER,BUF_SIZE);
	cout << "B+树初始化：序数 "<<ORDER<<" 任务缓冲区大小 "<<BUF_SIZE<<endl;
	int data_size = 1e8;
	cout << "开始插入1亿条数据"<<endl;
	for(int i=0;i<data_size;i++){
		bpt.insert_data(rand()/10, rand()/10);
	}
	cout <<"1亿条数据插入完成"<<endl << "开始多线程执行："<<endl;
	
	std::thread productor_threads[BUF_SIZE];
	std::thread consumer_threads[BUF_SIZE];
	srand((unsigned int)time(NULL));

	while (1) {

		// for(int i=0;i<5;i++){
		// 	productor_threads[i] = std::thread(&BPlusTree::multi_insert, std::ref(bpt), rand()/10, rand()/10);
		// }
		// for(int i=5;i<10;i++){
		// 	productor_threads[i] = std::thread(&BPlusTree::multi_remove, std::ref(bpt), rand()/10);
		// }
		// productor_threads[10] = std::thread(&BPlusTree::multi_search, std::ref(bpt), rand()/10);
		// productor_threads[11] = std::thread(&BPlusTree::multi_update, std::ref(bpt), rand()/10, rand()/10);
		// for(int i=0;i<12;i++){
		// 	consumer_threads[i]=std::thread(&BPlusTree::consume_task, std::ref(bpt));
		// }

		// for(int i=0;i<11;i++){
		// 	productor_threads[i].join();
		// 	consumer_threads[i].join();
		// }
		int threads_half_num = rand()%(BUF_SIZE/4*3); // 随机生成生产者的数量
		for(int i=0;i<threads_half_num;i++){ // 各个操作之间的比例   插入：删除：查找：更新 = 4:4:1:1
			switch (rand()%10)
			{
			case 0:
			case 1:
			case 2:
			case 3:
				productor_threads[i] = std::thread(&BPlusTree::multi_insert, std::ref(bpt), rand()/10, rand()/10);
				break;
			case 4:
			case 5:
			case 6:
			case 7:
				productor_threads[i] = std::thread(&BPlusTree::multi_remove, std::ref(bpt), rand()/10);
				break;
			case 8:
				productor_threads[i] = std::thread(&BPlusTree::multi_search, std::ref(bpt), rand()/10);
				break;
			case 9:
				productor_threads[i] = std::thread(&BPlusTree::multi_update, std::ref(bpt), rand()/10, rand()/10);
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
		// sleep(1);
		usleep(50);
		
	}
	//bpt.print_tree();
	return 0;
}