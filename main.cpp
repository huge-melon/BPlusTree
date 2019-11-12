# include "b_plus_tree.h"
# include <iostream>
# include <cstdlib>
using namespace std;

#define M 100

int main(void)
{
	cout << 1 << endl;
	BPlusTree t1(8);
	cout << 2 << endl;

	for (int i = M; i>=0; i--) {
		t1.insert_data(i*10, i * 11);


		cout << "Insert: " << i << endl;

	}

	t1.insert_data(16, 11);
	t1.insert_data(17,  11);
	t1.insert_data(20, 11);
	t1.insert_data(22, 11);
	t1.insert_data(23, 11);
	t1.insert_data(25, 11);
	t1.insert_data(26, 11); t1.insert_data(19, 11);
	t1.insert_data(36, 11);
	t1.insert_data(37, 11);
	t1.insert_data(30, 11);
	t1.insert_data(32, 11);
	t1.insert_data(33, 11);
	t1.insert_data(35, 11);
	t1.insert_data(36, 11); t1.insert_data(39, 11);
	t1.insert_data(42, 11);
	t1.insert_data(43, 11);
	t1.insert_data(45, 11);
	t1.insert_data(46, 11); 
	t1.insert_data(49, 11);
	t1.print_tree();

	for (int i = 1; i < M; i++) {
		cout << t1.search_data(i) << endl;

	}


	for (int i = 1; i < M; i++) {
		t1.remove_data(i);
		cout << "delete ：" << i << endl;
	}

	t1.print_tree();

	for (int i = 1; i < M; i++) {
		cout << t1.search_data(i) << endl;

	}

	system("pause");
	return 0;
}


//#include <iostream>
//#include <thread>
//#include <Windows.h>
//#include <mutex>
//
//using namespace std;
//
//int tickets = 100;
//
//
//std::mutex mx;//线程互斥对象
//
//void myThreadA() {
//	while (true)
//	{
//		mx.lock();
//		if (tickets > 0) {
//			cout << "A sell " << tickets << endl;
//			tickets--;
//			mx.unlock();
//		}
//		else {
//			mx.unlock();
//			break;
//		}
//	}
//}
//
//void myThreadB() {
//	while (true)
//	{
//		mx.lock();
//		if (tickets > 0) {
//			cout << "B sell " << tickets << endl;
//			tickets--;
//			mx.unlock();
//		}
//		else {
//			mx.unlock();
//			break;
//		}
//	}
//}
//
//int main(void)
//{
//	thread tA(&myThreadA);
//	thread tB(&myThreadB);
//	tA.join();
//	tB.join();
//
//	cout << "main function finish." << endl;
//	system("pause");
//	return 0;
//}