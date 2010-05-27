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
* Description:  Header file of the CSisxSifPlugin class. CSisxSifPlugin
*               implements ECom SIF::CSifPlugin interface for native
*               SISX installation.
*
*/

#ifndef C_SISXSIFPLUGIN_H
#define C_SISXSIFPLUGIN_H

#include <usif/sif/sifplugin.h>     // CSifPlugin

namespace Usif
{
    class CSisxSifPluginActiveImpl;


    /**
     *  SISX SIF plugin
     *  Universal Software Install Framework (USIF) plugin for native SISX
     *  installation. CSisxSifPlugin is an ECom plugin that provides native
     *  SISX install/uninstall functionality for USIF.
     */
    class CSisxSifPlugin : public CSifPlugin
        {
    public:     // constructors and destructor
        static CSisxSifPlugin* NewL();
        ~CSisxSifPlugin();

    public:     // from CSifPlugin
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
        void CancelOperation();

    private:    // new functions
        CSisxSifPlugin();
        void ConstructL();

    private:    // data
        CSisxSifPluginActiveImpl* iImpl;
        };
}

#endif      // C_SISXSIFPLUGIN_H

