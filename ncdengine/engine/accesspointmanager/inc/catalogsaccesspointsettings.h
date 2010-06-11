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
* Description:   Class CCatalogsAccessPointSettings declation
*
*/


#ifndef C_CATALOGSACCESSPOINTSETTINGS_H
#define C_CATALOGSACCESSPOINTSETTINGS_H

#include <e32base.h>

#ifdef _0
#include <ApAccessPointItem.h>
#endif

#include <s32strm.h>


#include <badesca.h>


class CCatalogsAccessPointSettings: public CBase
    {
public:
    static CCatalogsAccessPointSettings* NewL();
    static CCatalogsAccessPointSettings* NewLC();
    
    virtual ~CCatalogsAccessPointSettings();
    
#ifdef _0
    void SetLongTextParameterL(const TApMember& aKey, const TDesC16& aValue);
    void SetText16ParameterL(const TApMember& aKey, const TDesC16& aValue);
    void SetText8ParameterL(const TApMember& aKey, const TDesC8& aValue);
    void SetUintParameterL(const TApMember& aKey, const TUint32& aValue);
    void SetBoolParameterL(const TApMember& aKey, const TBool& aValue);
    
    TInt LongTextParameterCount() const;
    TInt Text16ParameterCount() const;
    TInt Text8ParameterCount() const;
    TInt UintParameterCount() const;
    TInt BoolParameterCount() const;
    
    void LongTextParameter(
        TInt aIndex, TApMember& aKey, TPtrC16& aValue) const;
    void Text16Parameter(
        TInt aIndex, TApMember& aKey, TPtrC16& aValue) const;
    void Text8Parameter(
        TInt aIndex, TApMember& aKey, TPtrC8& aValue) const;
    void UintParameter(
        TInt aIndex, TApMember& aKey, TUint32& aValue) const;
    void BoolParameter(
        TInt aIndex, TApMember& aKey, TBool& aValue) const;
#endif
    
protected:
    CCatalogsAccessPointSettings();
    void ConstructL();
    void ExternalizeL(RWriteStream& aStream);
    void InternalizeL(RReadStream& aStream);

private:
#ifdef _0
    RArray<TApMember> iLongTextMembers;
    RArray<TApMember> iText16Members;
    RArray<TApMember> iText8Members;
    RArray<TApMember> iUintMembers;
    RArray<TApMember> iBoolMembers;
#endif
    
    CDesC16ArrayFlat* iLongTextData;
    CDesC16ArrayFlat* iText16Data;
    CDesC8ArrayFlat* iText8Data;
    RArray<TUint32> iUintData;
    RArray<TBool> iBoolData;
    };


#endif