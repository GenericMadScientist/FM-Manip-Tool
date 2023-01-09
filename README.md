# FM Manip Tool

A program to route RNG Manipulation in Yu-Gi-Oh! Forbidden Memories.

## Getting Started

You can get the latest release at the [Github page](https://github.com/GenericMadScientist/FM-Manip-Tool/releases). Provided you're on Windows the .exe should run out-of-the-box. If you're on another OS and are comfortable with the command line, see the [Building from Source](#building-from-source) section for instructions. If not, then let me know and I'll see about making a build for you if you're unable to do so on your own. If this is an issue, or if you need help using this, then join the [FM Discord](https://discord.gg/ygofm) and feel free to ask questions.

An explanation of how to use this is given in TUTORIAL.txt.

## Libraries

I use Qt 5, which you can get from the [Qt website](https://info.qt.io/download-qt-for-application-development). I also use the SQLite3 C library, which you can get from the [SQLite website](https://www.sqlite.org).

## Building from Source
This has been tested on Linux, but should work for MacOS as well.

### Prerequisites
1. `git`
2. `unzip`
3. `qt5` (includes `qmake`) -  On Ubuntu the package is named `qt5-default`, on Arch it is `qt5-base`. [More info](https://wiki.archlinux.org/title/qt)
4. `make`
5. `sqlite3` - Download the zip from under the "Source Code" section on the [SQLite Download Page](https://www.sqlite.org/download.html). The file should be named something like `sqlite-amalgamation-3400100.zip` with differing numbers for a different version. The latest version listed there is fine.

### Steps
1. Clone this repository - `git clone https://github.com/GenericMadScientist/FM-Manip-Tool.git`
2. `cd ./FM-Manip-Tool/`
3. Extract required sqlite files from the zip downloaded (see [Prerequisites](#prerequisites)). Note the file name and path of the download may differ. - `unzip ~/Downloads/sqlite-amalgamation-3400100.zip '*/sqlite3.[ch]'`
4. Rename extracted directory to expected name - `mv ./sqlite-amalgamation-3400100/ ./lib/`
5. Make a build output directory - `mkdir ./build-release/`
6. `cd ./build-release/`
7. `QT_SELECT=5 qmake ../FM_Manip_Tool.pro` (prefixing with `QT_SELECT=5` to ensure qt5 is used instead of another installed version)
8. `QT_SELECT=5 make`
9. `cp ../data/* .`
10. Run the app and profit 🎉 - `./FM\ Manip\ Tool`
11. (Bonus) Make the app runnable from anywhere:
```sh
#!/usr/bin/env bash
DIR="/<YOUR-PATH-TO>/FM-Manip-Tool/build-release/"
cd "$DIR" && "./FM Manip Tool"
```
You can name the script something like `fm-manip-tool`, place it somewhere in your `PATH`, call it from anywhere in the command line, make a `.desktop` shortcut out of it, etc.

## Licensing

My code is released under the MIT license. Qt is released under the GNU LGPL version 3. SQLite3 is public domain. The MIT license can be found in LICENSE.txt, while Qt's license can be found in QT_LICENSE.txt.
