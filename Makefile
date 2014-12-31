build : allocator.c
	gcc -Wall allocator.c -o allocator -lm
clean : allocator
	rm -f allocator
