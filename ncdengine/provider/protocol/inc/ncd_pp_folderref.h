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
* Description:   MNcdProtocolElementFolderRef declaration
*
*/


#ifndef NcdPREMINETPROTOCOLFOLDERREF_H
#define NcdPREMINETPROTOCOLFOLDERREF_H

#include <e32base.h>

#include "ncd_pp_entityref.h"



class MNcdPreminetProtocolFolderRef : public MNcdPreminetProtocolEntityRef
    {
public:
    
    /**
     * Indicates uninitialized value.
     */
    enum { KValueNotSet = KMinTInt };
    
    /**
     * Destructor
     */
    virtual ~MNcdPreminetProtocolFolderRef() {}

    /**
     * Amount of entities in the list.
     * If not set, will return KValueNotSet.
     */
    virtual TInt Count() const = 0;

    /**
     * Authorized count.
     * If not set, will return KValueNotSet.
     */
    virtual TInt AuthorizedCount() const = 0;

    // Sub entities not reported here, they come separately with parent information set.
    };

#endif
