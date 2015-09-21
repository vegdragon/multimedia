CC=gcc
CPP=g++
CFLAGS=-I.
DEPS = 
OBJ = master.o

%.o: %.cpp $(DEPS)
	$(CPP) -c -o $@ $< $(CFLAGS)

master: $(OBJ)
	$(CPP) -o $@ $^ $(CFLAGS)


