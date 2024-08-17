# HABITS CLI

![example img](example.jpg)

Finally, a habit tracker that isn't bloated and that can be printed at terminal startup! 

## Install
```bash
git clone https://github.com/vascoalvesxyz/habitsc.git 
cd habitsc
make
```

**The binary is copied to ./local/bin, if you don't have this folder added to $PATH copy habitsc to /usr/bin !!!**.

### Configuration

config.h has some options you can change if you need to.

You need to run make again to apply the changes (duh).


## Example Use:

#### Shows last _n_ days for all habits. Defaults to 7. 
```
habitsc [n]
```

#### Show last 365 days for all habits. 
```
habitsc 365 | less
```

#### Adds/deletes habit _name_.
```
habitsc [add|del] [name]
```

#### Marks today as done in _name_.
```
habitsc mark [name]
```

#### Show the last _n_ days of habit _name_. Defaults to 7.
```
habitsc status [name] [n]
```

### Playlist Commands 
#### Create playlist _name_.
```
habitsc playlist -c [playlist]
```

#### Shows habits in _name_ playlist for last _n_ days.
```
habitsc playlist -s [playlist] [n]
```

#### Add/delete habit _name_ in _playlist_ for last _n_ days.
```
habitsc playlist -[a|d] [playlist] [name]
```

## Todo
- List playlists.
- Change habit order in playlist.
- Mark/unmark date at earlier day.
