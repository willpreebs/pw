
default:
	g++ -g -o pw src/main.cpp src/init.cpp src/encrypt.cpp src/login.cpp src/logged-in.cpp src/util.cpp src/fileIO.cpp src/userIO.cpp -lssl -lcrypto -I./boost_1_86_0
