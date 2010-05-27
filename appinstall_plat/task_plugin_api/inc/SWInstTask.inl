/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of CTask class member 
*                functions.
*
*/


using namespace SwiUI;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTask::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
inline CTask* CTask::NewL( const TUid& aImplUid, TBool aIsPersistent )
    {
    TAny* ptr = REComSession::CreateImplementationL( aImplUid,
                                                     _FOFF( CTask,
                                                            iDestructKey ) );
    CTask* impl = reinterpret_cast<CTask*> ( ptr );
    impl->iImplementationUid = aImplUid;
    impl->iIsPersistent = aIsPersistent;

    return impl;    
    }
    
// Destructor
inline CTask::~CTask()
    {
    REComSession::DestroyedImplementation( iDestructKey );   
    }

// -----------------------------------------------------------------------------
// CTask::ImplementationUid
// Return the ECOM implementation uid of this task.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
inline const TUid& CTask::ImplementationUid() const
    {
    return iImplementationUid;    
    }

// -----------------------------------------------------------------------------
// CTask::IsPersistent
// Indicate if this task is persistent or not.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
inline TBool CTask::IsPersistent() const
    {
    return iIsPersistent;    
    }

// -----------------------------------------------------------------------------
// CTask::Id
// Gets the id of the task.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
inline TInt CTask::Id() const
    {
    return iId;
    }

// -----------------------------------------------------------------------------
// CTask::SetId
// Sets the id of the task.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CTask::SetId( TInt aId )
    {
    iId = aId;
    }

// -----------------------------------------------------------------------------
// CTask::ExternalizeL
// Externalizes the task.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
inline void CTask::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iId );         
    DoExternalizeL( aStream );
    }

// -----------------------------------------------------------------------------
// CTask::InternalizeL
// Internalizes the task.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
inline void CTask::InternalizeL( RReadStream& aStream )
    {
    iId = aStream.ReadInt32L();
    iIsPersistent = ETrue;
    DoInternalizeL( aStream );
    }


//  End of File  
