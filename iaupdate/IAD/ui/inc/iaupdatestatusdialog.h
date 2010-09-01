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
* Description:   Header file of CIAUpdateStatusDialog
*
*/



#ifndef IAUPDATESTATUSDIALOG_H
#define IAUPDATESTATUSDIALOG_H


//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class TIAUpdateDialogParam;
class MIAUpdateTextLinkObserver;




/**
* CIAUpdateStatusDialog
*
* CIAUpdateStatusDialog is used for displaying status details.
*/
class CIAUpdateStatusDialog : public CBase
    {
	public:
	
	    /**
        * Launches dialog.
        * @param aParam Class that contains dialog parameters.
        * @return Completion code.
        */
		static TInt ShowDialogL( TIAUpdateDialogParam* aParam );
		
	public:// Constructors and destructor

        /**
        * Two-phased constructor.
        */
		static CIAUpdateStatusDialog* NewL( TIAUpdateDialogParam* aParam );
        
        /**
        * Destructor.
        */
        virtual ~CIAUpdateStatusDialog();
    
    private:

        /**
        * C++ default constructor.
        */
        CIAUpdateStatusDialog( TIAUpdateDialogParam* aParam );
	
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();
        
    private:
        void ConstructTextL();
        TInt ShowDialogL();
        void HandleLinkL();
        
    private:
        // callback function in message query
        static TInt HandleLink( TAny* aPtr );
        
    private:
        TIAUpdateDialogParam* iParam;
        HBufC* iBuf;
        MIAUpdateTextLinkObserver* iObserver;
    };


#endif      // IAUPDATESTATUSDIALOG_H
            
// End of File
