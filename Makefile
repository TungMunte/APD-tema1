build:
	gcc tema1.c -o tema1 -lpthread -lm

test:
	gcc tema1.c -o tema1 -lpthread -Wall -O0 -g3 -DDEBUG -lm
	gcc demo.c -o demo -lpthread -Wall -O0 -g3 -DDEBUG -lm

clean: 
	rm -rf tema1 out*.txt demo