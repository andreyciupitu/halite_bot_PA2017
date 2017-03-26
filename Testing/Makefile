CC=g++
CFLAGS=-g -Wall -std=c++11 -c
LFLAGS=-lm

SRCDIR=src
OBJDIR=obj

SOURCES=$(wildcard $(SRCDIR)/*.cpp)
INCLUDES=$(wildcard $(SRCDIR)/*.hpp)
OBJECTS=$(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

EXECUTABLE="202.exe"

build: $(EXECUTABLE)

run: $(EXECUTABLE)
	./halite.exe -d "50 50" -s 42 $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INCLUDES)
	$(CC) $(CFLAGS) $< -o $@
	@echo "Compiled $< successfully!"

clean:
	rm -f ${EXECUTABLE} ${OBJECTS} *.d *.hlt output_log.txt

.PHONY: all clean
