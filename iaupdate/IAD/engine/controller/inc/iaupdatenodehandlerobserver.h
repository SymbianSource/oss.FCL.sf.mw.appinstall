/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?description
*
*/



#ifndef IAD_UPDATE_NODE_HANDLER_OBSERVER_H
#define IAD_UPDATE_NODE_HANDLER_OBSERVER_H


#include <e32base.h>


class MNcdNode;


class MIAUpdateNodeHandlerObserver
{
public:

    /**
     * Node load of the node is completed successfully and data of the node
     * is updated.
     *
     * @param aNode The node whose data is updated.
     * @param aInstalling indicates if the node is being installed (only used by content handler)
     */
    virtual void NodeUpdated( MNcdNode* aNode, TBool aInstalling = EFalse ) = 0;
    
    /**
     * Node load of a parent node is completed.
     *
     * @param aNode The parent node which is loaded.
     * @param aError The completion code. KErrNone if the loading
     * process has been completed successfully. Otherwise some
     * of the system wide error codes.
     */
    virtual void ParentNodeLoadComplete( MNcdNode* aNode, TInt aError ) = 0;
    
    /**
     * All node loads started by this observer are completed.
     */
    virtual void NodeLoadsComplete() = 0;
    
    /**
     * All structure loads started by this observer are completed.
     *
     * @param aError Completion code:
     *   - KErrNone if strcuture loads was completed successfully.
     *   - KErrCancel if the structure load is canceled.
     *   - Otherwise some of the system wide error codes.
     *     NOTE: If a structure load operation fails all ongoing structure
     *     load operations started by this observer are canceled.
     */
    virtual void StructureLoadsComplete( TInt aError ) = 0;
    
    /**
     * Child node load has failed.
     *
     * @param aError Error code.
     */
    virtual void NotifyNodeLoadError( TInt aError ) = 0;


protected:

    /**
     * Protected destructor to prevent unwanted deletion.
     * The object can be deleted only through the inherited
     * classes.
     */
    virtual ~MIAUpdateNodeHandlerObserver() { }

};

#endif // IAD_UPDATE_NODE_HANDLER_OBSERVER_H

