% Copyright (C) 2022 Fondazione Istituto Italiano di Tecnologia (IIT)
% All Rights Reserved
% Authors: mattia.fussi@iit.it
%
function [rmse,h] = plot_results(model, idd_training, idd_validation, training_Ts, validation_Ts)

    arguments
        model {mustBeA(model, ["idproc", "zpk", "tf", "ss"])}
        idd_training {mustBeA(idd_training, "iddata")}
        idd_validation {mustBeA(idd_validation, "iddata")}
        training_Ts (1,1) {mustBePositive}
        validation_Ts (1,1) {mustBePositive}
    end

    h = figure;
    subplot(2,1,1)
    timevec = 0:training_Ts:((length(idd_training.InputData)-1)*training_Ts);
    [y_out, time] = lsim(model, idd_training.InputData, timevec);
    plot(time, y_out, "LineWidth", 1.5)
    hold on
    plot(timevec, idd_training.OutputData, "LineWidth", 1.5);
    ylabel("Angula vel (deg/s)")
    legend({"model", "sensor"}, 'location', 'northwest')
    title("Training data")
    h1 = gca;
    
    subplot(2,1,2)
    
    rmse(1, 1) = struct;
    
    timevec = 0:validation_Ts:((length(idd_validation.InputData)-1)*validation_Ts);
    [y_out, time] = lsim(model, idd_validation.InputData, timevec);
    plot(time, y_out, "-")
    ylabel("Angular vel (deg/s)")
    xlabel("Time (sec)")
    hold on
    plot(timevec(1:end), idd_validation.OutputData(1:end));
    title("Validation data")
    h2 = gca;
        
    rmse(1).name = "Validation set " + string(1) + " result";
    rmse(1).value = rms(y_out - idd_validation.OutputData);
    
    legend({"model est val set 1", "meas val set 1"});
end