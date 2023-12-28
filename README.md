# TVID

## Authors

Jean FECHTER \
Thibault BOUTET

## Build

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
```

## Run

```bash
$ ./TVID IMAGES_PATH -m HEADER_FILE_PATH [-doa]
```
IMAGES_PATH: path to the folder containing the images to be processed \
HEADER_FILE_PATH: path to the header file generated by mpeg2dec \
-o: Specify the output folder \
-a: Use adaptative deinterlacing by specifying the motion threshold \
-d: Display the output images without saving them [optionally specify the frame_rate]

### Examples

```bash
$ ./TVID ../images -m ../images/header/meta.info -o ../output
$ ./TVID ../images -m ../images/header/meta.info -o ../output -d
$ ./TVID ../images -m ../images/header/meta.info -d 25
$ ./TVID ../images -m ../images/header/meta.info -d 25 -a 20
```
