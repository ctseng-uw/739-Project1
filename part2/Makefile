ALL: server client

server: server.cpp comm.hpp
	g++ -g -o server server.cpp

client: client.cpp comm.hpp
	g++ -g -o client client.cpp

tcp: tcp_server.cpp tcp_client.cpp
	g++ -g -o tcp_server tcp_server.cpp
	g++ -g -o tcp_client tcp_client.cpp

clean:
	rm -f server client