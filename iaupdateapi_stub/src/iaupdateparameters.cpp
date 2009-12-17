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
* Description:   CIAUpdateParameters
*
*/


#include <iaupdateparameters.h>


EXPORT_C CIAUpdateParameters* CIAUpdateParameters::NewL()
    {
    CIAUpdateParameters* self =
        CIAUpdateParameters::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
    
EXPORT_C CIAUpdateParameters* CIAUpdateParameters::NewLC()
    {
    CIAUpdateParameters* self =
        new( ELeave ) CIAUpdateParameters();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CIAUpdateParameters::CIAUpdateParameters(): 
CBase(),
iUid( TUid::Null() ),
iImportance( 0xffffffff ),
iType( 0xffffffff )
    {
    
    }

void CIAUpdateParameters::ConstructL()
    {
    iSearchCriteria = KNullDesC().AllocL();
    iCommandLineExecutable = KNullDesC().AllocL();
    iCommandLineArguments = KNullDesC8().AllocL();
    }
                 

EXPORT_C CIAUpdateParameters::~CIAUpdateParameters()
    {
    delete iSearchCriteria;

    delete iCommandLineExecutable;
     
    delete iCommandLineArguments;
    }


EXPORT_C const TUid& CIAUpdateParameters::Uid() const
    {
    return iUid;
    }

EXPORT_C void CIAUpdateParameters::SetUid( const TUid& aUid )
    {
    iUid = aUid;
    }


EXPORT_C const TDesC& CIAUpdateParameters::SearchCriteria() const
    {
    if ( !iSearchCriteria )
        {
        return KNullDesC;
        }
    else
        {
        return *iSearchCriteria;
        }
    }

EXPORT_C void CIAUpdateParameters::SetSearchCriteriaL( const TDesC& aSearchCriteria )
    {
    HBufC* tmp( aSearchCriteria.AllocL() );
    delete iSearchCriteria;
    iSearchCriteria = tmp;
    }


EXPORT_C const TDesC& CIAUpdateParameters::CommandLineExecutable() const
    {
    if ( !iCommandLineExecutable  )
        {
        return KNullDesC;
        }
    else
        {
        return *iCommandLineExecutable;
        }
    }

EXPORT_C void CIAUpdateParameters::SetCommandLineExecutableL( const TDesC& aCommandLineExecutable )
    {
    HBufC* tmp( aCommandLineExecutable.AllocL() );
    delete iCommandLineExecutable;
    iCommandLineExecutable = tmp;
    }
    
    
EXPORT_C const TDesC8& CIAUpdateParameters::CommandLineArguments() const
    {
    if ( !iCommandLineArguments  )
        {
        return KNullDesC8;
        }
    else
        {
        return *iCommandLineArguments;
        }
    }

EXPORT_C void CIAUpdateParameters::SetCommandLineArgumentsL( const TDesC8& aCommandLineArguments )
    {
    HBufC8* tmp( aCommandLineArguments.AllocL() );
    delete iCommandLineArguments;
    iCommandLineArguments = tmp;
    }


EXPORT_C TBool CIAUpdateParameters::ShowProgress() const
    {
    return iShowProgress;
    }

EXPORT_C void CIAUpdateParameters::SetShowProgress( TBool aShowProgress )
    {
    iShowProgress = aShowProgress;
    }
    
EXPORT_C TUint CIAUpdateParameters::Importance() const
    {
    return iImportance;	
    }
    
EXPORT_C void CIAUpdateParameters::SetImportance( TUint aImportance )
    {
	iImportance = aImportance;
    }

EXPORT_C TUint CIAUpdateParameters::Type() const
    {
    return iType;	
    }

EXPORT_C void CIAUpdateParameters::SetType( TUint aType )
    {
	iType = aType;
    }
	
EXPORT_C TBool CIAUpdateParameters::Refresh() const
    {
    return iRefresh;    	
    } 

EXPORT_C void CIAUpdateParameters::SetRefresh( TBool aRefresh )
    {
	iRefresh = aRefresh;
    }

