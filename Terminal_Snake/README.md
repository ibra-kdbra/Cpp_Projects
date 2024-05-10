# tsnake, a terminal snake game

`tsnake` is a terminal snake game written in C++ using the ncurses library with
support for maps and interactive resizing.

Move the snake using the arrow keys [`↑↓←→`], [`wasd`] or vi's [`hjkl`].
Here are some controls:

- [`p`] Pause the game
- [`r`] Restart with next map
- [`q`] Quit

The game starts with a speed of 2 m/s and every 20 points the speed
is increased until a maximum of 20 m/s.
If cheat mode is enabled (`--cheat`), the speed can be increased with [`+`] and decreased with [`-`].

The game has a few maps which can be chosen using the `-m` option.
Otherwise, maps are cycled automatically when the game is restarted.

## Requirements

- C++17
- ncurses

## Building

Build and install.

```sh
make && make install
```

You may need to run the `make install` as a superuser.

## Running

To run the software, just do:

```sh
tsnake
```

## Usage

Here is the info listed by the `--help` flag.

```sh
Usage: tsnake [OPTION]...

Options:
 -h  shows usage information
 -v  prints version and exit
 -n [SIZE] set inital size of snake, which defaults to 4
 -m [MAP_NUM] number of the first map to use as an integer, which is cycled
  using (map % nmaps)
 -c  deactivate colors
 -x, --cheat activate cheat mode where speed can be increased and
  decreased with '+' and '-'
```

## This code don't belong to me, I just modified it to my need

## Credits

[AUTHOR]
Toni Sagrista Selles <admin@tonisagrista.com>

[SEE ALSO]
Online repository: <https://gitlab.com/langurmonkey/tsnake>
