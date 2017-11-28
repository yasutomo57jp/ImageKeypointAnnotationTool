# Image Keypoint Annotation Tool

## requirements

* gcc >= 4.9
* libboost
* opencv >= 3.0
* pkg-config

```bash
sudo apt install build_essential libboost-dev
```

## How to run

```bash
make
./annotator /path/to/images /path/to/annotationfile
```
or

```bash
./annotator /path/to/images /path/to/existing/annotationfile /path/to/newannotationfile
```

## usage

* next image: j, f, " ", →
* prev image: k, b, ←
* delete keypoint: BS
* save&quit: q
