# joint-pid-tuning

This folder contains material used to tune the PID gains for controlling a generic joint.

The C++ programs contained in `cpp` are used to move one joint in order to get the I/O identification data (`joint-open-loop-move`) and to verify the performance with repeated cycles (`joint-position-move`).

The MATLAB live script uses the datasets contained in `data` to identify a simple linear model of the finger, then runs the robust tuning procedure of a PI controller.

## Usage

The executable to run can be then found in the `build` or `install` folder.

### C++ Modules

#### joint-open-loop-move

After building it, navigate to the `build` folder, and then launch it with your desired arguments, e.g.:

```bash
./bin/joint-open-loop-move --port /ergocub/left_arm --joint-id 10 --cycles 5 --limits "(5 20)" --pwm-values "(10 20 30)" --timeout 5 --filename output.csv
```

The units of measure are:
- `limits`: degrees
- `pwm-valuse`: %
- `timeout`: seconds

#### joint-position-move

After building it, navigate to the `build` folder, and then launch it with your desired arguments, e.g.:

```bash
./bin/joint-position-move --robot ergocub --part left_arm --set-point 60 --filename output.csv
```

### MATLAB scripts

Upon opening MATLAB, navigate to the repo directory.

Copy the `.csv` datasets created with `joint-open-loop-move` in the `data` folder. Then:

1. Add to the path the folders `data` and `scripts`
2. Open the live script `identification_tuning.mlx`
3. Check the file names of the training and validation sets
   - it's better to also 'clear all output' by right-clicking in the livescript otherwise the report can be filled with old data and plots!
4. Run it by pressing "Run" or F5
   - sometimes (very often) the final command 'export' doesn't produce a complete pdf file. re-run this line by selecting it until the pdf looks fine
5. check the plots: if the fingers are not moving, then the PID gains are most likely not up to the task; e.g., see [here](https://github.com/icub-tech-iit/ergocub-design-lowerarm/issues/245#issuecomment-2095469706).
