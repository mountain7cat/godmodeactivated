TARGET      := game
CC		    := gcc
CFLAGS      := -std=c99 -g -Wall
OBJDIR      := obj
SRCFILES    := $(wildcard *.c)
HEADERS     := $(wildcard *.h)
OBJFILES    := $(SRCFILES:%.c=$(OBJDIR)/%.o)
# the vars below are for building with our provided game world
GAMEDIR := ./special_gamefiles
GAMEOBJFILES := $(filter-out $(OBJDIR)/puzzles.o,$(OBJFILES)) $(GAMEDIR)/puzzles.o

$(TARGET): $(OBJFILES)
	$(CC) $+ $(CFLAGS) -o $(TARGET)

$(OBJDIR)/%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

playthegame: $(GAMEOBJFILES)
	$(CC) $+ $(CFLAGS) -o $(TARGET)
	./$(TARGET) $(GAMEDIR)/cs61c-world.lvl

debug: $(TARGET)
	gdb $(TARGET)

clean:
	rm -f $(OBJDIR)/*.o $(TARGET)

print:
	@echo "SRCFILES: " $(SRCFILES)
	@echo "OBJFILES: " $(OBJFILES)
	@echo "CFLAGS:   " $(CFLAGS)

edit:
	vim -p *.h *.c

loc:
	wc *.c *.h
