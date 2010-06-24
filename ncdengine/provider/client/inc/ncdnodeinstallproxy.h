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
* Description:   Contains CNcdNodeInstallProxy class
*
*/


#ifndef C_NCD_NODE_INSTALL_PROXY_H
#define C_NCD_NODE_INSTALL_PROXY_H


// For the streams
#include <s32mem.h>

#include "ncdinterfacebaseproxy.h"
#include "ncdnodeinstall.h"
#include "ncdinstalloperationobserver.h"
#include "ncditempurpose.h"

class CNcdNodeMetadataProxy;
class CNcdInstallInfo;
class MNcdInstallationService;
class CNcdExtendedInstallInfo;

/**
 *  This class implements the functionality for the
 *  MNcdNodeInstall interfaces. The interface is provided for
 *  API users.
 *
 *  @since S60 v3.2
 */
class CNcdNodeInstallProxy : public CNcdInterfaceBaseProxy, 
                             public MNcdNodeInstall,
                             public MNcdInstallOperationObserver
    {

public:

    /**
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aMetadata Owns and uses the information of this proxy.
     * The metadata also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @return CNcdNodeInstallProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeInstallProxy* NewL(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdNodeMetadataProxy& aMetadata );

    /**
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aMetadata Owns and uses the information of this proxy.
     * The metadata also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @return CNcdNodeInstallProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeInstallProxy* NewLC(
        MCatalogsClientServer& aSession, 
        TInt aHandle,
        CNcdNodeMetadataProxy& aMetadata );


    /**
     * Destructor.
     * The destructor is set public. So, the node that owns the Install may
     * delete it directly when the reference count of the node reaches zero
     * and the destructor of the node is called.
     */
    virtual ~CNcdNodeInstallProxy();


    /**
     * @return CNcdNodeMetadataProxy& Gives the proxy that owns this class object.
     */
    CNcdNodeMetadataProxy& Metadata() const;


    /**
     * Gets the data for descriptors from the server side. This function is
     * called to update proxy data. The function uses the protected virtual
     * internalize functions. So, the child classes may provide their own
     * implementations for internalizations of certain metadata.
     */
    void InternalizeL();
    

    /**
     * The item may have multiple purposes. The given purpose is compared
     * to the item purposes and returned boolean value informs if 
     * the item has the given purpose.
     * 
     * @return ETrue if the item purpose is specified to contain the
     * given purpose. EFalse if the item purpose does not match the
     * given purpose.
     */
    TBool IsPurpose( TNcdItemPurpose aPurpose ) const;
    

public: // MNcdNodeInstall

    /**
     * @see MNcdNodeInstall::InstallL()
     */ 
    MNcdInstallOperation* InstallL( MNcdInstallOperationObserver& aObserver );

    /**
     * @see MNcdNodeInstall::SilentInstallL()
     */ 
    MNcdInstallOperation* SilentInstallL( 
                MNcdInstallOperationObserver& aObserver,
                Usif::COpaqueNamedParams* aInstallOptions );
    /**
     * @see MNcdNodeInstall::IsInstalledL()
     */ 
    TBool IsInstalledL() const;

    
    /**
     * @see MNcdNodeInstall::IsLaunchable()
     */
    TBool IsLaunchable() const;
    
    
    /**
     * @see MNcdNodeInstall::InstalledContentL()
     */
    RCatalogsArray<MNcdInstalledContent> InstalledContentL();


    /**
     * @see MNcdNodeInstall::SetApplicationInstalledL()
     */
    void SetApplicationInstalledL( TInt aErrorCode );
    

public: // New methods

    /**
     * Installation service getter
     */
    MNcdInstallationService& InstallationService();
    

    /**
     * File session getter
     */
    RFs& FileSession();
    
    
    /**
     * File opener
     */
    RFile OpenFileL( TInt aFileIndex );

    
public: // From MNcdInstallOperationObserver

    /**
     * @see MNcdInstallOperationObserver::InstallProgress()
     */
    void InstallProgress( MNcdInstallOperation& aOperation,
        TNcdProgress aProgress );

    /**
     * @see MNcdInstallOperationObserver::QueryReceived()
     */
    void QueryReceived( MNcdInstallOperation& aOperation,
        MNcdQuery* aQuery );

    /**
     * @see MNcdInstallOperationObserver::OperationComplete()
     */
    void OperationComplete( MNcdInstallOperation& aOperation, TInt aError );
    

protected:

    /**
     * Constructor
     *
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aMetadata Owns and uses the information of this proxy.
     * The metadata also keeps track of the refence counts of its objects. 
     * And handles the deletion of these objects when the total reference 
     * count reaches zero.
     * @return CNcdNodeInstallProxy* Pointer to the created object 
     * of this class.
     */
    CNcdNodeInstallProxy(
        MCatalogsClientServer& aSession,
        TInt aHandle,
        CNcdNodeMetadataProxy& aMetadata );

    /**
     * ConstructL
     */
    virtual void ConstructL();
    
    
    // These functions are used to update the data of this class object

    /**
     * @param aStream This stream will contain all the data content for
     * this class object. The stream is gotten from the server side. The
     * memeber variables will be updated according to the data from
     * the stream. 
     */
    virtual void InternalizeDataL( RReadStream& aStream );

    
private:

    // Prevent if not implemented
    CNcdNodeInstallProxy( const CNcdNodeInstallProxy& aObject );
    CNcdNodeInstallProxy& operator =( const CNcdNodeInstallProxy& aObject );

    void CreateInstalledContentL(
        const CNcdExtendedInstallInfo& aInfo, 
        TInt aFileIndex );
    
    
private: // data

    CNcdNodeMetadataProxy& iMetadata;
    
    MNcdInstallOperationObserver* iOperationObserver;
    
    TBool iInstalled; // Installed status
    
    TBool iLaunchable; // Is launchable
    
    
    // item's purpose
    TUint iPurpose;  
    
    // Installation service, not owned
    MNcdInstallationService* iInstallationService;      
    
    RCatalogsArray<MNcdInstalledContent> iContent;
    };


#endif // C_NCD_NODE_INSTALL_PROXY_H
