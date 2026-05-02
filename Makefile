battle.o: battle.cpp battle.h items.h skills.h types.h monster.h
	g++ -c battle.cpp

items.o: items.cpp items.h types.h
	g++ -c items.cpp

map.o: map.cpp map.h types.h
	g++ -c map.cpp

monster.o: monster.cpp monster.h types.h
	g++ -c monster.cpp

skills.o: skills.cpp skills.h types.h player.h
	g++ -c skills.cpp

savesystem.o: savesystem.cpp savesystem.h map.h player.h
	g++ -c savesystem.cpp

player.o: player.cpp player.h skills.h items.h
	g++ -c player.cpp

main_final.o: main_final.cpp battle.h map.h player.h savesystem.h items.h types.h
	g++ -c main_final.cpp

showdata.o: showdata.cpp showdata.h
	g++ -c showdata.cpp

types.o: types.cpp types.h
	g++ -c types.cpp	

main_final: main_final.o battle.o map.o monster.o skills.o savesystem.o player.o items.o types.o showdata.o
	g++ main_final.o battle.o map.o monster.o skills.o savesystem.o player.o items.o types.o showdata.o -o main_final
