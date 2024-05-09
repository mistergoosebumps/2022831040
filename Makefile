all:
	g++ -I src/include -L src/lib -o Task_101 Task_101.cpp -lmingw32 -lSDL2main -lSDL2