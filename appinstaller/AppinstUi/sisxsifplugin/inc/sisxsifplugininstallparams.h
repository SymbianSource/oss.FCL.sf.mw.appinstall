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
* Description:  Installation parameters class.
*
*/

#ifndef C_SISXSIFPLUGININSTALLPARAMS_H
#define C_SISXSIFPLUGININSTALLPARAMS_H

#include <e32base.h>                    // CBase
#include <usif/sif/sifcommon.h>         // COpaqueNamedParams, TSifPolicy

namespace Usif
{
    /**
     *  Installation parameters class.
     */
    class CSisxSifPluginInstallParams : public CBase
        {
    public:     // constructors and destructor
        static CSisxSifPluginInstallParams* NewL( const COpaqueNamedParams& aParams );
        static CSisxSifPluginInstallParams* NewL( const CSisxSifPluginInstallParams& aParams );
        ~CSisxSifPluginInstallParams();

    public:     // new functions
        TBool IsSilentMode() const;
        TBool IsInstallInactive() const;
        TUint Drive() const;
        TLanguage Language() const;
        const TDesC& OCSPUrl() const;
        TSifPolicy PerformOCSP() const;
        TSifPolicy IgnoreOCSPWarnings() const;
        TSifPolicy AllowUpgrade() const;
        TSifPolicy InstallOptionalItems() const;
        TSifPolicy AllowUntrusted() const;
        TSifPolicy GrantCapabilities() const;
        TSifPolicy AllowAppShutdown() const;
        TSifPolicy AllowAppBreakDependency() const;
        TSifPolicy AllowOverwrite() const;
        TSifPolicy PackageInfo() const;

    private:    // new functions
        CSisxSifPluginInstallParams();
        void ConstructL( const COpaqueNamedParams& aParams );
        void ConstructL( const CSisxSifPluginInstallParams& aParams );
        TInt GetIntParamL( const COpaqueNamedParams& aParams,
                const TDesC& aParamName, TInt aDefaultValue );
        void GetPolicyParamL( const COpaqueNamedParams& aParams, const TDesC& aParamName,
                TSifPolicy& aPolicy, const TSifPolicy& aDefault );
        void GetStringParamL( const COpaqueNamedParams& aParams,
                const TDesC& aParamName, HBufC*& aBuf );

    private:    // data
        TBool iUseSilentMode;
        TBool iIsInstallInactive;
        TUint iDrive;
        TLanguage iLanguage;
        HBufC* iOCSPUrl;
        TSifPolicy iPerformOCSP;
        TSifPolicy iIgnoreOCSPWarnings;
        TSifPolicy iAllowUpgrade;
        TSifPolicy iInstallOptionalItems;
        TSifPolicy iAllowUntrusted;
        TSifPolicy iGrantCapabilities;
        TSifPolicy iAllowAppShutdown;
        TSifPolicy iAllowAppBreakDependency;
        TSifPolicy iAllowOverwrite;
        TSifPolicy iPackageInfo;
        };

}   // namespace Usif

#endif  // C_SISXSIFPLUGININSTALLPARAMS_H

