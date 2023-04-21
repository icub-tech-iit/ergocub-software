## ergoCubEmotions 
This repo contains the `ergoCubEmotions` module that allows displaying several facial expressions on ergoCub.
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

https://user-images.githubusercontent.com/114698424/233656867-6d4cf48c-98aa-40d7-8bad-4053b5fc1818.mp4
