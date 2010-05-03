/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Private implementation part of the CSifUi API.
*/


#ifndef C_SIFUIPRIVATE_H
#define C_SIFUIPRIVATE_H

#include <e32base.h>                            // CActive
#include <hb/hbcore/hbsymbiandevicedialog.h>    // MHbDeviceDialogObserver
#include <bamdesca.h>                           // MDesCArray

class CHbDeviceDialog;
class CHbSymbianVariantMap;
class CActiveSchedulerWait;
class CApaMaskedBitmap;

namespace Swi {
    class CAppInfo;
    class CCertificateInfo;
}

/**
 * Private part of the SIF UI API.
 * Only CSifUi class should use CSifUiPrivate class.
 */
class CSifUiPrivate : public CActive, public MHbDeviceDialogObserver
    {
    public:     // constructor and destructor
        static CSifUiPrivate* NewL();
        CSifUiPrivate::~CSifUiPrivate();

    public:     // new functions
        TBool ShowConfirmationL( const Swi::CAppInfo& aAppInfo, TInt aAppSize,
        		const CApaMaskedBitmap* aAppIcon );
        void SetMemorySelectionL( const RArray<TInt>& aDriveNumbers );
        TInt SelectedDrive( TInt& aDriveNumber );
        void SetCertificateInfoL( const RPointerArray<Swi::CCertificateInfo>& aCertificates );
        void ShowProgressL( const Swi::CAppInfo& aAppInfo, TInt aAppSize,
                TInt aProgressBarFinalValue );
        void IncreaseProgressBarValueL( TInt aIncrement );
        void ShowCompleteL();
        void ShowFailedL( TInt aErrorCode, const TDesC& aErrorMessage,
                const TDesC& aErrorDetails );

    protected:  // from CActive
        void DoCancel();
        void RunL();

    private:    // from MHbDeviceDialogObserver
        void DataReceived( CHbSymbianVariantMap& aData );
        void DeviceDialogClosed( TInt aCompletionCode );

    private:    // new functions
        CSifUiPrivate();
        void ConstructL();
        void ClearParamsL();
        void ChangeNoteTypeL( TInt aType );
        void AddParamL( const TDesC& aKey, TInt aValue );
        void AddParamL( const TDesC& aKey, const TDesC& aValue );
        void AddParamListL( const TDesC& aKey, const MDesCArray& aList );
        void AddParamsAppInfoAndSizeL( const Swi::CAppInfo& aAppInfo, TInt aAppSize );
        void AddParamsIconL( const CApaMaskedBitmap* aIcon );
        void AddParamsCertificatesL( const RPointerArray<Swi::CCertificateInfo>& aCertificates );
        void DisplayDeviceDialogL();
        TInt WaitForResponse();
        void ResponseReceived( TInt aCompletionCode );

    private:    // data
        CHbDeviceDialog* iDeviceDialog;
        CHbSymbianVariantMap* iVariantMap;
        CActiveSchedulerWait* iWait;
        TBool iIsDisplayingDialog;
        TInt iCompletionCode;
        TInt iReturnValue;
        CApaMaskedBitmap* iBitmap;
        HBufC* iSelectableDrives;
        TBool iSelectedDriveSet;
        TChar iSelectedDrive;
    };


#endif  // C_SIFUIPRIVATE_H

