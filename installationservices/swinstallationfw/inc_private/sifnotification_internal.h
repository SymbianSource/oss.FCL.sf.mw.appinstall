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
* sifnotification_internal.h - Internal classes used by the Sif notifier.
*
*/

/**
 @file
 @released
 @internal
*/

#ifndef __SIFNOTIFICATION_INTERNAL_H__
#define __SIFNOTIFICATION_INTERNAL_H__

#include <e32std.h>
#include <usif/sif/sifnotification.h>

namespace Usif {

const TUint KBufferSize = 100;
const TInt KMaxNumberOfOperations = 11;//One more than the maximum number of operations that that could be tracked
//The Global operation key, this property will contain the total number and list of all the ongoing operations.
const TUint KSifOperationKey = 0x2002ED64; 
//StartEnd and progress key for the first operation
const TUint KSifFirstOperationStartEndKey = 0x2002ED65; 
//StartEnd and progress key for the second operation
const TUint KSifSecondOperationStartEndKey = 0x2002ED67;
//StartEnd and progress key for the third operation
const TUint KSifThirdOperationStartEndKey = 0x2002ED69;
//StartEnd and progress key for the fourth operation
const TUint KSifFourthOperationStartEndKey = 0x2002F7C4;
//StartEnd and progress key for the fifth operation
const TUint KSifFifthOperationStartEndKey = 0x2002F7C6;
//StartEnd and progress key for the sixth operation
const TUint KSifSixthOperationStartEndKey = 0x2002F7C8;
//StartEnd and progress key for the seventh operation
const TUint KSifSeventhOperationStartEndKey = 0x2002F7CA;
//StartEnd and progress key for the eight operation
const TUint KSifEightOperationStartEndKey = 0x2002F7CC;
//StartEnd and progress key for the ninth operation
const TUint KSifNinthOperationStartEndKey = 0x2002F7CE;
//StartEnd and progress key for the tenth operation
const TUint KSifTenthOperationStartEndKey = 0x2002F7D0;

const TUint KSifPropertyTable [KMaxNumberOfOperations-1] = {KSifFirstOperationStartEndKey, KSifSecondOperationStartEndKey, KSifThirdOperationStartEndKey,
                                                            KSifFourthOperationStartEndKey, KSifFifthOperationStartEndKey, KSifSixthOperationStartEndKey,
                                                            KSifSeventhOperationStartEndKey, KSifEightOperationStartEndKey, KSifNinthOperationStartEndKey, KSifTenthOperationStartEndKey};

const TInt KDeleteKeyDelay = 2000000; // 2 seconds

const TInt KStartBufLength = 100;
const TInt KProgressBufLength = 25;

_LIT_SECURITY_POLICY_C1(KSecurityPolicyNone, ECapability_None);
_LIT_SECURITY_POLICY_C1(KSecurityPolicyWDD, ECapabilityWriteDeviceData);

/**
 * This class will be used internally to start timer after publishing the completion data.
 * The property which holds the Start-End and Progress data will be deleted after this timer.
 */
NONSHARABLE_CLASS(CDeleteKeyTimer) : public CTimer
    {
public:
    static CDeleteKeyTimer* NewL(const TUint aKey);
    CDeleteKeyTimer();
    ~CDeleteKeyTimer();
    void Start();
private:
    void ConstructL(const TUint aKey);
    void RunL();
    TInt RunError(TInt aError);
    TUint iKey;//StartEnd key to be deleted
    };


NONSHARABLE_CLASS(CSifOperationKey) : public CBase
    /**
     * This class will be used internally to hold contents of KSIFOperationKey
     */
    {
public:
    IMPORT_C static CSifOperationKey* NewL();
    IMPORT_C static CSifOperationKey* NewLC();    
    static CSifOperationKey* NewL(RReadStream& aStream);
    ~CSifOperationKey(); 
    void AddKeyL(TUint aKey);
    void DeleteKey(TUint aPos);
    const RArray<TUint>& StartEndKeys() const;
    IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
private:
    void InternalizeL(RReadStream& aStream);
    CSifOperationKey();
    void ConstructL();

private:
    RArray<TUint> iStartEndKeyArray; 
    };


/**
 * This class will be used internally to subscribe to changes in individual operations requested by the client.
 */
class CSifNotifierBase : public CActive
    {
public:
	enum TNotifierType
        {
        EStartEndNotifier =1,
        EProgressNotifier
        };

    IMPORT_C static CSifNotifierBase* NewL(MSifOperationsHandler& aHandler, TUint aKey, TNotifierType aType);
    IMPORT_C static CSifNotifierBase* NewLC(MSifOperationsHandler& aHandler, TUint aKey, TNotifierType aType);
    IMPORT_C TUint Key();
    IMPORT_C ~CSifNotifierBase();
    
private:
    CSifNotifierBase(MSifOperationsHandler& aHandler, TUint aKey, TNotifierType aType);
    void WaitForChangeL();
    void ConstructL();
    void RunL();
    TInt RunError(TInt aError);
    void DoCancel();
    
private:
    MSifOperationsHandler& iHandler; 
    TUint iKey;
    RProperty iProperty;
    TInt iBufferSize;
    TNotifierType iType;
    };


} //namespace 

#endif //__SIFNOTIFICATION_INTERNAL_H__
