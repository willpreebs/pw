
default:
	g++ -g -o pw main.cpp init.cpp encrypt.cpp login.cpp logged-in.cpp util.cpp -lssl -lcrypto -I./boost_1_86_0
