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
* Description:  CSisxSifPluginActiveImpl implements active object that
*               handles installation operations requested via
*               CSisxSifPlugin interface.
*
*/

#ifndef C_SISXSIFPLUGINIMPL_H
#define C_SISXSIFPLUGINIMPL_H

#include <e32base.h>                    // CActive
#include <swi/msisuihandlers.h>         // MUiHandler
#include <usif/sif/sifcommon.h>         // TSecurityContext, COpaqueNamedParams
#include <f32file.h>                    // RFs, RFile
#include <barsc.h>                      // RResourceFile

namespace Swi
{
    class CAsyncLauncher;
    class CInstallPrefs;
}

namespace Usif
{
    class CSisxSifPluginUiHandler;
    class CSisxSifPluginUiHandlerSilent;

    /**
     *  SISX SIF plugin active implementation
     *  Universal Software Install Framework (USIF) plugin for native SISX
     *  installation. CSisxSifPluginActiveImpl is active object that takes
     *  care of SISX installation operations.
     */
    class CSisxSifPluginActiveImpl : public CActive
        {
    public:     // constructors and destructor
        static CSisxSifPluginActiveImpl* NewL();
        ~CSisxSifPluginActiveImpl();

    public:     // from CActive
        void DoCancel();
        void RunL();
        TInt RunError( TInt aError );

    public:     // new functions
        void GetComponentInfo( const TDesC& aFileName, const TSecurityContext& aSecurityContext,
                CComponentInfo& aComponentInfo, TRequestStatus& aStatus );
        void GetComponentInfo( RFile& aFileHandle, const TSecurityContext& aSecurityContext,
                CComponentInfo& aComponentInfo, TRequestStatus& aStatus );
        void Install( const TDesC& aFileName, const TSecurityContext& aSecurityContext,
                const COpaqueNamedParams& aArguments, COpaqueNamedParams& aResults,
                TRequestStatus& aStatus );
        void Install( RFile& aFileHandle, const TSecurityContext& aSecurityContext,
                const COpaqueNamedParams& aArguments, COpaqueNamedParams& aResults,
                TRequestStatus& aStatus );
        void Uninstall( TComponentId aComponentId, const TSecurityContext& aSecurityContext,
                const COpaqueNamedParams& aArguments, COpaqueNamedParams& aResults,
                TRequestStatus& aStatus );
        void Activate( TComponentId aComponentId, const TSecurityContext& aSecurityContext,
                TRequestStatus& aStatus );
        void Deactivate( TComponentId aComponentId, const TSecurityContext& aSecurityContext,
                TRequestStatus& aStatus );

    private:    // new functions
        CSisxSifPluginActiveImpl();
        void ConstructL();
        Swi::MUiHandler& UiHandlerL( TBool aUseSilentMode = EFalse );
        void CommonRequestPreamble( const COpaqueNamedParams& aInputParams,
                COpaqueNamedParams& aOutputParams, TRequestStatus& aStatus );
        void CompleteClientRequest( TInt aResult );
        void DoInstallL( const TDesC& aFileName );
        void DoUninstallL( TComponentId aComponentId );
        void DoActivateL( TComponentId aComponentId );
        void DoDeactivateL( TComponentId aComponentId );
        TInt ConvertToSifErrorCode( TInt aSwiErrorCode );
        TComponentId GetLastInstalledComponentIdL();
        TBool RequiresUserCapabilityL();
        void ProcessSilentInstallL();
        void ProcessSilentUninstallL();
        void SetSilentInstallFileL( const TDesC& aFileName );
        void SetSilentInstallFile( RFile& aFileHandle );
        static TInt GrantCapabilitiesHelpCallback( TAny* aPtr );

    private:    // data
        RFs iFs;
        Swi::CAsyncLauncher* iAsyncLauncher;
        CSisxSifPluginUiHandler* iUiHandler;
        CSisxSifPluginUiHandlerSilent* iUiHandlerSilent;
        Swi::CInstallPrefs* iInstallPrefs;
        TRequestStatus* iClientStatus;
        const COpaqueNamedParams* iInputParams;
        COpaqueNamedParams* iOutputParams;
        CComponentInfo* iComponentInfo;
        HBufC* iFileName;
        RFile* iFileHandle;             // not owned
        TBool iUseSilentMode;
        TBool iIsPackageCheckedForSilentInstall;

        enum TOperationType
            {
            ENone,
            EGetComponentInfo,
            EInstall,
            EUninstall,
            EActivate,
            EDeactivate
            };
        TOperationType iOperation;
        };

}   // namespace Usif

#endif      // C_SISXSIFPLUGINIMPL_H

