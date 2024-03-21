This directory contains file generated from `https://github.com/icub-tech-iit/cad-mechanics/blob/master/projects/simulation_model/ergocub_1-1/sim_ergocub_1-1.asm.1` (the file is currently private, please contact one of the mantainers of the repo for access) using the https://github.com/icub-tech-iit/cad-libraries/wiki/Prepare-PTC-Creo-Mechanism-for-URDF workflow .

In particular, this is the mapping between the files contained in this folder and the files contained in `ergocub-software/urdf/ergoCub/robots`:

| CAD model in `cad-mechanics` | yaml file | csv file | YARP_ROBOT_NAME |
|:----------:|:---------:|:----------:|:--------------:|
| `cad-mechanics/projects/simulation_model/ergocub_1-0/sim_ergocub_1-1.asm.1` | `ergocub-software/urdf/creo2urdf/data/ergocub1_1/ERGOCUB_all_options.yaml` | `ergocub-software/urdf/creo2urdf/data/ergocub1_1/ERGOCUB_all_options/ERGOCUB_all_options.csv` | `ergoCubSN001`  |
| `cad-mechanics/projects/simulation_model/ergocub_1-0/sim_ergocub_1-1.asm.1` | `ergocub-software/urdf/creo2urdf/data/ergocub1_1/ERGOCUB_all_options/ERGOCUB_all_options_gazebo.yaml` | `ergocub-software/urdf/creo2urdf/data/ergocub1_1/ERGOCUB_all_options/ERGOCUB_all_options_gazebo.csv` | `ergoCubGazeboV1_1`  |
| `cad-mechanics/projects/simulation_model/ergocub_1-0/sim_ergocub_1-0.asm.1` | `ergocub-software/urdf/creo2urdf/data/ergocub1_1/ERGOCUB_all_options/ERGOCUB_all_options_minContacts.yaml` | `ergocub-software/urdf/creo2urdf/data/ergocub1_1/ERGOCUB_all_options/ERGOCUB_all_options_minContacts.csv` | `ergoCubGazeboV1_1_minContacts`  |
