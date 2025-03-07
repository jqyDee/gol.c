# gol.c
This is an implementation of [Conways game of life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) written in C.

Currently this version lets you create a animation using [NetPBM](https://en.wikipedia.org/wiki/Netpbm) bitmaps and [ImageMagick](https://en.wikipedia.org/wiki/ImageMagick) to convert the bitmaps into a gif.

THE CODE IS ONLY TESTED ON MACOS SO FAR!

![gol.gif](./assets/gol.gif)

## Requirements
To install on MacOS using [homebrew](https://brew.sh).

- ImageMagick
```bash
brew install imagemagick
```

- GNU make
```bash
brew install make
```


## Build
### build
```bash
make
```

### run
```bash
make run
```
or
```bash
make
./gol <width> <height> <density> <animation-frames> <random|glider>
```

### create gif
```bash
make gif
```

or

```bash
make
./gol <width> <height> <density> <animation-frames> <random|glider>
convert
convert -filter point -resize 300%x300% -delay $(DELAY) ./pbm/gol_*.pbm gol.gif
```

