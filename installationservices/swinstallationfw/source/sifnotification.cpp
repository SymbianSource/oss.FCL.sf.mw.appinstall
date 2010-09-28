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
* This file implements the SifNotifier library
*
*/

#include <usif/sif/sifnotification.h>
#include <s32mem.h> 
#include <scs/cleanuputils.h>
#include <scs/streamingarray.h>
#include "sifnotification_internal.h"
#include <usif/sif/sif.h>
#include "e32property.h" 
#include "scrclient.inl"
#include "usiflog.h"


using namespace Usif;

/**
 * A global method which is used by both the publisher and subscriber code to retrieve data.
 * 
 */
void GetDataL(TUint aKey, TInt& aBufferSize, RBuf8& aData)
    {
    aData.CreateL(aBufferSize);
    aData.CleanupClosePushL();
    TInt err = RProperty::Get(KUidSystemCategory, aKey, aData);
    
    // If the buffer size is too small.
    while (err == KErrOverflow)
        {
        aBufferSize*=2;
        aData.ReAllocL(aBufferSize);
        err = RProperty::Get(KUidSystemCategory, aKey, aData);
        }
        
    User::LeaveIfError(err);
    CleanupStack::Pop();
    }

////////////////////////////
// CPublishSifOperationInfo
////////////////////////////

CPublishSifOperationInfo::CPublishSifOperationInfo():
    iBufferSize(KBufferSize)
    {
    // empty
    }

EXPORT_C CPublishSifOperationInfo::~CPublishSifOperationInfo()
    {
    delete iDeleteTimer;
    if (iStartEndKey != 0)
        {
        RProperty::Delete(KUidSystemCategory, iStartEndKey);
        RProperty::Delete(KUidSystemCategory, (iStartEndKey+1));
        }
    }

