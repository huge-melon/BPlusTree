bplustree: main.o b_plus_tree.o command_cache.o
	g++ -o bplustree main.o b_plus_tree.o command_cache.o -lpthread

main.o: main.cpp command_cache.h
	g++ -c main.cpp -lpthread

b_plus_tree.o: b_plus_tree.cpp b_plus_tree.h
	g++ -c b_plus_tree.cpp

command_cache.o: command_cache.cpp command_cache.h
	g++ -c command_cache.cpp -lpthread

.PHONY:clean

clean:
	rm bplustree main.o b_plus_tree.o command_cache.o