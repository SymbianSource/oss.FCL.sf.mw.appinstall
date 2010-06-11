/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of CNcdContentDescriptor
*
*/


#include "ncdcontentdescriptor.h"
#include "catalogsconstants.h"
#include "catalogsutils.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
CNcdContentDescriptor* CNcdContentDescriptor::NewL()
    {
    CNcdContentDescriptor* self = NewLC();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
CNcdContentDescriptor* CNcdContentDescriptor::NewLC()
    {
    CNcdContentDescriptor* self = new (ELeave) CNcdContentDescriptor();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }
            
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CNcdContentDescriptor::ConstructL()
    {
    iDescriptorType = KNullDesC().AllocL();
    iDescriptor = KNullDesC().AllocL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
CNcdContentDescriptor::~CNcdContentDescriptor()
    {
    ClearAllData();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CNcdContentDescriptor::SetDescriptorL( const TDesC& aDescriptorType, 
                                            const TDesC16& aDescriptor )
    {
    DLTRACEIN((""));
    ClearAllData();
    
    iDescriptorType = aDescriptorType.AllocL();
    iDescriptor = aDescriptor.AllocL();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
void CNcdContentDescriptor::SetDescriptorL( const TDesC& aDescriptorType, 
                                            const TDesC8& aDescriptor )
    {    
    DLTRACEIN((""));
    ClearAllData();
    
    iDescriptorType = aDescriptorType.AllocL();    
    iDescriptor = ConvertUtf8ToUnicodeL( aDescriptor );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
HBufC* CNcdContentDescriptor::ExtractDataL( const TDesC& aStartTag, 
                                         const TDesC& aEndTag,
                                         const TDesC& aPattern )
    {
    TInt beginningOfData = 0;
    TInt dataLength = 0;
    HBufC* extractedData = NULL;

    if ( aEndTag.Length() > 0 )
        {
        // End tag has been given

        // Find start tag
        beginningOfData = iDescriptor->FindF( aStartTag );
        User::LeaveIfError( beginningOfData );

        // Get beginning of the data
        beginningOfData += aStartTag.Length();

        // Find end tag
        dataLength = iDescriptor->FindF( aEndTag );
        User::LeaveIfError( dataLength );
        // Check validity of end tag position
        if ( dataLength < beginningOfData )
            {
            User::Leave( KErrCorrupt );
            }
        // Set data length
        dataLength -= beginningOfData;

        // Get data
        extractedData = iDescriptor->Mid( beginningOfData, 
            dataLength ).AllocLC();
        
        // Check if pattern is given
        if ( aPattern != KNullDesC )
            {
            // If pattern matches extracted mime type, try extract next tag
            if ( extractedData->MatchF( aPattern ) != KErrNotFound )
                {       
                // Get the end of end tag
                TInt endOfEndTag = beginningOfData + dataLength + 
                    aEndTag.Length();
                    
                // Get the rest of iDescriptor to find the next tag
                TPtrC16 restOfDescriptor = iDescriptor->Mid( endOfEndTag );
                        
                // Find start tag                       
                beginningOfData = restOfDescriptor.FindF( aStartTag );
                        
                if ( beginningOfData != KErrNotFound )
                    {       
                    User::LeaveIfError( beginningOfData );

                    // Get beginning of the data
                    beginningOfData += aStartTag.Length();

                    // Find end tag
                    dataLength = restOfDescriptor.FindF( aEndTag );
                    User::LeaveIfError( dataLength );
                    // Check validity of end tag position
                    if ( dataLength < beginningOfData )
                        {
                        User::Leave( KErrCorrupt );
                        }
                    // Set data length
                    dataLength -= beginningOfData;

                    CleanupStack::PopAndDestroy( extractedData );
                                        
                    // Get data
                    extractedData = restOfDescriptor.Mid( 
                        beginningOfData, dataLength ).AllocLC();
                    }
                }
            }
        }
    else
        {
        // End tag was not given

        // Find start tag
        beginningOfData = iDescriptor->FindF( aStartTag );
        User::LeaveIfError( beginningOfData );

        // Get beginning of the data
        beginningOfData += aStartTag.Length();

        const TDesC& descriptor = iDescriptor->Des();

        while ( 1 )
            {
            // Find end of the line
            while ( descriptor[beginningOfData + dataLength] != 0x0A &&
                   descriptor[beginningOfData + dataLength] != 0x0D &&
                   descriptor[beginningOfData + dataLength] != 0x1A )
                {
                dataLength++;
                // If end of the descriptor is reached,
                // use that as end of the line
                if ( beginningOfData + dataLength == iDescriptor->Length() )
                    {
                    break;
                    }
                }

            // Check if we have some data extracted already
            if ( extractedData == NULL )
                {
                // No previously extracted data found.
                // Create new heap.
                extractedData = iDescriptor->Mid( beginningOfData, 
                    dataLength ).AllocLC();
                }
            else
                {
                // Some previously extracted data found.
                // Realloc new heap and append new data into it.
                HBufC* newBuffer = extractedData->ReAllocL( 
                    extractedData->Length() + dataLength );
                CleanupStack::Pop( extractedData );
                extractedData = newBuffer;
                CleanupStack::PushL( extractedData );
                extractedData->Des().Append( 
                    iDescriptor->Mid( beginningOfData, dataLength ) );
                }

            // If we are at the end of the file, we are done here
            if ( beginningOfData + dataLength == iDescriptor->Length() )
                {
                break;
                }

            // Find start of the next line
            while ( descriptor[beginningOfData + dataLength] == 0x0A ||
                   descriptor[beginningOfData + dataLength] == 0x0D ||
                   descriptor[beginningOfData + dataLength] == 0x1A )
                {
                dataLength++;
                if ( beginningOfData + dataLength == iDescriptor->Length() )
                    {
                    // End of the descriptor reached
                    break;
                    }
                }

            // Check if data continues on the next line
            if ( beginningOfData + dataLength == iDescriptor->Length() ||
                descriptor[beginningOfData + dataLength] != 0x20 )
                {
                // Data doesn't continue on the next line
                break;
                }

            // Data continues on the next line.
            // Set beginning of the data.
            beginningOfData += dataLength + 1;
            dataLength = 0;
            }
        }

    CleanupStack::Pop( extractedData );
    return extractedData;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
const TDesC& CNcdContentDescriptor::DataUriL()
    {
    DLTRACEIN((""));
    _LIT( KObjectUriStartTag, "<objecturi>" );
    _LIT( KObjectUriEndTag, "</objecturi>" );

    _LIT( KMidletJarUriStartTag, "midlet-jar-url: " );

    // Dont parse data URI again, if it has been already parsed
    if ( iDataUri != NULL )
        {
        DLTRACEOUT(( _L("DataUri already parsed: %S"), iDataUri ));
        return *iDataUri;
        }

    if ( iDescriptorType->CompareF( KDescriptorTypeOdd ) == 0 )
        {
        // Descriptor type is DD

        TRAPD( err, 
               iDataUri = ExtractDataL( KObjectUriStartTag, 
                                        KObjectUriEndTag ) );
        if ( err != KErrNone )
            {
            return KNullDesC;
            }

        // Check validity of the URI
        if ( iDataUri->MatchF( KHttpMatchString ) == 0 )
            {
            return *iDataUri;
            }
        else
            {
            // Data URI is invalid, clear data URI
            delete iDataUri;
            iDataUri = NULL;
            iDataUri = KNullDesC().AllocL();
            }
        }
    else if ( iDescriptorType->CompareF( KDescriptorTypeJad ) == 0 )
        {
        // Descriptor type is JAD

        TRAPD( err, 
               iDataUri = ExtractDataL( KMidletJarUriStartTag, 
                                        KNullDesC ) );
        if ( err != KErrNone )
            {
            return KNullDesC;
            }

        // Check validity of the URI
        if ( iDataUri->MatchF( KHttpMatchString ) == 0 )
            {
            return *iDataUri;
            }
        else
            {
            // Data URI is invalid, clear data URI
            delete iDataUri;
            iDataUri = NULL;
            iDataUri = KNullDesC().AllocL();
            }
        }
    
    DLTRACEOUT(("Not JAD nor DD"));
    return KNullDesC;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
const TDesC& CNcdContentDescriptor::InstallNotificationUri()
    {
    DLTRACEIN((""));
    _LIT( KInstNotifyUriStartTag, "<installnotifyuri>" );
    _LIT( KInstNotifyUriEndTag, "</installnotifyuri>" );

    _LIT( KMidletInstNotifyStartTag, "midlet-install-notify: " );

    // Dont parse install notification URI again, if it has been already parsed
    if ( iInstallNotificationUri )
        {
        DLTRACEOUT(( _L("Install notification URI: %S"), 
            iInstallNotificationUri ));
        return *iInstallNotificationUri;
        }

    if ( iDescriptorType->CompareF( KDescriptorTypeOdd ) == 0 )
        {
        // Descriptor type is DD

        TRAPD( err, 
               iInstallNotificationUri = ExtractDataL( KInstNotifyUriStartTag,
                                                       KInstNotifyUriEndTag ) );
        if ( err != KErrNone )
            {
            return KNullDesC;
            }
        DLTRACEOUT(( _L("Install notification URI: %S"), iInstallNotificationUri ));
        return *iInstallNotificationUri;
        }
    else if ( iDescriptorType->CompareF( KDescriptorTypeJad ) == 0 )
        {
        // Descriptor type is JAD

        TRAPD( err, 
               iInstallNotificationUri = ExtractDataL( KMidletInstNotifyStartTag, 
                                                       KNullDesC ) );
        if ( err != KErrNone )
            {
            return KNullDesC;
            }
        DLTRACEOUT(( _L("Install notification URI: %S"), 
            iInstallNotificationUri ));
        return *iInstallNotificationUri;
        }

    return KNullDesC;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
const TDesC& CNcdContentDescriptor::FileName()
    {
    DLTRACEIN((""));
    _LIT( KNameStartTag, "<name>" );
    _LIT( KNameEndTag, "</name>" );

    _LIT( KMidletNameStartTag, "midlet-name: " );

    // Dont parse file name again, if it has been already parsed
    if ( iFileName != NULL )
        {
        return *iFileName;
        }

    if ( iDescriptorType->CompareF( KDescriptorTypeOdd ) == 0 )
        {
        // Descriptor type is DD

        TRAPD( err, iFileName = ExtractDataL( KNameStartTag, KNameEndTag ) );
        if ( err != KErrNone )
            {
            return KNullDesC;
            }
        return *iFileName;
        }
    else if ( iDescriptorType->CompareF( KDescriptorTypeJad ) == 0 )
        {
        // Descriptor type is JAD

        TRAPD( err, 
               iFileName = ExtractDataL( KMidletNameStartTag, KNullDesC ) );
        if ( err != KErrNone )
            {
            return KNullDesC;
            }
        return *iFileName;
        }

    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
const TDesC& CNcdContentDescriptor::MimeType()
    {
    DLTRACEIN((""));
    _LIT( KTypeStartTag, "<type>" );
    _LIT( KTypeEndTag, "</type>" );
    _LIT( KMimeTypeDrmMessage, "application/vnd.oma.drm.message" );

    // Dont parse mime type again, if it has been already parsed
    if ( iMimeType != NULL )
        {
        return *iMimeType;
        }

    if ( iDescriptorType->CompareF( KDescriptorTypeOdd ) == 0 )
        {
        // Descriptor type is DD

        TRAPD( err, 
               iMimeType = ExtractDataL( KTypeStartTag, KTypeEndTag, 
                KMimeTypeDrmMessage ) 
            );
        if ( err != KErrNone  )
            {
            return KNullDesC;
            }

        return *iMimeType;
        }

    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
const TDesC& CNcdContentDescriptor::Descriptor() const
    {
    return *iDescriptor;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
const TDesC& CNcdContentDescriptor::DescriptorType() const
    {
    return *iDescriptorType;
    }


// ---------------------------------------------------------------------------
// Clears all member variables
// ---------------------------------------------------------------------------
//    
void CNcdContentDescriptor::ClearAllData()
    {
    delete iDescriptorType;
    iDescriptorType = NULL;
    delete iDescriptor;
    iDescriptor = NULL;

    delete iDataUri;
    iDataUri = NULL;
    
    delete iInstallNotificationUri;
    iInstallNotificationUri = NULL;
    
    delete iFileName;
    iFileName = NULL;
    
    delete iMimeType;
    iMimeType = NULL;    
    }


