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
* Description:   Contains CNcdNodeSearch class
*
*/


#ifndef C_NCD_NODE_SEARCH_IMPL_H
#define C_NCD_NODE_SEARCH_IMPL_H


// For the streams
#include <s32mem.h>

#include "catalogsinterfacebase.h"
#include "ncdnodesearch.h"

class CNcdNodeProxy;
class CNcdOperationManagerProxy;


/**
 *  This class implements the functionality for the
 *  MNcdNodeSearch interfaces. The interface is provided for
 *  API users.
 *
 *  @since S60 v3.2
 */
class CNcdNodeSearch : public CCatalogsInterfaceBase, 
                       public MNcdNodeSearch
    {

public:

    /**
     * @param aNode Owns and uses the information of this class object.
     * The node also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @param aOperationManager This class uses operation manager to get the
     * operations for certain tasks. 
     * @return CNcdNodeSearch* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeSearch* NewL(
        CNcdNodeProxy& aNode,
        CNcdOperationManagerProxy& aOperationManager );

    /**
     * @param aNode Owns and uses the information of this proxy.
     * The node also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @param aOperationManager This class uses operation manager to get the
     * operations for certain tasks. 
     * @return CNcdNodeSearch* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeSearch* NewLC(
        CNcdNodeProxy& aNode,
        CNcdOperationManagerProxy& aOperationManager );


    /**
     * Destructor.
     * The destructor is set public. So, the node that owns the Search may
     * delete it directly when the reference count of the node reaches zero
     * and the destructor of the node is called.
     */
    virtual ~CNcdNodeSearch();
         

public: // MNcdNodeSearch

    /**
     * @see MNcdNodeSearch::SearchL
     */ 
    virtual MNcdSearchOperation* SearchL( MNcdLoadNodeOperationObserver& aObserver,
                                          MNcdSearchFilter& aSearchFilter );


protected:

    /**
     * Constructor
     *
     * @param aNode Owns and uses the information of this class object.
     * The node also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @param aOperationManager This class uses operation manager to get the
     * operations for certain tasks. 
     * @return CNcdNodeSearch* Pointer to the created object 
     * of this class.
     */
    CNcdNodeSearch(
        CNcdNodeProxy& aNode,
        CNcdOperationManagerProxy& aOperationManager );

    /**
     * ConstructL
     */
    virtual void ConstructL();
    
    
    /**
     * @return CNcdOperationManagerProxy& Gives the operation manager proxy.
     * Operation manager may be asked to create new operations for this class
     * object.
     */
    CNcdOperationManagerProxy& OperationManager() const;


    /**
     * @return CNcdNodeProxy& Gives the node proxy that owns this class object.
     * This node may be given for the operations. So, the operations may
     * inform the node about the progress of or about the completion of the
     * operation.
     */
    CNcdNodeProxy& Node() const;


private:

    // Prevent if not implemented
    CNcdNodeSearch( const CNcdNodeSearch& aObject );
    CNcdNodeSearch& operator =( const CNcdNodeSearch& aObject );
    
    
private: // data

    CNcdNodeProxy& iNode;
    CNcdOperationManagerProxy& iOperationManager;
    
    };


#endif // C_NCD_NODE_SEARCH_IMPL_H
