# GameBuilder
[![badge](https://img.shields.io/badge/license-MIT-blue)](https://github.com/usermicrodevices/game_builder/blob/main/LICENSE)

Map and logic editor.
Create game without programming, but optional you always can replace core engine (source code included).

# Dependencies
- wxWidgets
- Python

## build wxWidgets
```
git clone git@github.com:wxWidgets/wxWidgets.git
cd wxWidgets
./configure --enable-debug --enable-debug_info --enable-catch_segvs --enable-debugreport --enable-exceptions --disable-optimise --enable-log --enable-any --enable-cmdline --enable-aui --enable-propgrid --enable-stc --enable-loggui --enable-logwin --enable-logdialog --enable-artstd --enable-notebook
make -j4
cd ..
```

## build Python
download last edition from https://www.python.org/downloads/source/ and unpack
```
cd Python-3.X.X
./configure
make -j4
cd ..
```

# build this repo
```
git clone git@github.com:usermicrodevices/game_builder.git
cd game_builder
make -j4
```


## optional

### game engines
- Irrlicht3d
- pyirrlicht (Python)

### sound
- bass
- pybass (Python)
