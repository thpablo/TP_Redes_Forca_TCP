all:
	# @g++ ChatClient.cpp data.h -o ChatClient -lpthread -g -Wall
	@g++ server.cpp chat.cpp data.h -o server -lpthread -g -Wall

