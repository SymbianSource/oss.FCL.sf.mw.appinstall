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



#ifndef IAUPDATEQUERYHISTORY_H
#define IAUPDATEQUERYHISTORY_H


//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <s32strm.h> 


/**
* TIAUpdateQueryHistoryItem
*
*/
class TIAUpdateQueryHistoryItem
	{
	public:
	    void SetTime( TTime aTime );
	    TTime Time();
		void InternalizeL( RReadStream& aStream );
		void ExternalizeL( RWriteStream& aStream );


	public:
    	// application uid 
		TUint32 iUid;
		
		// last query time
		TTime iTime;
	};


/**
* CIAUpdateQueryHistory
*  
*/
class CIAUpdateQueryHistory : public CBase
    {
    static const TInt KDefaultDelayHours = 168; // one week
    
	public:
        static CIAUpdateQueryHistory* NewL();
        virtual ~CIAUpdateQueryHistory();
    
    private:
        CIAUpdateQueryHistory();
        void ConstructL();

	public:  // public api
	    void SetTimeL( TUint aUid );
	    void SetDelay( TInt aHours );
	    TBool IsDelayedL( TUint aUid );
		    
	private:
	    void ConnectLC( RFs& aFsSession, TDes& aPath );
	    TInt ItemIndex( TUint aUid );
	    void AddItemL( TIAUpdateQueryHistoryItem aItem );
	    TIAUpdateQueryHistoryItem& Item( TInt index );
	    TBool IsDelayed( TIAUpdateQueryHistoryItem& aItem );
	    
    
	private:
	    void WriteHistoryL( RFs& aFsSession, const TDesC& aPath );
	    void ReadHistoryL( RFs& aFsSession, const TDesC& aPath );
		void InternalizeL( RReadStream& aStream );
		void ExternalizeL( RWriteStream& aStream );
	   
		
    private:
		// list of query items
		RArray<TIAUpdateQueryHistoryItem> iList;
		
		TInt iDelay;
		
	};


#endif      // IAUPDATEQUERYHISTORY_H
            
// End of File
