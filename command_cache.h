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
	int buffer_size; // ��Դ�ź���
	//MyCommand* buffer; // ����ʹ��queue
	queue<MyCommand> TaskBuffer;

	std::mutex produced_mtx; // ������֮�以��
	std::mutex consumed_mtx; // ������֮�以��
	std::mutex pv_mtx; // ������������֮�以��
	std::condition_variable buffer_not_full; // �������Ƿ���
	std::condition_variable buffer_not_empty; // �������Ƿ��
};


#endif // command_cache_h

