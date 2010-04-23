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
* Description:   CNcdRequestGenerator implementation
*
*/


#include <e32std.h>
#include <e32base.h>
#include <badesca.h>
#include "ncdxmlheaders.h"
#include "ncdrequestgenerator.h"
#include "ncdrequestconfiguration.h"
#include "ncdrequestbrowsesearch.h"
#include "ncdrequestpurchase.h"
#include "ncdrequestmanagesubscriptions.h"
#include "ncdrequestinstallation.h"
#include "catalogsdebug.h"

    
CNcdRequestConfiguration* NcdRequestGenerator::CreateConfigurationRequestL()
    {
    DLTRACEIN(( "CreateConfigurationRequestL"));
    CNcdRequestConfiguration* req = CNcdRequestConfiguration::NewL();
    DASSERT( req );
    DLTRACEOUT(( "CreateConfigurationRequestL" ));
    return req;
    }
    
CNcdRequestConfiguration* NcdRequestGenerator::CreateConfigurationRequestLC()
    {
    DLTRACEIN(( _L("CreateConfigurationRequestLC")));
    CNcdRequestConfiguration* req = CNcdRequestConfiguration::NewLC();
    DASSERT( req );
    DLTRACEOUT(( _L("CreateConfigurationRequestLC")));
    return req;
    }
    
CNcdRequestBrowseSearch* NcdRequestGenerator::CreateBrowseRequestL()
    {
    DLTRACEIN(( _L("CreateBrowseRequestL")));
    CNcdRequestBrowseSearch* req = CNcdRequestBrowseSearch::NewL();
    DASSERT( req );
    req->SetSearch(EFalse);
    DLTRACEOUT(( _L("CreateBrowseRequestL")));
    return req;
    }

CNcdRequestBrowseSearch* NcdRequestGenerator::CreateBrowseRequestLC()
    {
    DLTRACEIN(( _L("CreateBrowseRequestLC")));
    CNcdRequestBrowseSearch* req = CNcdRequestBrowseSearch::NewLC();
    DASSERT( req );
    req->SetSearch(EFalse);
    DLTRACEOUT(( _L("CreateBrowseRequestLC")));
    return req;
    }

CNcdRequestBrowseSearch* NcdRequestGenerator::CreateSearchRequestL() 
    {
    DLTRACEIN(( _L("CreateSearchRequestL")));
    CNcdRequestBrowseSearch* req = CNcdRequestBrowseSearch::NewL();
    DASSERT( req );
    req->SetSearch(ETrue);
    DLTRACEOUT(( _L("CreateSearchRequestL")));
    return req;
    }
    
CNcdRequestBrowseSearch* NcdRequestGenerator::CreateSearchRequestLC()
    {
    DLTRACEIN(( _L("CreateSearchRequestLC")));
    CNcdRequestBrowseSearch* req = CNcdRequestBrowseSearch::NewLC();
    DASSERT( req );
    req->SetSearch(ETrue);
    DLTRACEOUT(( _L("CreateSearchRequestLC")));
    return req;
    }
    
    
CNcdRequestPurchase*  NcdRequestGenerator::CreatePurchaseRequestL()
    {
    DLTRACEIN(( _L("CreatePurchaseRequestL")));
    CNcdRequestPurchase* req = CNcdRequestPurchase::NewL();
    DLTRACEOUT(( _L("CreatePurchaseRequestL")));
    return req;
    }
    
CNcdRequestPurchase*  NcdRequestGenerator::CreatePurchaseRequestLC()
    {
    DLTRACEIN(( _L("CreatePurchaseRequestL")));
    CNcdRequestPurchase* req = CNcdRequestPurchase::NewLC();
    DLTRACEOUT(( _L("CreatePurchaseRequestL")));
    return req;
    }

CNcdRequestManageSubscriptions*  
NcdRequestGenerator::CreateManageSubscriptionsRequestL()
    {
    DLTRACEIN(( _L("CreateManageSubscriptionsRequestL")));
    CNcdRequestManageSubscriptions* req = 
        CNcdRequestManageSubscriptions::NewL();
    DLTRACEOUT(( _L("CreateManageSubscriptionsRequestL")));
    return req;
    }
    
CNcdRequestManageSubscriptions*  
NcdRequestGenerator::CreateManageSubscriptionsRequestLC()
    {
    DLTRACEIN(( _L("CreateManageSubscriptionsRequestLC")));
    CNcdRequestManageSubscriptions* req = 
        CNcdRequestManageSubscriptions::NewLC();
    DLTRACEOUT(( _L("CreateManageSubscriptionsRequestLC")));
    return req;
    }


CNcdRequestInstallation* 
    NcdRequestGenerator::CreateInstallationReportRequestL()
    {
    DLTRACEIN((""));
    CNcdRequestInstallation* req = 
        CNcdRequestInstallation::NewL();
    DLTRACEOUT((""));
    return req;
    }
        
        
CNcdRequestInstallation* 
    NcdRequestGenerator::CreateInstallationReportRequestLC()
    {
    DLTRACEIN((""));
    CNcdRequestInstallation* req = 
        CNcdRequestInstallation::NewLC();
    DLTRACEOUT((""));
    return req;
    }
