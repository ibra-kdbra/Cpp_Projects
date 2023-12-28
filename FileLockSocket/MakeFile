all: ufsend ufrec

ufsend: src/ufsend.cpp src/utility.cpp
	g++ -Wall src/ufsend.cpp src/utility.cpp -lcrypto -o ufsend

ufrec: src/ufrec.cpp src/utility.cpp
	g++ -Wall src/ufrec.cpp src/utility.cpp -lcrypto -o ufrec