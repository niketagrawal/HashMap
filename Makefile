compile: hashmap.c main.c
	gcc -o hashmap hashmap.c main.c -I.

test: compile
	./hashmap

clean:
	rm -f ./hashmap *.o 