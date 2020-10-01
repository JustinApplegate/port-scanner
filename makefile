all:
	g++ -pthread -o PortScanner PortScanner.cpp
	./PortScanner $(arguments)
clean:
	rm PortScanner