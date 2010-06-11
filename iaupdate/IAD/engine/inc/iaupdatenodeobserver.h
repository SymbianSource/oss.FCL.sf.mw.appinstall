/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/



#ifndef IAUPDATENODEOBSERVER_H
#define IAUPDATENODEOBSERVER_H

class MIAUpdateNode;

class MIAUpdateNodeObserver
    {

public:

    /**
     * Notifies observer on the progress of an operation.
     * @param aNode Node that is under operation
     * @param aProgress Current progress of operation
     * @param aMaxProgress Maximum progress of operation     
     **/
    virtual void DownloadProgress( MIAUpdateNode& aNode,
                                   TUint aProgress,
                                   TUint aMaxProgress ) = 0;
                            
    /**
     * Notifies observer that an operation has been completed
     * @param aNode Node who had its operation completed
     * @param aError Error value
     **/
    virtual void DownloadComplete( MIAUpdateNode& aNode,
                                   TInt aError ) = 0;

    /**
     * Notifies observer on the progress of an operation.
     * @param aNode Node that is under operation
     * @param aProgress Current progress of operation
     * @param aMaxProgress Maximum progress of operation     
     **/
    virtual void InstallProgress( MIAUpdateNode& aNode,
                                  TUint aProgress,
                                  TUint aMaxProgress ) = 0;
                            
    /**
     * Notifies observer that an operation has been completed
     * @param aNode Node who had its operation completed
     * @param aError Error value
     **/
    virtual void InstallComplete( MIAUpdateNode& aNode,
                                  TInt aError ) = 0;

                                   
protected:

    virtual ~MIAUpdateNodeObserver()
        {
        }
        
    };

#endif  //  IAUPDATENODEOBSERVER_H
