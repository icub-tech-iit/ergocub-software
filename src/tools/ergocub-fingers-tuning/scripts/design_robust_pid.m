% Copyright (C) 2022 Fondazione Istituto Italiano di Tecnologia (IIT)
% All Rights Reserved
% Authors: mattia.fussi@iit.it
%
function [C, T] = design_robust_pid(usys, SoftGoals, HardGoals, pidType)


    arguments
        usys  {mustBeA(usys, ["tf", "uss", "ss"])}
        SoftGoals {mustBeA(SoftGoals, "TuningGoal.SystemLevel")}
        HardGoals {mustBeA(HardGoals, "TuningGoal.SystemLevel")}
        pidType {mustBeA(pidType, "char")}
    end

    %% define the tunable controller
  
    switch pidType

    case ('PI')
            C = tunablePID('C', 'PI');
            C.Kp.Minimum = -inf;    C.Kp.Maximum = 0;
            C.Ki.Minimum = -inf;    C.Ki.Maximum = 0;
            %disp('PI')
            assignin('base','tYpe','PI');
    case ('PID')
            C = tunablePID('C', 'PID');
            C.Kp.Minimum = -inf;    C.Kp.Maximum = 0;
            C.Ki.Minimum = -inf;    C.Ki.Maximum = 0;
            C.Kd.Minimum = -inf;    C.Kd.Maximum = 0;
            C.Tf.Minimum = 0.01;   C.Tf.Maximum = 0.1;    % N = 1/Tf
            %disp('PID')
            assignin('base','tYpe','PID');

    otherwise 
        disp('`re-run and choose PI or PID')

    end
    %C = tunablePID('C', 'PI');
    %C.Kp.Minimum = -inf;    C.Kp.Maximum = 0;
    %C.Ki.Minimum = -inf;    C.Ki.Maximum = 0;
    %C.Kd.Minimum = -inf;    C.Kd.Maximum = 0;
    %C.Tf.Minimum = 10 * Ts;   C.Tf.Maximum = 100 * Ts;    % N = 1/Tf
    C.TimeUnit = 'seconds';
    C.InputName = 'e';
    C.OutputName = 'u';

    %% Create uncertain loop
    Sum = sumblk('e = r - y');
    analysis_points = {'y'};
    input_names = {'r'};
    T = connect(usys, C, Sum, input_names, {'y'} ,  analysis_points);
    
    %% Tune system
    tuneopts = systuneOptions('MaxIter', 100, 'RandomStart', 10, 'UseParallel', false, "Display", "off");
    
    Gcl = systune(T, SoftGoals, HardGoals, tuneopts);
    
    tunedValue = getTunedValue(Gcl);
    C = tunedValue.C;
    C.InputName = 'e';
    C.Outputname = 'u';

    T = connect(usys, C, Sum, input_names, {'y'} ,  analysis_points);
end
