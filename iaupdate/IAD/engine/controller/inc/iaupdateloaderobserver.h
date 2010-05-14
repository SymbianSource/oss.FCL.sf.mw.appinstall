/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/



#ifndef IA_UPDATE_LOADER_OBSERVER_H
#define IA_UPDATE_LOADER_OBSERVER_H


#include <e32def.h>

class MNcdNode;


/**
 * MIAUpdateLoaderObserver
 */
class MIAUpdateLoaderObserver
    {
    
public:

    /**
     * Informs observer that refresh operation has been completed.
     * @param aError Error code
     **/
    virtual void LoadComplete( TInt aError ) = 0;


protected:

    virtual ~MIAUpdateLoaderObserver() {}
        
    };

#endif  //  IA_UPDATE_LOADER_OBSERVER_H
