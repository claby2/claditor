# claditor

Modal text editor.

## Getting Started

### Dependencies

- ncurses

### Building

```shell
$ git clone https://github.com/claby2/claditor.git
$ cd claditor
$ cmake .
$ make clad
```

## Commands

Pressing `:` in normal mode will set the mode to command mode.

Basic commands:

- `q`: Quit buffer
- `w`: Write file
- `colo[rscheme] <name>`: Set the colorscheme to <name>

## Configuration

### Runtime Configuration

`claditor` will search two locations for a runtime configuration:

- `$HOME/.cladrc`
- `$HOME/.config/claditor/.cladrc`

Each line in a runtime configuration file is interpreted as a command.

### Colorscheme

`claditor` will search for color schemes in `$HOME/.config/claditor/colors/`.
Any valid color scheme files here will be parsed and available to use from the editor.

Example color scheme file `colorscheme.clad`:

```
background = #101010
foreground = #d1d1d1
comment = #808080
accent = #353535
color1 = #FF0000
color2 = #00FF00
color3 = #FFFF00
color4 = #0000FF
color5 = #FF00FF
color6 = #00FFFF
```
