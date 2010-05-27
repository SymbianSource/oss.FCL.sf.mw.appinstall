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
* Description:   This file contains declarations of file task parameters.
*
*/


#ifndef SWINSTFILETASKPARAM_H
#define SWINSTFILETASKPARAM_H

//  INCLUDES
#include <e32std.h>
#include <SWInstUid.h>

namespace SwiUI
{

/**
* Parameters for delete file task
*
* @since 3.0
*/
class TFileTaskDeleteParam
    {
    public:    
        
        TFileName iFile;
    };

// Package buffer for log task parameters
typedef TPckg<TFileTaskDeleteParam> TFileTaskDeleteParamPckg;

// Uid for startup list addition implementation
const TUid KFileTaskDeleteImplUid = { KSWInstFileTaskDeleteImplUid };

}

#endif // SWINSTFILETASKPARAM_H
