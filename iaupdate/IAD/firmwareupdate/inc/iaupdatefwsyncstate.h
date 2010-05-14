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
* Description:    
*
*/




#ifndef IAUPDATEFWSYNCSTATE_H
#define IAUPDATEFWSYNCSTATE_H


//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/*
* CIAUpdateFWSyncState 
*
* CIAUpdateFWSyncState handles sync states
*/
NONSHARABLE_CLASS (CIAUpdateFWSyncState) : public CBase
    {
    public:
		enum TSyncPhase
			{
			EPhaseConnecting = 0,
			EPhaseInitialising,
			EPhaseSending,
			EPhaseReceiving,
			EPhaseSynchronizing,
			EPhaseDisconnecting,
			EPhaseCanceling,
			EPhaseNone
			};

    public:
        /**
        * Two-phased constructor.
        */
         static CIAUpdateFWSyncState* NewL();
        
        /**
        * Destructor.
        */
         virtual ~CIAUpdateFWSyncState();
        
    private:
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        CIAUpdateFWSyncState();
        
        /**
        * C++ default constructor.
        */
        void ConstructL();
        
    public:
        /**
        * Reset state variables.
        * @param None.
        * @return None.
        */
         void Reset();
        
    public:
        /**
        * Set synchronization phase.
        * @param aPhase synchronization phase.
        * @return None.
        */
         void SetSyncPhase(TInt aPhase);
        
        /**
        * Get synchronization phase.
        * @param None.
        * @return Synchronization phase.
        */
         TInt SyncPhase();

        /**
        * Set total sync progress.
        * @param aCount Progress count.
        * @return None.
        */
		 void SetTotalProgress(TInt aCount);

        /**
        * Get total sync progress.
        * @param None. 
        * @return Progress count.
        */
		 TInt TotalProgress();

        /**
        * Set sync progress.
        * @param aCount Progress count.
        * @return None.
        */
		 void SetProgress(TInt aCount);

        /**
        * Increase sync progress.
        * @param None.
        * @return None.
        */
         void IncreaseProgress();
        
        /**
        * Get sync progress.
        * @param None. 
        * @return Progress count.
        */
		 TInt Progress();

        /**
        * Set sync content name.
        * @param aContent Content name.
        * @return None.
        */
         void SetContent(const TDesC& aContent);

        /**
        * Get sync content name.
        * @param None. 
        * @return Content name.
        */
		 const TDesC& Content();

        /**
        * Resets progress state.
        * @param None. 
        * @return None.
        */
		 void ResetProgress();
		
        /**
        * Finds out whether total progress count is known.
        * @param None. 
        * @return ETrue if total progress count is known, EFalse otherwise.
        */
		 TBool ProgressKnown();
        
    private:
        // synchronization phase
		TInt iSyncPhase;

		// total number of sent/received sync items
		TInt iTotalProgressCount;

		// number of sent/received sync items
		TInt iProgressCount;

		// name of the content being synced
		TName iSyncContent;
    };

#endif  // IAUPDATEFWSYNCSTATE_H

// End of File
