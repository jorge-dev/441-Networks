
CPPFLAGS= -O2
#CPPFLAGS=-ansi -pedantic-errors -Wall -Wconversion -std=c++17
#GTFLAGS=-lgtest -lgtest_main





mainserver: mainserver.o 
	g++ $(CPPFLAGS) -o mainserver mainserver.o

mainclient: mainclient.o
	g++ $(CPPFLAGS) -o mainclient mainclient.o

identity: identity.o
	g++ $(CPPFLAGS) -o identity identity.o 

reverse: reverse.o
	g++ $(CPPFLAGS) -o reverse reverse.o 

upper: upper.o
	g++ $(CPPFLAGS) -o upper upper.o 

lower: lower.o
	g++ $(CPPFLAGS) -o lower lower.o 

ceasar: ceasar.o
	g++ $(CPPFLAGS) -o ceasar ceasar.o 

yours: yours.o
	g++ $(CPPFLAGS) -o yours yours.o 


mainserver.o: mainserver.cpp 
	g++ $(CPPFLAGS) -c mainserver.cpp 

mainclient.o: mainclient.cpp 
	g++ $(CPPFLAGS) -c mainclient.cpp 

identity.o: identity.cpp 
	g++ $(CPPFLAGS) -c identity.cpp 

reverse.o: reverse.cpp 
	g++ $(CPPFLAGS) -c reverse.cpp 

upper.o: upper.cpp 
	g++ $(CPPFLAGS) -c upper.cpp 

lower.o: lower.cpp 
	g++ $(CPPFLAGS) -c lower.cpp 

ceasar.o: ceasar.cpp 
	g++ $(CPPFLAGS) -c ceasar.cpp 

yours.o: yours.cpp 
	g++ $(CPPFLAGS) -c yours.cpp 




clean:
	/bin/rm -f  *# *~ *.o 

main:	mainclient mainserver clean

micro: identity reverse upper lower ceasar yours clean

all: main micro









