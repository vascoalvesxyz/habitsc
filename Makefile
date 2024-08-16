# Usage:
# make        # compile all binary
# make clean  # remove ALL binaries and objects

.PHONY = clean all
SRCS := $(wildcard *.c)

all: install

install: 
	gcc -o habitsc -Wall -Wextra ${FLAGS} ${SRCS}
	mkdir -p "$$HOME/.local/bin"
	mkdir -p "$$HOME/.local/share/habits"
	cp -v habitsc "$$HOME/.local/bin"

clean:
	@echo "Cleaning up..."
	rm -rvf *.o
