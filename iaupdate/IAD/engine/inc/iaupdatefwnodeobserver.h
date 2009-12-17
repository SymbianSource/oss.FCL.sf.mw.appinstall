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



#ifndef IA_UPDATE_FW_NODE_OBSERVER_H
#define IA_UPDATE_FW_NODE_OBSERVER_H


#include <e32def.h>

class MIAUpdateFwNode;


/**
 * MIAUpdateFwNodeObserver
 * Observes firmware node operations.
 */
class MIAUpdateFwNodeObserver
    {

public:

    /**
     * Notifies observer that an operation has been completed
     * @param aNode Node who had its operation completed
     * @param aError Error value
     **/
    virtual void InitDownloadDataComplete( MIAUpdateFwNode& aNode,
                                           TInt aError ) = 0;

                                   
protected:

    virtual ~MIAUpdateFwNodeObserver() {}
        
    };

#endif  //  IA_UPDATE_FW_NODE_OBSERVER_H
