# ergocub-software
Main collector of ergoCub specific SW

## Installation

### Dependencies

Before installing `ergocub-software`, please be sure that you've installed [`YARP`](https://www.yarp.it/latest//) 3.12.0 or higher on your machine.

### Compiling from source

For installing it just:
```sh
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=<install-prefix> ..
make
(make install)
```
`ergocub` for Gazebo Classic simulation model needs a `gazebo-yarp-plugins` [latest master](https://github.com/robotology/gazebo-yarp-plugins/commit/c89280295d53279049bb7521cf6b6b3400130f23).

In order to use the model, the following env variables must be configured:
```sh
# ergoCub model in YARP
export YARP_DATA_DIRS=${YARP_DATA_DIRS}:<install-prefix>/share/ergoCub
# ergoCub model in Gazebo
export GAZEBO_MODEL_PATH=${GAZEBO_MODEL_PATH}:<install-prefix>/share/ergoCub/robots
export GAZEBO_MODEL_PATH=${GAZEBO_MODEL_PATH}:<install-prefix>/share/
```

In order to make the device `couplingXCubHandMk5` detectable, add `<installation_path>/share/yarp` to the `YARP_DATA_DIRS` environment variable of the system.

Alternatively, if `YARP` has been installed using the [robotology-superbuild](https://github.com/robotology/robotology-superbuild), it is possible to use `<directory-where-you-downloaded-robotology-superbuild>/build/install` as the `<installation_path>`.

### Use conda binary packages

This repository is packaged as `ergocub-software` in the `conda-forge` conda channel, see https://anaconda.org/conda-forge/ergocub-software and https://github.com/conda-forge/ergocub-software-feedstock .

To create an environment with it, just add it during the environment creation as any other conda package, for example:
~~~
conda create -n ergocubenv -c conda-forge ergocub-software
~~~

If you only need to use the ergoCub URDF models and you do not want to install the full dependencies of the `ergocub-software` (that include YARP and OpenCV), you can also install the lightweight dependency free package `ergocub-models`:
~~~
conda create -n ergocubenv -c conda-forge ergocub-models
~~~

The `ergocub-software` package depends on `ergocub-models`, so if you need both you can just install `ergocub-software`.

## Run Whole-body-dynamics
Currently whole-body-dynamics does not run along with `ergoCubGazeboV1`. To start it please run the following command in a console once `yarpserver` and
the robot has been placed in `gazebo`
```console
yarprobotinterface --config conf/launch_wholebodydynamics_ecub.xml
```

## URDF generation

This repository hosts the configuration files for generating ergoCub urdf. To generate the URDF, you need to have access to the `cad-mechanics` repo (that is currently private, if you need access ask it to the ergocub-software mantainer) and install the following repos and software:
* You need to install the version of Creo required by [`cad-mechanics`](https://github.com/icub-tech-iit/cad-mechanics/).
* You need to install the repos that contain the CAD models, i.e. cad-libraries (see https://github.com/icub-tech-iit/cad-libraries/wiki/Configure-PTC-Creo-with-cad-libraries) and cad-mechanics https://github.com/icub-tech-iit/cad-mechanics/.
* You need to install creo2urdf following the README in https://github.com/icub-tech-iit/creo2urdf, either from source or using the binary available for each release.

>[!NOTE]
> For generating ergoCub URDF `creo2urdf` [v0.5.14](https://github.com/icub-tech-iit/creo2urdf/releases/tag/v0.5.14) or greater is needed

The CAD files used to generate the URDF models have been prepare according to the procedure described in https://github.com/icub-tech-iit/cad-libraries/wiki/Prepare-PTC-Creo-Mechanism-for-URDF .

You can find there the relative documentation on how write those configuration files, and more details in the README of the following folders:
* [`urdf/creo2urdf/data/ergocub1_0`](./urdf/creo2urdf/data/ergocub1_0)
* [`urdf/creo2urdf/data/ergocub1_1`](./urdf/creo2urdf/data/ergocub1_1)
* [`urdf/creo2urdf/data/ergocub1_3`](./urdf/creo2urdf/data/ergocub1_3)

### Maintainers
This repository is maintained by:

| | |
|:---:|:---:|
| [<img src="https://github.com/Nicogene.png" width="40">](https://github.com/Nicogene) | [@Nicogene](https://github.com/Nicogene) |
