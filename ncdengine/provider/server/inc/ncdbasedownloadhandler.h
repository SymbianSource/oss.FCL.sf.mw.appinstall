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
* Description:   CNcdBaseDownloadHandler declaration
*
*/


#include "ncddownloadhandler.h"

#ifndef C_NCDBASEDOWNLOADHANDLER_H
#define C_NCDBASEDOWNLOADHANDLER_H

class CNcdNodeManager;
class MCatalogsHttpSession;
class CNcdNodeIdentifier;
class CNcdNode;

class CNcdBaseDownloadHandler : public CBase, public MNcdDownloadHandler
    {
public:

    ~CNcdBaseDownloadHandler();
            
public: // From MNcdDownloadHandler

    /**
     * @see MNcdDownloadHandler::CurrentDownload()
     */
    virtual TInt CurrentDownload() const;
    
    /**
     * @see MNcdDownloadHandler::DownloadCount()
     */
    virtual TInt DownloadCount() const;
    
    /**
     * @see MNcdDownloadHandler::DownloadL()
     */
    virtual MCatalogsHttpOperation* DownloadL() = 0;
        
    /**
     * @see MNcdDownloadHandler::FinishDownloadL()
     */
    virtual void FinishDownloadL( MCatalogsHttpOperation& aDownload, 
        MNcdFileHandler* aFileHandler ) = 0;    

    
    /**
     * @see MNcdDownloadHandler::SetCurrentDownload()
     */    
    virtual void SetCurrentDownload( TInt aCurrentDownload );

    /**
     * @see MNcdDownloadHandler::NodeId()
     */
    virtual const CNcdNodeIdentifier& NodeId() const;
    

    /**
     * @see MNcdDownloadHandler::NodeL()
     */
    virtual CNcdNode& NodeL() const;
    
protected:
    
    CNcdBaseDownloadHandler( CNcdNodeManager& aNodeManager,
        MCatalogsHttpSession& aSession );
    void ConstructL( const CNcdNodeIdentifier& aNodeId );

protected:

        
    
    MCatalogsHttpSession& HttpSession() const;

    CNcdNodeManager& NodeManager() const;

    void SetTotalDownloads( TInt aDownloads );


private:

    CNcdNodeManager& iNodeManager;
    MCatalogsHttpSession& iHttpSession;
    CNcdNodeIdentifier* iNodeId;
    
    TInt iDownloads;
    TInt iCurrentDownload;
    
    };
    
    
#endif // C_NCDBASEDOWNLOADHANDLER_H    
