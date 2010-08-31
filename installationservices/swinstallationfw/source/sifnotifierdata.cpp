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
* This file implements the data classes provided by the SifNotifier library
*
*/

#include <usif/sif/sifnotification.h> 
#include <scs/cleanuputils.h>
#include <scs/streamingarray.h>
#include "sifnotification_internal.h"
#include <usif/sif/sif.h>


using namespace Usif;

//////////////////////////
// CSifOperationStartData
//////////////////////////

CSifOperationStartData::CSifOperationStartData()
    {
    // empty
    }

EXPORT_C CSifOperationStartData::~CSifOperationStartData()
    {
    delete iGlobalComponentId;                                    
    delete iComponentName;
    delete iComponentIcon;
    iApplicationNames.ResetAndDestroy();
    iApplicationIcons.ResetAndDestroy();          
    delete iIconPath;      
    delete iSoftwareType;
    }


EXPORT_C CSifOperationStartData* CSifOperationStartData::NewL(RReadStream& aStream)
    {
    CSifOperationStartData *self = new(ELeave) CSifOperationStartData();
    CleanupStack::PushL(self);
    self->InternalizeL(aStream);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CSifOperationStartData* CSifOperationStartData::NewL(const TDesC& aGlobalComponentId, const TDesC& aComponentName,
                                        const RPointerArray<HBufC>& aApplicationNames, const RPointerArray<HBufC>& aApplicationIcons, 
                                        TInt aComponentSize, const TDesC& aIconPath, const TDesC& aComponentIcon, const TDesC& aSoftwareType)
    {
    return CSifOperationStartData::NewL(aGlobalComponentId, aComponentName, 
                                        aApplicationNames, aApplicationIcons, aComponentSize, aIconPath, aComponentIcon, aSoftwareType, EInstalling);
    }

EXPORT_C CSifOperationStartData* CSifOperationStartData::NewL(const TDesC& aGlobalComponentId, const TDesC& aComponentName,
                                        const RPointerArray<HBufC>& aApplicationNames, const RPointerArray<HBufC>& aApplicationIcons, 
                                        TInt aComponentSize, const TDesC& aIconPath, const TDesC& aComponentIcon, const TDesC& aSoftwareType, 
                                        TSifOperationPhase aOperationPhase)
    {
    CSifOperationStartData *self = CSifOperationStartData::NewLC(aGlobalComponentId, aComponentName, 
                                        aApplicationNames, aApplicationIcons, aComponentSize, aIconPath, aComponentIcon, aSoftwareType, aOperationPhase);
    CleanupStack::Pop(self);
    return self;
    }


EXPORT_C CSifOperationStartData* CSifOperationStartData::NewLC(const TDesC& aGlobalComponentId, const TDesC& aComponentName,
                                        const RPointerArray<HBufC>& aApplicationNames, const RPointerArray<HBufC>& aApplicationIcons, const TInt aComponentSize, 
                                        const TDesC& aIconPath, const TDesC& aComponentIcon, const TDesC& aSoftwareType)
    {
    return CSifOperationStartData::NewLC(aGlobalComponentId, aComponentName, 
                                        aApplicationNames, aApplicationIcons, aComponentSize, aIconPath, aComponentIcon, aSoftwareType, EInstalling);
    }

EXPORT_C CSifOperationStartData* CSifOperationStartData::NewLC(const TDesC& aGlobalComponentId, const TDesC& aComponentName,
										const RPointerArray<HBufC>& aApplicationNames, const RPointerArray<HBufC>& aApplicationIcons, const TInt aComponentSize, 
                                        const TDesC& aIconPath, const TDesC& aComponentIcon, const TDesC& aSoftwareType, TSifOperationPhase aOperationPhase)
    {
    CSifOperationStartData *self = new(ELeave) CSifOperationStartData();
    CleanupStack::PushL(self);
    self->ConstructL(aGlobalComponentId, aComponentName, aApplicationNames, aApplicationIcons, aComponentSize, aIconPath, aComponentIcon, aSoftwareType, aOperationPhase);
    return self;
    }

void CSifOperationStartData::ConstructL(const TDesC& aGlobalComponentId, const TDesC& aComponentName, const RPointerArray<HBufC>& aApplicationNames,
                                        const RPointerArray<HBufC>& aApplicationIcons, TInt aComponentSize, const TDesC& aIconPath, const TDesC& aComponentIcon, 
                                        const TDesC& aSoftwareType, TSifOperationPhase aOperationPhase)
    {
	iGlobalComponentId = aGlobalComponentId.AllocL();
    iComponentName = aComponentName.AllocL();
    iComponentIcon = aComponentIcon.AllocL();

    for(TInt i=0; i<aApplicationNames.Count();++i)
        {
        HBufC* applicationName = aApplicationNames[i]->AllocLC();
        iApplicationNames.AppendL(applicationName);
        CleanupStack::Pop(applicationName);
        }

    for(TInt i=0; i<aApplicationIcons.Count();++i)
        {
        HBufC* applicationIcon = aApplicationIcons[i]->AllocLC();
        iApplicationIcons.AppendL(applicationIcon);
        CleanupStack::Pop(applicationIcon);
        }
    
    iComponentSize = aComponentSize;
    iIconPath = aIconPath.AllocL();      
    iSoftwareType = aSoftwareType.AllocL();
    iOperationPhase = aOperationPhase;
    }

EXPORT_C void CSifOperationStartData::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint32L(ESifOperationStart);
	aStream << *iGlobalComponentId;
	aStream << *iComponentName;
	ExternalizePointersArrayL(iApplicationNames, aStream);
	ExternalizePointersArrayL(iApplicationIcons, aStream);
	aStream.WriteUint16L(iComponentSize);
	aStream << *iIconPath;
	aStream << *iComponentIcon;
	aStream << *iSoftwareType;
	aStream << TCardinality(iOperationPhase);
	}
	
