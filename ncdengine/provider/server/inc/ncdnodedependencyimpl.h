/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains CNcdNodeDependency class
*
*/


#ifndef NCD_NODE_DEPENDENCY_H
#define NCD_NODE_DEPENDENCY_H


// For streams
#include <s32mem.h>
// For array
#include <e32cmn.h>

#include "ncdcommunicable.h"
#include "ncdstoragedataitem.h"
#include "ncdnodeclassids.h"
#include "ncdproviderutils.h"
#include "ncdinstallationservice.h"
#include "ncdpurchasedownloadinfo.h"

class MNcdPreminetProtocolDataEntity;
class CNcdDependencyInfo;
class CNcdNodeMetaData;
class CNcdDownloadInfo;
class MNcdPurchaseDetails;
class CNcdPurchaseDetails;
class MNcdPreminetProtocolEntityDependency;

/**
 *  This server side class contains the data and the functionality
 *  that the proxy objects will use to internalize itself.
 *
 *  This object should be added to the session. So, it will be usable
 *  in the proxy side by using the handle gotten during addition. 
 *  The handle is used to identify to what object the proxy directs 
 *  the function call.  When objects are added to sessions, 
 *  multiple handles may be gotten for the same object if addition is 
 *  done multiple times.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdNodeDependency : public CNcdCommunicable,
                           public MNcdStorageDataItem
    {

public:
    /**
     * NewL
     *
     * @param aParentMetaData Metadata that owns this class object.
     * @return CNcdNodeDependency* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeDependency* NewL( CNcdNodeMetaData& aParentMetaData );

    /**
     * NewLC
     *
     * @param aParentMetaData Metadata that owns this class object.
     * @return CNcdNodeDependency* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeDependency* NewLC( CNcdNodeMetaData& aParentMetaData );


    /**
     * Destructor
     *
     * @note Because this is CCatalogsCommunicable function the
     * session that owns this object should delete this class object.
     * So, instead of directly deleting this object from some other
     * class. Close-method should be used instead.
     */
    virtual ~CNcdNodeDependency();


    /**
     * Retrieves the data type that informs what class the data is for.
     * By checking the data type information, an InternalizeL function
     * of a right class can be called when the object data is set
     * from the storage.
     * The data type may be decided and set in a object that creates this 
     * class object.
     *
     * @return NcdNodeClassIds::TNcdNodeClassId Describes the data type. 
     */
    NcdNodeClassIds::TNcdNodeClassId ClassId() const;


    /**
     * @return RPointerArray<CNcdDownloadInfo>& Contains list of content targets. 
     * This list provides objects that contain information about the contents 
     * that this class object may depend from.
     */
    const RPointerArray<CNcdDownloadInfo>& ContentTargets() const;


    /**
     * @return RPointerArray<CNcdDependencyInfo>& Contains list of node targets. 
     * This list provides objects that contain metadata ids and other information
     * of the nodes that this class object may depend on.
     */
    const RPointerArray<CNcdDependencyInfo>& NodeTargets() const;
    

    /**
     * This function is called when the owner of this object
     * wants to internalize the content according to the data
     * that has been received from the parser.
     * 
     * @param aData The data is set in the protocol parser and can
     * be used to initialize this class object.
     */
    void InternalizeL( MNcdPreminetProtocolDataEntity& aData );
   

    /**
     * Internalize from purchase history
     *
     * @param aDetails Purchase details
     */
    TBool InternalizeFromPurchaseDetailsL( const MNcdPurchaseDetails& aDetails );
    
    
    
    /**
     * Updates dependency states and checks if they are all installed
     *
     * @return ETrue if all dependencies are installed (upgrades may be available)
     */
    TBool UpdateDependencyStatesL();
    
    
    /**
     * Updates current dependencies to given purchase details
     */
    void UpdateDependenciesL( CNcdPurchaseDetails& aDetails ) const;
    
public: // MNcdStorageDataItem 

    // These functions are used to get the data from and to insert the data
    // into the database using by the given stream.

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
     * @param aParentMetaData Metadata that owns this class object.
     * @param aClassId Identifies this class. 
     * Is set in the NewLC function 
     */
    CNcdNodeDependency( CNcdNodeMetaData& aParentMetaData,
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
    void InternalizeRequestL( MCatalogsBaseMessage& aMessage );
        
    /**
     * This function writes the object data to the stream. 
     * The stream content will be sent to the proxy that requested the data.
     * Child classes should add their own data after this parent data.
     * @param aStream The data content of this class object will be written
     * into this stream.
     */
    virtual void ExternalizeDataForRequestL( RWriteStream& aStream );

    /**
     * This function is called from the proxy side when the proxy
     * is deleted.
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy
     */
    void ReleaseRequest( MCatalogsBaseMessage& aMessage ) const;

private:

    // Prevent these two if they are not implemented
    CNcdNodeDependency( const CNcdNodeDependency& aObject );
    CNcdNodeDependency& operator =( const CNcdNodeDependency& aObject );


    // Functions to help externalize and internalize data
    void ExternalizeNodeDependencyArrayL( RWriteStream& aStream );
    void InternalizeNodeDependencyArrayL( RReadStream& aStream );

    void ExternalizeContentDependencyArrayForRequestL( RWriteStream& aStream );
    void ExternalizeContentDependencyArrayL( RWriteStream& aStream );
    void InternalizeContentDependencyArrayL( RReadStream& aStream );


    void MoveOldContentTargetsToNew();
    
    // Updates the state of the dependency
    // Returns ETrue if the dependency is installed
    TBool UpdateDependencyStateL( CNcdDownloadInfo& aContentTarget );
    
    // Sets content targets state 
    // Returns ETrue if the aStatus indicates that the dependency
    // or a new version of it is installed
    TBool SetDependencyState( 
        CNcdDownloadInfo& aContentTarget,
        TNcdApplicationStatus aStatus,
        TInt aVersionCompResult,
        TBool aIsUpgrade );

    /**
     * Sets content usage depending on the dependency name
     *
     * @return ETrue if the dependency is a launcher application
     */
    TBool SetContentUsage( 
        const MNcdPreminetProtocolEntityDependency& aDependency, 
        CNcdDownloadInfo& aTarget ) const;
    
    
    MNcdPurchaseDownloadInfo::TContentUsage DetermineContentUsage(
        const TDesC& aDependencyName ) const;
    
private: // data
    
    // Metadata that owns this class object.
    CNcdNodeMetaData& iParentMetaData;
    
    // The class id identifies this class. The id may be used to identify
    // what kind of class object is created when data is gotten from the db.
    NcdNodeClassIds::TNcdNodeClassId iClassId;

    // The message is set when ReceiveMessage is called. The message
    // is used in the CounterPartLost-function that informs the message
    // if the session has been lost.
    MCatalogsBaseMessage* iMessage;
    
    RPointerArray<CNcdDependencyInfo> iDependencyNodeTargets;
    RPointerArray<CNcdDownloadInfo> iDependencyContentTargets;
    
    // Dependency content targets internalized from purchase history
    // exist only if the item has been bought before, used for
    // checking upgrade status
    RPointerArray<CNcdDownloadInfo> iOldDependencyContentTargets;
    
    };
    
#endif // NCD_NODE_DEPENDENCY_H    
