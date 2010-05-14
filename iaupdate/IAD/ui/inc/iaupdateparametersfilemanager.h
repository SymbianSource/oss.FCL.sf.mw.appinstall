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
* Description:   CIAUpdateParametersFileManager
*
*/




#ifndef IA_UPDATE_PARAMETERS_FILE_MANAGER_H
#define IA_UPDATE_PARAMETERS_FILE_MANAGER_H


#include <e32base.h>
#include <s32strm.h>
#include <f32file.h>

class CIAUpdateParameters;


/**
 *
 *
 * @since S60 v3.2
 */
class CIAUpdateParametersFileManager : public CBase
    {

public:

    /**
     * @note The default path to the file where ReadDataL gets its data
     * is set to the private directory file of the UI and the default
     * name of the file is used.
     *
     * @since S60 v3.2
     */
    static CIAUpdateParametersFileManager* NewL();

    IMPORT_C static CIAUpdateParametersFileManager* NewLC();
    
    
    IMPORT_C virtual ~CIAUpdateParametersFileManager();
    

    /**
     * @return CIAUpdateParameters* Parameter object that was created from
     * file data. If file did not exist, then NULL is returned.
     * @exception Leaves with system wide error code.
     *
     * @since S60 v3.2
     */
    CIAUpdateParameters* ReadL();

    void WriteL( const CIAUpdateParameters& aParams );


    TInt RemoveFile();    


private:

    // Prevent these if not implemented
    CIAUpdateParametersFileManager( const CIAUpdateParametersFileManager& aObject );
    CIAUpdateParametersFileManager& operator =( const CIAUpdateParametersFileManager& aObject );


    CIAUpdateParametersFileManager();
    
    void ConstructL();


    CIAUpdateParameters* InternalizeL( RReadStream& aStream );

    void ExternalizeL( RWriteStream& aStream, const CIAUpdateParameters& aParams );


private: // data

	// file server session
	RFs iFsSession;
	
	// controller data file path
	TFileName iPath;

    };
		
#endif // IA_UPDATE_PARAMETERS_FILE_MANAGER_H

