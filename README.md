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
This repository hosts the configuration files for generating ergoCub urdf via [`creo2urdf`](https://github.com/icub-tech-iit/creo2urdf).
You can find there the relative documentation on how write those configuration files.

### Maintainers
This repository is maintained by:

| | |
|:---:|:---:|
| [<img src="https://github.com/Nicogene.png" width="40">](https://github.com/Nicogene) | [@Nicogene](https://github.com/Nicogene) |
