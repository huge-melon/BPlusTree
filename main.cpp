//# include "b_plus_tree.h"
//# include <iostream>
//# include <cstdlib>
//using namespace std;
//
//#define M 100
//
//int main(void)
//{
//	cout << 1 << endl;
//	BPlusTree t1(8);
//	cout << 2 << endl;
//
//	for (int i = M; i>=0; i--) {
//		t1.insert_data(i*10, i * 11);
//
//
//		cout << "Insert: " << i << endl;
//
//	}
//
//	t1.insert_data(16, 11);
//	t1.insert_data(17,  11);
//	t1.insert_data(20, 11);
//	t1.insert_data(22, 11);
//	t1.insert_data(23, 11);
//	t1.insert_data(25, 11);
//	t1.insert_data(26, 11); t1.insert_data(19, 11);
//	t1.insert_data(36, 11);
//	t1.insert_data(37, 11);
//	t1.insert_data(30, 11);
//	t1.insert_data(32, 11);
//	t1.insert_data(33, 11);
//	t1.insert_data(35, 11);
//	t1.insert_data(36, 11); t1.insert_data(39, 11);
//	t1.insert_data(42, 11);
//	t1.insert_data(43, 11);
//	t1.insert_data(45, 11);
//	t1.insert_data(46, 11); 
//	t1.insert_data(49, 11);
//	t1.print_tree();
//
//	for (int i = 1; i < M; i++) {
//		cout << t1.search_data(i) << endl;
//
//	}
//
//	t1.save_bpt("D:/tree.txt");
//
//
//	for (int i = 1; i < M; i++) {
//		t1.remove_data(i);
//		cout << "delete ：" << i << endl;
//	}
//
//	t1.print_tree();
//
//	for (int i = 1; i < M; i++) {
//		cout << t1.search_data(i) << endl;
//
//	}
//
//	BPlusTree t1;
//	t1.read_bpt("D:/tree.txt");
//	t1.print_tree();
//	system("pause");
//	return 0;
//}


// 需要修改输入输出，各个操作的输出
// 测试的时候隔一段事件记录以下算了

// 先插入1亿条数据进去，返回运行事件，然后分别测试各个功能的正确性

// 开启多线程，先插入n条数据，之后再开始多线程

// 定时启动一个线程打印所有的数据信息？

// #include <iostream>
// #include <thread>
// // #include <unistd.h>
// // #include "command_cache.h"
// int main()
// {
// 	ComBuffer cb(100, 5);

// 	std::thread producer1(&ComBuffer::ProducerTask, std::ref(cb));
// 	std::thread producer2(&ComBuffer::ProducerTask, std::ref(cb));
// 	std::thread producer3(&ComBuffer::ProducerTask, std::ref(cb));
// 	std::thread producer4(&ComBuffer::ProducerTask, std::ref(cb));
// 	std::thread producer5(&ComBuffer::ProducerTask, std::ref(cb));
// 	std::thread producer6(&ComBuffer::ProducerTask, std::ref(cb));

// 	std::thread consumer1(&ComBuffer::ConsumerTask, std::ref(cb));
// 	std::thread consumer2(&ComBuffer::ConsumerTask, std::ref(cb));
// 	std::thread consumer3(&ComBuffer::ConsumerTask, std::ref(cb));
// 	std::thread consumer4(&ComBuffer::ConsumerTask, std::ref(cb));

// 	producer1.join();
// 	producer2.join();
// 	producer3.join();
// 	producer4.join();
// 	producer5.join();
// 	producer6.join();

// 	consumer1.join();
// 	consumer2.join();
// 	consumer3.join();
// 	consumer4.join();


// }
