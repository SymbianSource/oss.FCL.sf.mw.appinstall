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
* Description:   Contains CNcdNodeUpgrade class
*
*/


#ifndef NCD_NODE_UPGRADE_H
#define NCD_NODE_UPGRADE_H


// For streams
#include <s32mem.h>
// For array
#include <e32cmn.h>

#include "ncdcommunicable.h"
#include "ncdstoragedataitem.h"
#include "ncdnodeclassids.h"

class MNcdPreminetProtocolDataEntity;
class CNcdDependencyInfo;
class CNcdNodeMetaData;
class CNcdDownloadInfo;
class CNcdAttributes;


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
class CNcdNodeUpgrade : public CNcdCommunicable,
                        public MNcdStorageDataItem
    {
public:

    enum TUpgradeData
        {
        EUpgradeDataVersion = 0,
        EUpgradeDataUid,
        
        // Keep this last
        EUpgradeDataInternal
        };
public:
    /**
     * NewL
     *
     * @param aParentMetaData Metadata that owns this class object.
     * @return CNcdNodeUpgrade* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeUpgrade* NewL( CNcdNodeMetaData& aParentMetaData );

    /**
     * NewLC
     *
     * @param aParentMetaData Metadata that owns this class object.
     * @return CNcdNodeUpgrade* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeUpgrade* NewLC( CNcdNodeMetaData& aParentMetaData );


    /**
     * Destructor
     *
     * @note Because this is CCatalogsCommunicable function the
     * session that owns this object should delete this class object.
     * So, instead of directly deleting this object from some other
     * class. Close-method should be used instead.
     */
    virtual ~CNcdNodeUpgrade();


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
     * that this class object can upgrade.
     */
    const RPointerArray<CNcdDownloadInfo>& ContentTargets() const;


    /**
     * @return RPointerArray<CNcdDependencyInfo>& Contains list of node targets. 
     * This list provides objects that contain metadata ids and other information 
     * of the nodes that this class object can upgrade.
     */
    const RPointerArray<CNcdDependencyInfo>& NodeTargets() const;
    

    /**
     * This function is called when the owner of this object
     * wants to internalize the content according to the data
     * that has been received from the parser.
     * 
     * @param aData The data is set in the protocol parser and can
     * be used to initialize this class object.
     * @param aVersion Version number of this item. Used to check whether this actually
     * upgrades anything     
     */
    void InternalizeL( MNcdPreminetProtocolDataEntity& aData, const TDesC& aVersion );


    /**
     * Checks if all of the upgradable items have been installed already     
     */
    TBool AllUpgradesInstalledL() const;
    
    /**
     * Sets content upgrade data
     *
     * If aContentUpgrades is true, aUid and aVersion will be 
     * available on the proxy side
     */
    void SetContentUpgradesL( 
        TBool aContentUpgrades, 
        const TUid& aUid,
        const TDesC& aVersion );
    
    TBool ContentUpgrades() const;
    
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
    CNcdNodeUpgrade( CNcdNodeMetaData& aParentMetaData,
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
    CNcdNodeUpgrade( const CNcdNodeUpgrade& aObject );
    CNcdNodeUpgrade& operator =( const CNcdNodeUpgrade& aObject );


    // Functions to help externalize and internalize data
    void ExternalizeNodeUpgradeArrayL( RWriteStream& aStream );
    void InternalizeNodeUpgradeArrayL( RReadStream& aStream );

    void ExternalizeContentUpgradeArrayForRequestL( RWriteStream& aStream );
    void ExternalizeContentUpgradeArrayL( RWriteStream& aStream );
    void InternalizeContentUpgradeArrayL( RReadStream& aStream );


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
    
    RPointerArray<CNcdDependencyInfo> iUpgradeNodeTargets;
    RPointerArray<CNcdDownloadInfo> iUpgradeContentTargets;
    
    TBool iContentUpgrades;
    
    // If this node is an content upgrade (content version has been
    // updated in the server) this contains the old version number    
    CNcdAttributes* iUpgradeData;
    };
    
#endif // NCD_NODE_UPGRADE_H    
