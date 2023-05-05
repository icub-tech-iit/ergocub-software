## ergoCubEmotions 
This repo contains the `ergoCubEmotions` module that allows displaying several facial expressions on ergoCub.

<div align="center">
  <video src="https://user-images.githubusercontent.com/114698424/236434116-8c9d69a1-5cce-43b8-8655-6fc3d20aec82.mp4" width="400" />
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

The module is ["_thrifted_"](https://yarp.it/latest/idl_thrift.html), hence you can get a list of the available options via the `help` command.
