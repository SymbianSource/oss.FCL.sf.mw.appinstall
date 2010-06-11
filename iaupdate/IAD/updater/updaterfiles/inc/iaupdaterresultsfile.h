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
* Description:   CIAUpdaterResultsFile
*
*/




#ifndef IA_UPDATER_RESULTS_FILE_H
#define IA_UPDATER_RESULTS_FILE_H


#include <e32base.h>
#include <s32strm.h>
#include <e32cmn.h>
#include <f32file.h>

class CIAUpdaterResult;


/**
 *
 *
 * @since S60 v3.2
 */
class CIAUpdaterResultsFile : public CBase
    {

public:

    /**
     * @note ReadDataL should be called to get the values from the file.
     * Otherwise, default values are used.
     *
     * @note The default path to the file where ReadDataL gets its data
     * is set to the private directory file of the UI and the default
     * name of the file is used. If the file path should be changed, 
     * then use SetFilePathL for this.
     *
     * @since S60 v3.2
     */
    IMPORT_C static CIAUpdaterResultsFile* NewL();

    IMPORT_C static CIAUpdaterResultsFile* NewLC();
    
    
    IMPORT_C virtual ~CIAUpdaterResultsFile();


    /**
     * Convenience method to get the count instead of
     * checking the array and its content values by hand.
     *
     * @since S60 v3.2
     */
    IMPORT_C TInt SuccessCount() const;

    /**
     * Convenience method to get the count instead of
     * checking the array and its content values by hand.
     *
     * @since S60 v3.2
     */
    IMPORT_C TInt FailCount() const;

    /**
     * Convenience method to get the count instead of
     * checking the array and its content values by hand.
     *
     * @since S60 v3.2
     */
    IMPORT_C TInt CancelCount() const;


    /**
     *
     * @note FilePath is not set to a default value when Reset is called.
     * The current value will remain after this function call. SetFilePathL function
     * can be used to set values for the file path.
     *
     */
    IMPORT_C void Reset();
    

    IMPORT_C TInt PckgStartIndex() const;

    IMPORT_C void SetPckgStartIndex( const TInt& aIndex );

    /**
     * @note This is not the count of Results-list but this is the
     * count of all the items that should be installed and shown
     * in the UI dialog.
     */
    IMPORT_C TInt TotalPckgCount() const;

    IMPORT_C void SetTotalPckgCount( const TInt& aCount );



    IMPORT_C RPointerArray< CIAUpdaterResult >& Results();
    

    /**
     * @return const TDesC& File path that is used for ReadDataL, WriteDataL
     * and RemoveFile.
     *
     * @since S60 v3.2
     */
    IMPORT_C const TDesC& FilePath() const;

    /**
     * @param aPath The full path of the data file that is used when
     * ReadDataL, WriteDataL and RemoveFile are used.
     *
     * @since S60 v3.2
     */
    IMPORT_C void SetFilePathL( const TDesC& aPath );


    /**
     * @exception Leaves with KErrNotFound if the file was not found.
     * Otherwise system wide error code.
     *
     * @since S60 v3.2
     */
    IMPORT_C TBool ReadDataL();

    IMPORT_C void WriteDataL();


    IMPORT_C TInt RemoveFile();    


private:

    // Prevent these if not implemented
    CIAUpdaterResultsFile( const CIAUpdaterResultsFile& aObject );
    CIAUpdaterResultsFile& operator =( const CIAUpdaterResultsFile& aObject );


    CIAUpdaterResultsFile();
    
    void ConstructL();


    void InternalizeL( RReadStream& aStream );

    void ExternalizeL( RWriteStream& aStream );


private: // data

	// file server session
	RFs iFsSession;
	
	// controller data file path
	TFileName iPath;


    // This informs how many packages have already been handled.
    // This way the UI can show correct information in its dialogs.
    TInt iPckgStartIndex;

    // This counter informs how many packages will be totally installed.
    // Used in the dialog. 
    TInt iTotalPckgCount;
    

    RPointerArray< CIAUpdaterResult > iResults;
    
    };
		
#endif // IA_UPDATER_RESULTS_FILE_H

