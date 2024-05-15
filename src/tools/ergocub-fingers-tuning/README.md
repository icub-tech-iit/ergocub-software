# ergocub-fingers-tuning

This folder contains material used to tune the PID controllers for index and middle fingers of the ergoCub hands.

The C++ programs contained in `cpp` are used to move one of the fingers in order to get the I/O identification data (`ident-finger`) and to verify the performance with repeated cycles (`move-finger-pos`).

The MATLAB live script uses the datasets contained in `data` to identify a simple linear model of the finger, then runs the robust tuning procedure of a PI controller.

## Usage

For each of the C++ programs, to build them just do:

```bash
cd <ident-finger or move-finger-pos>
mkdir build
cd build
cmake ..
make
```

The executable to run can be then found in the `build` folder.

### C++ Modules

#### ident-finger

After building it, navigate to the `build` folder, and then launch it with your desired arguments, e.g.:

```bash
./ident-finger --robot ergocub --part left_arm --joint-id 10 --cycles 5 --threshold 20 <degrees> --timeout 5000 <sec> --filename output.csv
```

#### move-finger-pos

After building it, navigate to the `build` folder, and then launch it with your desired arguments, e.g.:

```bash
./move-finger-pos --robot ergocub --part left_arm --set-point 60 --filename output.csv
```

### MATLAB scripts

Upon opening MATLAB, navigate to the repo directory.

Copy the `.csv` datasets created with `ident-finger` in the `data` folder. Then:

1. Add to the path the folders `data` and `scripts`
2. Open the live script `identification_tuning.mlx`
3. Check the file names of the training and validation sets
   - the file opened is the last one, unless the user run **init_ws** which resets the file name
   - **init_ws** must also be run as the first command to insure the livescript works.
   - it's better to also 
   - it's better to also 'clear all output' by right-clicking in the livescript otherwise the report can be filled with old data and plots!
4. Run it by pressing "Run" or F5
   - sometimes (very often) the final command 'export' doesn't produce a complete pdf file. re-run this line by selecting it until the pdf looks fine 
5. check the plots: anytime the fingers are not moving, the PID parameters are screwed up see e.g. [here](https://github.com/icub-tech-iit/ergocub-design-lowerarm/issues/245#issuecomment-2095469706)

