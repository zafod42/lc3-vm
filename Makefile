CC=gcc
CFLAGS=-c -Wall	
LDFLAGS=
SOURCES=lc3.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=lc3

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
