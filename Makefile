all: compile

dir:
	[[ -d "$$HOME/.local/bin" ]] || mkdir -p "$$HOME/.local/bin"
	[[ -d "$$HOME/.share/habits" ]] || mkdir -p "$$HOME/.local/share/habits"

compile:
	gcc -o habitsc main.c h_draw.c h_files.c h_playlist.c h_habits.c
	cp -v habitsc "$$HOME/.local/bin"

install: dir compile

clean:
	rm -rvf *.out