EXPORT_C CPublishSifOperationInfo* CPublishSifOperationInfo::NewL()
    {
    CPublishSifOperationInfo *self = CPublishSifOperationInfo::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CPublishSifOperationInfo* CPublishSifOperationInfo::NewLC()
    {
    CPublishSifOperationInfo *self = new(ELeave) CPublishSifOperationInfo();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CPublishSifOperationInfo::ConstructL()
    {
    
    }

TUint CPublishSifOperationInfo::AvailableKeyL()
    {
    TBuf8<KMaxNumberOfOperations*sizeof(TInt)> buf;
    for(TInt i=0; i< KMaxNumberOfOperations-1; ++i)
        {
        if(KErrNotFound == RProperty::Get(KUidSystemCategory, KSifPropertyTable[i], buf))
            {
            return KSifPropertyTable[i];
            }
        }

	User::Leave(KErrInUse);
    return KErrNone; // Keep compiler happy.
    }

TUint CPublishSifOperationInfo::KeyForGlobalComponentIdL(const TDesC& aGlobalComponentId)
    {
    TBuf8<KMaxNumberOfOperations*sizeof(TInt)> buf;
    //Get all ongoing operations
    User::LeaveIfError(RProperty::Get(KUidSystemCategory, KSifOperationKey, buf));
   
    RDesReadStream readStream(buf);
    CleanupClosePushL(readStream);
    CSifOperationKey* opKey = CSifOperationKey::NewL(readStream);
    CleanupStack::PopAndDestroy(&readStream);
    CleanupStack::PushL(opKey);
    
    RArray<TUint> startEndKeyArr = opKey->StartEndKeys();
	// Get the data for all the ongoing operations and check for global component id
    for (TInt i=0; i<startEndKeyArr.Count(); ++i)
        {
        RBuf8 startDatabuf;
        startDatabuf.CleanupClosePushL();

		GetDataL(startEndKeyArr[i], iBufferSize, startDatabuf);
        RDesReadStream readStream(startDatabuf);
        CleanupClosePushL(readStream);
        
        // Read the first 4 bytes
        TSifOperation opType = static_cast<TSifOperation>(readStream.ReadInt32L());         
        if(opType != ESifOperationStart)
            {
            CleanupStack::PopAndDestroy(2, &startDatabuf);
            continue;
            }
                    
        // Release the stream and reopen it.
        readStream.Release();
        readStream.Open(startDatabuf);
                    
        CSifOperationStartData* startObj = CSifOperationStartData::NewL(readStream);
        CleanupStack::PushL(startObj);
		// Check if the global component id is the same as what we are looking for
        if( KErrNone == aGlobalComponentId.Compare(startObj->GlobalComponentId()))
            {
            TInt key = startEndKeyArr[i];
            CleanupStack::PopAndDestroy(4, opKey);
            return key;
            }
        CleanupStack::PopAndDestroy(3, &startDatabuf);
        }
    CleanupStack::PopAndDestroy(opKey);
    
    // Could not find corresponding key
    User::Leave(KErrNotFound);
    return KErrNone; // Keep compiler happy.
    }
    
void CPublishSifOperationInfo::SetKeyToDelete(TUint aStartEndKey)
    {
    iStartEndKey = aStartEndKey;
    }

EXPORT_C void CPublishSifOperationInfo::PublishStartL(CSifOperationStartData& aSifOperationStartData)
    {
    // Get Property key from free pool which could be used for this operation.
    TUint key = AvailableKeyL();    
    
    // Define key from free pool. Start data could be bigger than 512 bytes, hence using ELargeByteArray.
    User::LeaveIfError(RProperty::Define(KUidSystemCategory, key, RProperty::ELargeByteArray,KSecurityPolicyWDD,KSecurityPolicyNone,KStartBufLength));
        
    // Store this key and delete it in case of any leaves
    SetKeyToDelete(key);
    
    // Publish start info of operation in defined key.
    RBuf8 bufStartData;
    bufStartData.CleanupClosePushL();
    ExternalizeRefObjectL(aSifOperationStartData, bufStartData);

    User::LeaveIfError(RProperty::Set(KUidSystemCategory, key, bufStartData));
    
    CleanupStack::PopAndDestroy(&bufStartData);
    
    // Define key for progress data for this operation.
    User::LeaveIfError(RProperty::Define(KUidSystemCategory, (key+1), RProperty::EByteArray,KSecurityPolicyWDD,KSecurityPolicyNone, KProgressBufLength));
    
    // Publish(append this key to existing info) the newly defined key in the global SIF key.
    TBuf8<KMaxNumberOfOperations*sizeof(TUint)> buf;

    User::LeaveIfError(RProperty::Get(KUidSystemCategory, KSifOperationKey, buf));

    RDesReadStream stream(buf);
    CleanupClosePushL(stream);      
    
    // Retrieve current keys.
    CSifOperationKey* currentKeys = CSifOperationKey::NewL(stream);
    CleanupStack::PopAndDestroy(&stream);
    
    CleanupStack::PushL(currentKeys);
    
    // Append the new key.
    currentKeys->AddKeyL(key);
    
    RBuf8 bufOperationKeys;
    bufOperationKeys.CleanupClosePushL();
    ExternalizeRefObjectL(*currentKeys, bufOperationKeys);
    
    // Publish the new set of keys.
    User::LeaveIfError(RProperty::Set(KUidSystemCategory, KSifOperationKey, bufOperationKeys));
    
    // Everything finished fine, we need not delete the keys defined above.
    SetKeyToDelete(0);
    
    CleanupStack::PopAndDestroy(2, currentKeys);
    }

EXPORT_C void CPublishSifOperationInfo::PublishProgressL(CSifOperationProgressData& aSifOperationProgressData)
    {
    TUint startEndKey = 0;
    
    HBufC16 *compGlobalId = aSifOperationProgressData.GlobalComponentId().AllocLC();
	//Get the key defined for this operation
    startEndKey = KeyForGlobalComponentIdL(compGlobalId->Des());
    CleanupStack::PopAndDestroy(compGlobalId);

    //Publish progress info of operation in defined key
    RBuf8 bufProgressData;
    bufProgressData.CleanupClosePushL();
    ExternalizeRefObjectL(aSifOperationProgressData, bufProgressData);
    User::LeaveIfError(RProperty::Set(KUidSystemCategory, (startEndKey+1), bufProgressData));
    CleanupStack::PopAndDestroy(&bufProgressData);
    }

EXPORT_C void CPublishSifOperationInfo::PublishCompletionL(CSifOperationEndData& aSifOperationEndData)
    {
    TUint endKey = 0;
    HBufC *compGlobalId = aSifOperationEndData.GlobalComponentId().AllocLC();
	//Get the key defined for this operation
    endKey = KeyForGlobalComponentIdL(compGlobalId->Des());
    CleanupStack::PopAndDestroy(compGlobalId);
    
    // Store this key and delete it in case of any leaves
    SetKeyToDelete(endKey);
    
    //Publish end info of operation in defined key
    RBuf8 bufEndData;
    bufEndData.CleanupClosePushL();
    ExternalizeRefObjectL(aSifOperationEndData, bufEndData);

    User::LeaveIfError(RProperty::Set(KUidSystemCategory, endKey, bufEndData));
    CleanupStack::PopAndDestroy(&bufEndData);
    
    // Wait for KDeleteKeyDelay seconds before deleting this key.
    if(iDeleteTimer)
        iDeleteTimer->Cancel();
    delete iDeleteTimer;
    iDeleteTimer = CDeleteKeyTimer::NewL(endKey);
    
    // Everything finished fine, we need not delete the keys defined for this operation
    SetKeyToDelete(0);
    
    iDeleteTimer->Start();

    }



//////////////////////////
// CSifOperationsNotifier
//////////////////////////


EXPORT_C CSifOperationsNotifier* CSifOperationsNotifier::NewL(MSifOperationsHandler& aHandler)
    {
    CSifOperationsNotifier* self = CSifOperationsNotifier::NewLC(aHandler);
    CleanupStack::Pop(self);
    return self;
    }


EXPORT_C CSifOperationsNotifier* CSifOperationsNotifier::NewLC(MSifOperationsHandler& aHandler)
    {
    CSifOperationsNotifier* self = new (ELeave) CSifOperationsNotifier(aHandler);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CSifOperationsNotifier::CSifOperationsNotifier(MSifOperationsHandler& aHandler):
    CActive(EPriorityNormal),
    iHandler(aHandler),
    iBufferSize(KBufferSize) 
    {
    //Empty
    }


void CSifOperationsNotifier::ConstructL()
    {
    CActiveScheduler::Add(this);
    iProperty.Attach(KUidSystemCategory,KSifOperationKey,EOwnerThread);
    
    // On first start, the notifier looks for any active operations and 
    // notifies the client.
    iNotifierState = EFirstStart;
    
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }


void CSifOperationsNotifier::RunL()
    {  
    User::LeaveIfError(iStatus.Int());
    
    // Resubscribe for any changes to the global key.
    WaitForChangeL();
    
    if( iNotifierState == EFirstStart)
        {
        // Notify the client of any active operation.
        ActiveOperationsNotificationL();
        iNotifierState = EWaitForChange;    
        return;
        }
 
   // Global key change notification received.      
    RArray<TUint> newKeyArray;
    GetKeysL(newKeyArray);
    CleanupClosePushL(newKeyArray);
    
    if(iKeyArray.Count() < newKeyArray.Count())
        {
        // New Keys added.
        RBuf8 externalizedBuffer;
        externalizedBuffer.CleanupClosePushL();
        
        // Retrieve the new keys and send the start operation notification to each.
        for(TInt i = iKeyArray.Count(); i< newKeyArray.Count(); ++i)
            {
            externalizedBuffer.Close();
            GetDataL(newKeyArray[i], iBufferSize, externalizedBuffer);
            
            RDesReadStream stream(externalizedBuffer);
            CleanupClosePushL(stream);
            // Read the first 4 bytes
            TSifOperation opType = static_cast<TSifOperation>(stream.ReadInt32L());         
            if(opType != ESifOperationStart)
                {
                CleanupStack::PopAndDestroy(&stream);
                continue;
                }
            
            // Release the stream and reopen it.
            stream.Release();
            stream.Open(externalizedBuffer);
           
            CSifOperationStartData* startData = CSifOperationStartData::NewL(stream);
            CleanupStack::PushL(startData);
            
            //Update iKeyArray with the new key.
            iKeyArray.AppendL(newKeyArray[i]);
            
            iHandler.StartOperationHandler(newKeyArray[i], *startData);
            CleanupStack::PopAndDestroy(2, &stream);            
            
            }
        CleanupStack::PopAndDestroy(&externalizedBuffer);
        }
    else if (iKeyArray.Count() > newKeyArray.Count())
        {
        // Keys removed.
        for(TInt i = 0; i<iKeyArray.Count(); ++i)
            {
            // Key present in iKeyArray (the previous array of keys) is no longer
            // present in the newKeyArray.
            if(newKeyArray.Find(iKeyArray[i]) == KErrNotFound)
                {
                CancelSubscribeL(iKeyArray[i]);
                iKeyArray.Remove(i);
                }
            }
        }   
    CleanupStack::PopAndDestroy(&newKeyArray);
    }

void CSifOperationsNotifier::ActiveOperationsNotificationL()
    {
    // Notify the caller of any operation currently in progress.
    GetKeysL(iKeyArray);
    
    RBuf8 externalizedBuffer;
    
    for(TInt i=0; i<iKeyArray.Count(); ++i)
        {
        externalizedBuffer.Close();
        GetDataL(iKeyArray[i], iBufferSize, externalizedBuffer);
        CleanupClosePushL(externalizedBuffer);
        
        RDesReadStream stream(externalizedBuffer);
        CleanupClosePushL(stream);
        
        //Read the first 4 bytes
        TSifOperation opType = static_cast<TSifOperation>(stream.ReadUint32L());
        if(opType != ESifOperationStart)
            {
            // Ignore this key and continue.
            CleanupStack::PopAndDestroy(2, &externalizedBuffer);
            continue;
            }
        // Release the stream and reopen it.
        stream.Release();
        stream.Open(externalizedBuffer);
        
        CSifOperationStartData* startData = CSifOperationStartData::NewL(stream);
        CleanupStack::PushL(startData);
        iHandler.StartOperationHandler(iKeyArray[i], *startData); 
        CleanupStack::PopAndDestroy(startData);  
           
        CleanupStack::PopAndDestroy(2, &externalizedBuffer);  
        }
    
    }
void CSifOperationsNotifier::WaitForChangeL()
    {   
    iProperty.Subscribe(iStatus);
    SetActive();
    }


void CSifOperationsNotifier::GetKeysL(RArray<TUint>& aKeys)
    {
    TBuf8<KMaxNumberOfOperations * sizeof(TInt)> keysBuffer;
    User::LeaveIfError(iProperty.Get(keysBuffer));
    
    RDesReadStream stream(keysBuffer);
    CleanupClosePushL(stream);
    
    // Construct the CSifOperationKey object from the buffer.
    CSifOperationKey* opKey = CSifOperationKey::NewL(stream);
    CleanupStack::PushL(opKey);
    
    for(TInt i=0; i<opKey->StartEndKeys().Count(); ++i)
        {
        aKeys.AppendL(opKey->StartEndKeys()[i]);
        }
    
    CleanupStack::PopAndDestroy(2, &stream);
    }


EXPORT_C void CSifOperationsNotifier::SubscribeL(TUint aKey, TBool aSubscribeForProgressNotifications)
    {
    User::LeaveIfError(iKeyArray.Find(aKey));
    
    // Start End notifier.
    CSifNotifierBase* startEndNotifier = CSifNotifierBase::NewLC(iHandler, aKey, CSifNotifierBase::EStartEndNotifier);
    iNotifierArray.AppendL(startEndNotifier);
    CleanupStack::Pop(startEndNotifier);
    
    if(aSubscribeForProgressNotifications)
        {
        // Progress Notifier.
        CSifNotifierBase* progressNotifier = CSifNotifierBase::NewLC(iHandler, aKey+1, CSifNotifierBase::EProgressNotifier);
        iNotifierArray.AppendL(progressNotifier);
        CleanupStack::Pop(progressNotifier);
        }
    }


EXPORT_C void CSifOperationsNotifier::CancelSubscribeL(TUint aKey)
    {
    TInt index = iKeyArray.Find(aKey);
    
    if(index == KErrNotFound)
        {
        // No need to leave.
        return;
        }
    
    for(TInt i=0; i< iNotifierArray.Count(); ++i)
        {

        if(iNotifierArray[i]->Key() == aKey)
            {
            delete iNotifierArray[i];
            iNotifierArray.Remove(i);
            
            // Delete progress notification object also, if present.
            // i now points to the next element
            if( i <= iNotifierArray.Count()-1 && iNotifierArray[i]->Key() == aKey+1)
                {
                delete iNotifierArray[i];
                iNotifierArray.Remove(i);
                }
            }
        
        }

    }


EXPORT_C void CSifOperationsNotifier::CancelSubscribeL()
    {
    //Destroy all the notifiers.
    Cancel();
    }


void CSifOperationsNotifier::DoCancel()
    {
    iProperty.Cancel();
    iNotifierArray.ResetAndDestroy();
    }

TInt CSifOperationsNotifier::RunError(TInt aError)
    {
    DEBUG_PRINTF2(_L8("CSifOperationsNotifier::RunError : Error code %d"), aError);
    // If RunL() leaves due to ActiveOperationsNotificationL(), change the state.
    if(iNotifierState == EFirstStart)
        {
        iNotifierState = EWaitForChange;
        }
    (void)aError;
    return KErrNone;
    }


EXPORT_C CSifOperationsNotifier::~CSifOperationsNotifier()
    {
    Cancel();
    iProperty.Close();
    iKeyArray.Close();
    }



//////////////////////////
// CSifNotifierBase
//////////////////////////


EXPORT_C CSifNotifierBase* CSifNotifierBase::NewL(MSifOperationsHandler& aHandler, TUint aKey, TNotifierType aType)
    {
    CSifNotifierBase* self = CSifNotifierBase::NewLC(aHandler, aKey, aType);
    CleanupStack::Pop(self);
    return self;
    }


EXPORT_C CSifNotifierBase* CSifNotifierBase::NewLC(MSifOperationsHandler& aHandler, TUint aKey, TNotifierType aType)
    {
    CSifNotifierBase* self = new (ELeave) CSifNotifierBase(aHandler, aKey, aType);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CSifNotifierBase::CSifNotifierBase(MSifOperationsHandler& aHandler, TUint aKey, TNotifierType aType):
    CActive(EPriorityNormal),
    iHandler(aHandler),
    iKey(aKey),
    iBufferSize(KBufferSize),
    iType(aType)
    {
    //Empty
    }


void CSifNotifierBase::ConstructL()
    {
    CActiveScheduler::Add(this);
    iProperty.Attach(KUidSystemCategory,iKey,EOwnerThread);
    // Subscribe to the key and wait for a notification.
    WaitForChangeL();
    }


void CSifNotifierBase::RunL()
    {
    User::LeaveIfError(iStatus.Int());
    
    //Resubscribe for any changes.
    WaitForChangeL();
    
    RBuf8 externalizedBuffer;
    GetDataL(iKey, iBufferSize, externalizedBuffer);  
    externalizedBuffer.CleanupClosePushL();
    
    RDesReadStream stream(externalizedBuffer);
    CleanupClosePushL(stream);
    
    switch (iType)
        {
        case EStartEndNotifier:
            {
            //Read the first 4 bytes
            TSifOperation opType = static_cast<TSifOperation>(stream.ReadInt32L());
            stream.Release();
            stream.Open(externalizedBuffer);
            switch(opType)
                {
                case ESifOperationEnd:
                    {
                    CSifOperationEndData* endData = CSifOperationEndData::NewL(stream);
                    CleanupStack::PushL(endData);
                    iHandler.EndOperationHandler(*endData);
                    CleanupStack::PopAndDestroy(endData);
                    break;
                    }
                    
                default:
                    // Cannot receive a start notification here !
                    User::Leave(KErrNotSupported);
                    break; 
                }            
            }
            
         break;
            
        case EProgressNotifier:
            {
            CSifOperationProgressData* progressData = CSifOperationProgressData::NewL(stream);
            CleanupStack::PushL(progressData);
            iHandler.ProgressOperationHandler(*progressData);
            CleanupStack::PopAndDestroy(progressData);
            }
        }

    
    CleanupStack::PopAndDestroy(2, &externalizedBuffer);
    }



void CSifNotifierBase::WaitForChangeL()
    {
    iProperty.Subscribe(iStatus);
    SetActive();
    }


TInt CSifNotifierBase::RunError(TInt aError)
    {
    DEBUG_PRINTF2(_L8("CSifNotifierBase::RunError : Error code %d"), aError);
    (void)aError;
    return KErrNone;
    }

EXPORT_C TUint CSifNotifierBase::Key()
    {
    return iKey;
    }

void CSifNotifierBase::DoCancel()
    {
    iProperty.Cancel();
    }


EXPORT_C CSifNotifierBase::~CSifNotifierBase()
    {
    Cancel();
    }



/////////////////////
// CSifOperationKey
/////////////////////

CSifOperationKey::CSifOperationKey()
    {
    // empty
    }

CSifOperationKey::~CSifOperationKey()
    {
    iStartEndKeyArray.Close();
    }

EXPORT_C CSifOperationKey* CSifOperationKey::NewL()
    {
    CSifOperationKey *self = CSifOperationKey::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CSifOperationKey* CSifOperationKey::NewLC()
    {
    CSifOperationKey *self = new(ELeave) CSifOperationKey();
    CleanupStack::PushL(self);
    return self;
    }

CSifOperationKey* CSifOperationKey::NewL(RReadStream& aStream)
    {
    CSifOperationKey *self = new(ELeave) CSifOperationKey();
    CleanupStack::PushL(self);
    self->InternalizeL(aStream);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C void CSifOperationKey::ExternalizeL(RWriteStream& aStream) const
    {
    TInt count = iStartEndKeyArray.Count();
    aStream.WriteUint32L(count);
    for(TInt i=0 ; i< count; ++i)
        {
        aStream.WriteUint32L(iStartEndKeyArray[i]);
        }
    }
    
void CSifOperationKey::InternalizeL(RReadStream& aStream)
    {
    TInt count = aStream.ReadUint32L();
    for(TInt i=0 ; i< count; ++i)
        {
        iStartEndKeyArray.AppendL(aStream.ReadUint32L());
        }
    }

const RArray<TUint>& CSifOperationKey::StartEndKeys() const
    {
    return iStartEndKeyArray;
    }

void CSifOperationKey::AddKeyL(TUint aKey)
    {
    iStartEndKeyArray.AppendL(aKey);
    }

void CSifOperationKey::DeleteKey(TUint aPos)
    {
    iStartEndKeyArray.Remove(aPos);
    }


/////////////////////
// CDeleteKeyTimer
/////////////////////


CDeleteKeyTimer::CDeleteKeyTimer() : CTimer(EPriorityLow)
    {
    CActiveScheduler::Add(this);
    }

CDeleteKeyTimer::~CDeleteKeyTimer()
    {
    Cancel();
    }

CDeleteKeyTimer* CDeleteKeyTimer::NewL(const TUint aKey)
    {
    CDeleteKeyTimer* self = new(ELeave) CDeleteKeyTimer();
    CleanupStack::PushL(self);
    self->ConstructL(aKey);
    CleanupStack::Pop(self);
    return self;
    }

void CDeleteKeyTimer::ConstructL(const TUint aKey)
    {
    CTimer::ConstructL();
    iKey = aKey;
    }

void CDeleteKeyTimer::Start()
    {
    After(KDeleteKeyDelay);
    }

void CDeleteKeyTimer::RunL()
    {
    // Publish(remove this key from existing info) the remaining keys in the global SIF key.
    TBuf8<KMaxNumberOfOperations * sizeof(TInt)> buf;
	RProperty oprnProperty;
	CleanupClosePushL(oprnProperty);
	
	oprnProperty.Attach(KUidSystemCategory, KSifOperationKey);
    TInt err = oprnProperty.Get(buf);
    User::LeaveIfError(err);
    
    RDesReadStream readStream(buf);
    CleanupClosePushL(readStream);
    CSifOperationKey* opKey = CSifOperationKey::NewL(readStream);
    CleanupStack::PushL(opKey);

    //Find the position of the key 
    TInt pos = opKey->StartEndKeys().Find(iKey);
    
    // Delete key, externalize the updated CSifOperationKey object and publish it.
    opKey->DeleteKey(pos);
    
    RBuf8 operationKeyData;
    operationKeyData.CleanupClosePushL();
    ExternalizeRefObjectL(*opKey, operationKeyData);
    
    User::LeaveIfError(oprnProperty.Set(operationKeyData));
    
    // StartEnd Key.
    RProperty::Delete(KUidSystemCategory, iKey);
    // Progress Key.
    RProperty::Delete(KUidSystemCategory, (iKey+1));
    
    CleanupStack::PopAndDestroy(4, &oprnProperty);
    }


TInt CDeleteKeyTimer::RunError(TInt aError)
    {
    DEBUG_PRINTF2(_L8("CDeleteKeyTimer::RunError : Error code %d"), aError);
    (void)aError;
    
    // StartEnd Key.
    RProperty::Delete(KUidSystemCategory, iKey);
    // Progress Key.
    RProperty::Delete(KUidSystemCategory, (iKey+1));
    return KErrNone;
    }