void CSifOperationStartData::InternalizeL(RReadStream& aStream)
    {
    iOperationType = static_cast<TSifOperation>(aStream.ReadUint32L());
    DeleteObjectZ(iGlobalComponentId);
    iGlobalComponentId = HBufC::NewL(aStream, KMaxTInt);
    DeleteObjectZ(iComponentName);
    iComponentName = HBufC::NewL(aStream, KMaxTInt);
    iApplicationNames.ResetAndDestroy();
    InternalizePointersArrayL(iApplicationNames, aStream);
    iApplicationIcons.ResetAndDestroy();
    InternalizePointersArrayL(iApplicationIcons, aStream);
    iComponentSize = aStream.ReadUint16L();
    DeleteObjectZ(iIconPath);
    iIconPath = HBufC::NewL(aStream, KMaxTInt);
    DeleteObjectZ(iComponentIcon);
    iComponentIcon = HBufC::NewL(aStream, KMaxTInt);
    DeleteObjectZ(iSoftwareType);
    iSoftwareType = HBufC::NewL(aStream, KMaxTInt);
    TCardinality c;
    aStream >> c;
    iOperationPhase = static_cast<TSifOperationPhase>(static_cast<TInt>(c));
    }

EXPORT_C const HBufC& CSifOperationStartData::GlobalComponentId() const
    {
    return *iGlobalComponentId;
    }

EXPORT_C const HBufC& CSifOperationStartData::ComponentName() const
    {
    return *iComponentName;
    }

EXPORT_C const RPointerArray<HBufC>& CSifOperationStartData::ApplicationNames() const
    {
    return iApplicationNames;
    }

EXPORT_C const RPointerArray<HBufC>& CSifOperationStartData::ApplicationIcons() const
    {
    return iApplicationIcons;
    }

EXPORT_C const HBufC& CSifOperationStartData::IconPath() const
    {
    return *iIconPath;
    }

