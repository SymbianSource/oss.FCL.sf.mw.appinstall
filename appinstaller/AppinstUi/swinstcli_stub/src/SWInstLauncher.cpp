/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Unsupported old RSWInstLauncher API
*
*/

#include "SWInstApi.h"
#include "SWInstDefs.h"
#include <e32debug.h>

using namespace SwiUI;

_LIT(KUnsupportedMsg, "RSWInstLauncher is obsolete. Use Usif::RSoftwareInstall instead.");

#define KSWInstInstallServiceUid 0x102071C1


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// RSWInstLauncher::RSWInstLauncher
// -----------------------------------------------------------------------------
//
EXPORT_C RSWInstLauncher::RSWInstLauncher() : iConnected( EFalse )
    {
    RDebug::Print(KUnsupportedMsg);
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::Connect
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::Connect()
    {
    RDebug::Print(KUnsupportedMsg);
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::Close
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::Close()
    {
    RDebug::Print(KUnsupportedMsg);
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::Install
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::Install( const TDesC& /*aFileName*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::Install
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::Install( TRequestStatus& aReqStatus,
                                        const TDesC& /*aFileName*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    TRequestStatus* status = &aReqStatus;
    aReqStatus = KRequestPending;
    User::RequestComplete(status, KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::Install
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::Install( RFile& /*aFile*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::Install
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::Install( TRequestStatus& aReqStatus,
                                        RFile& /*aFile*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    TRequestStatus* status = &aReqStatus;
    aReqStatus = KRequestPending;
    User::RequestComplete(status, KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::Install
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::Install( const TDesC& /*aFileName*/,
                                        const TDesC8& /*aParams*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::Install
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::Install( TRequestStatus& aReqStatus,
                                        const TDesC& /*aFileName*/,
                                        const TDesC8& /*aParams*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    TRequestStatus* status = &aReqStatus;
    aReqStatus = KRequestPending;
    User::RequestComplete(status, KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::Install
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::Install( RFile& /*aFile*/,
                                        const TDesC8& /*aParams*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::Install
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::Install( TRequestStatus& aReqStatus,
                                        RFile& /*aFile*/,
                                        const TDesC8& /*aParams*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    TRequestStatus* status = &aReqStatus;
    aReqStatus = KRequestPending;
    User::RequestComplete(status, KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentInstall
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::SilentInstall( const TDesC& /*aFileName*/,
                                              const TDesC8& /*aOptions*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentInstall
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::SilentInstall( TRequestStatus& aReqStatus,
                                              const TDesC& /*aFileName*/,
                                              const TDesC8& /*aOptions*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    TRequestStatus* status = &aReqStatus;
    aReqStatus = KRequestPending;
    User::RequestComplete(status, KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentInstall
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::SilentInstall( RFile& /*aFile*/,
                                              const TDesC8& /*aOptions*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentInstall
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::SilentInstall( TRequestStatus& aReqStatus,
                                              RFile& /*aFile*/,
                                              const TDesC8& /*aOptions*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    TRequestStatus* status = &aReqStatus;
    aReqStatus = KRequestPending;
    User::RequestComplete(status, KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentInstall
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::SilentInstall( const TDesC& /*aFileName*/,
                                              const TDesC8& /*aParams*/,
                                              const TDesC8& /*aOptions*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentInstall
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::SilentInstall( TRequestStatus& aReqStatus,
                                              const TDesC& /*aFileName*/,
                                              const TDesC8& /*aParams*/,
                                              const TDesC8& /*aOptions*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    TRequestStatus* status = &aReqStatus;
    aReqStatus = KRequestPending;
    User::RequestComplete(status, KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentInstall
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::SilentInstall( RFile& /*aFile*/,
                                              const TDesC8& /*aParams*/,
                                              const TDesC8& /*aOptions*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentInstall
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::SilentInstall( TRequestStatus& aReqStatus,
                                              RFile& /*aFile*/,
                                              const TDesC8& /*aParams*/,
                                              const TDesC8& /*aOptions*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    TRequestStatus* status = &aReqStatus;
    aReqStatus = KRequestPending;
    User::RequestComplete(status, KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::Uninstall
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::Uninstall( const TUid& /*aUid*/,
                                          const TDesC8& /*aMIME*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::Uninstall
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::Uninstall( TRequestStatus& aReqStatus,
                                          const TUid& /*aUid*/,
                                          const TDesC8& /*aMIME*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    TRequestStatus* status = &aReqStatus;
    aReqStatus = KRequestPending;
    User::RequestComplete(status, KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentUninstall
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::SilentUninstall( const TUid& /*aUid*/,
                                                const TDesC8& /*aOptions*/,
                                                const TDesC8& /*aMIME*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentUninstall
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::SilentUninstall( TRequestStatus& aReqStatus,
                                                const TUid& /*aUid*/,
                                                const TDesC8& /*aOptions*/,
                                                const TDesC8& /*aMIME*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    TRequestStatus* status = &aReqStatus;
    aReqStatus = KRequestPending;
    User::RequestComplete(status, KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::CancelAsyncRequest
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::CancelAsyncRequest( TInt /*aReqToCancel*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::CustomUninstall
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::CustomUninstall( TOperation /*aOperation*/,
                                                const TDesC8& /*aParams*/,
                                                const TDesC8& /*aMIME*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::CustomUninstall
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::CustomUninstall( TRequestStatus& aReqStatus,
                                                TOperation /*aOperation*/,
                                                const TDesC8& /*aParams*/,
                                                const TDesC8& /*aMIME*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    TRequestStatus* status = &aReqStatus;
    aReqStatus = KRequestPending;
    User::RequestComplete(status, KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::GetLastErrorL
// -----------------------------------------------------------------------------
//
EXPORT_C CErrDetails* RSWInstLauncher::GetLastErrorL()
    {
    RDebug::Print(KUnsupportedMsg);
    User::Leave( KErrNotSupported );
    // Keep the compiler happy
    return NULL;
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentCustomUninstall
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RSWInstLauncher::SilentCustomUninstall( TOperation /*aOperation*/,
                                                      const TDesC8& /*aOptions*/,
                                                      const TDesC8& /*aParams*/,
                                                      const TDesC8& /*aMIME*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::SilentCustomUninstall
// -----------------------------------------------------------------------------
//
EXPORT_C void RSWInstLauncher::SilentCustomUninstall( TRequestStatus& aReqStatus,
                                                      TOperation /*aOperation*/,
                                                      const TDesC8& /*aOptions*/,
                                                      const TDesC8& /*aParams*/,
                                                      const TDesC8& /*aMIME*/ )
    {
    RDebug::Print(KUnsupportedMsg);
    TRequestStatus* status = &aReqStatus;
    aReqStatus = KRequestPending;
    User::RequestComplete(status, KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::PanicClient
// -----------------------------------------------------------------------------
//
void RSWInstLauncher::PanicClient( TInt aPanic ) const
    {  
    _LIT( KSWInstClientFault, "SWInstCli" );
    User::Panic( KSWInstClientFault, aPanic );
    }

// -----------------------------------------------------------------------------
// RSWInstLauncher::ServiceUid
// -----------------------------------------------------------------------------
//
TUid RSWInstLauncher::ServiceUid() const
    {
    return TUid::Uid( KSWInstInstallServiceUid );    
    }

