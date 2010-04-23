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
* Description:   Contains MNcdNode interface
*
*/


#ifndef NCD_SERVER_REPORT_MANAGER_PROXY_H
#define NCD_SERVER_REPORT_MANAGER_PROXY_H

#include "ncdinterfacebaseproxy.h"
#include "ncdserverreportmanager.h"

class MCatalogsClientServer;
class CNcdProviderProxy;
class CNcdNodeProxy;


/**
 * CNcdServerReportManagerProxy provides proxy side functionality
 * for MNcdServerReportManager interface.
 *
 * @see MNcdServerReportManager
 *
 *
 */
class CNcdServerReportManagerProxy : public CNcdInterfaceBaseProxy,
                                     public MNcdServerReportManager
    {
    
public:

    /**
     * @see CNcdServerReportManagerProxy::CNcdServerReportManagerProxy
     * @return CNcdServerReportManagerProxy* Newly created object.
     */
    static CNcdServerReportManagerProxy* NewL( MCatalogsClientServer& aSession, 
                                               TInt aHandle,
                                               CNcdProviderProxy& aProvider );

    /**
     * @see CNcdServerReportManagerProxy::NewL
     */
    static CNcdServerReportManagerProxy* NewLC( MCatalogsClientServer& aSession, 
                                                TInt aHandle,
                                                CNcdProviderProxy& aProvider );
    

    /**
     * Destructor
     */
    virtual ~CNcdServerReportManagerProxy();


    /**
     * This is meant only for the external installation reporting.
     * 
     * @see CNcdNodeInstallProxy::SetAsInstalledL
     *
     * @param aNode Node that has been tried to install.
     * @param aErroCode Error code describes the success of the install.
     */
    void NodeSetAsInstalledL( CNcdNodeProxy& aNode, TInt aErrorCode );
        

public: // MNcdServerReportManager

    /**
     * @see MNcdServerReportManager::SetReportingMethodL
     */
    virtual void SetReportingMethodL( const TReportingMethod& aMethod );


    /**
     * @see MNcdServerReportManager::ReportingMethodL
     */
    virtual TReportingMethod ReportingMethodL() const;


    /**
     * @see MNcdServerReportManager::SetReportingStyleL
     */
    virtual void SetReportingStyleL( const TReportingStyle& aStyle );


    /**
     * @see MNcdServerReportManager::ReportingStyleL
     */
    virtual TReportingStyle ReportingStyleL() const;


    /**
     * @see MNcdServerReportManager::SendL
     */
    virtual MNcdServerReportOperation* SendL( MNcdServerReportOperationObserver& aObserver );


protected:

    // Prevent these if not implemented
    CNcdServerReportManagerProxy( const CNcdServerReportManagerProxy& aObject );
    CNcdServerReportManagerProxy& operator =( const CNcdServerReportManagerProxy& aObject );


    /**
     * Constructor
     *
     * @param aSession The session between the client proxy and the
     * corresponding server object.
     * @param aHandle The handle which identifies the server object
     * that this proxy uses.
     * @param aProvider Provider that owns this class object.
     *
     *
     */
    CNcdServerReportManagerProxy( MCatalogsClientServer& aSession, 
                                  TInt aHandle,
                                  CNcdProviderProxy& aProvider );
    
    /**
     * ConstructL
     *
     *
     */
    virtual void ConstructL();


    /**
     * @return CNcdProviderProxy Provider that owns this class object.
     */
    CNcdProviderProxy& Provider();


private: // data

    CNcdProviderProxy& iProvider;

    };

#endif // NCD_SERVER_REPORT_MANAGER_PROXY_H

