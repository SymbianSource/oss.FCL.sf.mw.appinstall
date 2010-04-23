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
* Description:   This file contains the implementation of CStartupItem
*                class member functions.
*
*/


// INCLUDE FILES
#include <starterclient.h>
#include <s32strm.h>
#include "StartupItem.h"

using namespace SwiUI;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CStartupItem::CStartupItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CStartupItem::CStartupItem()
    : iRecoveryPolicy( EStartupItemExPolicyNone )
    {
    }

// -----------------------------------------------------------------------------
// CStartupItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CStartupItem::ConstructL( const TStartupTaskParam& aParam )
    {    
    iExecutableFile = aParam.iFileName.AllocL();
    iRecoveryPolicy = aParam.iRecoveryPolicy;
    }

// -----------------------------------------------------------------------------
// CStartupItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CStartupItem::ConstructL( RReadStream& aStream )
    {    
    iExecutableFile = HBufC::NewL( aStream, KMaxFileName );
    iRecoveryPolicy = TStartupExceptionPolicy( aStream.ReadInt32L() );
    }

// -----------------------------------------------------------------------------
// CStartupItem::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CStartupItem* CStartupItem::NewL( const TStartupTaskParam& aParam )
    {
    CStartupItem* self = new( ELeave ) CStartupItem();
    CleanupStack::PushL( self );
    self->ConstructL( aParam );
    CleanupStack::Pop( self );
    return self;    
    }

// -----------------------------------------------------------------------------
// CStartupItem::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
 CStartupItem* CStartupItem::NewL( RReadStream& aStream )
    {
    CStartupItem* self = new( ELeave ) CStartupItem();
    CleanupStack::PushL( self );
    self->ConstructL( aStream );
    CleanupStack::Pop( self );
    return self;    
    }

// Destructor
CStartupItem::~CStartupItem()
    {
    delete iExecutableFile;    
    }

// -----------------------------------------------------------------------------
// CStartupItem::ExecutableFile
// Get the executable file of this startup item.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 const TDesC& CStartupItem::ExecutableFile() const
    {
    if ( iExecutableFile )
        {
        return *iExecutableFile;
        }
    else
        {
        return KNullDesC();        
        }    
    }
        
// -----------------------------------------------------------------------------
// CStartupItem::RecoveryPolicy
// Get the startup recovery policy for this item.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 TStartupExceptionPolicy CStartupItem::RecoveryPolicy() const
    {
    return iRecoveryPolicy;    
    }

// -----------------------------------------------------------------------------
// CStartupItem::ExternalizeL
// Externalize the class.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CStartupItem::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream << *iExecutableFile;
    aStream.WriteInt32L( iRecoveryPolicy );    
    }

//  End of File  
