all: 
	g++ -std=c++11 -I include/ src/model/DB.cpp src/model/log.cpp src/run.cpp src/control/logic.cpp src/model/redis.cpp src/connect/connection.cpp -lmysqlclient -lpthread -lhiredis -o run

clean:
	rm *.o run ./log/*
