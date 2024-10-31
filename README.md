# Life Timer

This is a simple desktop application written in C++ using GTK+ 3.0 that displays the current time and a countdown timer until a specific date (22nd March 2089). The application is designed to remain on the desktop, making it easy to keep track of time while working.

## Features

- Displays the current time in hours, minutes, and seconds.
- Counts down to a specified date (22nd March 2089).

## Requirements

To compile and run this application, you need:

- A Linux environment (the program is designed for Linux).
- `g++` (GNU C++ Compiler).
- `GTK+ 3.0` development libraries.
- `libX11` development libraries.
- `meson` build system.

### Installing Dependencies

You can install the required libraries on Ubuntu/Debian-based systems using the following command:

```bash
sudo apt-get install build-essential libgtk-3-dev libx11-dev
```

### Building

This application uses [Meson](https://mesonbuild.com) as its' build system.

```bash
meson build
cd build
meson compile
```

### Running

You must run the application with `GDK_BACKEND=x11` in your environment:

```bash
GDK_BACKEND=x11 ./time
```
