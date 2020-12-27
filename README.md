# claditor

Modal text editor.

## Getting Started

### Dependencies

*   ncurses
*   cxxopts

### Building

```shell
$ git clone --recursive https://github.com/claby2/claditor.git
$ cd claditor
$ mkdir build
$ cd build
$ cmake ..
$ make clad
```

## Commands

Pressing `:` in normal mode will set the mode to command mode.

Basic commands:

*   `q`: Quit buffer
*   `w`: Write file

## Configuration

### Runtime Configuration

`claditor` will search `$HOME/.config/claditor/cladrc` for a runtime configuration where each line in a runtime configuration file will be interpreted as a command.

### Colorscheme

`claditor` will search for color schemes in `$HOME/.config/claditor/colors/`.
Any valid color scheme files here will be parsed and available to use from the editor.

Example color scheme file `colorscheme.clad`:

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
