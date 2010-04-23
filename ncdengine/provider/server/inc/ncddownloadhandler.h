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
* Description:   Download handling interface
*
*/


#ifndef M_NCDDOWNLOADHANDLER_H
#define M_NCDDOWNLOADHANDLER_H

class MNcdFileHandler;
class MCatalogsHttpOperation;
class CNcdNodeIdentifier;
class CNcdNode;

/**
 * Helper interface for simplifying the implementation of different
 * types of file downloads
 */
class MNcdDownloadHandler
    {
public:

    /**
     * Current download number
     */
    virtual TInt CurrentDownload() const = 0;
    
    /**
     * Total downloads
     */
    virtual TInt DownloadCount() const = 0;
    
    /**
     * Creates the next download 
     *     
     */        
    virtual MCatalogsHttpOperation* DownloadL() = 0;
        
    virtual void FinishDownloadL( MCatalogsHttpOperation& aDownload, 
        MNcdFileHandler* aFileHandler ) = 0;
        
    
    /**
     * Sets the index for the next download
     *
     */    
    virtual void SetCurrentDownload( TInt aCurrentDownload ) = 0;
    
    /**
     * Node ID getter
     */
    virtual const CNcdNodeIdentifier& NodeId() const = 0;
    
    
    /**
     * Node getter
     * @return Node
     */
    virtual CNcdNode& NodeL() const = 0;
    
    virtual ~MNcdDownloadHandler()
        {
        }
    };

#endif // M_NCDDOWNLOADCONFIGURATOR_H