ALL: server client

server: server.cpp comm.hpp
	g++ -g -o server server.cpp

client: client.cpp comm.hpp
	g++ -g -o client client.cpp

clean:
	rm -f server client