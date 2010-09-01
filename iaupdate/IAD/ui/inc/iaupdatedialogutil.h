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
* Description:    
*
*/



#ifndef IAUPDATEDIALOGUTIL_H
#define IAUPDATEDIALOGUTIL_H


//  INCLUDES
#include <e32base.h>


//FORWARD DECLARATIONS
class MIAUpdateNode;


/**
* TIAUpdateTextLinkObserver
* 
* Observer interface function for observing text link.
*/
class MIAUpdateTextLinkObserver
    {
    public:
        virtual void TextLinkL( TInt aLinkId ) = 0;
    };


/**
* TIAUpdateDialogParam
*
* This class is used as general dialog parameter type.
*/
class TIAUpdateDialogParam
	{
    public:
		TInt iCountSuccessfull;
		TInt iCountCancelled;
		TInt iCountFailed;
		TInt iShowCloseAllText;
		TInt iResourceId;
		MIAUpdateNode* iNode;
		MIAUpdateTextLinkObserver* iLinkObserver;
		
    public:
    	TIAUpdateDialogParam();
		
	};




/**
* IAUpdateDialogUtil
* 
* IAUpdateDialogUtil contains general dialog utilities.
*/
class IAUpdateDialogUtil
	{
    public:
		static void ShowMessageQueryL( const TDesC& aTitle, const TDesC& aText) ;
		static void ShowMessageQueryL( const TDesC& aTitle, TInt aResource );
		
        static void ShowInformationQueryL( const TDesC& aText );
        static void ShowInformationQueryL( TInt aResource );
        
        static TInt ShowConfirmationQueryL( const TDesC& aText, TInt aSoftkeyResourceId );
        static TInt ShowConfirmationQueryL( TInt aResource, TInt aSoftkeyResourceId );
        
        static void Panic( TInt aReason );
	};





#endif      // IAUPDATEDIALOGUTIL_H
            
// End of File
