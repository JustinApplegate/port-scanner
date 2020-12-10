all:
	g++ -pthread -o scanner PortScanner.cpp
clean:
	rm PortScanner