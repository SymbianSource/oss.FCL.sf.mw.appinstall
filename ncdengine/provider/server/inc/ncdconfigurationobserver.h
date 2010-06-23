/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/


#ifndef M_NCDCONFIGURATIONOBSERVER_H
#define M_NCDCONFIGURATIONOBSERVER_H

/**
 * Server side configuration observer interface.
 *
 * This interface informs observers if configuration is changed.
 */
class MNcdConfigurationObserver 
    {
public:
    virtual void ConfigurationChangedL() = 0;
    };

#endif