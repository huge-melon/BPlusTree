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
//		cout << "delete £º" << i << endl;
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


#include <iostream>
#include <thread>
#include <Windows.h>
#include <mutex>

using namespace std;

std::mutex mtx;           // mutex for critical section

void print_block(int n, char c) {
	// critical section (exclusive access to std::cout signaled by lifetime of lck):
	std::unique_lock<std::mutex> lck(mtx);
	for (int i = 0; i < n; ++i) {
		std::cout << c;
	}
	std::cout << '\n';
}

int main()
{
	std::thread th1(print_block, 50, '*');
	std::thread th2(print_block, 50, '$');

	th1.join();
	th2.join();
	system("pause");
	return 0;
}