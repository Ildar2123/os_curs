all:
	g++ -O2 -Wall -o client src/client.cpp 
	g++ -O2 -std=c++17 -Wall -o server src/server.cpp src/FileManager.cpp -lpthread -lstdc++fs
test:
	g++ -O2 -std=c++17 -Wall -o test_m ./test/main_test.cpp ./test/test.cpp ./src/FileManager.cpp -lpthread -lstdc++fs
.PHONY: test