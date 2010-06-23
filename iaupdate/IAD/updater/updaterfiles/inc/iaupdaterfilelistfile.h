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
* Description:   CIAUpdaterFileListFile
*
*/




#ifndef IA_UPDATER_FILE_LIST_FILE_H
#define IA_UPDATER_FILE_LIST_FILE_H


#include <e32base.h>
#include <s32strm.h>
#include <e32cmn.h>
#include <f32file.h>

class CIAUpdaterFileList;


/**
 *
 *
 * @since S60 v3.2
 */
class CIAUpdaterFileListFile : public CBase
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
    IMPORT_C static CIAUpdaterFileListFile* NewL();

    IMPORT_C static CIAUpdaterFileListFile* NewLC();
    
    /**
     * @note Because FileList is handled outside of this class, the list
     * is only resetted when this object is deleted. The objects that use this object,
     * have the ownership and the responsibility to delete list elements separately.
     *
     * @since S60 v3.2
     */
    IMPORT_C virtual ~CIAUpdaterFileListFile();


    /**
     *
     * @note FilePath is not set to a default value when Reset is called.
     * The current value will remain after this function call. SetFilePathL 
     * function can be used to set values for the file path. This does not 
     * remove the file list file. If the file should also be removed, 
     * then call RemoveFile function.
     *
     * @since S60 v3.2
     */
    IMPORT_C void Reset();


    IMPORT_C TBool Silent() const;
    
    IMPORT_C void SetSilent( TBool aSilent );


    IMPORT_C TInt PckgStartIndex() const;

    IMPORT_C void SetPckgStartIndex( const TInt aIndex );


    IMPORT_C TInt TotalPckgCount() const;

    IMPORT_C void SetTotalPckgCount( const TInt aCount );


    /**
     * @note Because FileList is handled outside of this class.
     * When object are inserted to the list, the ownership is
     * thought to be transferred here. So, when the list is resetted
     * by this class, also its items are deleted.
     *
     * @since S60 v3.2
     */
    IMPORT_C RPointerArray< CIAUpdaterFileList >& FileList();


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
    CIAUpdaterFileListFile( const CIAUpdaterFileListFile& aObject );
    CIAUpdaterFileListFile& operator =( const CIAUpdaterFileListFile& aObject );

    CIAUpdaterFileListFile();
    
    void ConstructL();

    void InternalizeL( RReadStream& aStream );

    void ExternalizeL( RWriteStream& aStream );


private: // data

	// file server session
	RFs iFsSession;
	
	// controller data file path
	TFileName iPath;


    // This informs if operations should be silent and no
    // dialogs should be shown.
    TBool iSilent;

    // This informs how many packages have already been handled
    // before updater is started. This way the updater can show correct
    // information in its dialogs.
    TInt iPckgStartIndex;

    // This counter informs how many packages will be totally installed.
    // Used in the dialog. 
    TInt iTotalPckgCount;
    

    RPointerArray< CIAUpdaterFileList > iFileList;
    };
		
#endif // IA_UPDATER_FILE_LIST_FILE_H

