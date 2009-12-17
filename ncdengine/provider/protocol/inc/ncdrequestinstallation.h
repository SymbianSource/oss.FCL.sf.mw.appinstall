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
* Description:   CNcdRequestInstallation declaration
*
*/


#ifndef C_NCDREQUESTINSTALLATION_H
#define C_NCDREQUESTINSTALLATION_H

#include <e32std.h>
#include <e32base.h>
#include "ncdrequestbase.h"
#include "ncdrequestconstants.h"


class CNcdRequestInstallation : public CNcdRequestBase
    {
public:


struct TNcdRequestInstallation
    {
    TXmlEngString iId;
    // These are needed if the installation report functionality is added
    //TXmlEngString iTimestamp;
    //TXmlEngString iPurchaseOptionId;
    TXmlEngString iUri;
    
    TInt iStatusCode;
    TXmlEngString iElapsedTime;    
    };

public:

    HBufC8* CreateRequestL();
    
    /**
     * Adds details for a download report
     */
    void AddDownloadDetailsL( 
        const TDesC& aId, 
        const TDesC& aUri,
        TInt aStatusCode,
        const TInt64& aElapsedTime );

    /**
     * Adds details for a install report
     */
    void AddInstallDetailsL( 
        const TDesC& aId, 
        TInt aStatusCode );        
    
public:
    static CNcdRequestInstallation* NewL();
    static CNcdRequestInstallation* NewLC();

    void ConstructL();
    
    virtual ~CNcdRequestInstallation();

private:
    CNcdRequestInstallation();


private:
    TXmlEngString iName;
    TXmlEngString iNamespaceUri;
    TXmlEngString iPrefix;
    TXmlEngString iType;        
    
    RArray<TNcdRequestInstallation> iInstallationDetails;
        
    };

#endif //C_NCD_REQUEST_INSTALLATION_HH
    