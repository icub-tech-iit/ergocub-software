%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Copyright (C) 2022 Fondazione Istituto Italiano di Tecnologia (IIT)
% All Rights Reserved.
%
function [sys, sysd] = estimate_ss(train_id, nx, input_delays, max_iter)

    Ts = train_id.Ts;
    nu = length(train_id.InputName);
    ny = length(train_id.OutputName);

    if nx < ny
        error('the number of state shall be >= %d', ny);
    end

    A = -eye(nx, nx);
    B = ones(nx, nu);
    C = [eye(ny, ny) zeros(ny, nx-ny)];
    D = zeros(ny, nu);
    K = zeros(nx, nu);
    x0 = zeros(nx, 1);
    init_sys = idss(A, B, C, D, K, x0, 0);
    init_sys.InputDelay = input_delays * Ts;
    init_sys.Structure.C.Free = false * init_sys.Structure.C.Free;
    init_sys.Structure.D.Free = false * init_sys.Structure.D.Free;
    init_sys.Structure.K.Free = false * init_sys.Structure.K.Free;

    opt = ssestOptions('Focus', 'simulation', 'EnforceStability', true, ...
                       'InitialState', 'zero', 'SearchMethod', 'lm', ...
                       'Display','on');
    opt.SearchOptions.MaxIterations = max_iter;
    
    sys = ssest(train_id, init_sys, opt);
    sysd = c2d(sys, Ts, 'tustin');
    
end

