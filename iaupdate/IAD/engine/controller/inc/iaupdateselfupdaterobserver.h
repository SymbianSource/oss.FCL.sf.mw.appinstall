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



#ifndef IA_UPDATE_SELF_UPDATER_OBSERVER_H
#define IA_UPDATE_SELF_UPDATER_OBSERVER_H


/**
 * MIAUpdateSelfUpdaterObserver
 *
 * @since S60 v3.2
 */
class MIAUpdateSelfUpdaterObserver
    {
    
public:

    /**
     * @param aErrorCode Error code of the operation.
     *
     * @since S60 v3.2
     */
    virtual void SelfUpdaterComplete( TInt aErrorCode ) = 0;


protected:

    /**
     *
     * @since S60 v3.2
     */
    virtual ~MIAUpdateSelfUpdaterObserver() { }
    
    };

#endif // IA_UPDATE_SELF_UPDATER_OBSERVER_H

