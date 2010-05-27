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
* Description: Native SISX SIF (Software Install Framework) plugin.
*
*/

#include "sisxsifplugin.h"              // CSisxSifPlugin
#include "sisxsifpluginactiveimpl.h"    // CSisxSifPluginActiveImpl

using namespace Usif;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSisxSifPlugin::NewL()
// ---------------------------------------------------------------------------
//
CSisxSifPlugin* CSisxSifPlugin::NewL()
	{
	CSisxSifPlugin *self = new( ELeave ) CSisxSifPlugin;
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
// CSisxSifPlugin::~CSisxSifPlugin()
// ---------------------------------------------------------------------------
//
CSisxSifPlugin::~CSisxSifPlugin()
	{
	delete iImpl;
	}

// ---------------------------------------------------------------------------
// CSisxSifPlugin::GetComponentInfo()
// ---------------------------------------------------------------------------
//
void CSisxSifPlugin::GetComponentInfo( const TDesC& aFileName,
        const TSecurityContext& aSecurityContext,
        CComponentInfo& aComponentInfo,
        TRequestStatus& aStatus )
	{
	iImpl->GetComponentInfo( aFileName, aSecurityContext, aComponentInfo, aStatus );
	}

// ---------------------------------------------------------------------------
// CSisxSifPlugin::GetComponentInfo()
// ---------------------------------------------------------------------------
//
void CSisxSifPlugin::GetComponentInfo( RFile& aFileHandle,
        const TSecurityContext& aSecurityContext,
        CComponentInfo& aComponentInfo,
        TRequestStatus& aStatus )
	{
	iImpl->GetComponentInfo( aFileHandle, aSecurityContext, aComponentInfo, aStatus );
	}

// ---------------------------------------------------------------------------
// CSisxSifPlugin::Install()
// ---------------------------------------------------------------------------
//
void CSisxSifPlugin::Install( const TDesC& aFileName,
        const TSecurityContext& aSecurityContext,
        const COpaqueNamedParams& aInputParams,
        COpaqueNamedParams& aOutputParams,
        TRequestStatus& aStatus )
	{
	iImpl->Install( aFileName, aSecurityContext, aInputParams, aOutputParams, aStatus );
	}

// ---------------------------------------------------------------------------
// CSisxSifPlugin::Install()
// ---------------------------------------------------------------------------
//
void CSisxSifPlugin::Install( RFile& aFileHandle,
        const TSecurityContext& aSecurityContext,
        const COpaqueNamedParams& aInputParams,
        COpaqueNamedParams& aOutputParams,
        TRequestStatus& aStatus )
	{
	iImpl->Install( aFileHandle, aSecurityContext, aInputParams, aOutputParams, aStatus );
	}

// ---------------------------------------------------------------------------
// CSisxSifPlugin::Uninstall()
// ---------------------------------------------------------------------------
//
void CSisxSifPlugin::Uninstall( TComponentId aComponentId,
        const TSecurityContext& aSecurityContext,
        const COpaqueNamedParams& aInputParams,
        COpaqueNamedParams& aOutputParams,
        TRequestStatus& aStatus )
	{
	iImpl->Uninstall( aComponentId, aSecurityContext, aInputParams, aOutputParams, aStatus );
	}

// ---------------------------------------------------------------------------
// CSisxSifPlugin::Activate()
// ---------------------------------------------------------------------------
//
void CSisxSifPlugin::Activate( TComponentId aComponentId,
        const TSecurityContext& aSecurityContext,
        TRequestStatus& aStatus )
	{
	iImpl->Activate( aComponentId, aSecurityContext, aStatus );
	}

// ---------------------------------------------------------------------------
// CSisxSifPlugin::Deactivate()
// ---------------------------------------------------------------------------
//
void CSisxSifPlugin::Deactivate( TComponentId aComponentId,
        const TSecurityContext& aSecurityContext,
        TRequestStatus& aStatus )
	{
	iImpl->Deactivate( aComponentId, aSecurityContext, aStatus );
	}

// ---------------------------------------------------------------------------
// CSisxSifPlugin::CancelOperation()
// ---------------------------------------------------------------------------
//
void CSisxSifPlugin::CancelOperation()
    {
    iImpl->Cancel();
    }

// ---------------------------------------------------------------------------
// CSisxSifPlugin::CSisxSifPlugin()
// ---------------------------------------------------------------------------
//
CSisxSifPlugin::CSisxSifPlugin()
    {
    }

// ---------------------------------------------------------------------------
// CSisxSifPlugin::ConstructL()
// ---------------------------------------------------------------------------
//
void CSisxSifPlugin::ConstructL()
    {
    iImpl = CSisxSifPluginActiveImpl::NewL();
    }

