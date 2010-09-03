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
* Description:  Base class for SISX SIF plugin UI handlers.
*
*/

#ifndef C_SISXSIFPLUGUIHANDLERBASE_H
#define C_SISXSIFPLUGUIHANDLERBASE_H

#include <e32base.h>                    // CBase
#include <swi/msisuihandlers.h>         // MUiHandler
#include <f32file.h>                    // RFs
#include <sifui.h>                      // CSifUi
#include <usif/sif/sifcommon.h>         // CComponentInfo::CNode
#include <usif/sif/sifnotification.h>   // TSifOperationSubPhase, TErrorCategory


namespace Usif
{
    class CComponentEntry;
    class CSisxSifPluginInstallParams;
    class CSisxSifPluginErrorHandler;


    /**
     * Base class for SISX SIF plugin UI handlers.
     */
    class CSisxSifPluginUiHandlerBase : public CBase, public Swi::MUiHandler
        {
    public:     // constructors and destructor
        CSisxSifPluginUiHandlerBase( RFs& aFs, CSisxSifPluginErrorHandler& aErrorHandler );
        ~CSisxSifPluginUiHandlerBase();

    public:     // new functions
        virtual void DisplayPreparingInstallL( const TDesC& aFileName ) = 0;
        virtual void DisplayCompleteL() = 0;
        virtual void DisplayFailedL( const CSisxSifPluginErrorHandler& aError ) = 0;
        virtual void CancelDialogs() = 0;

    public:     // new functions
        void SetInstallParamsL( const CSisxSifPluginInstallParams& aInstallParams );
        void SetMaxInstalledSize( TInt aSize );
        void SetDriveSelectionRequired( TBool aIsRequired );
        TBool IsOcspMandatoryL() const;

        // functions to publish operation progress
        void PublishStartL( const CComponentInfo::CNode& aRootNode, TSifOperationPhase aPhase );
        void PublishStartL( const CComponentEntry& aEntry, TSifOperationPhase aPhase );
        void PublishProgressL( TSifOperationSubPhase aSubPhase );
        void PublishCompletionL();

    protected:  // new functions
        void SetErrorL( TInt aErrorCode, TInt aExtErrorCode );
        void SetErrorL( TInt aErrorCode, TInt aExtErrorCode, const TDesC& aErrMsgDetails );
        void SetErrorSwiErrorL( Swi::TErrorDialog aType, const TDesC& aParam );
        void SetOcspErrorL( Swi::TRevocationDialogMessage aMessage );
        TBool ShowQuestionL( const TDesC& aText ) const;
        void ShowQuestionWithContinueL( const TDesC& aText ) const;
        static TBool CompareDriveLetters( const TChar& aDriveFirst, const TChar& aDriveSecond );

    private:    // new functions
        void SetLocalisedErrorMessageText( const TDesC& aLogicalName );

    protected:	// data
        RFs& iFs;
        CSisxSifPluginErrorHandler& iErrorHandler;
        TInt iMaxInstalledSize;
        TBool iIsDriveSelectionRequired;
        CSisxSifPluginInstallParams* iInstallParams;

        // members for publishing operation progress
        CPublishSifOperationInfo* iPublishSifOperationInfo;
        TSifOperationPhase iOperationPhase;
        HBufC* iGlobalComponentId;
        TInt iProgressBarCurrentValue;
        TInt iProgressBarFinalValue;
        };

}   // namespace Usif

#endif      // C_SISXSIFPLUGUIHANDLERBASE_H

