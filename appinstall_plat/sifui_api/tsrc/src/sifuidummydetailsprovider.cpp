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
* Description: Dummy implementation for details provider interface.
*
*/

#include <sifuidummydetailsprovider.h>  // CDummyDetailsProvider
#include <f32file.h>                    // RFs, TVolumeInfo


CDummyDetailsProvider::CDummyDetailsProvider( RFs& aFs ) : iFs( aFs )
    {
    }

CDummyDetailsProvider::~CDummyDetailsProvider()
    {
    }

void CDummyDetailsProvider::SetToFirstFieldL()
    {
    iCurrentField = 0;
    }

TBool CDummyDetailsProvider::HasNextField()
    {
    return( iCurrentField < 3 );
    }

void CDummyDetailsProvider::GetNextFieldL( TDes& aFieldName, TDes& aFieldValue )
    {
    __ASSERT_ALWAYS( iCurrentField < 3, User::Invariant() );
    aFieldName.Zero();
    aFieldName.AppendFormat( _L("Field_%d"), iCurrentField );
    aFieldValue.Zero();
    aFieldValue.AppendFormat( _L("Value %d"), iCurrentField );
    ++iCurrentField;
    }

void CDummyDetailsProvider::SetToFirstItemL()
    {
    iCurrentItem = 0;
    }

TBool CDummyDetailsProvider::HasNextItem()
    {
    return( iCurrentField < 3 );
    }

void CDummyDetailsProvider::GetNextItemL( TDes& aItemName )
    {
    __ASSERT_ALWAYS( iCurrentItem < 3, User::Invariant() );
    aItemName.Zero();
    aItemName.AppendFormat( _L("Item_%d"), iCurrentItem );
    ++iCurrentItem;
    }

MSifUiDetailsIterator* CDummyDetailsProvider::DetailsIterator( const TDesC& aItemName )
    {
    return this;
    }

MSifUiDetailsIterator* CDummyDetailsProvider::AppDetailsIterator()
    {
    return this;
    }

MSifUiListIterator* CDummyDetailsProvider::CertificatesListL()
    {
    return this;
    }

MSifUiListIterator* CDummyDetailsProvider::DrmFilesListL()
    {
    return this;
    }

void CDummyDetailsProvider::GetDriveListL( RArray<TDriveUnit>& aDriveList )
    {
    aDriveList.Reset();
    TDriveList driveList;
    User::LeaveIfError( iFs.DriveList( driveList ) );
    for( TInt driveNumber = EDriveA; driveNumber <= EDriveZ; ++driveNumber )
        {
        if( driveList[ driveNumber ] )
            {
            TVolumeInfo volInfo;
            if( iFs.Volume( volInfo, driveNumber ) == KErrNone )
                {
                if( volInfo.iDrive.iType != EMediaNotPresent &&
                    volInfo.iDrive.iType != EMediaRom &&
                    volInfo.iDrive.iType != EMediaRemote &&
                    !( volInfo.iDrive.iDriveAtt & KDriveAttRom ) &&
                    !( volInfo.iDrive.iDriveAtt & KDriveAttSubsted ) )
                    {
                    aDriveList.AppendL( driveNumber );
                    }
                }
            }
        }
    }

void CDummyDetailsProvider::GetFolderListL( const TDesC& aParent, CDesCArray& aFolderList )
    {
    aFolderList.Reset();
    if( aParent.Find( _L("SubSub") ) != KErrNotFound )
        {
        // no subfolders, should not get here
        User::Invariant();
        }
    else if( ( aParent.Find( _L("Applications") ) != KErrNotFound ) ||
             ( aParent.Find( _L("Programs") ) != KErrNotFound ) ||
             ( aParent.Find( _L("Installations") ) != KErrNotFound ) )
        {
        aFolderList.AppendL( _L("SubSubDir") );
        aFolderList.AppendL( _L("SubSubFolder") );
        }
    else
        {
        aFolderList.AppendL( _L("Applications") );
        aFolderList.AppendL( _L("Programs") );
        aFolderList.AppendL( _L("Installations") );
        }
    }

TBool CDummyDetailsProvider::HasFolderSubfolders( const TDesC& aParent, const TDesC& aFolder )
    {
    return( aParent.Find( _L("SubSub") ) != KErrNotFound );
    }

