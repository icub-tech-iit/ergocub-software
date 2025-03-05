
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

        /**
        * Change the visibility of a specific graphic.
        * @param name is the name of the graphic that will be set.
        * @param visible is the visibility of the graphic.
        * @return true/false on success/failure.
        */
        bool setGraphicVisibility(1:string name, 2:bool visible);

        /**
        * Print the list of all the available graphics.
        * @return a list of names.
        */
        list<string> availableGraphics();
}
