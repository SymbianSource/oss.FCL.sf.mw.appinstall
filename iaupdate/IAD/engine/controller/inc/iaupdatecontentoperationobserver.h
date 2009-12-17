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



#ifndef IA_UPDATE_CONTENT_OPERATION_OBSERVER_H
#define IA_UPDATE_CONTENT_OPERATION_OBSERVER_H

#include <e32def.h>

class CIAUpdateBaseNode;


/**
 * MIAUpdateContentOperationObserver
 * Observer for content operations.
 */
class MIAUpdateContentOperationObserver
    {

public:

    /**
     * @param aNode Node whose content operation has finished.
     * @param aError Error code for the operation.
     */
    virtual void ContentOperationComplete( CIAUpdateBaseNode& aNode, 
                                           TInt aError ) = 0;

    /**
     * @param aNode Node whose content operation has progressed.
     * @param aProgress Progress at the moment.
     * @param aMaxProgress Maximum progress when the operation should
     * be ready.
     */
    virtual void ContentOperationProgress( CIAUpdateBaseNode& aNode, 
                                           TInt aProgress, 
                                           TInt aMaxProgress ) = 0;

protected:

    /**
     * Protected virtual destructor to prevent unwanted
     * deletions by the user.
     */        
    virtual ~MIAUpdateContentOperationObserver() { }
    
    };

#endif // IA_UPDATE_CONTENT_OPERATION_OBSERVER_H
