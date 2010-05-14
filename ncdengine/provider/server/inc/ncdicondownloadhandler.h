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
* Description:   CNcdIconDownloadHandler declaration
*
*/


#ifndef C_NCDICONDOWNLOADHANDLER_H
#define C_NCDICONDOWNLOADHANDLER_H

#include <e32base.h>
#include "ncdbasedownloadhandler.h"
#include "ncdnodeimpl.h"

class MCatalogsHttpSession;

/**
 * Download handler for icon downloads
 */
class CNcdIconDownloadHandler : public CNcdBaseDownloadHandler
    {
public:

    static CNcdIconDownloadHandler* NewL( const CNcdNodeIdentifier& aNodeId,
        CNcdNodeManager& aNodeManager,
        MCatalogsHttpSession& aSession );

    virtual ~CNcdIconDownloadHandler();

public: // From MNcdDownloadHandler

    /**
     *
     *     
     */        
    MCatalogsHttpOperation* DownloadL();
        
    void FinishDownloadL( MCatalogsHttpOperation& aDownload, 
        MNcdFileHandler* aFileHandler );    

protected:
    
    CNcdIconDownloadHandler( CNcdNodeManager& aNodeManager, 
        MCatalogsHttpSession& iSession );
    
    };


#endif // C_NCDSKINDOWNLOADHANDLER_H