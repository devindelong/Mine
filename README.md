# BZFlag Server Plugin: Mine

## Loading the plugin

This plugin takes no optional arguments, so load it with:
  -loadplugin Mine

An extremely basic server config:

```
 -loadplugin /path/to/Mine/build/release/src/mine.so
 +f MN{100}
```

## Server Commands

/mine
  Drops a mine at your current location if you have the Mine flag.

## Compiling

```bash
cmake --preset release -DBZFLAG_PROJECT_ROOT=/path/to/bzflag
cmake --build --preset release
```
