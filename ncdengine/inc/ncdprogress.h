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
* Description:   Operation progress information data structure.
*
*/


#ifndef T_NCD_PROGRESS_H
#define T_NCD_PROGRESS_H

#include <e32base.h>

/**
 *  Operation progress information. Progress is represented as two
 *  integer values: one defining the maximum progress value, and the
 *  other defining the current value. The maximum value stays
 *  (preferably) the same during an operation, while the current value
 *  is incremented during an operation, until it reaches the maximum
 *  value when the operation is complete.
 *
 *  @note Operation completion should not be determined using the
 *        progress values, the values are indicative only.
 *
 *  
 */
class TNcdProgress
    {
public:
    
    /**
     * Default constructor.
     * 
     * Progress values are left uninitialized.
     *
     * 
     */
    IMPORT_C TNcdProgress();
    
    /**
     * Constructor.
     * 
     * Initializes progress values with given parameters.
     *
     * 
     */
    IMPORT_C TNcdProgress( TUint aProgress, TUint aMaxProgress );

public:
    /**
     * Current progress value of the current state (zero-based)
     *
     * 
     */
    TUint iProgress;

    /**
     * Maximum progress value for the current state
     *
     * 
     */
    TUint iMaxProgress;
    
    };

#endif //  T_NCD_PROGRESS_H
