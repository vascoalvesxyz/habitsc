# Usage:
# make        # compile all binary
# make clean  # remove ALL binaries and objects

.PHONY = clean all
SRCS := $(wildcard *.c)

all: install

install: 
	gcc -o habitsc main.c h_draw.c h_files.c h_playlist.c h_habits.c
	mkdir -p "$$HOME/.local/bin"
	mkdir -p "$$HOME/.local/share/habits"
	cp -v habitsc "$$HOME/.local/bin"

clean:
	@echo "Cleaning up..."
	rm -rvf *.o
