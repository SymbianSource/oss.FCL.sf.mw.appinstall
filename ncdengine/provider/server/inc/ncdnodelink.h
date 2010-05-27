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
* Description:   Contains CNcdNodeLink class
*
*/


#ifndef NCD_NODE_LINK_H
#define NCD_NODE_LINK_H


#include <e32std.h>
#include <e32cmn.h>
#include <s32mem.h>

#include "catalogscommunicable.h"
#include "ncdnodemanager.h"
#include "ncdstorable.h"
#include "ncdnodeclassids.h"
#include "ncd_pp_entityref.h"

class CNcdNodeIdentifier;
class CNcdNode;


/**
 *  CNcdNodeLink ...
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdNodeLink : public CCatalogsCommunicable,
                     public MNcdStorable
    {

public:

    /**
     * Destructor
     */
    virtual ~CNcdNodeLink();


    /**
     * Retrieves the data type that informs what class the data is for.
     * By checking the data type information, an InternalizeL function
     * of a right class can be called when the object data is set
     * from the storage.
     * The data type may be decided and set in a factory that creates object.
     * The factory should know which integer is reserved for which class.
     *
     * @return NcdNodeClassIds::TNcdNodeClassId Describes the data type. 
     */
    NcdNodeClassIds::TNcdNodeClassId ClassId() const;

    
    /**
     * @see MNcdPreminetProtocolEntityRef::Timestamp
     */
    const TDesC& Timestamp() const;

    /**
     * @see MNcdPreminetProtocolEntityRef::ServerUri
     */
    const TDesC& ServerUri() const;
    
    /**
     * Setter for server uri.
     *
     * @param aServerUri Uri to set.
     */
    void SetServerUriL( const TDesC& aServerUri );

    /**
     * @see MNcdPreminetProtocolEntityRef::RemoteUri
     */
    const TDesC& RemoteUri() const;

    /**
     * @see MNcdPreminetProtocolEntityRef::Description
     */
    MNcdPreminetProtocolEntityRef::TDescription Description() const;

    /**
     * @see MNcdPreminetProtocolEntityRef::ValidUntilDelta
     */
    TInt ValidUntilDelta() const;
    
    /**
     * Setter for validity time, needed for setting root nodes
     * validity time. Also sets iLinkDownloadTime to current time.
     *
     * @param aValidUntilDelta Validity time.
     */
    void SetValidUntilDelta( TInt aValidUntilDelta );

    /**
     * @see MNcdPreminetProtocolEntityRef::ValidUntilAutoUpdate
     */
    TBool ValidUntilAutoUpdate() const;

    /**
     * @return CNcdNodeIdentifier contains parent identifier infromation.
     */
    const CNcdNodeIdentifier& ParentIdentifier() const;
    
    /**
     * Set the parent identifier.
     *
     * @note This setter function also sets the given identifier to the
     * request parent identifier variable. So, if the request parent identifier
     * should differ from the actual parent identifier, 
     * then the SetRequestParentIdentifierL function has to be called after
     * this function.
     *
     * @param aParentIdentifier The new parent identifier.
     */
    void SetParentIdentifierL( const CNcdNodeIdentifier& aParentIdentifier );


    /**
     * @note The parent identifier differs from the request parent identifier
     * if the parent is transparent. Then the proxy side should get the
     * grandparent info instead of the actual parent identifier.
     *
     * @return CNcdNodeIdentifier Parent identifier that is used when the
     * parent information is sent to the proxy side.
     */
    const CNcdNodeIdentifier& RequestParentIdentifier() const;
    
    /**
     * Set the request parent identifier.
     *
     * @param aRequestParentIdentifier The new parent identifier.
     */
    void SetRequestParentIdentifierL( 
        const CNcdNodeIdentifier& aRequestParentIdentifier );


    /**
     * @return TTIme Time when this link was downloaded.
     */
    TTime LinkDownloadTime() const;

    /**
     * @return TTime Expiration time. Sum of link download time and
     * valid until delta. Notice that valid until delta should give
     * the time value in minutes.
     */
    TTime ExpiredTime() const;
    
    /**
     * Checks whether this link is expired. 
     * @return ETrue if expired, EFalse if not.
     */
    TBool IsExpired() const;

    /**
     * Sets the catalogs source name for this nodelink.
     * The source name is set only for the root node childs.
     * @param aSourceName is the provider name that is gotten from the
     * cp response.
     */
    void SetCatalogsSourceNameL( const TDesC& aSourceName );

    /**
     * @return const TDesC& provider name that has been gotten from the 
     * cp response. This is only set for the root node childs. KNullDesC
     * if the value has not been set.
     */
    const TDesC& CatalogsSourceName() const;

    
    /**
     * @note The node may be a remote node if the parent gets remote uri information
     * for the strcuture data of its child. This child then has to be loaded from
     * the given remote uri. An example is a catalog. This function can be used
     * to set the flag that tells the original purpose of the node. For example, after
     * downloading a catalog it will act as a normal folder but the flag can be used
     * to inform its original purpose. 
     *
     * @param aRemote ETrue if the node is originally a remote node.
     * If the node is directly part of the structure then EFalse.
     */
    void SetRemoteFlag( TBool aRemoteFlag );

    /**
     * @return TBool ETrue if the node is originally a remote node.
     * If the node is directly part of the structure then EFalse.
     */
    TBool RemoteFlag() const;


    /**
     * Returns the identifier of the metadata that this link points to.
     * This should always be used when accessing metadata via a link.
     * 
     * @return const CNcdNodeIdentifier&
     */
    const CNcdNodeIdentifier& MetaDataIdentifier() const;
    
    /**
     * Sets the metadata identifier.
     *
     * @param aIdentifier The identifier of the metadata. Old one will be
     * replaced with this one.
     */
    void SetMetaDataIdentifierL( const CNcdNodeIdentifier& aIdentifier );
    
    
    /**
     * This function is called when the owner of this object
     * wants to internalize the content according to the data
     * that has been received from the parser.
     * 
     * @param aData The data is set in the protocol parser and can
     * be used to initialize this class object.
     * @param aParentIdentifier contains the parent information.
     * @param aRequestParentIdentifier Parent identifier that is used 
     * when the parent information is sent to the proxy side. Usually this
     * is most likely same as aParentIdentifier. But in case of transparent
     * nodes, aRequestParentIdentifier value should be the grandparent identifier
     * instead of the actual parent.
     */
    virtual void InternalizeL( const MNcdPreminetProtocolEntityRef& aData,
                               const CNcdNodeIdentifier& aParentIdentifier,
                               const CNcdNodeIdentifier& aRequestParentIdentifier,
                               const TUid& aClientUid );


    /**
     * Sets metadata's timestamp to link
     */
    void SetMetadataTimeStampL( const TDesC& aTimeStamp );

