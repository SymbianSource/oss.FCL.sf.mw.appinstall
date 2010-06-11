/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/



#ifndef IA_UPDATE_XML_PARSER_H
#define IA_UPDATE_XML_PARSER_H


#include <e32base.h>
#include <e32cmn.h>
#include <f32file.h>

class CIAUpdateXmlSubParser;
namespace Xml
    {
    class CParser;
    }


/**
 * CIAUpdateXmlParser is the basic class  
 * for IA Update XML parsers and provides basic XML parser
 * functionality.
 *
 * @since S60 v3.2
 */
class CIAUpdateXmlParser : public CBase
    {

public:

    /**
     * @param aSubParser This XML parser uses the sub parser to forward the
     * parsed elements and attributes for further handling.
     * NULL value is NOT accepted here. Ownership is transferred.
     * @return CIAUpdateXmlParser* Created object.
     *
     * @since S60 v3.2
     */
    IMPORT_C static CIAUpdateXmlParser* NewL( CIAUpdateXmlSubParser* aSubParser );

    /**
     * @see CIAUpdateXmlParser::NewL
     *
     * @since S60 v3.2
     */
    IMPORT_C static CIAUpdateXmlParser* NewLC( CIAUpdateXmlSubParser* aSubParser );

    
    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    IMPORT_C virtual ~CIAUpdateXmlParser();


    /**
     * Parses the data from the given XML file.
     *
     * @param aFilePath The path to the XML file.
     *
     * @since S60 v3.2
     */
    IMPORT_C void ParseFileL( const TDesC& aFilePath );

    
    /**
     * Parses the data from the given XML file.
     * The correct file is searched from the application private 
     * directories of different drives. The correct file is
     * searched by accepting the first file that is not in a ROM drive
     * or if no file from a non-ROM drive is found, then a file
     * from ROM is accepted if it is found. The file is searched by
     * starting from the A drive and first acceptable file is parsed.
     *
     * @para aFileName The name of the XML file. Notice, that this is only
     * the name of the file, not the whole path to the file.
     *
     * @since S60 v3.2     
     */
    IMPORT_C void ParsePrivateFileL( const TDesC& aFileName );


    /**
     * Parses the given data.
     *
     * @param aData XML data.
     *
     * @since S60 v3.2
     */
    IMPORT_C void ParseL( const TDesC8& aData );

    /**
     * Parses the given data.
     *
     * @param aData XML data.
     *
     * @since S60 v3.2
     */
    IMPORT_C void ParseL( const TDesC& aData );


protected:
    
    /**
     * Constructor
     *
     * @see CIAUpdateXmlParser::NewL
     *
     * @since S60 v3.2
     */
    IMPORT_C CIAUpdateXmlParser( CIAUpdateXmlSubParser* aSubParser );
    
    /**
     * 2nd. phase constructor.
     *
     * @since S60 v3.2
     */
    IMPORT_C virtual void ConstructL();


    /**
     * @return CIAUpdateXmlSubParser& Reference to the sub parser that handles
     * the elements that this parser parses from the XML data. This sub parser is
     * the root element for the possible sub element hierarchy below it.
     *
     * @since S60 v3.2
     */ 
    IMPORT_C CIAUpdateXmlSubParser& SubParser();    


private:
    
    // Prevent these if not implemented
    CIAUpdateXmlParser( const CIAUpdateXmlParser& aObject );
    CIAUpdateXmlParser& operator =( const CIAUpdateXmlParser& aObject );

    /**
     * @return Xml::CParser& Reference to the XML parser that gives the XML elements
     * for sub parsers.
     *
     * @since S60 v3.2
     */
    Xml::CParser& Parser();

    HBufC8* ReadFileL( const TDesC& aFilePath );
    HBufC8* ConvertUnicodeToUtf8L( const TDesC16& aUnicodeText );
    void SetPrivateDriveL( RFs& aFs, const TDesC& aFileName ) const;
    TInt SetSessionPrivatePathL( RFs& aFs, const TDesC& aPath ) const;


private: // data

    // This parser parses the XML data and gives the elements
    // for the sub parsers.
    Xml::CParser* iParser;

    // Sub parser is owned by this class object.
    CIAUpdateXmlSubParser* iSubParser;

    };

#endif // IA_UPDATE_XML_PARSER_H

