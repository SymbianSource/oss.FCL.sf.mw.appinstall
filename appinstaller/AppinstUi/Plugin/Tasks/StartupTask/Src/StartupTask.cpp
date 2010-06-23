/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of CStartupTask
*                class member functions.
*
*/


#include <dscstore.h>                   // RDscStore
#include "StartupTask.h"
#include "SWInstStartupTaskParam.h"

using namespace SwiUI;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CStartupTask::CStartupTask
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CStartupTask::CStartupTask()
    {
    }

// Destructor
CStartupTask::~CStartupTask()
    {
    iItems.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CStartupTask::SetParameterL
// Adds a parameter to the task.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CStartupTask::SetParameterL( const TDesC8& aParam, TInt aIndex )
    {
    TStartupTaskParam param;
    TStartupTaskParamPckg pckg( param );
    pckg.Copy( aParam );

    CStartupItem* item = CStartupItem::NewL( param );
    iItems.Insert( item, aIndex );
    }

// -----------------------------------------------------------------------------
// CStartupTask::SetParameterL
// Checks if given RDcsStore exists, and creates it if necessary.
// -----------------------------------------------------------------------------
//
void CStartupTask::CreateIfNotExistL( RDscStore& aDsc )
    {
    if( !aDsc.DscExistsL() )
        {
        aDsc.CreateDscL();
        }
    }

// -----------------------------------------------------------------------------
// CStartupTask::DoExternalizeL
// Externalizes the task.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CStartupTask::DoExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iItems.Count() );
    for ( TInt index = 0; index < iItems.Count(); index++ )
        {
        aStream << *(iItems[index]);
        }
    }

// -----------------------------------------------------------------------------
// CStartupTask::DoInternalizeL
// Internalizes the task.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CStartupTask::DoInternalizeL( RReadStream & aStream )
    {
    TInt itemCount( aStream.ReadInt32L() );
    for ( TInt index = 0; index < itemCount; index++ )
        {
        CStartupItem* item = CStartupItem::NewL( aStream );
        iItems.Append( item );
        }
    }

//  End of File
