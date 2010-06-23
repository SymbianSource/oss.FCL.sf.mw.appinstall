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



#ifndef IA_UPDATE_CACHE_CLEANER_H
#define IA_UPDATE_CACHE_CLEANER_H


#include <e32base.h>

class MNcdProvider;


/**
 * CIAUpdateCacheCleaner provides methdos to clean
 * the cache. This uses the cache clean method
 * of the NCD Engine.
 *
 * @see MNcdProvider::ClearCacheL
 */
class CIAUpdateCacheCleaner : public CActive
    {

public:

    /**
     * @param aProvider Provider that is used for cache cleaning.
     * @return CIAUpdateCacheCleaner* Newly created object.
     */
    static CIAUpdateCacheCleaner* NewL( MNcdProvider& aProvider );

    /**
     * @see CIAUpdateCacheCleaner::NewL
     */
    static CIAUpdateCacheCleaner* NewLC( MNcdProvider& aProvider );
    

    /**
     * Destructor
     */
    virtual ~CIAUpdateCacheCleaner();


    /**
     * ClearL
     *
     * Starts an asynchronous clear operation.
     * When the operation finishes, aStatus will be completed with
     * User::RequestComplete.
     *
     * @note If Cancel is called, then User::RequestComplete is not
     * called for the given status. With Cancel, the operation just
     * completes itself synchronously.
     *
     * @param aStatus Reference to the status, that will be updated
     * when the operation proceeds.
     */    
    void ClearL( TRequestStatus& aStatus );


protected: // CActive
    
    /**
     * @see CActive::DoCancel
     */
	virtual void DoCancel();

    /**
     * @see CActive::RunL
     */
	virtual void RunL();

    /**
     * @see CActive::RunError
     */
     virtual TInt RunError( TInt aError );
    

private:
    
    // Prevent these if not implemented
    CIAUpdateCacheCleaner( const CIAUpdateCacheCleaner& aObject );
    CIAUpdateCacheCleaner& operator =( const CIAUpdateCacheCleaner& aObject );


    /**
     * @see CIAUpdateCacheCleaner::NewL
     */
    CIAUpdateCacheCleaner( MNcdProvider& aProvider );

    /**
     * ConstructL
     */
    void ConstructL();


private: // data

    // Provider that is used for the cleaning.
    MNcdProvider& iProvider;

    // Pointer to the request status of the user of this class object.
    // NULL if the operation is not going on.
    // Not owned.
    TRequestStatus* iRequestStatus;

    // CActiveSchedulerWait is required for the cancel operation
    // to complete correctly.
    CActiveSchedulerWait* iWaiter;

    };

#endif // IA_UPDATE_CACHE_CLEANER_H
