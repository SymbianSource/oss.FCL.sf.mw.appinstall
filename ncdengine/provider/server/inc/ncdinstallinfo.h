/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CNcdInstallInfo declaration
*
*/


#ifndef C_NCDINSTALLINFO_H	
#define C_NCDINSTALLINFO_H	

#include <e32base.h>

#include "ncdsendable.h"

class CNcdFileInfo;


class CNcdInstallInfo : public CBase, public MNcdSendable
    {
public:

    /**
     * Different install types
     */    
    enum TNcdInstallType
        {
        /**
         * Normal content & sisx install
         */
        ENcdInstallNormal,
        
        /**
         * Jar install
         */
        ENcdInstallJar,
        
        /**
         * Jar install with a JAD descriptor
         */
        ENcdInstallJad,
        
        /**
         * Widget install
         */
        ENcdInstallWidget
        };
        
        
public:

    /**
     * @param aInfo Contained info. Ownership is transferred Can be NULL
     */
    static CNcdInstallInfo* NewL( CNcdFileInfo* aInfo,
        TNcdInstallType aType );


    /**
     * @param aInfo Contained info. Ownership is transferred. Can be NULL
     */
    static CNcdInstallInfo* NewLC( CNcdFileInfo* aInfo,
        TNcdInstallType aType );

    static CNcdInstallInfo* NewLC( RReadStream& aStream );
        
    virtual ~CNcdInstallInfo();
    
    
    /**
     * Adds a new file info.
     *
     * @param aInfo Info. Ownership is transferred
     * @panic ENcdPanicInvalidArgument if aInfo is NULL     
     */
    void AddFileInfoL( CNcdFileInfo* aInfo );
    
    /**
     * File info count getter
     *
     * @return File info count
     */
    TInt FileInfoCount() const;
    
    
    /**
     * File info getter
     *
     * @param aIndex Index
     * @return Modifiable file info
     */
    CNcdFileInfo& FileInfo( TInt aIndex );
    
    
    /**
     * File info getter
     *
     * @param aIndex Index
     * @return Unmodifiable file info
     */
    const CNcdFileInfo& FileInfo( TInt aIndex ) const;


    /**
     * Install type
     *
     * @return Install type
     */
    TNcdInstallType InstallType() const;
    
    
    /**
     */
    TInt32 Index() const;
    
    
    /**
     */
    void SetIndex( TInt32 aIndex );
        
public: // from MNcdSendable

    /**  
     * @see MNcdSendable::InternalizeL()
     */
    void InternalizeL( RReadStream& aStream );

    /**  
     * @see MNcdSendable::ExternalizeL()
     */
    void ExternalizeL( RWriteStream& aStream ) const;        


    /**
     * Externalizes the info without filenames
     *
     * @param aStream Target stream
     */    
    void ExternalizeWithoutFilenamesL( 
        RWriteStream& aStream ) const;
    
protected:

    CNcdInstallInfo( TNcdInstallType aType );
    void ConstructL( CNcdFileInfo* aInfo );

    CNcdInstallInfo( const CNcdInstallInfo& );
    CNcdInstallInfo& operator=( const CNcdInstallInfo& );
        
private:

    RPointerArray<CNcdFileInfo> iFiles;
    TNcdInstallType iType;
    TInt32 iIndex; 
    };

#endif // C_NCDINSTALLINFO_H
