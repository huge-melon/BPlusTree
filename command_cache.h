#ifdef command_cache_h
#define command_cache_h

#include "bpt_node.h"
#include <string>
#include <vector>
#include <queue>

typedef struct command
{
	string operation;
	int key;
	int value;
} Command;

class ComBuffer
{
public:
	ComBuffer();
	~ComBuffer();
	void add_command();
	void get_command();
private:
	int buffer_size; // 资源信号量
	Command* buffer; // 可以使用queue
	//queue<Command > buffer;

	//多个信号量
};

ComBuffer::ComBuffer()
{
}

ComBuffer::~ComBuffer()
{
}
#endif // command_cache_h

