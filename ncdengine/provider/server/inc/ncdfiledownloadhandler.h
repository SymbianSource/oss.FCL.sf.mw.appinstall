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
* Description:   CNcdFileDownloadHandler declaration
*
*/


#ifndef C_NCDFILEDOWNLOADHANDLER_H
#define C_NCDFILEDOWNLOADHANDLER_H

#include <e32base.h>
#include "ncdbasedownloadhandler.h"
#include "ncdnodeimpl.h"

class MCatalogsHttpSession;

/**
 * Download handler for icon downloads
 */
class CNcdFileDownloadHandler : public CNcdBaseDownloadHandler
    {
public:

    static CNcdFileDownloadHandler* NewL( const CNcdNodeIdentifier& aNodeId,
        CNcdNodeManager& aNodeManager,
        MCatalogsHttpSession& aSession );

    virtual ~CNcdFileDownloadHandler();

public: // From MNcdDownloadHandler
    
    /**
     *
     *     
     */        
    MCatalogsHttpOperation* DownloadL();
        
    void FinishDownloadL( MCatalogsHttpOperation& aDownload, 
        MNcdFileHandler* aFileHandler );    

protected:
    
    CNcdFileDownloadHandler( CNcdNodeManager& aNodeManager, 
        MCatalogsHttpSession& iSession );

    };


#endif // C_NCDFILEDOWNLOADHANDLER_H
