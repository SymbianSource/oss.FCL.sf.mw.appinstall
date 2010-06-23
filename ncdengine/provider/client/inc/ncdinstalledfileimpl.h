/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains CNcdInstalledFile class
*
*/


#ifndef NCD_INSTALLED_FILE_H
#define NCD_INSTALLED_FILE_H



#include "ncdinstalledcontentimpl.h"
#include "ncdinstalledfile.h"

class CNcdFileInfo;
class CNcdNodeInstallProxy;

/**
 * 
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdInstalledFile: public CNcdInstalledContent,
                         public MNcdInstalledFile
    {
public:

    /**
     * NewL
     *
     * @param aParent Parent
     * @param aInfo File info. Ownership is transferred
     *
     */
    static CNcdInstalledFile* NewL( 
        CNcdNodeInstallProxy& aParent,
        CNcdFileInfo* aInfo,
        TInt aFileIndex );


    /**
     * NewLC
     *
     * @param aParent Parent
     * @param aInfo File info. Ownership is transferred
     *
     */
    static CNcdInstalledFile* NewLC( 
        CNcdNodeInstallProxy& aParent,
        CNcdFileInfo* aInfo, 
        TInt aFileIndex );
    
    
public: // MNcdInstalledContent


    /**
     * @see MNcdInstalledContent::IsInstalledL()
     */
    TBool IsInstalledL() const;

    

public: // MNcdInstalledFile

    /**
     * @see MNcdInstalledFile::MimeType()
     */
    const TDesC& MimeType() const;


    /**
     * @see MNcdInstalledFile::OpenFileL()    
     */
    RFile OpenFileL();

    
    
protected:

    /**
     * Constructor
     *
     */
    CNcdInstalledFile( 
        CNcdNodeInstallProxy& aParent, 
        CNcdFileInfo* aInfo, 
        TInt aFileIndex );
    

    /**
     * ConstructL
     */
    virtual void ConstructL();


    /**
     * Destructor
     *
     * Informs the node manager that this node is deleted.
     */
    virtual ~CNcdInstalledFile();



private:
    // Prevent if not implemented
    CNcdInstalledFile( const CNcdInstalledFile& aObject );
    CNcdInstalledFile& operator =( const CNcdInstalledFile& aObject );
            
private: // data

    CNcdFileInfo* iInfo; 
    TInt iFileIndex;
    };


#endif // NCD_INSTALLED_FILE_H
