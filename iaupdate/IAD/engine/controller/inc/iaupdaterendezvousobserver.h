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



#ifndef IA_UPDATE_RENDEZVOUS_OBSERVER_H
#define IA_UPDATE_RENDEZVOUS_OBSERVER_H


/**
 * MIAUpdateRendezvousObserver
 *
 * @since S60 v3.2
 */
class MIAUpdateRendezvousObserver
    {
    
public:

    /**
     * @param aErrorCode Error code of the operation.
     *
     * @since S60 v3.2
     */
    virtual void RendezvousComplete( TInt aErrorCode ) = 0;


protected:

    /**
     *
     * @since S60 v3.2
     */
    virtual ~MIAUpdateRendezvousObserver() { }
    
    };

#endif // IA_UPDATE_RENDEZVOUS_OBSERVER_H

