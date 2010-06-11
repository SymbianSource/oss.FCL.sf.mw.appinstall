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
* Description:   CNcdScreenshotDownloadHandler declaration
*
*/


#ifndef C_NCDSCREENSHOTDOWNLOADHANDLER_H
#define C_NCDSCREENSHOTDOWNLOADHANDLER_H

#include <e32base.h>
#include "ncdbasedownloadhandler.h"
#include "ncdnodeimpl.h"

class MCatalogsHttpSession;

/**
 * Download handler for preview downloads
 */
class CNcdScreenshotDownloadHandler : public CNcdBaseDownloadHandler
    {
public:

    static CNcdScreenshotDownloadHandler* NewL( 
        const CNcdNodeIdentifier& aNodeId,
        CNcdNodeManager& aNodeManager,
        MCatalogsHttpSession& aSession );

    virtual ~CNcdScreenshotDownloadHandler();

public: // From MNcdDownloadHandler

    /**
     *
     *     
     */        
    MCatalogsHttpOperation* DownloadL();
        
    void FinishDownloadL( MCatalogsHttpOperation& aDownload, 
        MNcdFileHandler* aFileHandler );    

protected:
    
    CNcdScreenshotDownloadHandler( 
        CNcdNodeManager& aNodeManager, MCatalogsHttpSession& iSession );

    };


#endif // C_NCDSCREENSHOTDOWNLOADHANDLER_H
