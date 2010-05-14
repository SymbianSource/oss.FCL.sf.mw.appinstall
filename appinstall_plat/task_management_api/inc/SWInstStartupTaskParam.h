/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains declarations of log task parameters.
*
*/


#ifndef SWINSTSTARTUPTASKPARAM_H
#define SWINSTSTARTUPTASKPARAM_H

//  INCLUDES
#include <e32std.h>
#include <StartupItem.hrh>
#include <SWInstUid.h>

namespace SwiUI
{

/**
* Parameters for startup task.
*
* @since 3.0
*/
class TStartupTaskParam
    {
    public:    
        
        TFileName iFileName;  // Full path to executable file
        TStartupExceptionPolicy iRecoveryPolicy; // Not used in remove
    };

// Package buffer for log task parameters
typedef TPckg<TStartupTaskParam> TStartupTaskParamPckg;

// Uid for startup list addition implementation
const TUid KSTartupTaskAddImplUid = { KSWInstStartupAddImplUid };

// Uid for startup list removal implementation
const TUid KSTartupTaskRemoveImplUid = { KSWInstStartupRemoveImplUid };    

}

#endif // SWINSTSTARTUPPARAM_H
