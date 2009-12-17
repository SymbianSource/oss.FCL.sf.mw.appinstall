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
* Description:  
*
*/


#ifndef NcdPROTOCOLUTILS_H
#define NcdPROTOCOLUTILS_H

#include <e32base.h>
#include "ncdprotocoltypes.h"

class NcdProtocolUtils
    {
public:
    static void AssignEmptyDesL( HBufC16*& aDes );
    static void AssignEmptyDesL( HBufC8*& aDes );
    static void AssignDesL( HBufC16*& aDes, const TDesC16& aSource );
    // Does UTF8->unicode conversion:
    static void AssignDesL( HBufC16*& aDes, const TDesC8& aSource );
    static void AssignDesL( HBufC8*& aDes, const TDesC8& aSource );
    static HBufC16* ConvertUtf8ToUnicodeL( const TDesC8& aUtfText );
    static HBufC16* ConvertUtf8ToUnicodeLC( const TDesC8& aUtfText );
    static HBufC8* ConvertUnicodeToUtf8L( const TDesC16& aUnicodeText );
    static TBool IsWhitespace( const TDesC8& aUtfText );
    static TInt DesDecToIntL( const TDesC8& aDes );
    static TInt DesDecToIntL( const TDesC16& aDes );
    static TInt DesDecToInt( const TDesC8& aDes, TInt& aValue );
    static TInt DesDecToInt( const TDesC16& aDes, TInt& aValue );

    static TReal32 DesDecToRealL( const TDesC8& aDes );
    
    static void DesToBool( TBool& aBool, const TDesC8& aDes );
    static void DesToBool( TBool& aBool, const TDesC16& aDes );
    static void DesToNcdBool( TNcdBool& aBool, const TDesC8& aDes );
    static void DesToNcdBool( TNcdBool& aBool, const TDesC16& aDes );
    /**
     * Parses xs:dateTime. If KNullDesC8 parameter is given,
     * the returned TTime value will be zero. Malformed input
     * will cause a leave.
     * @param aDes xs:dateTime format string
     */
//     static TTime TimeStampL( const TDesC8& aDes );

    static void BoolToStringL(TXmlEngString& aString, const TBool aValue);
    static void NcdBoolToStringL(TXmlEngString& aString, const TNcdBool aValue);
    static const TDesC8& BoolToDes( TBool aValue );
    static const TDesC8& NcdBoolToDes( TNcdBool aValue );
    static void DesToStringL(TXmlEngString& aString, const TDesC& aValue);
    static void DesToStringL(TXmlEngString& aString, const TDesC8& aValue);
    static void IntToStringL(TXmlEngString& aString, TInt aValue);

    static void NewAttributeL(
        TXmlEngElement& aParent, const TDesC8& aKey, const TDesC8& aValue);
    static void NewAttributeL(
        TXmlEngElement& aParent, const TDesC8& aKey, const TDesC& aValue);
    static void NewBoolAttributeL(
        TXmlEngElement& aParent, const TDesC8& aKey, const TBool aValue);
    static void NewNcdBoolAttributeL(
        TXmlEngElement& aParent, const TDesC8& aKey, const TNcdBool aValue);
    static void NewAttributeL(
        TXmlEngElement& aParent, const TDesC8& aKey, const TInt aValue);
    static void NewAttributeL(
        TXmlEngElement& aParent, const TDesC8& aKey, const TXmlEngString& aValue);
    
    static TXmlEngElement NewElementL( RXmlEngDocument& aDocument, 
        TXmlEngElement& aParent, const TDesC8& aName, const TXmlEngString& aPrefix);
    static TXmlEngElement NewElementL( RXmlEngDocument& aDocument, 
        TXmlEngElement& aParent, const TDesC8& aName, 
        const TDesC8& aPrefix = KNullDesC8);

    static TXmlEngElement NewElementL(
        RXmlEngDocument& aDocument, const TDesC8& aName, const TXmlEngString& aPrefix);
    static TXmlEngElement NewElementL(
        RXmlEngDocument& aDocument, const TDesC8& aName, 
        const TDesC8& aPrefix = KNullDesC8);
    /**
     * Decodes base64 encoded data into a descriptor.
     * @param Base64 data
     * @return HBufC8* Decoded data
     */
    static HBufC8* DecodeBase64L( const TDesC8& aData );
    /**
     * Decodes base64 encoded data into a descriptor.
     * @param Base64 data
     * @return HBufC8* Decoded data
     */
    static HBufC8* DecodeBase64LC( const TDesC8& aData );

    static TTime DesToTimeL(const TDesC8& aDes);
    static TTime DesToTimeL(const TDesC16& aDes);
    static void TimeToDesL(const TTime aTime, HBufC16*& aDes);
    
private:
    NcdProtocolUtils();
    ~NcdProtocolUtils();

        /**
     * Decodes data.
     * @param aData Input buffer
     * @param aOutput Output buffer
     * @param aCalculateLength ETrue to only calculate decoded length
     * @return TInt Decoded length 
     */
    static TInt DecodeBase64L( const TDesC8& aData, 
                         TDes8& aOutput,
                         TBool aCalculateLength );
    /**
     * Decodes a character
     * @param c
     * @return TInt
     */
    static TInt DecodeBase64L( TInt c );

    };

#endif
