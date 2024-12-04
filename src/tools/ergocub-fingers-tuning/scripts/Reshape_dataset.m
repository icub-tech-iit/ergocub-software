% Copyright (C) 2022 Fondazione Istituto Italiano di Tecnologia (IIT)
% All Rights Reserved
% Authors: mattia.fussi@iit.it, simeone.dussoni@iit.it
%
function [t, Ts] = Reshape_dataset(file, tau, downsample, interpolate, sampling_time_significant_digits)

    arguments
        file {mustBeFile}
        tau (1, 1) {mustBeNonnegative}
        downsample (1,1) {mustBeInteger}
        interpolate (1,1) {mustBeInteger}
        sampling_time_significant_digits (1,1) {mustBeInteger}
    end

    training_table = readtable(file);
    training_table.Properties.VariableNames = {'time','pwm','theta','pid', 'pwmsp'};
    if(downsample == 1) % resampling down to the original refresh rate in FAP data
        [a, t_index] = findpeaks(abs(diff(training_table.theta)));
        training_table_temp = training_table(t_index,:);
        training_table_temp.Properties.VariableNames = {'time','pwm','theta','pid', 'pwmsp'};
    end
    Ts = mean(diff(training_table_temp.time)); % Ts is  now computed directly from data rather than imposed as a parameters

    Ts = floor(Ts*10^(sampling_time_significant_digits))/10^(sampling_time_significant_digits); % truncate to the millisecond

    if(Ts >0.002 && interpolate > 0.5) % resampling with interpolation
        ttable = interp1(training_table_temp.time,table2array(training_table_temp), [0:0.001:training_table.time(end)],'spline');
        training_table = array2table(ttable);
        training_table.Properties.VariableNames = {'time','pwm','theta','pid', 'pwmsp'};
        Ts = 0.001;
    else 
        training_table = training_table_temp;
    end

    t1 = training_table.time;
    
    t1 = t1 - t1(1);
    t1_dot = t1;
    
    theta_f =  apply_acausal_filter(training_table.theta, tau, Ts);
    theta_f_dot = diff(theta_f)/Ts;
    
    theta_dot = diff(training_table.theta)/Ts;
    
    pwm = [training_table.time training_table.pwm];
    pwm = pwm(1:end-1, :);
    
    t1_dot(end) = [];
    pos = [t1 theta_f];
    pos = pos(1:end-1, :);

    vel = [t1_dot theta_f_dot];
    vel_uf = [t1_dot theta_dot];

    t = table(t1_dot, pwm(:,2), vel(:,2), vel_uf(:,2), pos(:, 2), training_table.theta(1:end-1), ...
        'VariableNames', {'Time', 'pwm', 'vel', 'vel_uf', 'pos', 'pos_uf'});
end 
