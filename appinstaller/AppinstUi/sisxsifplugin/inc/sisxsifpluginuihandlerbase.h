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


namespace Usif
{
    class CSisxSifPluginInstallParams;


    /**
     * Base class for SISX SIF plugin UI handlers.
     */
    class CSisxSifPluginUiHandlerBase : public CBase, public Swi::MUiHandler
        {
    public:     // constructors and destructor
        CSisxSifPluginUiHandlerBase( RFs& aFs );
        ~CSisxSifPluginUiHandlerBase();

    public:     // new functions
        virtual void DisplayPreparingInstallL( const TDesC& aFileName ) = 0;
        virtual void DisplayCompleteL() = 0;
        virtual void DisplayFailedL( TInt aErrorCode ) = 0;

    public:     // new functions
        void SetInstallParamsL( const CSisxSifPluginInstallParams& aInstallParams );
        void SetMaxInstalledSize( TInt aSize );
        void SetDriveSelectionRequired( TBool aIsRequired );

    protected:	// data
        RFs& iFs;
        TInt iMaxInstalledSize;
        TBool iIsDriveSelectionRequired;
        CSisxSifPluginInstallParams* iInstallParams;
        };

}   // namespace Usif

#endif      // C_SISXSIFPLUGUIHANDLERBASE_H