public: // MNcdStorable

    /**
     * @see MNcdStorageDataItem::ExternalizeL
     */
    virtual void ExternalizeL( RWriteStream& aStream );


    /**
     * @see MNcdStorageDataItem::InternalizeL
     */
    virtual void InternalizeL( RReadStream& aStream );


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


protected:

    /**
     * Constructor
     *
     * @param aNode The node that owns this link.
     * @param aClassId This identifier can be asked to identify
     * the class for example when it is read from db.
     */
    CNcdNodeLink( CNcdNode& aNode,
                  NcdNodeClassIds::TNcdNodeClassId aClassId );

    /**
     * ConstructL
     */
    virtual void ConstructL();
    

    // These functions are called from the ReceiveMessage when
    // the given function id has matched to the function.

    /**
     * This function is called when the proxy wants to get the
     * data from the serverside. This function calls the
     * InternalizeDataForRequestL which may be overloaded in the
     * child classes
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy
     */
    void InternalizeRequestL( MCatalogsBaseMessage& aMessage ) const;

    /**
     * This function writes the object data to the stream. 
     * The stream content will be sent to the proxy that requested the data.
     * Child classes should add their own data after this parent data.
     *
     * @param aStream The data content of this class object will be written
     * into this stream.
     */
    virtual void ExternalizeDataForRequestL( RWriteStream& aStream ) const;    


    /**
     * This function is called from the proxy side. When the proxy
     * is deleted.
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy
     */
    void ReleaseRequest( MCatalogsBaseMessage& aMessage ) const;


    /**
     * @param CNcdNode& Node that owns this link.
     */
    CNcdNode& Node() const;    

    
private:

    // Prevent these two if they are not implemented
    CNcdNodeLink( const CNcdNodeLink& aObject );
    CNcdNodeLink& operator =( const CNcdNodeLink& aObject );


    /**
     * This function is used to insert the right parent identifier 
     * into the stream for the proxy.
     * In the case of the transparent parent, the proxy  
     * gets the grandparent as a parent instead of the real parent.     
     *
     * @param aStream The data content of this class object will be written
     * into this stream.
     */
    void ParentIdentifierForRequestL( RWriteStream& aStream ) const;


private: // data

    // The class id is the id for this class that a factory uses
    // when it creates an object of this class.
    NcdNodeClassIds::TNcdNodeClassId iClassId;
    
    // Node that owns this link
    CNcdNode& iNode;

    HBufC* iTimeStamp;
    HBufC* iCatalogsSourceName;
    HBufC* iRemoteUri;
    HBufC* iServerUri;

    MNcdPreminetProtocolEntityRef::TDescription iDescription;
    TInt iValidUntilDelta;
    TBool iValidUntilAutoUpdate;

    // The actual parent of the node
    CNcdNodeIdentifier* iParentIdentifier;
    
    // The parent identifier that is given to the proxy side
    // when the parent is asked. This may differ from the actual parent
    // identifier for example if parent is transparent. Then the grandparent
    // should be returned to the proxy side instead the actual parent.
    CNcdNodeIdentifier* iRequestParentIdentifier;

    RArray<TInt> iQueries;

    TTime iLinkDownloadTime;

    // The message is set when ReceiveMessage is called. The message
    // is used in the CounterPartLost-function that informs the message
    // if the session has been lost.
    MCatalogsBaseMessage* iMessage;
    
    // This identifies the metadata that this link points to
    CNcdNodeIdentifier* iMetaDataIdentifier;

    // The node may be a remote node if the parent gets remote uri information
    // for the strcuture data of its child. This child then has to be loaded from
    // the given remote uri. An example is a catalog. For example, after
    // downloading a catalog it will act as a normal folder but the flag can be used
    // to inform its original purpose. EFalse means that the node was normal node,
    // ETrue means that the remote uri was originally given.
    TBool iRemoteFlag;

    // Metadata's timestamp is used for expiration checking    
    HBufC* iMetadataTimeStamp;
    };


#endif // NCD_NODE_LINK_H
