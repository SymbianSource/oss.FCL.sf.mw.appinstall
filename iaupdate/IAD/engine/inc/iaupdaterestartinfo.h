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



#ifndef IA_UPDATE_RESTART_INFO_H
#define IA_UPDATE_RESTART_INFO_H


#include "e32base.h"

class CIAUpdaterResultsFile;
class CIAUpdatePendingNodesFile;

/**
 *
 *
 * @since S60 v3.2
 */
class CIAUpdateRestartInfo : public CBase
    {

public:

    IMPORT_C static CIAUpdateRestartInfo* NewL();
    IMPORT_C static CIAUpdateRestartInfo* NewLC();

    IMPORT_C virtual ~CIAUpdateRestartInfo();

    /**
     * Deletes files.
     * 
     * @note That even if the files are deleted, the class objects are still
     * available until this class object is deleted.
     *
     * @since S60 v3.2
     */
    IMPORT_C void DeleteFiles();

    IMPORT_C CIAUpdatePendingNodesFile& PendingNodesFile();
    
    IMPORT_C CIAUpdaterResultsFile& ResultsFile();


private:

    // Prevent these if not implemented
    CIAUpdateRestartInfo( const CIAUpdateRestartInfo& aObject );
    CIAUpdateRestartInfo& operator =( const CIAUpdateRestartInfo& aObject );

    CIAUpdateRestartInfo();
    void ConstructL();
    

private: // data

    // Contains the UIDs of the nodes that have been
    // set as pending.
    CIAUpdatePendingNodesFile* iPendingNodesFile;
    
    // Gives the results of the updater actions.
    CIAUpdaterResultsFile* iResultsFile;
    
    };
		
#endif // IA_UPDATE_RESTART_INFO_H
