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
* Description:   CNcdSilentInstallOperationProxy declaration
*
*/


#ifndef C_NCDSILENTINSTALLOPERATIONPROXY_H	
#define C_NCDSILENTINSTALLOPERATIONPROXY_H	

#include <e32cmn.h>

// For silent install options and options package
#include <usif/sif/sifcommon.h>

#include "ncdinstalloperationproxy.h"



class CNcdSilentInstallOperationProxy : public CNcdInstallOperationProxy
    {
public:

    /**
     * Constructor
     *
     * @param aSession is the session that is used between
     *                 the proxy and the server.
     * @param aDownloadType Download type
     * @param aHandle is the handle that identifies the serverside
     *                object that this proxy uses.
     * @param aRemoveHandler An observer that is called during destruction.
     * @param aNode The node this operation was started from.
     * @param aInstallOptions These options are used for the silent install.
     */    
    static CNcdSilentInstallOperationProxy* NewLC( MCatalogsClientServer& aSession,                        
                        TInt aHandle,
                        MNcdOperationProxyRemoveHandler* aRemoveHandler,
                        CNcdNodeProxy* aNode,
                        CNcdNodeManagerProxy* aNodeManager,
                        MNcdInstallOperationObserver* aObserver,
                        MNcdInstallationService& aInstallationService,
                        Usif::COpaqueNamedParams* aInstallOptions );


protected: // CActive

    /**
     * @see CActive::DoCancel
     */    
    void DoCancel();

    
protected:

    /**
     * Constructor
     *
     * @see NewLC
     */
    CNcdSilentInstallOperationProxy( MNcdInstallationService& aInstallationService,
                                     Usif::COpaqueNamedParams* aInstallOptions );
    
    virtual ~CNcdSilentInstallOperationProxy();


    /**
     * ConstructL
     *
     * @param aSession is the session that is used between
     *                 the proxy and the server.
     * @param aHandle is the handle that identifies the serverside
     *                object that this proxy uses.
     * @param aRemoveHandler An observer that is called during destruction.
     * @param aNode The node this operation was started from.
     */
    void ConstructL( MCatalogsClientServer& aSession,
                    TInt aHandle,
                    MNcdOperationProxyRemoveHandler* aRemoveHandler,
                    CNcdNodeProxy* aNode,
                    CNcdNodeManagerProxy* aNodeManager,
                    MNcdInstallOperationObserver* aObserver );
    

    /**
     * @see CNcdInstallOperationProxy::UseInstallServiceL
     */
    virtual void UseInstallServiceL( const CNcdFileInfo& aFile );
    
    /**
     * @return const Usif::COpaqueNamedParams* Contains the install options 
     * for silent install in a data buffer.
     */
    const Usif::COpaqueNamedParams* SilentInstallOptions() const;
    
    
private: // new methods

    // Prevent these if not implemented
    CNcdSilentInstallOperationProxy( const CNcdSilentInstallOperationProxy& aObject );
    CNcdSilentInstallOperationProxy& operator =( const CNcdSilentInstallOperationProxy& aObject );
    

private: // data

    
    // This will contain the install options for silent install in a data buffer.
    Usif::COpaqueNamedParams* iInstallOptions;

    };

#endif // C_NCDSILENTINSTALLOPERATIONPROXY_H
