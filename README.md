# ergocub-software
Main collector of ergoCub specific SW

## Installation

For installing it just:
```sh
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=<install-prefix> ..
make
(make install)
```
`ergocub` model needs [Gazebo YARP Plugins v4.7.0](https://github.com/robotology/gazebo-yarp-plugins/releases/tag/v4.7.0) or higher in order to be used.

In order to use the model, the following env variables must be configured:
```sh
# ergoCub model in YARP
export YARP_DATA_DIRS=${YARP_DATA_DIRS}:<install-prefix>/share/ergoCub
# ergoCub model in Gazebo
export GAZEBO_MODEL_PATH=${GAZEBO_MODEL_PATH}:<install-prefix>/share/ergoCub/robots
export GAZEBO_MODEL_PATH=${GAZEBO_MODEL_PATH}:<install-prefix>/share/
```

## Run Whole-body-dynamics
Currently whole-body-dynamics does not run along with `ergoCubGazeboV1`. To start it please run the following command in a console once `yarpserver` and
the robot has been placed in `gazebo`
```console
yarprobotinterface --config conf/launch_wholebodydynamics_ecub.xml
```

## URDF generation
This repo hosts the `ergoCub` urdf, enabling the cmake flag `ERGOCUB_MODEL_GENERATE_SIMMECHANICS` it is possible to generate it from simmechanics xml.
Enabling also `ERGOCUB_MODEL_COPY_TO_SRC` the generated models are copied in the `urdf` directory for being committed.
### Dependencies
- [Ruby](https://www.ruby-lang.org/en/)
- [iDynTree](https://github.com/robotology/idyntree)
- [YARP](https://github.com/robotology/yarp)
- [simmechanics-to-urdf](https://github.com/robotology/simmechanics-to-urdf)
### Usage
```sh
mkdir build
cd build
cmake -DERGOCUB_MODEL_GENERATE_SIMMECHANICS=BOOL:ON -DERGOCUB_MODEL_COPY_TO_SRC=BOOL:ON ..
make
(make install)
```

### Automatic generation
[This GitHub Action](/.github/workflows/generate_models.yml) generates automatically the urdf everytime a commit in `urdf/simmechanics` on master branch is done and opens a PR containing the changes in the models.

### Maintainers
This repository is maintained by:

| | |
|:---:|:---:|
| [<img src="https://github.com/Nicogene.png" width="40">](https://github.com/Nicogene) | [@Nicogene](https://github.com/Nicogene) |
