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
* Description:   Contains CNcdNode class
*
*/


#ifndef NCD_NODE_H
#define NCD_NODE_H


#include <e32cmn.h>
#include <s32mem.h>

#include "catalogscommunicable.h"
#include "ncdstoragedataitem.h"
#include "ncdnodemanager.h"
#include "ncdnodeclassids.h"
#include "ncdnodeidentifier.h"
// This is included for the state enum value
#include "ncdnode.h"

class CNcdNodeLink;
class CNcdNodeMetaData;
class CNcdNodeIdentifier;
class CNcdNodeUserData;
class CNcdNodeSeen;


/**
 *  This class object provides node specific functions.
 *  Node contains additional classes that will contain more node
 *  or metadata specific information.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdNode : public CCatalogsCommunicable,
                 public MNcdStorageDataItem
    {
public:

    static TInt Compare( const CNcdNode& aFirst, const CNcdNode& aSecond )
        {
        return CNcdNodeIdentifier::Compare( 
            aFirst.Identifier(), 
            aSecond.Identifier() );
        }
        
public:

    /**
     * Destructor
     */
    virtual ~CNcdNode();
    

    /**
     * @return CNcdNodeIdentifier& Unique identifier of this node.
     */
    virtual const CNcdNodeIdentifier& Identifier() const;
    
    /**
     * Retrieves the data type that informs what class the data is for.
     * By checking the data type information, an InternalizeL function
     * of a right class can be called when the object data is set
     * from the storage.
     * The data type may be decided and set in a factory that creates object.
     * The factory should know which integer is reserved for which class.
     *
     * @return TInt The integer that describes the data type. 
     */
    NcdNodeClassIds::TNcdNodeClassId ClassId() const;


    /**
     * @return The nodemanager that has created this node.
     */
    CNcdNodeManager& NodeManager() const;


    /**
     * Uses CreateLinkL to create the link if the link does not already
     * exist. If a new link is created it is set as a link for the node.
     *
     * @return CNcdNodeLink& The link that has been set for this node.
     */
    CNcdNodeLink& CreateAndSetLinkL();

    /**
     * @return CNcdNodeLink& Reference to the node link. 
     * @exception KErrNotFound if node link has not been set.
     */
    CNcdNodeLink& NodeLinkL() const;

    /**
     * @return CNcdNodeLink Pointer to the node link. NULL if no link
     * is set. Ownership is NOT transferred. 
     */
    CNcdNodeLink* NodeLink() const;
    
    /** 
     * @return The node seen object of the node.
     */
    CNcdNodeSeen& NodeSeen() const; 
   
 
    /**
     * Internalizes data to the node link. If the node link does
     * not exists it is created by callin CreateLinkL which should be
     * overloaded in child classes to give the right kind of link.
     *
     * @param
     * @param
     */
    virtual void InternalizeLinkL( const MNcdPreminetProtocolEntityRef& aData,
                                   const CNcdNodeIdentifier& aParentIdentifier,
                                   const CNcdNodeIdentifier& aRequestParentIdentifier,
                                   const TUid& aClientUid );
    
    
    /**
     * @note Node manager is used to set the metadata
     * that corresponds to the node link.
     *
     * @return CNcdNodeMetaData& Reference to the metadata.
     * @exception KErrNotFound if meta data or node link has not been set.
     */
    CNcdNodeMetaData& NodeMetaDataL() const;
    
    /**
     * @return CNcdNodeMetaData* Pointer to the metadata, NULL if no metadata
     * is set. Ownership is NOT transferred.
     */
    CNcdNodeMetaData* NodeMetaData() const;

    /**
     * Sets the metadata. Does not delete old one because metadata is owned
     * byt the nodemanager.
     *
     * @param aMetaData is the new meta data that this node will contain.
     * @exception KErrBadName if the metadata is not of the accepted type.
     */
    void SetNodeMetaDataL( CNcdNodeMetaData& aMetaData );
    
    /**
     * Gives information about the state of the node. This information can be used to
     * check whether the item data has been loaded or if the data is expired.
     *
     * @return The state of the node.
     */
    MNcdNode::TState State() const;
        

