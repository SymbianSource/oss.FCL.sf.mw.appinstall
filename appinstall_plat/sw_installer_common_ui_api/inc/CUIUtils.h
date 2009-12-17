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
* Description:   This file contains the header file of the CUIUtils class
*
*/


#ifndef CUIUTILS_H
#define CUIUTILS_H

//  INCLUDES
#include <e32base.h>

_LIT( KCUIResourceFileName, "SWInstCommonUI.rsc" );

//  FORWARD DECLARATIONS
class CCoeEnv;

namespace SwiUI
{
namespace CommonUI
{

/**
* This class contains some common utility functions.
* @lib SWInstCommonUI.lib
* @since 3.0
*/
class CUIUtils
    {
    public:  // Constructors and destructor
        
        /**
        * Load the given resource file.
        * @since 3.0
        * @param aFileName - Resource file name with path, but without the drive letter
        * @param aCoeEnv - Pointer to valid CCoeEnv
        * @return Offset of the resource file.
        */
        IMPORT_C static TInt LoadResourceFileL( const TDesC& aFileName, CCoeEnv* aCoeEnv );
        
        /**
        * Construct a string representation of given version.
        * @since 3.0
        * @param aMajor - Major version number.
        * @param aMinor - Minor version number.
        * @return Allocated buffer containing the version string. Ownership transfers 
        * to the caller.
        */        
        IMPORT_C static HBufC* ConstructVersionStringLC( TInt aMajor, TInt aMinor, TInt aBuild );
    };
}
}

#endif      // CUIUTILS_H
            
// End of File
