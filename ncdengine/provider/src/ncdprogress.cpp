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


#include "ncdprogress.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C TNcdProgress::TNcdProgress() :
    iProgress( 0 ),
    iMaxProgress( 0 )
    {    
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C TNcdProgress::TNcdProgress( TUint aProgress, 
                                     TUint aMaxProgress ) 
: iProgress( aProgress ), 
  iMaxProgress( aMaxProgress )
    {
    }
