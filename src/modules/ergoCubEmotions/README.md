## ergoCubEmotions 
This repo contains the `ergoCubEmotions` module that allows displaying several facial expressions on ergoCub.

<div align="center">
  <video src="https://user-images.githubusercontent.com/114698424/233678415-f705705e-0358-4293-aae8-c488f0a4946a.mp4" width="400" />
</div>

This module can be run enabling the cmake flag `COMPILE_ergoCubEmotions` and it needs [OpenCV v4.x](https://github.com/opencv/opencv) or higher to be used.

### Usage
```sh
mkdir build
cd build
cmake -DCOMPILE_ergoCubEmotions=BOOL:ON ..
make
(make install)
```
The module is already integrated in `yarpmanager` and, in order to use it, run the following command in a console:
```console
yarp rpc /ergoCubEmotions/rpc
```
