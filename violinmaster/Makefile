CC=gcc
CPP=g++
CFLAGS=-g -I.
DEPS = 
OBJ = master.o

%.o: %.cpp $(DEPS)
	$(CPP) -c -o $@ $< $(CFLAGS)

master: $(OBJ)
	$(CPP) -o $@ $^ $(CFLAGS)

clean: $(OBJ)
	rm $<