EXPORT_C const HBufC& CSifOperationStartData::ComponentIcon() const
    {
    return *iComponentIcon;
    }

EXPORT_C const HBufC& CSifOperationStartData::SoftwareType() const
    {
    return *iSoftwareType;
    }


EXPORT_C TInt CSifOperationStartData::ComponentSize() const
    {
    return iComponentSize;
    }

EXPORT_C TSifOperationPhase CSifOperationStartData::OperationPhase() const
    {
    return iOperationPhase;
    }

////////////////////////
// CSifOperationEndData
////////////////////////

CSifOperationEndData::CSifOperationEndData()
    {
    // empty
    }

EXPORT_C CSifOperationEndData::~CSifOperationEndData()
    {
    delete iGlobalComponentId;
    delete iErrMsg;                                    
    delete iErrMsgDetails;
    }


EXPORT_C CSifOperationEndData* CSifOperationEndData::NewL(RReadStream& aStream)
    {
    CSifOperationEndData *self = new(ELeave) CSifOperationEndData();
    CleanupStack::PushL(self);
    self->InternalizeL(aStream);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CSifOperationEndData* CSifOperationEndData::NewL(const TDesC& aGlobalComponentId, TErrorCategory aErrCategory, TInt aErrCode, const TDesC& aErrMsg, const TDesC& aErrMsgDetails)
    {
    CSifOperationEndData *self = CSifOperationEndData::NewLC(aGlobalComponentId, aErrCategory, aErrCode, aErrMsg, aErrMsgDetails);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CSifOperationEndData* CSifOperationEndData::NewLC(const TDesC& aGlobalComponentId, TErrorCategory aErrCategory, TInt aErrCode, const TDesC& aErrMsg, const TDesC& aErrMsgDetails)
    {
    CSifOperationEndData *self = new(ELeave) CSifOperationEndData();
    CleanupStack::PushL(self);
    self->ConstructL(aGlobalComponentId, aErrCategory, aErrCode, aErrMsg, aErrMsgDetails);
    return self;
    }

void CSifOperationEndData::ConstructL(const TDesC& aGlobalComponentId, TErrorCategory aErrCategory, TInt aErrCode, const TDesC& aErrMsg, const TDesC& aErrMsgDetails)
    {
	iGlobalComponentId = aGlobalComponentId.AllocL();      
    iErrCategory = aErrCategory;
    iErrCode = aErrCode;
    iErrMsg = aErrMsg.AllocL();
    iErrMsgDetails = aErrMsgDetails.AllocL();
    }

EXPORT_C void CSifOperationEndData::ExternalizeL(RWriteStream& aStream) const
    {
	aStream.WriteUint32L(ESifOperationEnd);
    aStream << *iGlobalComponentId;
    aStream << TCardinality(iErrCategory);
    aStream.WriteUint16L(iErrCode);
    aStream << *iErrMsg;
    aStream << *iErrMsgDetails;
    }
    
void CSifOperationEndData::InternalizeL(RReadStream& aStream)
    {
    iOperationType = static_cast<TSifOperation>(aStream.ReadUint32L());
    iGlobalComponentId = HBufC::NewL(aStream, KMaxTInt);
    TCardinality c;
    aStream >> c;
	iErrCategory = static_cast<TErrorCategory>(static_cast<TInt>(c));
    iErrCode = aStream.ReadUint16L();
    DeleteObjectZ(iErrMsg);
    iErrMsg = HBufC::NewL(aStream, KMaxTInt);
    DeleteObjectZ(iErrMsgDetails);
    iErrMsgDetails = HBufC::NewL(aStream, KMaxTInt);
    }

EXPORT_C const HBufC& CSifOperationEndData::GlobalComponentId() const
    {
    return *iGlobalComponentId;
    }

EXPORT_C TErrorCategory CSifOperationEndData::ErrorCategory() const
    {
    return iErrCategory;
    }

EXPORT_C TInt CSifOperationEndData::ErrorCode() const
    {
    return iErrCode;
    }

EXPORT_C const HBufC& CSifOperationEndData::ErrorMessage() const
    {
    return *iErrMsg;
    }

EXPORT_C const HBufC& CSifOperationEndData::ErrorMessageDetails() const
    {
    return *iErrMsgDetails;
    }

/////////////////////////////
// CSifOperationProgressData
/////////////////////////////

CSifOperationProgressData::CSifOperationProgressData()
    {
    // empty
    }


EXPORT_C CSifOperationProgressData* CSifOperationProgressData::NewL(RReadStream& aStream)
    {
    CSifOperationProgressData *self = new(ELeave) CSifOperationProgressData();
    CleanupStack::PushL(self);
    self->InternalizeL(aStream);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CSifOperationProgressData* CSifOperationProgressData::NewL(const TDesC& aGlobalComponentId, TSifOperationPhase aPhase, TSifOperationSubPhase aSubPhase, TInt aCurrentProgress, TInt aTotal)
    {
    CSifOperationProgressData *self = CSifOperationProgressData::NewLC(aGlobalComponentId, aPhase, aSubPhase, aCurrentProgress, aTotal);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CSifOperationProgressData* CSifOperationProgressData::NewLC(const TDesC& aGlobalComponentId, TSifOperationPhase aPhase, TSifOperationSubPhase aSubPhase, TInt aCurrentProgress, TInt aTotal)
    {
    CSifOperationProgressData *self = new(ELeave) CSifOperationProgressData();
    CleanupStack::PushL(self);
    self->ConstructL(aGlobalComponentId, aPhase, aSubPhase, aCurrentProgress, aTotal);
    return self;
    }

void CSifOperationProgressData::ConstructL(const TDesC& aGlobalComponentId, TSifOperationPhase aPhase, TSifOperationSubPhase aSubPhase, TInt aCurrentProgress, TInt aTotal)
    {
    iGlobalComponentId = aGlobalComponentId.AllocL();      
    iPhase = aPhase;
    iSubPhase = aSubPhase;
    iCurrentProgress = aCurrentProgress;
    iTotal = aTotal;
    }

EXPORT_C void CSifOperationProgressData::ExternalizeL(RWriteStream& aStream) const
    {
    aStream << *iGlobalComponentId;
    aStream << TCardinality(iPhase);
    aStream << TCardinality(iSubPhase);
    aStream.WriteUint16L(iCurrentProgress);
    aStream.WriteUint16L(iTotal);
    }
    
void CSifOperationProgressData::InternalizeL(RReadStream& aStream)
    {
    iGlobalComponentId = HBufC::NewL(aStream, KMaxTInt);
    TCardinality c;
    aStream >> c;
    iPhase = static_cast<TSifOperationPhase>(static_cast<TInt>(c));
    aStream >> c;
    iSubPhase = static_cast<TSifOperationSubPhase>(static_cast<TInt>(c));
    iCurrentProgress = aStream.ReadUint16L();
    iTotal = aStream.ReadUint16L();
    }

EXPORT_C const HBufC& CSifOperationProgressData::GlobalComponentId() const
    {
    return *iGlobalComponentId;
    }

EXPORT_C TSifOperationPhase CSifOperationProgressData::Phase() const
    {
    return iPhase;
    }

EXPORT_C TSifOperationSubPhase CSifOperationProgressData::SubPhase() const
    {
    return iSubPhase;
    }

EXPORT_C TInt CSifOperationProgressData::CurrentProgress() const
    {
    return iCurrentProgress;
    }

EXPORT_C TInt CSifOperationProgressData::CurrentProgess() const
    {
    return CurrentProgress();
    }

EXPORT_C TInt CSifOperationProgressData::Total() const
    {
    return iTotal;
    }


EXPORT_C CSifOperationProgressData::~CSifOperationProgressData()
    {
    delete iGlobalComponentId;
    }
