
/******************************************************************************
 *                                                                            *
 * Copyright (C) 2023 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

service ergoCubEmotions_IDL
{
        bool startModule();
        bool setHappy();
        bool setNeutral();
        bool setAngry();
        bool setShy();
}