/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* sifcommon_internal.h.
*
*/

/**
 @file
 @released
 @internal
*/

#ifndef __SIFCOMMON_INTERNAL_H__
#define __SIFCOMMON_INTERNAL_H__

namespace Usif {

const TUint KMaxOpaqueParamNameDescriptorSize = 128;
const TUint KMaxOpaqueParamsExternalizedSize = 4096;
const TUint KMaxOpaqueParamValueDescriptorSize = 1024;

class MOpaqueParam
    {
public:
    enum TType
        {
        EString,
        EStringArray,
        EInteger,
        EIntegerArray
        };
    
    virtual void ExternalizeL(RWriteStream& aStream) const = 0;
    virtual void InternalizeL(RReadStream& aStream) = 0;
    virtual const HBufC& Name() const = 0;
    virtual TType Type() const = 0;
    virtual TInt ValueSize() const = 0;
    virtual ~MOpaqueParam() {}
    };

class CItemBase : public CBase, public MOpaqueParam
    {
public:
    void ExternalizeL(RWriteStream& aStream) const;
    void InternalizeL(RReadStream& aStream);
    const HBufC& Name() const;
    TType Type() const;
    TInt ValueSize() const;
    ~CItemBase();
    
protected:
    CItemBase(TType aType);
    void SetValueSize(TInt aSize);
    void VerifyExternalizedSizeForParamL(TUint aValueSize) const;
    void SetNameL(const TDesC& aName);
    
private:
    HBufC* iName;
    TType iType;
    TUint iSize;
    };
class CStringItem : public CItemBase
    {
public:
    static CStringItem* NewL(RReadStream& aStream);
    static CStringItem* NewL(const TDesC& aName, const TDesC& aValue);
    void ExternalizeL(RWriteStream& aStream) const;
    void InternalizeL(RReadStream& aStream);  
    const HBufC& Name() const;
    TType Type() const;
    TInt ValueSize() const;
    const TDesC& StringValue() const;
    ~CStringItem();
    
private:
    CStringItem();
    void ConstructL(const TDesC& aName, const TDesC& aValue);
    
private:
    HBufC* iString;
    };

class CStringArrayItem : public CItemBase
    {
public:
    static CStringArrayItem* NewL(RReadStream& aStream);
    static CStringArrayItem* NewL(const TDesC& aName, const RPointerArray<HBufC>& aValueArray);
    void ExternalizeL(RWriteStream& aStream) const;
    void InternalizeL(RReadStream& aStream);
    const HBufC& Name() const;
    TType Type() const;
    TInt ValueSize() const;
    const RPointerArray<HBufC>& StringArrayValue() const;
    ~CStringArrayItem();
    
private:
    void ConstructL(const TDesC& aName, const RPointerArray<HBufC>& aValueArray);
    CStringArrayItem();
    
private:
    RPointerArray<HBufC> iStringArray;    
    };

class CIntegerItem : public CItemBase
    {
public:
    static CIntegerItem* NewL(RReadStream& aStream);
    static CIntegerItem* NewL(const TDesC& aName, TInt aValue);
    void ExternalizeL(RWriteStream& aStream) const;
    void InternalizeL(RReadStream& aStream);
    const HBufC& Name() const;
    TType Type() const;
    TInt ValueSize() const;
    TInt IntegerValue() const;
    ~CIntegerItem();
    
private:
    CIntegerItem(TInt aValue);
    void ConstructL(const TDesC& aName);
    
private:
    TInt iInteger;
    };

class CIntegerArrayItem : public CItemBase
    {
public:
    static CIntegerArrayItem* NewL(RReadStream& aStream);
    static CIntegerArrayItem* NewL(const TDesC& aName, const RArray<TInt>& aValueArray);
    void ExternalizeL(RWriteStream& aStream) const;
    void InternalizeL(RReadStream& aStream);
    const HBufC& Name() const;
    TType Type() const;
    TInt ValueSize() const;
    const RArray<TInt>& IntegerArrayValue() const;
    ~CIntegerArrayItem();
    
private:
    CIntegerArrayItem();
    void ConstructL(const TDesC& aName, const RArray<TInt>& aValueArray);
    
private:
    RArray<TInt> iIntegerArray;
    };


} //namespace 

#endif //__SIFCOMMON_INTERNAL_H__
