% Copyright (C) 2022 Fondazione Istituto Italiano di Tecnologia (IIT)
% All Rights Reserved
% Authors: mattia.fussi@iit.it
%
function t = prepare_dataset(file, Ts, tau)

    training_table = readtable(file);
    training_table.Properties.VariableNames = {'time','pwm','theta','pid', 'pwmsp'};
    
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

    t = table(t1_dot, pwm(:,2), vel(:,2), vel_uf(:,2), pos(:, 2), training_table.theta(1:end-1), 'VariableNames', {'Time', 'pwm', 'vel', 'vel_uf', 'pos', 'pos_uf'});

end 
