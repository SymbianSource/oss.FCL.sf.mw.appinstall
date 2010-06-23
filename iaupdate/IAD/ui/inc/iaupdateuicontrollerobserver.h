/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the MIAUpdateUiControllerObserver class 
*
*/



#ifndef IAUPDATEUICONTROLLEROBSERVER_H
#define IAUPDATEUICONTROLLEROBSERVER_H

class MIAUpdateNode;

class MIAUpdateUiControllerObserver
	{
public: // Observer API
    
    /**
    * Called when async engine start-up is completed
    *
    * @param aError Error code
    */
    virtual void StartupComplete( TInt aError ) = 0;
    
    /**
    * Handles leave error. Possible async client request to be completed
    *
    * @param aError Error code
    */      
    virtual void HandleLeaveErrorL( TInt aError ) = 0;
    
    /**
    * Handles leave error without leaving. 
    * Possible async client request to be completed
    *
    * @param aError Error code
    */      
    virtual void HandleLeaveErrorWithoutLeave( TInt aError ) = 0;
    
    /**
     Called when UI to be redrawn during update process
     *
     * @param aError Error code
    */
    virtual void RefreshUI() = 0;
    
    /**
    * Called when async update list refresh is completed
    *
    * @param aError Error code
    */
    virtual void RefreshCompleteL( TBool aWithViewActivation, TInt aError ) = 0;
    
    /**
    * Called when async updating is completed
    */  
    virtual void UpdateCompleteL( TInt aError ) = 0;
    
  	};
	
#endif // IAUPDATEUICONTROLLEROBSERVER_H