public: // CCatalogsCommunicable

    /**
     * @see CCatalogsCommunicable::ReceiveMessage
     */
    virtual void ReceiveMessage( MCatalogsBaseMessage* aMessage,
                                 TInt aFunctionNumber );

    /**
     * @see CCatalogsCommunicable::CounterPartLost
     */
    virtual void CounterPartLost( const MCatalogsSession& aSession );


public: // MNcdStorageDataItem

    /**
     * @see MNcdStorageDataItem::ExternalizeL
     */
    virtual void ExternalizeL( RWriteStream& aStream );


    /**
     * @see MNcdStorageDataItem::InternalizeL
     */
    virtual void InternalizeL( RReadStream& aStream );


protected:

    /**
     * Constructor
     */
    CNcdNode( CNcdNodeManager& aNodeManager,
              NcdNodeClassIds::TNcdNodeClassId aNodeClassId,
              NcdNodeClassIds::TNcdNodeClassId aAcceptedLinkClassId,
              NcdNodeClassIds::TNcdNodeClassId aAcceptedMetaDataClassId );
    
    /**
     * @param atIdentifier Identifier of this node. The identifier is unique
     * to all the nodes.
     */
    virtual void ConstructL( const CNcdNodeIdentifier& aIdentifier );
        

    /**
     * Child classes should implement this function. 
     * So, the right type of link will be created when this creator is called.
     * This function will create the link only once. If the link has already 
     * been created then the old link will be returned.
     *
     * @param CNcdNodeLink* The created link or the old link if it already existed.
     */
    virtual CNcdNodeLink* CreateLinkL() = 0;


    // Here are all the functions that are used when ReceiveMessageL is called and
    // the functionality is delegated according to the given function number.
    
    /**
     *
     */
    void InternalizeRequestL( MCatalogsBaseMessage& aMessage ) const;

    /**
     * Child classes should overload this or add their own data after
     * the parent class data.
     */    
    virtual void ExternalizeDataForRequestL( RWriteStream& aStream ) const;    


    void LinkHandleRequestL( MCatalogsBaseMessage& aMessage ) const;
    void MetadataHandleRequestL( MCatalogsBaseMessage& aMessage ) const;
    void NodeSeenHandleRequestL( MCatalogsBaseMessage& aMessage ) const;
    void ClassIdRequestL( MCatalogsBaseMessage& aMessage ) const;
    void ReleaseRequest( MCatalogsBaseMessage& aMessage ) const;
    
    
private:
    
    // Prevent if not implemented
    CNcdNode( const CNcdNode& aObject );
    CNcdNode& operator =( const CNcdNode& aObject );


    // This is used to forward the internalization/externalization
    // to the link
    void ExternalizeLinkL(  RWriteStream& aStream ); 
    void InternalizeLinkL( RReadStream& aStream );

    void DoReceiveMessageL( 
        MCatalogsBaseMessage& aMessage, 
        TInt aFunctionNumber );


private: // data

    // Manager manages and owns nodes.
    // This class does not own the manager.    
    CNcdNodeManager& iNodeManager;

    NcdNodeClassIds::TNcdNodeClassId iNodeClassId;

    // The link type is used to check if the given link is of
    // the right type. 
    NcdNodeClassIds::TNcdNodeClassId iAcceptedLinkClassId;
    
    // The metadata type is used to check if the given metadata is of
    // the right type. 
    NcdNodeClassIds::TNcdNodeClassId iAcceptedMetaDataClassId;

    // This is the identifier of the node. Owns.
    CNcdNodeIdentifier* iNodeIdentifier;
    
protected:
    // The message is set when ReceiveMessage is called. The message
    // is used in the CounterPartLost-function that informs the message
    // if the session has been lost.
    MCatalogsBaseMessage* iMessage;
    
private:
    // These objects provide additional functionality for the node.

    // This class owns node link if it has been set.
    CNcdNodeLink* iNodeLink;

    // This class does not own meta data.
    CNcdNodeMetaData* iNodeMetaData;
    
    // This class owns the seen information.
    CNcdNodeSeen* iNodeSeen;
    };


#endif // NCD_NODE_H
