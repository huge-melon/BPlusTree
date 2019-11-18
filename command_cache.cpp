#include "command_cache.h"
#include <ctime>
#include <Windows.h>
#include <sstream>

#define MAX_RANDNUM 9223372036854775807
using namespace std;


ComBuffer::ComBuffer(int buffer_size, int bpt_order)
{
	this->buffer_size = buffer_size;
	this->bpt_tree = new BPlusTree(bpt_order);

}

ComBuffer::~ComBuffer()
{

}


void ComBuffer::add_command(MyCommand com) {
	
	unique_lock<mutex> pvlock(this->pv_mtx); // ��û�����
	while ((this->TaskBuffer).size() == this->buffer_size) {
		// �����������
		cout << "$$Task producer is waiting for an empty slot��##" << endl;
		(this->buffer_not_full).wait(pvlock);
	}

	TaskBuffer.push(com);
	(this->buffer_not_empty).notify_all();
	// ��������Ϊ�գ����ѱ��������������߳�
	//pvlock.unlock();

}

void ComBuffer::take_command() {
	unique_lock<mutex> pvlock(this->pv_mtx); // ��û�����
	while ((this->TaskBuffer).size() == 0) {
		cout << "$$Task take_command is waiting for commands.##" << endl;
		(this->buffer_not_empty).wait(pvlock);
	}

	MyCommand com = (this->TaskBuffer).front();
	(this->TaskBuffer).pop();

	if (com.operation == "insert") {
		this->bpt_tree->insert_data(com.key, com.value);
	}
	else if (com.operation == "remove") {
		this->bpt_tree->remove_data(com.key);
	}
	else if (com.operation == "update") {
		this->bpt_tree->update_data(com.key, com.value);
	}
	else if (com.operation == "search") {
		this->bpt_tree->search_data(com.key);

	}
	else {
		cout << "$$Invalid command##" << endl;
	}


	std::cout << "$$Consumer thread " << std::this_thread::get_id()
		<< " is taking command. ##" << std::endl;
	(this->buffer_not_full).notify_all();
	//pvlock.unlock();
}

void ComBuffer::ProducerTask() {
	std::ostringstream oss;
	oss << std::this_thread::get_id();
	unsigned long long tid = stoull(oss.str());
	srand((unsigned)time(NULL) - (tid << 4));

	while (1) {
		Sleep(1);
		unique_lock<mutex> product_lock(this->produced_mtx);
		MyCommand com;
		
		// ����1-4 �ĸ����������Ӧ��ͬ�Ĳ���

		//cout << "$$time:" << (unsigned)time(NULL) <<"##"<< endl;
		switch (rand()%5)
		{
		case 0: 
		{
			com.operation = "insert";
			com.key = ((long long)rand() << 0x30) | ((long long)rand() << 0x21) | ((long long)rand() << 0x12) | ((long long)rand() << 0x03) | ((long long)rand() >> 0x0C);
			com.value = ((long long)rand() << 0x30) | ((long long)rand() << 0x21) | ((long long)rand() << 0x12) | ((long long)rand() << 0x03) | ((long long)rand() >> 0x0C);
			//com.key = ((double)rand())/RAND_MAX * MAX_RANDNUM;
			//com.value = ((double)rand()) / RAND_MAX * MAX_RANDNUM;
			if (com.key < 0 || com.value <0)
			cout << endl << endl << "############################"<< "insert: " << com.key << ' ' << com.value << endl << "############################" << endl << endl;
		}
			break;
		case 1: 
		{
			com.operation = "remove";
			com.key = ((long long)rand() << 0x30) | ((long long)rand() << 0x21) | ((long long)rand() << 0x12) | ((long long)rand() << 0x03) | ((long long)rand() >> 0x0C);
		}
			break;
		case 2:
		{
			com.operation = "search";
			com.key = ((long long)rand() << 0x30) | ((long long)rand() << 0x21) | ((long long)rand() << 0x12) | ((long long)rand() << 0x03) | ((long long)rand() >> 0x0C);
		}
			break;
		case 3:
		{
			com.operation = "update";
			com.key = ((long long)rand() << 0x30) | ((long long)rand() << 0x21) | ((long long)rand() << 0x12) | ((long long)rand() << 0x03) | ((long long)rand() >> 0x0C);
			com.value = ((long long)rand() << 0x30) | ((long long)rand() << 0x21) | ((long long)rand() << 0x12) | ((long long)rand() << 0x03) | ((long long)rand() >> 0x0C);
		}
			break;
		default: 
		{
			com.operation = "insert";
			com.key = ((long long)rand() << 0x30) | ((long long)rand() << 0x21) | ((long long)rand() << 0x12) | ((long long)rand() << 0x03) | ((long long)rand() >> 0x0C);
			com.value = ((long long)rand() << 0x30) | ((long long)rand() << 0x21) | ((long long)rand() << 0x12) | ((long long)rand() << 0x03) | ((long long)rand() >> 0x0C);
		}
			break;
		}
		add_command(com);
		//std::cout << "$$Producer thread " << std::this_thread::get_id()
		//	<< " is producing the item##" << std::endl;
		// ִ��ʲô������ӡ����
		//product_lock.unlock();
	}

}
void ComBuffer::ConsumerTask() {
	while (1) {
		Sleep(1);

		unique_lock<mutex> consumer_lock(this->consumed_mtx);
		take_command();
		// ����ִ�е� ����� ��Ҫ��B+������ӽ������
		//std::cout << "$$Consumer thread " << std::this_thread::get_id()
		//	<< " is consuming the item##" << std::endl;
		//consumed_mtx.unlock();
	}


}

