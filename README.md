# RAHasher

RAHasher is a CLI utility for verifying ROM checksums [with hashing methods used by RetroAchievements](https://docs.retroachievements.org/developer-docs/game-identification.html).

_(It's a copy of the same utility provided by [RALibretro](https://github.com/RetroAchievements/RALibretro), but with a bit more convenient CLI.)_

## Building RAHasher with MSYS2/Makefile

### Install MSYS2

1. Go to [http://www.msys2.org/](http://www.msys2.org/) and download the 32 bit version.
2. Follow the installation instructions on the site [http://www.msys2.org/](http://www.msys2.org/).
3. When on the prompt for the first time, run `pacman -Syu` (**NOTE**: at the end of this command it will ask you to close the terminal window **without** going back to the prompt.)
4. Launch the MSYS2 terminal again and run `pacman -Su`

### Install the toolchain

```
$ pacman -S make git zip mingw-w64-i686-gcc mingw-w64-i686-SDL2 mingw-w64-i686-gcc-libs
```

### Clone the repo

```
$ git clone --recursive --depth 1 https://github.com/LeXofLeviafan/RAHasher.git
```

### Build

```
$ cd RAHasher
$ make -f Makefile.RAHasher HAVE_CHD=1
```

**NOTE**: use `make` for a release build or `make DEBUG=1` for a debug build. Don't forget to run `make clean` first if switching between a release build and a debug build.

## Building RALibretro with Visual Studio

### Clone the repo

```
> git clone --recursive --depth 1 https://github.com/LeXofLeviafan/RAHasher.git
```

### Build

Load `RALibretro.sln` in Visual Studio and build it (specifically the `RAHasher` target).

## Command Line Arguments

Argument|Description
-|-
-v|(optional) enables verbose messages for debugging
-s systempath|(optional) specifies where supplementary files are stored (typically a path to RetroArch/system)
system|specifies the system key or id associated to the game (which hash algorithm to use)
filepath|specifies the path to the game file (file may include wildcards, path may not)

I.e., in order to find out what checksum RA would assign to a NES game file, you can invoke the program like this:
```bat
RAHasher.exe NES "C:\ROMS\NES\Alwa's Awakening 8-bit edition.nes"
```
You can also pass `?` as the system key; in which case RAHasher will attempt to detect the system based on ROM file extension. (Note: equivalent "system ID" number is `91`.)

Additionally, you can pass multiple filenames and/or specify it a glob template (with `*`/`?` wildcards). Note that wildcards are only allowed in _filename itself_ (not in the path), and that system detection is not allowed in multiple files mode.

Finally, the full list of valid console keys/IDs will be printed along with usage info if you run RAHasher without arguments:
```bat
RAHasher.exe
```
The list ordering matches RetroAchievements website menu. (Also, system keys match short names from [RetroAchievents game lists](https://retroachievements.org/games), sans whitespace.)
