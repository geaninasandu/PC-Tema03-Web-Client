all: build

build:
	g++ client.cpp buffer.cpp commands.cpp helpers.cpp requests.cpp -o client

run: client
	./client

clean: client
	rm client
