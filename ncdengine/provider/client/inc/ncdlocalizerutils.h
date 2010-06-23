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
* Description:   Declaration of CNcdLocalizerUtils
*
*/


#include <e32base.h>

class MNcdClientLocalizer;
class CNcdString;

/**
 * Localizer utility functions.
 */
class CNcdLocalizerUtils : public CBase 
    {
public:    
    /**
     * Localizes the given string by means of the given localizer object.
     * The result returned by the localizer is assigned to the given
     * descriptor pointer.
     *
     * @param aString The string to localize.
     * @param aLocalizer The localizer or NULL if not available.
     * @param aDescriptor The descriptor.
     * @return The localized string.
     */     
    static const TDesC& LocalizedString(
        const CNcdString& aString,
        MNcdClientLocalizer* aLocalizer,
        HBufC*& aDescriptor );
        
private:
    CNcdLocalizerUtils();
    };
