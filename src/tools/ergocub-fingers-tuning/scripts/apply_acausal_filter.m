%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Copyright (C) 2022 Fondazione Istituto Italiano di Tecnologia (IIT)
% All Rights Reserved.
%
function y = apply_acausal_filter(x, tau, Ts)

    s = tf('s');
    sys = 1 / (tau * Ts * s + 1);
    sysd = c2d(sys, Ts, 'tustin');

    y = x;
    for i = 1:size(x, 2)
        y(:, i) = filtfilt(sysd.Num{1}, sysd.Den{1}, x(:, i));
    end

end