CCFLAGS=-g -Wall
all:lsls

lsls:lsls.c
	gcc $(CCFLAGS) -o $@ $<

clean:
	rm lsls 
