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

    public:     // new functions
        void SetInstallParamsL( const CSisxSifPluginInstallParams& aInstallParams );
        void SetMaxInstalledSize( TInt aSize );
        void SetDriveSelectionRequired( TBool aIsRequired );
        void PublishStartL( const CComponentInfo::CNode& aRootNode );
        void PublishProgressL( TSifOperationPhase aPhase, TSifOperationSubPhase aSubPhase,
                TInt aCurrentProgress, TInt aTotal );
        void PublishCompletionL( const CSisxSifPluginErrorHandler& aError );

    protected:  // new functions
        void SetDisplayErrorL( Swi::TErrorDialog aType, const TDesC& aParam );

    protected:	// data
        RFs& iFs;
        CSisxSifPluginErrorHandler& iErrorHandler;
        TInt iMaxInstalledSize;
        TBool iIsDriveSelectionRequired;
        CSisxSifPluginInstallParams* iInstallParams;
        HBufC* iGlobalComponentId;
        CPublishSifOperationInfo* iPublishSifOperationInfo;
        };

}   // namespace Usif

#endif      // C_SISXSIFPLUGUIHANDLERBASE_H

