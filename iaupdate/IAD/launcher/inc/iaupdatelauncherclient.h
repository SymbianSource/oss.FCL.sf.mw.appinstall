/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the RIAUpdateLauncherClient class 
*
*/



#ifndef IA_UPDATE_LAUNCHER_CLIENT_H
#define IA_UPDATE_LAUNCHER_CLIENT_H

#include <AknServerApp.h> 
#include <e32std.h>

/**
 *  
 */
class RIAUpdateLauncherClient : public RAknAppServiceBase 
    {

public:

    /**
     * Constructor.
     */
     
     RIAUpdateLauncherClient();

    /** 
     * This function will crete new embedded IAUpdate instance.
     */
    TInt Open();
    
    /** 
     * Close client server connection to IAUpdate
     */
    void Close();


    /** 
     * Command asks IAUpdate to show updates list
     *
     * @param aStatus The status will be updated when the operation
     * has been completed.
     */
    void ShowUpdates( TBool& aRefreshFromNetworkDenied, 
                      TRequestStatus& aStatus );
    
    /** 
     * Cancel async ShowUpdates() request
     */
     void CancelAsyncRequest();
 
public: // RAknAppServiceBase

    /**
     * @see RAknAppServiceBase::ServiceUid
     */
    TUid ServiceUid() const;

                               
private: //data

     TBool iConnected;
     
     TPtr8 iPtr1;

     };

#endif // IA_UPDATE_LAUNCHER_CLIENT_H
