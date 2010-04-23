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
* Description:   Definition of ncd operation type enumeration.
*
*/


#ifndef NCDOPERATIONTYPE_H
#define NCDOPERATIONTYPE_H



/** Operation types */
enum TNcdOperationType
    {
    ELoadNodeOperation,
    ELoadRootNodeOperation,
    ELoadBundleNodeOperation,
    EDownloadOperation,
    EDownloadSubOperation,
    EDescriptorDownloadSubOperation,        
    EContentDownloadOperation,
    EInstallOperation,
    EPurchaseOperation,
    ESearchOperation,
    ESendNotificationSubOperation,
    ERightsObjectOperation,
    ECreateAccessPointOperation,
    ESendHttpRequestOperation,
    EServerReportOperation,
    ESubscriptionOperation
    };


#endif // NCDOPERATIONTYPE_H
