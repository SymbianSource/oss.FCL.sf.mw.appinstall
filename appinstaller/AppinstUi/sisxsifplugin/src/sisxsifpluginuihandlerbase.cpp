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

#include "sisxsifpluginuihandlerbase.h"     // CSisxSifPluginUiHandlerBase
#include "sisxsifplugininstallparams.h"     // CSisxSifPluginInstallParams
#include "sisxsifcleanuputils.h"            // CleanupResetAndDestroyPushL

using namespace Usif;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::CSisxSifPluginUiHandlerBase()
// ---------------------------------------------------------------------------
//
CSisxSifPluginUiHandlerBase::CSisxSifPluginUiHandlerBase( RFs& aFs ) : iFs( aFs )
    {
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::~CSisxSifPluginUiHandlerBase()
// ---------------------------------------------------------------------------
//
CSisxSifPluginUiHandlerBase::~CSisxSifPluginUiHandlerBase()
    {
    delete iInstallParams;
    delete iGlobalComponentId;
    delete iPublishSifOperationInfo;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::SetInstallParamsL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerBase::SetInstallParamsL(
        const CSisxSifPluginInstallParams& aInstallParams )
    {
    if( iInstallParams )
        {
        delete iInstallParams;
        iInstallParams = NULL;
        }
    iInstallParams = CSisxSifPluginInstallParams::NewL( aInstallParams );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::SetMaxInstalledSize()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerBase::SetMaxInstalledSize( TInt aSize )
    {
    iMaxInstalledSize = aSize;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::SetDriveSelectionRequired()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerBase::SetDriveSelectionRequired( TBool aIsRequired )
    {
    iIsDriveSelectionRequired = aIsRequired;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::PublishStartL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerBase::PublishStartL( const CComponentInfo::CNode& aRootNode )
    {
    RPointerArray<HBufC> appNames;
    CleanupResetAndDestroyPushL( appNames );
    RPointerArray<HBufC> appIcons;
    CleanupResetAndDestroyPushL( appIcons );

    const RPointerArray<CComponentInfo::CApplicationInfo>& apps = aRootNode.Applications();
    for( TInt index = 0; index < apps.Count(); ++index )
        {
        HBufC* name = apps[ index ]->Name().AllocLC();
        appNames.AppendL( name );
        CleanupStack::Pop( name );
        HBufC* icon = apps[ index ]->IconFileName().AllocLC();
        appIcons.AppendL( icon );
        CleanupStack::Pop( icon );
        }

    if( iGlobalComponentId )
        {
        delete iGlobalComponentId;
        iGlobalComponentId = NULL;
        }
    iGlobalComponentId = aRootNode.GlobalComponentId().AllocL();

    CSifOperationStartData* data = CSifOperationStartData::NewLC( *iGlobalComponentId,
            aRootNode.ComponentName(), appNames, appIcons, aRootNode.MaxInstalledSize(),
            KNullDesC, KNullDesC, aRootNode.SoftwareTypeName() );

    if( !iPublishSifOperationInfo )
        {
        iPublishSifOperationInfo = CPublishSifOperationInfo::NewL();
        }
    iPublishSifOperationInfo->PublishStartL( *data );

    CleanupStack::PopAndDestroy( 3, &appNames );    // data, appIcons, appNames
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::PublishProgressL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerBase::PublishProgressL( TSifOperationPhase aPhase,
        TSifOperationSubPhase aSubPhase, TInt aCurrentProgress, TInt aTotal )
    {
    User::LeaveIfNull( iPublishSifOperationInfo );
    CSifOperationProgressData* data = CSifOperationProgressData::NewLC( *iGlobalComponentId,
            aPhase, aSubPhase, aCurrentProgress, aTotal );
    iPublishSifOperationInfo->PublishProgressL( *data );
    CleanupStack::PopAndDestroy( data );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::PublishCompletionL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerBase::PublishCompletionL( TErrorCategory aErrorCategory,
        TInt aErrorCode, const TDesC& aErrorMessage, const TDesC& aErrorDetails )
    {
    User::LeaveIfNull( iPublishSifOperationInfo );
    CSifOperationEndData* data = CSifOperationEndData::NewLC( *iGlobalComponentId,
            aErrorCategory, aErrorCode, aErrorMessage, aErrorDetails );
    iPublishSifOperationInfo->PublishCompletionL( *data );
    CleanupStack::PopAndDestroy( data );
    }

