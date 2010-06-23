/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CIAUpdateRestartInfo
*
*/




#include "iaupdaterestartinfo.h"
#include "iaupdatependingnodesfile.h"
#include "iaupdaterresultsfile.h"


EXPORT_C CIAUpdateRestartInfo* CIAUpdateRestartInfo::NewL()
    {
    CIAUpdateRestartInfo* self =
        CIAUpdateRestartInfo::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
    
    
EXPORT_C CIAUpdateRestartInfo* CIAUpdateRestartInfo::NewLC()
    {
    CIAUpdateRestartInfo* self =
        new( ELeave) CIAUpdateRestartInfo();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CIAUpdateRestartInfo::CIAUpdateRestartInfo()
: CBase()
    {
    
    }


void CIAUpdateRestartInfo::ConstructL()
    {
    iPendingNodesFile =
        CIAUpdatePendingNodesFile::NewL();
    iPendingNodesFile->ReadDataL();
    
    iResultsFile =
        CIAUpdaterResultsFile::NewL();
    iResultsFile->ReadDataL();
    }


EXPORT_C CIAUpdateRestartInfo::~CIAUpdateRestartInfo()
    {
    delete iPendingNodesFile;
    delete iResultsFile;
    }


EXPORT_C void CIAUpdateRestartInfo::DeleteFiles()
    {
    iPendingNodesFile->RemoveFile();
    iResultsFile->RemoveFile();
    }


EXPORT_C CIAUpdatePendingNodesFile& CIAUpdateRestartInfo::PendingNodesFile()
    {
    return *iPendingNodesFile;
    }


EXPORT_C CIAUpdaterResultsFile& CIAUpdateRestartInfo::ResultsFile()
    {
    return *iResultsFile;
    }
