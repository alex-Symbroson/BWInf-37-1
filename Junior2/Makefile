
	# Project name
NAME = $(basename $(wildcard *.cpp))

	# program base name
BASE = base
DIR_BASE = ../base/

DIR_BUILD = build/

COMPILER = g++ -std=c++11
CPPFLAGS = -I. -Wall -fPIE #-O3
LDFLAGS = -pie

SRC = $(NAME).cpp $(DIR_BASE)$(BASE).cpp
OBJ = $(DIR_BUILD)$(BASE).o $(DIR_BUILD)$(NAME).o
OUTPUT = $(DIR_BUILD)$(NAME).out

.SECONDARY: $(OBJECTS_NORMAL)
.PHONY: all all_init clean run


all: all_init $(OUTPUT)
	@printf "Compilation done - No Errors.\n"

all_init:
	mkdir -p $(DIR_BUILD)

$(OUTPUT): $(OBJ)
	$(COMPILER) $(LDFLAGS) -o $@ $(OBJ)

$(DIR_BUILD)$(BASE).o: $(DIR_BASE)$(BASE).cpp
	$(COMPILER) $(CPPFLAGS) -c $< -o $@

$(DIR_BUILD)%.o: %.cpp
	$(COMPILER) $(CPPFLAGS) -c $< -o $@

run: all
	$(OUTPUT) $(ARGS)

clean:
	rm -rf build
