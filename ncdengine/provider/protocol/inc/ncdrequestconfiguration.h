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
* Description:   CNcdRequestBrowseSearch declaration
*
*/


#ifndef C_NCD_REQUEST_CONFIGURATION_HH
#define C_NCD_REQUEST_CONFIGURATION_HH

#include <e32std.h>
#include <e32base.h>
#include "ncdrequestbase.h"
#include "ncdprotocoltypes.h"
#include "ncdrequestconstants.h"

class CNcdRequestConfiguration : public CNcdRequestBase
    {

public:
    HBufC8* CreateRequestL();

public:
    static CNcdRequestConfiguration* NewL();
    static CNcdRequestConfiguration* NewLC();

    void ConstructL();
    
    ~CNcdRequestConfiguration();

public:
    void AddCatalogBundleRequestL(TDesC8& bundleId);
    
protected:
    
private:
    CNcdRequestConfiguration();

private:
    TXmlEngString iType;
    TXmlEngString iNamespaceUri;
    TXmlEngString iPrefix;
    
    CDesC8ArrayFlat* iBundles;
    
    };

#endif //C_NCD_REQUEST_CONFIGURATION_HH
    