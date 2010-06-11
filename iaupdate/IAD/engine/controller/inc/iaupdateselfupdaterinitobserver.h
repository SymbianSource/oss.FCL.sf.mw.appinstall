/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   MIAUpdateContentOperationObserver 
*
*/



#ifndef IA_UPDATE_SELF_UPDATER_INIT_OBSERVER_H
#define IA_UPDATE_SELF_UPDATER_INIT_OBSERVER_H

#include <e32def.h>

/**
 * MIAUpdateSelfUpdaterInitObserver
 */
class MIAUpdateSelfUpdaterInitObserver
    {

public:

    /**
     * This function is called when the preliminary operations for self update
     * content are handled. After this self updater can be started for the rest
     * of the contents to be installed.
     *
     * @param aError Error value
     */
    virtual void SelfUpdateInitComplete( TInt aError ) = 0;

protected:

    /**
     * Protected virtual destructor to prevent unwanted
     * deletions by the user.
     */        
    virtual ~MIAUpdateSelfUpdaterInitObserver() { }
    
    };

#endif // IA_UPDATE_SELF_UPDATER_INIT_OBSERVER_H
