#ifndef command_cache_h
#define command_cache_h

#include "b_plus_tree.h"
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

struct MyCommand
{
	string operation;
	long long key;
	long long value;
};

class ComBuffer
{
public:
	ComBuffer(int buffer_size, int bpt_order);
	~ComBuffer();
	void add_command(MyCommand com);
	void take_command();
	void ProducerTask();
	void ConsumerTask();
private:

	BPlusTree* bpt_tree;
	int buffer_size; // 资源信号量
	//MyCommand* buffer; // 可以使用queue
	queue<MyCommand> TaskBuffer;

	std::mutex produced_mtx; // 生产者之间互斥
	std::mutex consumed_mtx; // 消费者之间互斥
	std::mutex pv_mtx; // 生产者消费者之间互斥
	std::condition_variable buffer_not_full; // 缓存区是否满
	std::condition_variable buffer_not_empty; // 缓存区是否空
};


#endif // command_cache_h

