CC=g++
CFLAGS=-g -Wall -std=c++11 -c
LFLAGS=-lm

SRCDIR=src
OBJDIR=obj

SOURCES=$(wildcard $(SRCDIR)/*.cpp)
INCLUDES=$(wildcard $(SRCDIR)/*.hpp)
OBJECTS=$(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

EXECUTABLE="./202.exe"

build: $(EXECUTABLE)

run: $(EXECUTABLE)
	$(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(OBJECTS): $(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INCLUDES)
	@mkdir -p obj/
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f ${EXECUTABLE} ${OBJECTS} *.d *.hlt output_log.txt

.PHONY: all clean
