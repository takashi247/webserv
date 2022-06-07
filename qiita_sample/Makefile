CC = g++
HTTP1 = server
HTTP1_SOURCE = server.cpp socket.cpp http1header.cpp http1parser.cpp http1response.cpp

.PHONY: all clean

all: $(HTTP1)

$(HTTP1): $(HTTP1_SOURCE)
	g++ $(HTTP1_SOURCE) -o $(HTTP1) -g -std=c++11 -O0 -Wall


clean:
	rm -rf *.o $(HTTP1)
