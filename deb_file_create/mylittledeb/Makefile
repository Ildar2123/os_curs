all:
	g++ -O2 -Wall -o client client.cpp 
	g++ -O2 -std=c++17 -Wall -o server server.cpp FileManager.cpp -lpthread -lstdc++fs
clear:
	rm client || true
	rm server || true