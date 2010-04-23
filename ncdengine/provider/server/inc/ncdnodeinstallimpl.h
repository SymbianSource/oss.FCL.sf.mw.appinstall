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
* Description:   Contains CNcdNodeInstall class
*
*/


#ifndef NCD_NODE_INSTALL_H
#define NCD_NODE_INSTALL_H


// For streams
#include <s32mem.h>
#include <badesca.h>

#include "catalogscommunicable.h"
#include "ncdnodeclassids.h"
#include "catalogsutils.h"
#include "ncdinstallationservice.h"

class MNcdPreminetProtocolDataEntity;
class CNcdInstallInfo;
class MNcdPurchaseDetails;
class CNcdExtendedInstallInfo;
class CNcdNodeMetaData;
class MNcdPurchaseDownloadInfo;

/**
 *  CNcdNodeInstall does not handle db information directly. The information
 *  is Internalized from the purchase history data and also the data is saved
 *  to the purchase history when the purchase operation progresses.
 *
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
class CNcdNodeInstall : public CCatalogsCommunicable
    {

public:
    /**
     * NewL
     *
     * @return CNcdNodeInstall* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeInstall* NewL( const CNcdNodeMetaData& aMetadata );

    /**
     * NewLC
     *
     * @return CNcdNodeInstall* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeInstall* NewLC( const CNcdNodeMetaData& aMetadata );


    /**
     * Destructor
     *
     * @note Because this is CCatalogsCommunicable function the
     * session that owns this object should delete this class object.
     * So, instead of directly deleting this object from some other
     * class. Close-method should be used instead.
     */
    virtual ~CNcdNodeInstall();


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
     * This function is called when the owner of this object
     * wants to internalize the content according to the data
     * that has been received from the purchase history.
     * 
     * @param aDetails Purchase details from purchase history.
     * @return ETrue if purchase details contained install
     * data to internalize.
     */
     TBool InternalizeL( const MNcdPurchaseDetails& aDetails );
    

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

public:

    /**
     * Paths of downloaded files
     */
    const MDesCArray& DownloadedFiles() const;
    
    /**
     * Number of installed content
     */
    TInt ContentCount() const;
    
    /**
     * Returns if the content is available (installed & present)
     *
     * @param aIgnoreMissingUri If application doesn't have content URI
     * it's always considered to be installed
     */    
    TNcdApplicationStatus IsContentInstalledL( 
        TInt aIndex, 
        TBool aIgnoreMissingUri );
    
    /**
     * Checks if all of the content is installed
     *
     * @return ETrue if all content files are installed (includes newer versions)
     */
    TBool IsAllContentInstalledL();
        
    
    /**
     * Checks if the application specified in content info is installed
     */
    TBool InternalizeContentInfoL();
    
    /**
     * Returns the content version that was received in downloadableContent's 
     * version attribute. This value is read from purchase history, not
     * CNcdContentInfo
     */
    const TCatalogsVersion& ContentVersion() const;

public:

    /**
     * Used for delayed file opening
     */
    void OpenContentFileRunner();

protected:
        
    /**
     * Constructor
     * @param aClassId Identifies this class. 
     * Is set in the NewLC function 
     */
    CNcdNodeInstall( NcdNodeClassIds::TNcdNodeClassId aClassId,
        const CNcdNodeMetaData& aMetadata );

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

    /**
     * This function is called from the proxy side when some application
     * is installed separately outside of the NCD Engine and the install 
     * status should be updated into the purchase history.
     * @param aMessage Contains data from the proxy and can be used
     * to send data back to proxy
     */
    void SetApplicationInstalledRequestL( MCatalogsBaseMessage& aMessage );

protected:
    
    void OpenContentFileL( MCatalogsBaseMessage& aMessage );
    
    
    void OpenContentFileL();

    TInt DependencyCount( 
        const TArray<MNcdPurchaseDownloadInfo*>& aInfos ) const;


    void SetContentType( 
        CNcdExtendedInstallInfo& aInstall,
        const MNcdPurchaseDownloadInfo& aDownload ) const;
        
    void SetLaunchParameterL( 
        RPointerArray<CNcdExtendedInstallInfo>& aInstalls, 
        TInt aIndex ) const;
        
    // Sets iContentVersion from aVersion
    void SetContentVersionL( const TDesC& aVersion );

private:

    // Prevent these two if they are not implemented
    CNcdNodeInstall( const CNcdNodeInstall& aObject );
    CNcdNodeInstall& operator =( const CNcdNodeInstall& aObject );

   

private: // data
    
    // The class id identifies this class. The id may be used to identify
    // what kind of class object is created when data is gotten from the db.
    NcdNodeClassIds::TNcdNodeClassId iClassId;


    const CNcdNodeMetaData& iMetadata;

    // The message is set when ReceiveMessage is called. The message
    // is used in the CounterPartLost-function that informs the message
    // if the session has been lost.
    MCatalogsBaseMessage* iMessage;
    
    TBool iInstalled;                
    TBool iLaunchable;
        
    
    RPointerArray<CNcdExtendedInstallInfo> iInstallInfos;
    TUint iPurpose;
    CDesCArray* iDownloadedFiles;
    
    // Install info from Content info
    CNcdExtendedInstallInfo* iInstalledContent;
    
    // Used for delayed file open attempts
    CPeriodic* iPeriodic;
        
    // Current file open attempt number    
    TInt iRetryCount;
    
    // File index of the file that is being opened
    TInt iFileIndex;
    
    // Version number from purchase history
    TCatalogsVersion iContentVersion;
    };
    
#endif // NCD_NODE_Install_H    
