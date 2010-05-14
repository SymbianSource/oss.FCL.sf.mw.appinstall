/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Defines for IAUpdater parameters.
*
*/



#ifndef IAUPDATERPARAMS_H
#define IAUPDATERPARAMS_H

#include <e32base.h>
#include <e32cmn.h>


// Slot for start params given to IAUpdater process.
const TInt KIAUpdaterParamSlot = 1; 

/**
 *  This class contains start parameters for IAUpdater 
 *
 *  @code   
 *  @endcode
 *  @lib 
 *  @since S60 
 */ 
class TIAUpdaterStartParams
    {
public: 
    // Paths to the files that contain set up values
    // for the updater.  
    TFileName iInitFilePath;
    
    // Paths to the files that will contain the result values
    // when the updater has finished its job. 
    TFileName iResultFilePath;
    };
    
#endif // IAUPDATERPARAMS_H

// EOF
