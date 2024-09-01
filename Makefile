all:
	@g++ ChatClient.cpp data.h -o ChatClient -lpthread -g -Wall
	@g++ ChatServer.cpp data.h -o ChatServer -lpthread -g -Wall

