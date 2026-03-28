map-visual.o: map-visual.cpp map-visual.h
	g++ -c map-visual.cpp

gamemap.o: gamemap.cpp gamemap.h
	g++ -c gamemap.cpp

main.o: main.cpp gamemap.h map-visual.h
	g++ -c main.cpp

main: main.o gamemap.o map-visual.o
	g++ main.o gamemap.o map-visual.o -o main
