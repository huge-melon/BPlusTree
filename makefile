test: test.o b_plus_tree.o
	g++ -o test test.o b_plus_tree.o -lpthread

test.o: test.cpp b_plus_tree.h
	g++ -c test.cpp -lpthread

b_plus_tree.o: b_plus_tree.cpp b_plus_tree.h
	g++ -c b_plus_tree.cpp


.PHONY:clean

clean:
	rm test test.o b_plus_tree.o