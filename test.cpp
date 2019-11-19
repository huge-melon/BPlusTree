#include <stdio.h>
// #include <unistd.h>
#include <stdlib.h>
#include <thread>
#include "b_plus_tree.h"

using namespace std;

int main()
{
	BPlusTree bpt(8,50);
	int i = 1000;
	while (i--) {
		std::thread producer1(&BPlusTree::multi_insert, std::ref(bpt), rand(), rand());
		std::thread consumer1(&BPlusTree::consume_task, std::ref(bpt));

		producer1.detach();
		consumer1.detach();
		cout << "round: " << i << endl;
	}
	//bpt.print_tree();
	system("pause");
	return 0;
}