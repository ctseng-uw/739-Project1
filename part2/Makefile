ALL: server client

O3:
	g++  -O3 -DNDEBUG -o server server.cpp
	g++  -O3 -DNDEBUG -o client client.cpp
	
server: server.cpp comm.hpp
	g++  -o server server.cpp

client: client.cpp comm.hpp
	g++  -o client client.cpp

clean:
	rm -f server client