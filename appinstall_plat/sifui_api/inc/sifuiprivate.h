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
#include <hb/hbcore/hbdevicedialogsymbian.h>    // MHbDeviceDialogObserver
#include <hb/hbcore/hbindicatorsymbian.h>       // MHbIndicatorSymbianObserver
#include <bamdesca.h>                           // MDesCArray
#include <sifui.h>                              // CSifUi::TOptionalButtons
#include <sifuidefs.h>                          // TSifUiDeviceDialogType

class CHbSymbianVariantMap;
class CHbSymbianVariant;
class CActiveSchedulerWait;

namespace Swi {
    class CAppInfo;
    class CCertificateInfo;
}

/**
 * Private part of the SIF UI API.
 * Only CSifUi class should use CSifUiPrivate class.
 */
class CSifUiPrivate : public CActive, public MHbDeviceDialogObserver,
        public MHbIndicatorSymbianObserver
    {
    public:     // constructor and destructor
        static CSifUiPrivate* NewL();
        ~CSifUiPrivate();

    public:     // new functions
        void ShowPreparingL();
        void SetMemorySelectionL( const RArray<TInt>& aDriveNumbers );
        void SetCertificateInfoL( const RPointerArray<CSifUiCertificateInfo>& aCertificates );
        TBool ShowConfirmationL( const CSifUiAppInfo& aAppInfo );
        TInt SelectedDrive( TInt& aDriveNumber );
        void ShowProgressL( const CSifUiAppInfo& aAppInfo, TInt aProgressBarFinalValue,
                CSifUi::TInstallingPhase aPhase );
        void IncreaseProgressBarValueL( TInt aIncrement );
        TBool IsCancelled();
        void ShowCompleteL();
        void ShowFailedL( const CSifUiErrorInfo& aErrorInfo );
        void SetButtonVisible( CSifUi::TOptionalButton aButton, TBool aIsVisible );
        TBool ShowGrantCapabilitiesL( const TCapabilitySet& aCapabilities );
        TInt ShowSelectLanguageL( const RArray<TLanguage>& aLanguages );
        TBool ShowSelectOptionsL( const MDesCArray& aSelectableItems,
                RArray<TInt>& aSelectedIndexes );
        void CancelDialogs();

    protected:  // from CActive
        void DoCancel();
        void RunL();

    private:    // from MHbDeviceDialogObserver
        void DataReceived( CHbSymbianVariantMap& aData );
        void DeviceDialogClosed( TInt aCompletionCode );

    private:    // from MHbIndicatorSymbianObserver
        void IndicatorUserActivated( const TDesC& aType, CHbSymbianVariantMap& aData );

    private:    // new functions
        CSifUiPrivate();
        void ConstructL();
        void ClearParams();
        CHbSymbianVariantMap* VariantMapL();
        void ChangeNoteTypeL( TInt aType );
        void AddParamL( const TDesC& aKey, TInt aValue );
        void AddParamL( const TDesC& aKey, const TDesC& aValue );
        void AddParamListL( const TDesC& aKey, const MDesCArray& aList );
        void AddParamBinaryL( const TDesC& aKey, const CBufBase& aBinary );
        void AddParamPckgL( const TDesC& aKey, const TDesC8& aPckg );
        void AddParamsAppInfoL( const CSifUiAppInfo& aAppInfo );
        void AddParamsCertificatesL();
        void AddParamsHiddenButtonsL();
        void ResendAllInstallationDetailsL();
        void ActivateInstallIndicatorL();
        void UpdateInstallIndicatorProgressL();
        void CloseInstallIndicator();
        TBool IsIndicatorActive();
        void ShowInstallIndicatorCompleteL( TInt aErrorCode );
        void UpdateDialogAndWaitForResponseL();
        void UpdateDialogOrIndicatorWithoutWaitingL();
        void CompleteDialogOrIndicatorAndWaitForResponseL( TInt aErrorCode );
        void DisplayDeviceDialogL();
        void WaitForResponseL();
        void WaitedResponseReceived( TInt aCompletionCode );

    private:    // data
        CHbDeviceDialogSymbian* iDeviceDialog;
        CHbIndicatorSymbian* iIndicator;
        CHbSymbianVariantMap* iVariantMap;
        CActiveSchedulerWait* iWait;
        TBool iIsDisplayingDialog;
        TBool iIsFirstTimeToDisplay;
        TInt iWaitCompletionCode;
        TInt iDialogReturnValue;
        TSifUiDeviceDialogType iDialogType;
        CSifUiAppInfo* iAppInfo;
        CBufBase* iCertificateBuffer;
        HBufC* iSelectableDrives;
        TBool iSelectedDriveSet;
        TChar iSelectedDrive;
        TInt iProgressBarFinalValue;
        TInt iProgressBarCurrentValue;
        CSifUi::TInstallingPhase iInstallingPhase;
        TBool iNoHideProgressButton;
        TBool iNoCancelProgressButton;
        TBool iNoShowInAppLibButton;
        TBool iNoErrorDetailsButton;
        TInt iSelectedLanguage;
        RArray<TInt> iSelectedOptions;
    };


#endif  // C_SIFUIPRIVATE_H

