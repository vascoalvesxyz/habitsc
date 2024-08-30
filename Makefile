CFLAGS   = -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Os
CC = cc

SRC = habits.c h_files.c

all: install clean

install:
	@[[ -d "$$HOME/.local/bin" ]] || mkdir -p "$$HOME/.local/bin"
	@[[ -d "$$HOME/.share/habits" ]] || mkdir -p "$$HOME/.local/share/habits"
	${CC} ${CFLAGS} -o habitsc ${SRC}
	cp -v habitsc "$$HOME/.local/bin"

clean:
	rm -rf *.o

.PHONY: all clean install 
