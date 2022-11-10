# ergocub
Main collector of ergoCub specific SW

## URDF generation
This repo hosts the `ergoCub` urdf, enabling the cmake flag `ERGOCUB_MODEL_GENERATE_SIMMECHANICS` it is possible to generate it from simmechanics xml.
### Dependencies
- [iDynTree](https://github.com/robotology/idyntree)
- [YARP](https://github.com/robotology/yarp)
- [simmechanics-to-urdf](https://github.com/robotology/simmechanics-to-urdf)
### Usage
```
mkdir build
cd build
cmake -DERGOCUB_MODEL_GENERATE_SIMMECHANICS=BOOL:ON ..
make
```
### Automatic generation
[This GitHub Action]() generates automatically the urdf everytime a commit in `usdf/simmechanics` on master branch is done and opens a PR containing the changes in the models.
