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
          
    private:
        // synchronization phase
		TInt iSyncPhase;

    };

#endif  // IAUPDATEFWSYNCSTATE_H

// End of File
