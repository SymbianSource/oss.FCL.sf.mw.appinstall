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
* Description:   CNcdRequestGenerator declaration
*
*/


#ifndef NCDREQUESTGENERATOR_H
#define NCDREQUESTGENERATOR_H

#include <e32std.h>
#include <e32base.h>

class CNcdRequestConfiguration;
class CNcdRequestBrowseSearch;
class CNcdRequestPurchase;
class CNcdRequestManageSubscriptions;
class CNcdRequestInstallation;

class NcdRequestGenerator : public CBase
    {
    
public:
    
    static CNcdRequestConfiguration* CreateConfigurationRequestL();
    static CNcdRequestConfiguration* CreateConfigurationRequestLC();
    
    static CNcdRequestBrowseSearch* CreateBrowseRequestL();
    static CNcdRequestBrowseSearch* CreateBrowseRequestLC();
    
    static CNcdRequestBrowseSearch* CreateSearchRequestL();
    static CNcdRequestBrowseSearch* CreateSearchRequestLC();
    
    static CNcdRequestPurchase* CreatePurchaseRequestL();
    static CNcdRequestPurchase* CreatePurchaseRequestLC();
    
    static CNcdRequestManageSubscriptions* CreateManageSubscriptionsRequestL();
    static CNcdRequestManageSubscriptions* CreateManageSubscriptionsRequestLC();

    static CNcdRequestInstallation* CreateInstallationReportRequestL();
    static CNcdRequestInstallation* CreateInstallationReportRequestLC();

    
    };

#endif //NCDREQUESTGENERATOR_H