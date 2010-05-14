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
* Description:   Descriptor data handler class.
 *
*/


#ifndef C_NCDCONTENTDESCRIPTOR_H
#define C_NCDCONTENTDESCRIPTOR_H

#include <e32base.h>

/**
 * Class for storing and handling descriptor data.
 */
class CNcdContentDescriptor : public CBase
    {
public:
    /**
     * Constructor
     */
    static CNcdContentDescriptor* NewL();
    /**
     * Constructor
     */
    static CNcdContentDescriptor* NewLC();

    /**
     * Destructor
     */
    ~CNcdContentDescriptor();

public:
    /**
     * Set descriptor and descriptor type
     * @param aDescriptorType Type of the descriptor
     * @param aDescriptor Descriptor
     */
    void SetDescriptorL( const TDesC& aDescriptorType,
                         const TDesC16& aDescriptor );


    /**
     * Set descriptor and descriptor type
     * @param aDescriptorType Type of the descriptor
     * @param aDescriptor Descriptor
     */
    void SetDescriptorL( const TDesC& aDescriptorType,
                         const TDesC8& aDescriptor );

    /**
     * Gets the data URI.
     * Data URI will be extracted from the descriptor only when this
     * function is called for the first time after SetDescriptorL().
     * @return Data URI
     */
    const TDesC& DataUriL();
    
    /**
     * Gets the install notification URI.
     * Install notification URI will be extracted from the descriptor
     * only when this function is called for the first time after
     * SetDescriptorL().
     * @return Install notification URI
     */
    const TDesC& InstallNotificationUri();
    
    /**
     * Gets the file name.
     * File name will be extracted from the descriptor only when this
     * function is called for the first time after SetDescriptorL().
     * @return File name
     */
    const TDesC& FileName();
    
    /**
     * Gets the mime type.
     * Mime type will be extracted from the descriptor only when this
     * function is called for the first time after SetDescriptorL().
     * @return Mime type
     */
    const TDesC& MimeType();

    /**
     * Gets the type of the descriptor.
     * @return Type of the descriptor
     */
    const TDesC& DescriptorType() const;
    
    /**
     * Gets the descriptor.
     * @return Descriptor
     */
    const TDesC& Descriptor() const;

private:
    /**
     * Private constructor
     */
    void ConstructL();

    /**
     * Extracts data from the descriptor.
     * If end tag is KNullDesC, extractor will get data from the
     * start tag to the end of line. If aMatch is given, next tag
     * with same name is extracted. If next tag not found, match
     * type is returned.
     * @param aStartTag Start tag of the extracted data
     * @param aEndTag End tag of the extracted data
     * @param aPattern Mime type pattern
     * @return Extracted mime type
     */
    HBufC* ExtractDataL( const TDesC& aStartTag, const TDesC& aEndTag,
                         const TDesC& aPattern = KNullDesC );
                        
    /**
     * Clears all member variables
     */                         
    void ClearAllData();                         

private:
    // Type of the descriptor
    HBufC*  iDescriptorType;
    // Descriptor
    HBufC*  iDescriptor;

    // Data URI
    HBufC*  iDataUri;
    // Install notification URI
    HBufC*  iInstallNotificationUri;
    // File name
    HBufC*  iFileName;
    // Mime name
    HBufC*  iMimeType;
    };

#endif // C_NCDCONTENTDESCRIPTOR_H
