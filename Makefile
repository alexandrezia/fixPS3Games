
all: fixPS3Games

fixPS3Games.o: fixPS3Games.c fixPS3Games.h
	gcc -c -g fixPS3Games.c
	
fixPS3Games: fixPS3Games.o 
	gcc -g -o fixPS3Games fixPS3Games.o

install: fixPS3Games
	install -s fixPS3Games /usr/local/bin
	
clean:
	\rm -f fixPS3Games fixPS3Games.o
	
