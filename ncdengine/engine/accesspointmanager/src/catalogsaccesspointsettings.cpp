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
* Description:   Class CCatalogsAccessPointSettings implementation
*
*/


#include "catalogsaccesspointsettings.h"
#include "catalogsutils.h"


CCatalogsAccessPointSettings* CCatalogsAccessPointSettings::NewL() 
    {
    CCatalogsAccessPointSettings* self = NewLC();
    CleanupStack::Pop();
    return self;
    }
    
CCatalogsAccessPointSettings* CCatalogsAccessPointSettings::NewLC() 
    {
    CCatalogsAccessPointSettings* self =
        new (ELeave) CCatalogsAccessPointSettings;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CCatalogsAccessPointSettings::CCatalogsAccessPointSettings()
    {
    }
    
void CCatalogsAccessPointSettings::ConstructL() 
    {
    iLongTextData = new (ELeave) CDesC16ArrayFlat(2);
    iText16Data = new (ELeave) CDesC16ArrayFlat(2);
    iText8Data = new (ELeave) CDesC8ArrayFlat(2);
    }

CCatalogsAccessPointSettings::~CCatalogsAccessPointSettings()
    {
    delete iLongTextData;
    delete iText16Data;
    delete iText8Data;

#ifdef _0
    iLongTextMembers.Close();
    iText16Members.Close();
    iText8Members.Close();
    iUintMembers.Close();
    iBoolMembers.Close();
    iUintData.Close();
    iBoolData.Close(); 
#endif  
    }
    
#ifdef _0
void CCatalogsAccessPointSettings::SetLongTextParameterL(
    const TApMember& aKey, const TDesC& aValue) 
    {
    TInt index = iLongTextMembers.Find(aKey);
    
    if (index == KErrNotFound) 
        {
        // new parameter
        iLongTextMembers.AppendL(aKey);
        iLongTextData->AppendL(aValue);
        }
    else 
        {
        // parameter exists already, change the value
        iLongTextData->Delete(index);
        iLongTextData->InsertL(index, aValue);   
        }        
    }
    
void CCatalogsAccessPointSettings::SetText16ParameterL(
    const TApMember& aKey, const TDesC16& aValue) 
    {
    TInt index = iText16Members.Find(aKey);
    
    if (index == KErrNotFound) 
        {
        // new parameter
        iText16Members.AppendL(aKey);
        iText16Data->AppendL(aValue);
        }
    else 
        {
        // parameter exists already, change the value
        iText16Data->Delete(index);
        iText16Data->InsertL(index, aValue);   
        }
    }
    
void CCatalogsAccessPointSettings::SetText8ParameterL(
    const TApMember& aKey, const TDesC8& aValue) 
    {
    TInt index = iText8Members.Find(aKey);
    
    if (index == KErrNotFound) 
        {
        // new parameter
        iText8Members.AppendL(aKey);
        iText8Data->AppendL(aValue);
        }
    else 
        {
        // parameter exists already, change the value
        iText8Data->Delete(index);
        iText8Data->InsertL(index, aValue);   
        }
    }
    
void CCatalogsAccessPointSettings::SetUintParameterL(
    const TApMember& aKey, const TUint32& aValue) 
    {
    TInt index = iUintMembers.Find(aKey);
    if (index == KErrNotFound) 
        {
        // new parameter
        iUintMembers.AppendL(aKey);
        iUintData.AppendL(aValue);
        }
    else 
        {
        // replace existing data
        iUintData.Remove(index);
        iUintData.InsertL(aValue, index);
        }
    }
    

void CCatalogsAccessPointSettings::SetBoolParameterL(
    const TApMember& aKey, const TBool& aValue) 
    {
    TInt index = iBoolMembers.Find(aKey);
    if (index == KErrNotFound) 
        {
        // new parameter
        iBoolMembers.AppendL(aKey);
        iBoolData.AppendL(aValue);
        }
    else 
        {
        // replace existing value
        iBoolData.Remove(index);
        iBoolData.InsertL(aValue, index);
        }
    }
        
TInt CCatalogsAccessPointSettings::LongTextParameterCount() const 
    {
    return iLongTextMembers.Count();
    }

TInt CCatalogsAccessPointSettings::Text16ParameterCount() const 
    {
    return iText16Members.Count();
    }

TInt CCatalogsAccessPointSettings::Text8ParameterCount() const 
    {
    return iText8Members.Count();
    }

TInt CCatalogsAccessPointSettings::UintParameterCount() const 
    {
    return iUintMembers.Count();
    }

TInt CCatalogsAccessPointSettings::BoolParameterCount() const 
    {
    return iBoolMembers.Count();
    }
    
void CCatalogsAccessPointSettings::LongTextParameter(
    TInt aIndex, TApMember& aKey, TPtrC& aValue) const 
    {
    aKey = iLongTextMembers[aIndex];
    aValue.Set((*iLongTextData)[aIndex]);
    }
    
void CCatalogsAccessPointSettings::Text16Parameter(
    TInt aIndex, TApMember& aKey, TPtrC16& aValue) const 
    {
    aKey = iText16Members[aIndex];
    aValue.Set((*iText16Data)[aIndex]);
    }

void CCatalogsAccessPointSettings::Text8Parameter(
    TInt aIndex, TApMember& aKey, TPtrC8& aValue) const 
    {
    aKey = iText8Members[aIndex];
    aValue.Set((*iText8Data)[aIndex]);
    }
    
void CCatalogsAccessPointSettings::UintParameter(
    TInt aIndex, TApMember& aKey, TUint32& aValue) const 
    {
    aKey = iUintMembers[aIndex];
    aValue = iUintData[aIndex];
    }
        
void CCatalogsAccessPointSettings::BoolParameter(
    TInt aIndex, TApMember& aKey, TBool& aValue) const 
    {
    aKey = iBoolMembers[aIndex];
    aValue = iBoolData[aIndex];
    }
#endif

void CCatalogsAccessPointSettings::ExternalizeL(RWriteStream& /* aStream */) 
    {
    DLTRACEIN((""));
    
#ifdef _0
    // write long text parameters
    TInt longTextCount = LongTextParameterCount();
    aStream.WriteInt32L(longTextCount);
    for (TInt i = 0; i < longTextCount; i++) 
        {
        aStream.WriteInt32L(iLongTextMembers[i]);
        ExternalizeDesL((*iLongTextData)[i], aStream);
        }

    // write 16-bit text parameters
    TInt text16Count = Text16ParameterCount();
    aStream.WriteInt32L(text16Count);
    for (TInt i = 0; i < text16Count; i++) 
        {
        aStream.WriteInt32L(iText16Members[i]);
        ExternalizeDesL((*iText16Data)[i], aStream);
        }
        
    // write 8-bit text parameters
    TInt text8Count = Text8ParameterCount();
    aStream.WriteInt32L(text8Count);
    for (TInt i = 0; i < text8Count; i++) 
        {
        aStream.WriteInt32L(iText8Members[i]);
        ExternalizeDesL((*iText8Data)[i], aStream);
        }        
        
    // write uint parameters
    TInt uintCount = UintParameterCount();
    aStream.WriteInt32L(uintCount);
    for (TInt i = 0; i < uintCount; i++) 
        {
        aStream.WriteInt32L(iUintMembers[i]);
        aStream.WriteUint32L(iUintData[i]);
        }
        
    // write bool parameter
    TInt boolCount = BoolParameterCount();
    aStream.WriteInt32L(boolCount);
    for (TInt i = 0; i < boolCount; i++) 
        {
        aStream.WriteInt32L(iBoolMembers[i]);
        aStream.WriteInt8L(iBoolData[i]);
        }

#endif
    DLTRACEOUT((""));        
    }
    
void CCatalogsAccessPointSettings::InternalizeL(RReadStream& /* aStream */) 
    {
    
#ifdef _0
    // read long text parameters
    TInt longTextCount = aStream.ReadInt32L();
    iLongTextMembers.ReserveL(longTextCount);
    for (TInt i = 0; i < longTextCount; i++) 
        {
        iLongTextMembers.AppendL((TApMember)aStream.ReadInt32L());
        HBufC* buf = NULL;
        InternalizeDesL(buf, aStream);
        CleanupStack::PushL(buf);
        iLongTextData->AppendL(*buf);
        CleanupStack::PopAndDestroy(buf);
        }
        
    // read 16-bit text parameters
    TInt text16Count = aStream.ReadInt32L();
    iText16Members.ReserveL(text16Count);
    for (TInt i = 0; i < text16Count; i++) 
        {
        iText16Members.AppendL((TApMember)aStream.ReadInt32L());
        HBufC16* buf = NULL;
        InternalizeDesL(buf, aStream);
        CleanupStack::PushL( buf );
        iText16Data->AppendL(*buf);
        CleanupStack::PopAndDestroy( buf );
        }

    // read 8-bit text parameters
    TInt text8Count = aStream.ReadInt32L();
    iText8Members.ReserveL(text8Count);
    for (TInt i = 0; i < text8Count; i++) 
        {
        iText8Members.AppendL((TApMember)aStream.ReadInt32L());
        HBufC8* buf = NULL;
        InternalizeDesL(buf, aStream);
        CleanupStack::PushL( buf );
        iText8Data->AppendL(*buf);
        CleanupStack::PopAndDestroy( buf );
        }
    
    // read uint parameters
    TInt uintCount = aStream.ReadInt32L();
    iUintMembers.ReserveL(uintCount);
    iUintData.ReserveL(uintCount);
    for (TInt i = 0; i < uintCount; i++) 
        {
        iUintMembers.AppendL((TApMember)aStream.ReadInt32L());
        iUintData.AppendL(aStream.ReadUint32L());
        }
        
    // read bool parameters
    TInt boolCount = aStream.ReadInt32L();
    iBoolMembers.ReserveL(boolCount);
    iBoolData.ReserveL(boolCount);
    for (TInt i = 0; i < boolCount; i++) 
        {
        iBoolMembers.AppendL((TApMember)aStream.ReadInt32L());
        iBoolData.AppendL(aStream.ReadInt8L());
        }
       
#endif
    }
