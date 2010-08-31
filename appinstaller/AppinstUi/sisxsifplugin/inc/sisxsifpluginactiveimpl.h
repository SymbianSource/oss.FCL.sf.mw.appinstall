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
#include <usif/usiferror.h>             // TErrorCategory
#include <f32file.h>                    // RFs, RFile
#include <barsc.h>                      // RResourceFile

namespace Swi
{
    class CAsyncLauncher;
    class CInstallPrefs;
}

namespace Usif
{
    class CComponentEntry;
    class CSisxSifPluginUiHandlerBase;
    class CSisxSifPluginInstallParams;
    class CSisxSifPluginErrorHandler;
    
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
        void CommonRequestPreambleL( TRequestStatus& aStatus );
        void CommonRequestPreambleL( const COpaqueNamedParams& aInputParams,
                COpaqueNamedParams& aOutputParams, TRequestStatus& aStatus );
        void CompleteSelf( TInt aResult );
        void CreateUiHandlerL();
        TBool IsSilentMode();
        void CompleteClientRequest( TInt aResult );
        void DoGetComponentInfoL( CComponentInfo& aComponentInfo, TRequestStatus& aStatus );
        void DoInstallL( const TSecurityContext& aSecurityContext,
        		const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams,
        		TRequestStatus& aStatus );
        void DoUninstallL( TComponentId aComponentId, const COpaqueNamedParams& aInputParams,
            COpaqueNamedParams& aOutputParams, TRequestStatus& aStatus );
        void DoActivateL( TComponentId aComponentId, TRequestStatus& aStatus );
        void DoDeactivateL( TComponentId aComponentId, TRequestStatus& aStatus );
        void DoHandleErrorL( TInt aError );
        void SetFileL( const TDesC& aFileName );
        void SetFile( RFile& aFileHandle );
        TComponentId GetLastInstalledComponentIdL();
        void GetComponentAndUidL( TComponentId aComponentId, CComponentEntry& aEntry, TUid& aUid ) const;
        TBool RequiresUserCapabilityL( const CComponentInfo::CNode& aRootNode );
        void SetInstallPrefsRevocationServerUriL( const TDesC& aUri );
        void UpdateInstallPrefsForPerformingOcspL();
        void StartInstallingL();
        void StartSilentInstallingL();
        void StartSilentUninstallingL();
        void FinalizeInstallationL();
        void UpdateStartupListL();
        void FillDeviceSupportedLanguagesL();

    private:    // data
        RFs iFs;
        Swi::CAsyncLauncher* iAsyncLauncher;
        CSisxSifPluginUiHandlerBase* iUiHandler;
        Swi::CInstallPrefs* iInstallPrefs;
        TRequestStatus* iClientStatus;
        const COpaqueNamedParams* iInputParams;     // not owned
        COpaqueNamedParams* iOutputParams;      // not owned
        CSisxSifPluginInstallParams* iInstallParams;
        CSisxSifPluginErrorHandler* iErrorHandler;
        CComponentInfo* iComponentInfo;
        TBool iHasAllFilesCapability;
        HBufC* iFileName;
        RFile* iFileHandle;             // not owned
        enum TOperationType
            {
            ENoOperation,
            EGetComponentInfo,
            EInstall,
            EUninstall,
            EActivate,
            EDeactivate
            } iOperation;
        enum TPhase
            {
            ENotActive,
            EPreprocessing,
            ERunningOperation,
            EPostprocessing
            } iPhase;
        RArray<TInt> iDeviceSupportedLanguages;
        };

}   // namespace Usif

#endif      // C_SISXSIFPLUGINIMPL_H

