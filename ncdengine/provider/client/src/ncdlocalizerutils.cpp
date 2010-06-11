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
* Description:   CNcdLocalizerUtils implementation
*
*/


#include "ncdlocalizerutils.h"
#include "catalogsdebug.h"
#include "ncdclientlocalizer.h"
#include "ncdstring.h"

const TDesC& CNcdLocalizerUtils::LocalizedString(
    const CNcdString& aString,
    MNcdClientLocalizer* aLocalizer,
    HBufC*& aDescriptor ) 
    {
    DLTRACEIN((""));
    if ( aDescriptor ) 
        {
        return *aDescriptor;
        }
        
    if ( aString.Data() != KNullDesC || aString.Key() == KNullDesC ) 
        {
        return aString.Data();
        }
    else if ( aLocalizer )
        {
        aDescriptor = aLocalizer->LocalizeString( aString.Key() );
        if ( !aDescriptor ) 
            {
            return KNullDesC;
            }
        else 
            {            
            return *aDescriptor;
            }
        }
    else 
        {
        return KNullDesC;
        }
    }
