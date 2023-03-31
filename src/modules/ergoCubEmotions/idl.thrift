
/******************************************************************************
 *                                                                            *
 * Copyright (C) 2023 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

/**
 * @file idl.thrift
 */

/**
 * ergoCubEmotions_IDL
 *
 * IDL Interface to ergoCub Emotion Handler services.
 */

service ergoCubEmotions_IDL
{
        /**
        * Display a specific facial expression.
        * @param command is the name of the emotion that will be set.
        * @return true/false on success/failure.
        */
        bool setEmotion(1:string command);

        /**
        * Print the list of all the available facial expressions.
        * @return a list of commands.
        */
        list<string> availableEmotions();
}