all: mainServer.cpp client.cpp moniter.cpp dbServer.cpp calcServer.cpp
	@g++ -o main_Server mainServer.cpp
	@g++ -o client client.cpp
	@g++ -o moniter_ moniter.cpp
	@g++ -o db_Server dbServer.cpp
	@g++ -o calc_Server calcServer.cpp

mainServer: main_Server
	@./main_Server

moniter: moniter_
	@./moniter_

dbServer: db_Server
	@./db_Server

calcServer: calc_Server
	@./calc_Server

clean:
	@rm -rf main_Server client moniter_ db_Server calc_Server
