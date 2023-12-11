# ergocub-software
Main collector of ergoCub specific SW

## Installation

### Dependencies

Before installing `ergocub-software`, please be sure that you've installed [YARP](https://www.yarp.it/latest//) on your machine.

### Compiling from source

For installing it just:
```sh
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=<install-prefix> ..
make
(make install)
```
`ergocub` for Gazebo Classic simulation model needs a gazebo-yarp-plugins >= 4.9.0 .

In order to use the model, the following env variables must be configured:
```sh
# ergoCub model in YARP
export YARP_DATA_DIRS=${YARP_DATA_DIRS}:<install-prefix>/share/ergoCub
# ergoCub model in Gazebo
export GAZEBO_MODEL_PATH=${GAZEBO_MODEL_PATH}:<install-prefix>/share/ergoCub/robots
export GAZEBO_MODEL_PATH=${GAZEBO_MODEL_PATH}:<install-prefix>/share/
```

### Use conda binary packages

This repository is packaged as `ergocub-software` in the `robotology` conda channel, see https://anaconda.org/robotology/ergocub-software .

To create an environment with it, just add it during the environment creation as any other conda package, for example:
~~~
conda create -n ergocubenv -c conda-forge -c robotology ergocub-software
~~~

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
