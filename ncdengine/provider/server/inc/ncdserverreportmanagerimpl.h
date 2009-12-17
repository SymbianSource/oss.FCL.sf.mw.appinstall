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
* Description:   Contains CNcdServerReportManager class
*
*/


#ifndef NCD_SERVER_REPORT_IMPL_H
#define NCD_SERVER_REPORT_IMPL_H


#include "catalogscommunicable.h"
#include "ncdproviderimpl.h"

class CNcdNode;
class CNcdNodeMetaData;
class CNcdReportManager;
class MCatalogsHttpSession;


/**
 *  This class object provides node specific functions.
 *  Node contains additional classes that will contain more node
 *  or metadata specific information.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdServerReportManager : public CCatalogsCommunicable
    {
    
public:

    /**
     * NewL
     *
     * @param aProvider
     * @return CNcdServerReportManager* Pointer to the created object 
     * of this class.
     */
    static CNcdServerReportManager* NewL( CNcdProvider& aProvider );
    
    /**
     * @see CNcdServerReportManager::NewL
     */
    static CNcdServerReportManager* NewLC( CNcdProvider& aProvider );


    /**
     * Destructor
     */
    virtual ~CNcdServerReportManager();
    

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
     * @param aProvider
     */
    CNcdServerReportManager( CNcdProvider& aProvider );
    
    /**
     * ConstructL
     */
    virtual void ConstructL();


    CNcdProvider& Provider();
    
    CNcdReportManager& ReportManagerL( MCatalogsBaseMessage& aMessage );
            

    // Here are all the functions that are used when ReceiveMessageL is called and
    // the functionality is delegated according to the given function number.
    
    void SetReportingMethodRequestL( MCatalogsBaseMessage& aMessage );
    void ReportingMethodRequestL( MCatalogsBaseMessage& aMessage );
    void SetReportingStyleRequestL( MCatalogsBaseMessage& aMessage );
    void ReportingStyleRequestL( MCatalogsBaseMessage& aMessage );
    void NodeSetAsInstalledRequestL( MCatalogsBaseMessage& aMessage );
    void ReleaseRequest( MCatalogsBaseMessage& aMessage ) const;

    void UpdateInstallReportAccessPointL( const TUid& aClientUid,
                                          TInt aReportId,
                                          CNcdNode& aNode,
                                          CNcdNodeMetaData& aMetaData,
                                          CNcdReportManager& aReportManager,
                                          MCatalogsHttpSession& aHttpSession );    

    MCatalogsHttpSession& HttpSessionL( MCatalogsContext& aContext );

    
private:
    
    // Prevent if not implemented
    CNcdServerReportManager( const CNcdServerReportManager& aObject );
    CNcdServerReportManager& operator =( const CNcdServerReportManager& aObject );


private: // data

    // The message is set when ReceiveMessage is called. The message
    // is used in the CounterPartLost-function that informs the message
    // if the session has been lost.
    MCatalogsBaseMessage* iMessage;
    
    CNcdProvider& iProvider;
    };


#endif // NCD_SERVER_REPORT_IMPL_H
