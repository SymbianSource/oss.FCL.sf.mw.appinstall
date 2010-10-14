/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef IAUPDATEFWSYNCUTIL_H
#define IAUPDATEFWSYNCUTIL_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
* TUtil
* 
* This class contains general utility functions. 
*/
NONSHARABLE_CLASS (TUtil)
	{
    public:
    
        /**
        * Panic
        * @param aReason.
        * @return None
        */
         static void Panic( TInt aReason );
        
        /**
        * String copy tool
        * @param aTarget.
        * @param aSource.
        * @return None
        */        
		 static void StrCopy( TDes& aTarget, const TDesC& aSource );
        /**
        * String copy tool
        * @param aTarget.
        * @param aSource.
        * @return None
        */      		
		 static void StrCopy( TDes& aTarget, const TDesC8& aSource );
        
        /**
        * String copy tool
        * @param aTarget.
        * @param aSource.
        * @return None
        */      		
		 static void StrCopy( TDes8& aTarget, const TDesC& aSource );
        
        /**
        * String append tool
        * @param aTarget.
        * @param aSource.
        * @return None
        */      		

		static TInt StrToInt( const TDesC& aText, TInt& aNum );
        
        /**
        * Check whether textline is empty
        * @param aText.
        * @return ETrue if empty
        */				

	};



/**
* MIAUpdateFWActiveCallerObserver
*
* MIAUpdateFWActiveCallerObserver is for observing CIAUpdateFWActiveCaller.
*/
class MIAUpdateFWActiveCallerObserver
    {
    public:
	    virtual void HandleActiveCallL() = 0;
    };

/**
* CIAUpdateFWActiveCaller
*
* CIAUpdateFWActiveCaller is used for generating a call from active scheduler.
*/
NONSHARABLE_CLASS (CIAUpdateFWActiveCaller) : public CActive
    {
    public:
        /**
        * Two-phased constructor.
        */
         static CIAUpdateFWActiveCaller* NewL(MIAUpdateFWActiveCallerObserver* aObserver);

	    /**
        * Destructor.
        */
        virtual ~CIAUpdateFWActiveCaller();

    private:
        /**
        * C++ default constructor.
        */
	    CIAUpdateFWActiveCaller(MIAUpdateFWActiveCallerObserver* aObserver);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
	    void ConstructL();

	public:
        /**
        * Calls observer using active object framework.
        * @param aCallId.
		* @param aMilliseconds Delay before observer is called.
        * @return None
        */
		 void Start( TInt aCallId, TInt aMilliseconds = 0 );

    private:
        /**
        * From CActive. Called when asynchronous request completes.
        * @param None
        * @return None
        */
	    void RunL();
       
        /**
        * From CActive. Called when RunL Leaves.
        * @param None
        * @return error
        */
        TInt RunError( TInt aError );

        /**
        * From CActive. Cancels asynchronous request.
        * @param None
        * @return None
        */
	    void DoCancel();
        
        /**
        * Make active scheduler call RunL.
        * @param None
        * @return None
        */
		void CompleteSelf();

    private:
		// call that is passed to observer
		TInt iCallId;

		// timer
		RTimer iTimer;

		// observer that gets called
		MIAUpdateFWActiveCallerObserver* iObserver;
    };

#endif  // IAUPDATEFWSYNCUTIL_H

// End of File
