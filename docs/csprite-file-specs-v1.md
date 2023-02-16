# Csprite File Format (.csprite) Specifications

1. [References](#references)
2. [Introduction](#introduction)
3. [Header](#header)
4. [Layers](#layers)
    1. [Layer names](#layer-names)
    1. [Layers pixel data](#layers-pixel-data)

---
## References

.csprite files uses little-endian byte order.

* `uint8_t`:  An 8-bit unsigned integer value
* `uint16_t`: A 16-bit unsigned integer value
* `int16_t`:  A 16-bit signed integer value
* `uint32_t`: A 32-bit unsigned integer value
* `int32_t`:  A 32-bit signed integer value
* `uint8_t[n]`:  "n" bytes.
* `STRING`:   characters (ASCII) with the `'\0'` character to determine the end of the string.

## Introduction

The format is dead simple to parse.

To read the csprite file:

* Read the [Csprite header](#header)
* Read the [Layer Names](#layers) & [Pixel Data Of Each Layer](#pixel-data) With The Information Given in Header.

## Header

A 22-byte header:

```
uint8_t[4] Magic (DEEZ)
uint16_t   File-Format Version (1 in this case)
int32_t    Width in pixels
int32_t    Height in pixels
int32_t    Number of channels
int32_t    Number of layers
```

## Layers
the layers are stored in 2 parts, first is the [Layer names](#layer-names) and second is the [Layers pixel data](#layers-pixel-data).

### Layer Names
after the [Header](#header) comes the layer names, layer names are basically `STRING` and are aligned, so after one layer name comes another layer name upto total layers

### Layers Pixel Data
after the [Layer Names](#layer-names) comes the layers pixel data, the pixel data is in compressed form, and was compressed using the [DEFLATE algorithm](https://en.wikipedia.org/wiki/Deflate) using [ZLib](https://en.wikipedia.org/wiki/Zlib), but it doesn't matter what library you use, it should be able to de-compress & compress the data using the DEFLATE algorithm.

after de-compressing the pixel data, your data size should become `width * height * number of channels * number of layers` which can be used to check if data isn't broken.

when you've de-compressed the data, your pixel data will be aligned as the layer names, so after one layer data comes another layer data upto total number of layers.


