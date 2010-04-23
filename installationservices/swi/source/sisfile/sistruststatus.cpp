/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* sistruststatus.h
* TSisTrustStatus definition
*
*/


/**
 @file 
 @released
 @publishedPartner
*/
 
#include <swi/sistruststatus.h>
#include <s32strm.h>
 
using namespace Swi;
 
EXPORT_C TSisTrustStatus::TSisTrustStatus() :
           iValidationStatus(EUnknown),
           iRevocationStatus(EUnknown2),
           iResultDate(0),
           iLastCheckDate(0),
           iQuarantined(false),
           iQuarantinedDate(0)
    {
    }
    
EXPORT_C TSisTrustStatus::TSisTrustStatus(
    TValidationStatus aValidation,
    TRevocationStatus aRevocation,
    TTime             aResultDate,
    TTime             aCheckDate) : 
        iValidationStatus(aValidation),
        iRevocationStatus(aRevocation),
        iResultDate(aResultDate),
        iLastCheckDate(aCheckDate),
        iQuarantined(false),
        iQuarantinedDate(0)
    {
    }
    
EXPORT_C void TSisTrustStatus::InternalizeL(RReadStream& aStream) 
    {   
    iValidationStatus = static_cast<TValidationStatus>(aStream.ReadInt32L());
    iRevocationStatus = static_cast<TRevocationStatus>(aStream.ReadInt32L());
    
    TPckg <TTime> timePckg(iResultDate);
	aStream.ReadL(timePckg, timePckg.MaxLength());
    
    TPckg <TTime> timePckg2(iLastCheckDate);
	aStream.ReadL(timePckg2, timePckg2.MaxLength());

    iQuarantined = static_cast<TBool>(aStream.ReadInt32L());
    
    TPckg <TTime> timePckg3(iQuarantinedDate);
	aStream.ReadL(timePckg3, timePckg3.MaxLength());
    }
    
EXPORT_C void TSisTrustStatus::ExternalizeL(RWriteStream& aStream) const
    {   
    aStream.WriteInt32L(iValidationStatus);
    aStream.WriteInt32L(iRevocationStatus); 
    
    TPckg <TTime> timePckg(iResultDate);
	aStream.WriteL(timePckg, timePckg.MaxLength());

    TPckg <TTime> timePckg2(iLastCheckDate);
	aStream.WriteL(timePckg2, timePckg2.MaxLength());

    aStream.WriteInt32L(iQuarantined); 

    TPckg <TTime> timePckg3(iQuarantinedDate);
	aStream.WriteL(timePckg3, timePckg3.MaxLength());
    }

EXPORT_C TBool TSisTrustStatus::IsTrusted() const 
    {
    return (iValidationStatus == EPackageInRom) || 
        (iValidationStatus >= EValidatedToAnchor &&
             iRevocationStatus >= EOcspTransient); 
    }

