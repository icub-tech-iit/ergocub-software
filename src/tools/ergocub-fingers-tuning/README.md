# ergocub-fingers-tuning
This folder contains material used to tune the PID controllers for index and middle fingers of the ergoCub hands.

The MATLAB live script uses the datasets contained in `data` to identify a simple linear model of the finger, then runs the robust tuning procedure of a PI controller.

## Usage

Upon opening MATLAB, navigate to the repo directory, then:
1. Add to the path the folders `data` and `lib`
2. Open the live script `identification_tuning.mlx` and run it by pressing "Run" or F5