/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include "catalogsconstants.h"
#include "catalogsbigdes.h"

CCatalogsBigDes* CCatalogsBigDes::NewL()
    {
    CCatalogsBigDes* s = NewLC();
    CleanupStack::Pop();
    return s;
    }

CCatalogsBigDes* CCatalogsBigDes::NewLC()
    {
    CCatalogsBigDes* self = new(ELeave) CCatalogsBigDes();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

void CCatalogsBigDes::ConstructL()
    {
    iArray = new(ELeave) CDesCArrayFlat( KListGranularity );
    }

CCatalogsBigDes::CCatalogsBigDes()
    {
    }

CCatalogsBigDes::~CCatalogsBigDes()
    {
    delete iArray;
    }


void CCatalogsBigDes::AppendL( const TDesC& aDes )
    {
    iArray->AppendL( aDes );
    }

// CCatalogsBigDes& CCatalogsBigDes::operator+=( const TDesC& aDes )
// {
//     AppendL( aDes );
//     return *this;
// }

HBufC* CCatalogsBigDes::DesLC() const
    {
    TInt length = 0;
    TInt i;
    TInt count = iArray->Count();
    for ( i = 0; i < count; i++ )
        {
        length += ( *iArray )[i].Length();
        }
    HBufC* buf = HBufC::NewLC( length );
    for ( i = 0; i < count; i++ )
        {
        buf->Des().Append( ( *iArray )[i] );
        }
    return buf;
    }

HBufC* CCatalogsBigDes::DesL() const
    {
    HBufC* b = DesLC();
    CleanupStack::Pop();
    return b;
    }

