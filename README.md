<div align="center">
<h1>alienhook v4</h1>
Free as in freedom CS:GO cheat for GNU/Linux.<br>
Written in C99.<br>
Licensed under GPLv3 or later version.<br>
</div>

## Note to alienhook v3 users

Configs from ah3 can be used in ah4.
Most used features have already been ported.

## Installing dependencies

### Arch (Artix, Manjaro, etc.)

```sh
sudo pacman -S git make gcc cjson sdl2 glew
```

### Debian (Ubuntu, etc.)

```sh
sudo apt install git make gcc libcjson-dev libsdl2-dev libglew-dev
```

### Fedora

```sh
sudo dnf install glibc-devel cjson-devel sdl2-devel glew-devel
```

## Cloning

```sh
git clone --depth 1 https://github.com/deboogerxyz/ah4
```

## Building

```sh
cd ah4
make -j10
```

## Usage

```sh
sudo ./load.sh   # Reloads if cheat is already loaded
sudo ./unload.sh
sudo ./debug.sh  # load.sh, but leaves GDB attached
```

When loaded, open the GUI using the <kbd>HOME</kbd> key.

## Updating

```sh
git pull
make -j10
```

## License

> Copyright (c) 2022 debooger

Project is licensed under [GPLv3](https://www.gnu.org/licenses/gpl-3.0.en.html) or later version.

Includes projects licensed under other licenses. See `libs/*.LICENSE` files for more information.
