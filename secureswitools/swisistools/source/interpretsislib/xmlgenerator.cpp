/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* CXmlGenerator - Used to generate the xml file containing registry information.
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#include "util.h"
#include "xmlgenerator.h"
#include "exception.h"
#include "is_utils.h"

#include "utf8_wrapper.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/util/XercesVersion.hpp>

#include <exception>
#include <string>
#include <vector>

#include <memory>
#include <cstdio>
#include <stdexcept>

#include "toolsconf.h"

XERCES_CPP_NAMESPACE_USE

// these function pointers is used to call appropriate release methods of 
// which are member function of a class. 
typedef void (XERCES_CPP_NAMESPACE::DOMDocument::*releaseDOMDoc) ();
typedef void (XERCES_CPP_NAMESPACE::DOMWriter::*releaseDOMWriter) ();
typedef void (*xmlPlatform) ();
typedef void (*releaseXmlChPtr) (XMLCh** buf);


//------------------------------------- Literals ------------------------------------------
// Xerces literals are of type XMLCh*. For gcc XMLCh is defined as unsigned short (UCS16). 
// In windows wchar_t has the same size as XMLCh therefor literals defined as L"This is a 
// literal" will work. But in linux size of wchar_t is 4 (UCS32) bytes and hence L cannot 
// be used for defining literals. To avoid string conversion and dynamic memory allocation 
// following method is used to represent unsigned short literal. Current compiler has the 
// limitation to represent such literals.
//-----------------------------------------------------------------------------------------
const XMLCh* KPreProvisionInformation = (const XMLCh*)    "P\0r\0e\0P\0r\0o\0v\0i\0s\0i\0o\0n\0I\0n\0f\0o\0r\0m\0a\0t\0i\0o\0n\0\0\0";
const XMLCh* KSoftwareTypeName = (const XMLCh*)           "S\0o\0f\0t\0w\0a\0r\0e\0T\0y\0p\0e\0N\0a\0m\0e\0\0\0";
const XMLCh* KUTF16 = (const XMLCh*)                      "U\0T\0F\0-\0001\0006\0\0\0";
const XMLCh* KComponent = (const XMLCh*)                  "C\0o\0m\0p\0o\0n\0e\0n\0t\0\0\0";
const XMLCh* KDependentId = (const XMLCh*)                "D\0e\0p\0e\0n\0d\0e\0n\0t\0I\0d\0\0\0"; 
const XMLCh* KToVersion = (const XMLCh*)                  "T\0o\0V\0e\0r\0s\0i\0o\0n\0\0\0";
const XMLCh* KFromVersion = (const XMLCh*)                "F\0r\0o\0m\0V\0e\0r\0s\0i\0o\0n\0\0\0";
const XMLCh* KSupplierId = (const XMLCh*)                 "S\0u\0p\0p\0l\0i\0e\0r\0I\0d\0\0\0";
const XMLCh* KName = (const XMLCh*)                       "N\0a\0m\0e\0\0\0";
const XMLCh* KFileProperty_StrValue = (const XMLCh*)      "F\0i\0l\0e\0P\0r\0o\0p\0e\0r\0t\0y\0_\0S\0t\0r\0V\0a\0l\0u\0e\0\0\0";
const XMLCh* KFileProperty_IntValue = (const XMLCh*)      "F\0i\0l\0e\0P\0r\0o\0p\0e\0r\0t\0y\0_\0I\0n\0t\0V\0a\0l\0u\0e\0\0\0";
const XMLCh* KLocation = (const XMLCh*)                   "L\0o\0c\0a\0t\0i\0o\0n\0\0\0";
const XMLCh* KComponentProperty_IsBinary = (const XMLCh*) "C\0o\0m\0p\0o\0n\0e\0n\0t\0P\0r\0o\0p\0e\0r\0t\0y\0_\0I\0s\0B\0i\0n\0a\0r\0y\0\0\0";
const XMLCh* KComponentProperty_StrValue = (const XMLCh*) "C\0o\0m\0p\0o\0n\0e\0n\0t\0P\0r\0o\0p\0e\0r\0t\0y\0_\0S\0t\0r\0V\0a\0l\0u\0e\0\0\0";
const XMLCh* KComponentProperty_IntValue = (const XMLCh*) "C\0o\0m\0p\0o\0n\0e\0n\0t\0P\0r\0o\0p\0e\0r\0t\0y\0_\0I\0n\0t\0V\0a\0l\0u\0e\0\0\0";
const XMLCh* KComponentProperty_Locale = (const XMLCh*)   "C\0o\0m\0p\0o\0n\0e\0n\0t\0P\0r\0o\0p\0e\0r\0t\0y\0_\0L\0o\0c\0a\0l\0e\0\0\0";
const XMLCh* KComponentLocalizable_Vendor=(const XMLCh*)  "C\0o\0m\0p\0o\0n\0e\0n\0t\0L\0o\0c\0a\0l\0i\0z\0a\0b\0l\0e\0_\0V\0e\0n\0d\0o\0r\0\0\0";
const XMLCh* KComponentLocalizable_Name = (const XMLCh*)  "C\0o\0m\0p\0o\0n\0e\0n\0t\0L\0o\0c\0a\0l\0i\0z\0a\0b\0l\0e\0_\0N\0a\0m\0e\0\0\0";
const XMLCh* KComponentLocalizable_Locale=(const XMLCh*)  "C\0o\0m\0p\0o\0n\0e\0n\0t\0L\0o\0c\0a\0l\0i\0z\0a\0b\0l\0e\0_\0L\0o\0c\0a\0l\0e\0\0\0";
const XMLCh* KBuild = (const XMLCh*)                      "B\0u\0i\0l\0d\0\0\0";
const XMLCh* KMinor = (const XMLCh*)                      "M\0i\0n\0o\0r\0\0\0";
const XMLCh* KMajor = (const XMLCh*)                      "M\0a\0j\0o\0r\0\0\0";
const XMLCh* KVersion = (const XMLCh*)                    "V\0e\0r\0s\0i\0o\0n\0\0\0"; 
const XMLCh* KHidden = (const XMLCh*)                     "H\0i\0d\0d\0e\0n\0\0\0";
const XMLCh* KOriginVerified = (const XMLCh*)             "O\0r\0i\0g\0i\0n\0V\0e\0r\0i\0f\0i\0e\0d\0\0\0";
const XMLCh* KGlobalId = (const XMLCh*)                   "G\0l\0o\0b\0a\0l\0I\0d\0\0\0";
const XMLCh* KScomoState = (const XMLCh*)                 "S\0c\0o\0m\0o\0S\0t\0a\0t\0e\0\0\0";
const XMLCh* KSize = (const XMLCh*)                       "S\0i\0z\0e\0\0\0";
const XMLCh* KRemovable = (const XMLCh*)                  "R\0e\0m\0o\0v\0a\0b\0l\0e\0\0\0";
const XMLCh* KComponentLocalizable = (const XMLCh*)       "C\0o\0m\0p\0o\0n\0e\0n\0t\0L\0o\0c\0a\0l\0i\0z\0a\0b\0l\0e\0\0\0";
const XMLCh* KComponentProperty = (const XMLCh*)          "C\0o\0m\0p\0o\0n\0e\0n\0t\0P\0r\0o\0p\0e\0r\0t\0y\0\0\0";
const XMLCh* KComponentProperty_Value = (const XMLCh*)    "C\0o\0m\0p\0o\0n\0e\0n\0t\0P\0r\0o\0p\0e\0r\0t\0y\0_\0V\0a\0l\0u\0e\0\0\0";
const XMLCh* KComponentFile = (const XMLCh*)              "C\0o\0m\0p\0o\0n\0e\0n\0t\0F\0i\0l\0e\0\0\0";
const XMLCh* KFileProperty = (const XMLCh*)               "F\0i\0l\0e\0P\0r\0o\0p\0e\0r\0t\0y\0\0\0";
const XMLCh* KFileProperty_Value = (const XMLCh*)         "F\0i\0l\0e\0P\0r\0o\0p\0e\0r\0t\0y\0_\0V\0a\0l\0u\0e\0\0\0";
const XMLCh* KComponentDependency = (const XMLCh*)        "C\0o\0m\0p\0o\0n\0e\0n\0t\0D\0e\0p\0e\0n\0d\0e\0n\0c\0y\0\0\0"; 	
const XMLCh* KDependencyList = (const XMLCh*)             "D\0e\0p\0e\0n\0d\0e\0n\0c\0y\0L\0i\0s\0t\0\0\0";
const XMLCh* KRomApplication = (const XMLCh*)             "R\0o\0m\0A\0p\0p\0l\0i\0c\0a\0t\0i\0o\0n\0\0\0";
const XMLCh* KApplicationRegistrationInfo = (const XMLCh*) "A\0p\0p\0l\0i\0c\0a\0t\0i\0o\0n\0R\0e\0g\0i\0s\0t\0r\0a\0t\0i\0o\0n\0I\0n\0f\0o\0\0\0";
const XMLCh* KOpaqueData = (const XMLCh*) 				  "O\0p\0a\0q\0u\0e\0D\0a\0t\0a\0\0\0";
const XMLCh* KData = (const XMLCh*)						  "D\0a\0t\0a\0\0\0";
const XMLCh* KOpaqueLocale = (const XMLCh*)				  "O\0p\0a\0q\0u\0e\0L\0o\0c\0a\0l\0e\0\0\0";
const XMLCh* KIsBinary = (const XMLCh*)				  	  "I\0s\0B\0i\0n\0a\0r\0y\0\0\0";
const XMLCh* KFileOwnershipinfo = (const XMLCh*)		  "F\0i\0l\0e\0O\0w\0n\0e\0r\0s\0h\0i\0p\0i\0n\0f\0o\0\0\0";
const XMLCh* KFileName = (const XMLCh*) 				  "F\0i\0l\0e\0N\0a\0m\0e\0\0\0";
const XMLCh* KApplicationLocalizableInfo = (const XMLCh*) "A\0p\0p\0l\0i\0c\0a\0t\0i\0o\0n\0L\0o\0c\0a\0l\0i\0z\0a\0b\0l\0e\0I\0n\0f\0o\0\0\0";
const XMLCh* KLocalizableAttribute = (const XMLCh*)       "L\0o\0c\0a\0l\0i\0z\0a\0b\0l\0e\0A\0t\0t\0r\0i\0b\0u\0t\0e\0\0\0";
const XMLCh* KLocalizableAttribute_Value = (const XMLCh*) "L\0o\0c\0a\0l\0i\0z\0a\0b\0l\0e\0A\0t\0t\0r\0i\0b\0u\0t\0e\0_\0V\0a\0l\0u\0e\0\0\0";
const XMLCh* KLocalizableAttribute_IntValue = (const XMLCh*) "L\0o\0c\0a\0l\0i\0z\0a\0b\0l\0e\0A\0t\0t\0r\0i\0b\0u\0t\0e\0_\0I\0n\0t\0V\0a\0l\0u\0e\0\0\0";
const XMLCh* KLocalizableAttribute_StrValue = (const XMLCh*) "L\0o\0c\0a\0l\0i\0z\0a\0b\0l\0e\0A\0t\0t\0r\0i\0b\0u\0t\0e\0_\0S\0t\0r\0V\0a\0l\0u\0e\0\0\0";
const XMLCh* KViewData = (const XMLCh*) "V\0i\0e\0w\0D\0a\0t\0a\0\0\0";
const XMLCh* KViewDataAttribute = (const XMLCh*) "V\0i\0e\0w\0D\0a\0t\0a\0A\0t\0t\0r\0i\0b\0u\0t\0e\0\0\0";
const XMLCh* KApplicationDataType = (const XMLCh*) "A\0p\0p\0l\0i\0c\0a\0t\0i\0o\0n\0D\0a\0t\0a\0T\0y\0p\0e\0\0\0";
const XMLCh* KPriority = (const XMLCh*) "P\0r\0i\0o\0r\0i\0t\0y\0\0\0";
const XMLCh* KType = (const XMLCh*) "T\0y\0p\0e\0\0\0";
const XMLCh* KViewData_Value = (const XMLCh*) "V\0i\0e\0w\0D\0a\0t\0a\0_\0V\0a\0l\0u\0e\0\0\0";
const XMLCh* KViewData_IntValue = (const XMLCh*) "V\0i\0e\0w\0D\0a\0t\0a\0_\0I\0n\0t\0V\0a\0l\0u\0e\0\0\0";
const XMLCh* KViewData_StrValue = (const XMLCh*) "V\0i\0e\0w\0D\0a\0t\0a\0_\0S\0t\0r\0V\0a\0l\0u\0e\0\0\0";
const XMLCh* KApplicationServiceInfo = (const XMLCh*) "A\0p\0p\0l\0i\0c\0a\0t\0i\0o\0n\0S\0e\0r\0v\0i\0c\0e\0I\0n\0f\0o\0\0\0";
const XMLCh* KUid = (const XMLCh*) "U\0i\0d\0\0\0";
const XMLCh* KServiceOpaqueData = (const XMLCh*) "S\0e\0r\0v\0i\0c\0e\0O\0p\0a\0q\0u\0e\0D\0a\0t\0a\0\0\0";
const XMLCh* KServiceDataIsBinary = (const XMLCh*) "S\0e\0r\0v\0i\0c\0e\0D\0a\0t\0a\0I\0s\0B\0i\0n\0a\0r\0y\0\0\0";
const XMLCh* KServiceData = (const XMLCh*) "S\0e\0r\0v\0i\0c\0e\0D\0a\0t\0a\0\0\0";
const XMLCh* KServiceOpaqueLocale = (const XMLCh*) "S\0e\0r\0v\0i\0c\0e\0O\0p\0a\0q\0u\0e\0L\0o\0c\0a\0l\0e\0\0\0";
const XMLCh* KApplicationProperty = (const XMLCh*) "A\0p\0p\0l\0i\0c\0a\0t\0i\0o\0n\0P\0r\0o\0p\0e\0r\0t\0y\0\0\0";
const XMLCh* KLocale = (const XMLCh*) "L\0o\0c\0a\0l\0e\0\0\0";
const XMLCh* KServiceUid = (const XMLCh*) "S\0e\0r\0v\0i\0c\0e\0U\0i\0d\0\0\0";
const XMLCh* KIntValue = (const XMLCh*) "I\0n\0t\0V\0a\0l\0u\0e\0\0\0";
const XMLCh* KStrValue = (const XMLCh*) "S\0t\0r\0V\0a\0l\0u\0e\0\0\0";
const XMLCh* KIsStr8Bit = (const XMLCh*) "I\0s\0S\0t\0r\08\0B\0i\0t\0\0\0";
const XMLCh* KApplicationAttribute = (const XMLCh*) "A\0p\0p\0l\0i\0c\0a\0t\0i\0o\0n\0A\0t\0t\0r\0i\0b\0u\0t\0e\0\0\0";
const XMLCh* KApplicationAttribute_Value = (const XMLCh*) "A\0p\0p\0l\0i\0c\0a\0t\0i\0o\0n\0A\0t\0t\0r\0i\0b\0u\0t\0e\0_\0V\0a\0l\0u\0e\0\0\0";
const XMLCh* KApplicationAttribute_IntValue = (const XMLCh*) "A\0p\0p\0l\0i\0c\0a\0t\0i\0o\0n\0A\0t\0t\0r\0i\0b\0u\0t\0e\0_\0I\0n\0t\0V\0a\0l\0u\0e\0\0\0";
const XMLCh* KApplicationAttribute_StrValue = (const XMLCh*) "A\0p\0p\0l\0i\0c\0a\0t\0i\0o\0n\0A\0t\0t\0r\0i\0b\0u\0t\0e\0_\0S\0t\0r\0V\0a\0l\0u\0e\0\0\0";




//-----------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------
//											UTILITY FUNCTIONS
//------------------------------------------------------------------------------------------------------------------------------

XercesString ConvertToXercesString(const char* aString, int aLength)
	{
	XMLCh* buffer = new UTF16[aLength + 1];
	XMLCh* temp = buffer; // save as buffer gets modified..
	ConvertUTF8toUTF16(&aString, aString + aLength, &temp, buffer + aLength, lenientConversion);
	
	// Appending NUL to the converted buffer.
	*temp = 0;

	XercesString result;

	result.append(buffer);
	delete[] buffer;

	return result;
	}

XercesString IntegerToXercesString(int aValue)
	{
	std::stringstream str;
	str << aValue;

	return ConvertToXercesString(str.str().c_str(), str.str().length());
	}


XercesString Int64ToXercesString(TInt64 aInt)
	{
	char str[50];
	
	sprintf(str, "%I64u", aInt);

	return ConvertToXercesString(str, strlen(str));
	}
	

//------------------------------------------------------------------------------------------------------------------------------


CXmlGenerator::CXmlGenerator()
	{}

CXmlGenerator::~CXmlGenerator()
	{}


void CXmlGenerator::WritePreProvisionDetails(const std::wstring aXmlFileName, 
												const XmlDetails::TScrPreProvisionDetail& aPreProvisionDetail,
												int& aRomApplication
												)
	{
	try 
		{
		XMLPlatformUtils::Initialize();
		static_fn_auto_ptr<xmlPlatform> xmlPlatformTerm(&XMLPlatformUtils::Terminate);

		DOMImplementation* domImpl = DOMImplementation::getImplementation() ;

		mem_fn_auto_ptr<releaseDOMWriter, DOMWriter*> domWriter( &XERCES_CPP_NAMESPACE::DOMWriter::release, ((DOMImplementationLS*)domImpl)->createDOMWriter() );
				
		SetWriterFeatures(domWriter.get());
		
		std::auto_ptr < SchemaDomErrorHandler > errHandler(new SchemaDomErrorHandler());
		domWriter->setErrorHandler(errHandler.get());

#ifdef _WIN32
		// XMLString::trascode() function should be used for conversion.
		std::auto_ptr < XMLFormatTarget > outputFile( new LocalFileFormatTarget( aXmlFileName.c_str() ) );	
#else
        std::string fileName = wstring2string(aXmlFileName);
		std::auto_ptr < XMLFormatTarget > outputFile( new LocalFileFormatTarget( fileName.c_str() ) );
#endif // _WIN32		
		const char* epocRoot = getenv("EPOCROOT");		
		if(NULL == epocRoot)
			{
			throw std::runtime_error("EPOCROOT environment variable not specified.");
			}
		
		std::string epocRootStr(epocRoot); 

		#ifdef __LINUX__ 
		std::string dtdFilePath = epocRootStr + "epoc32/tools/preprovision.dtd";
		#else 	  	  	 
		std::string dtdFilePath = epocRootStr + "epoc32\\tools\\preprovision.dtd"; 	
		#endif
		
		fn_auto_ptr<releaseXmlChPtr, XMLCh> dtdPath( &XMLString::release, XMLString::transcode(dtdFilePath.c_str()) );
		DOMDocumentType* documentType = domImpl->createDocumentType(KPreProvisionInformation,NULL, dtdPath.get());
		
		
		mem_fn_auto_ptr< releaseDOMDoc, DOMDocument* > domDocument( &XERCES_CPP_NAMESPACE::DOMDocument::release, domImpl->createDocument(	0, KPreProvisionInformation, documentType) );

		DOMElement* rootElement = domDocument->getDocumentElement();
		
		// SoftwareTypeName
		AddChildElement(rootElement, domDocument.get(), KSoftwareTypeName, aPreProvisionDetail.SoftwareTypeName().c_str() );

		std::vector<XmlDetails::TScrPreProvisionDetail::TComponent>::const_iterator compIter;
		for(compIter = aPreProvisionDetail.iComponents.begin(); compIter != aPreProvisionDetail.iComponents.end() ; ++compIter)
			{
			DOMElement*  component = domDocument->createElement(KComponent);
			rootElement->appendChild(component);

			WriteComponent(component,domDocument.get(), *compIter, aRomApplication);
			}
		// do the serialization through DOMWriter::writeNode();
		domWriter->writeNode(outputFile.get(), *domDocument.get()); 	  			
		}
        catch (const XMLException& toCatch) 
			{
            char* message = XMLString::transcode(toCatch.getMessage());
            XMLString::release(&message);
			}
        catch (const DOMException& toCatch) 
			{
            char* message = XMLString::transcode(toCatch.msg);
            XMLString::release(&message);
			}
        catch (...) 
			{
			} 
	}

void CXmlGenerator::SetWriterFeatures(DOMWriter* aDomWriter)
	{
	if (aDomWriter->canSetFeature(XMLUni::fgDOMWRTEntities, true))
		aDomWriter->setFeature(XMLUni::fgDOMWRTEntities, true);

	if (aDomWriter->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
		 aDomWriter->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);

	if (aDomWriter->canSetFeature(XMLUni::fgDOMWRTNormalizeCharacters, false))
		aDomWriter->setFeature(XMLUni::fgDOMWRTNormalizeCharacters, false);

	if (aDomWriter->canSetFeature(XMLUni::fgDOMWRTSplitCdataSections, true))
		 aDomWriter->setFeature(XMLUni::fgDOMWRTSplitCdataSections, true);
	
	if (aDomWriter->canSetFeature(XMLUni::fgDOMWRTValidation, false))
		aDomWriter->setFeature(XMLUni::fgDOMWRTValidation, false);

	if (aDomWriter->canSetFeature(XMLUni::fgDOMWRTWhitespaceInElementContent, true))
		 aDomWriter->setFeature(XMLUni::fgDOMWRTWhitespaceInElementContent, true);
	
	if (aDomWriter->canSetFeature(XMLUni::fgDOMWRTBOM, true))
		aDomWriter->setFeature(XMLUni::fgDOMWRTBOM, true);

	if (aDomWriter->canSetFeature(XMLUni::fgDOMXMLDeclaration, true))
		 aDomWriter->setFeature(XMLUni::fgDOMXMLDeclaration, true);
	
	aDomWriter->setEncoding(KUTF16);
	}

void CXmlGenerator::WriteComponent( DOMElement* aRootElement, DOMDocument* aDocument, 
										const XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, 
										int& aRomApplication
										)
	{

	if (aRomApplication) 
	{
		int isRomApplication = 1;
		XercesString isRomApp = IntegerToXercesString(isRomApplication);
		AddChildElement(aRootElement, aDocument, KRomApplication, isRomApp.c_str());
	}
	
	XercesString isRemovable = IntegerToXercesString(aComponent.iComponentDetails.iIsRemovable);
	AddChildElement(aRootElement, aDocument, KRemovable, isRemovable.c_str() );

	
	XercesString size = Int64ToXercesString(aComponent.iComponentDetails.iSize);
	AddChildElement(aRootElement, aDocument, KSize, size.c_str() );
	
	XercesString scomoState = IntegerToXercesString(aComponent.iComponentDetails.iScomoState);
	AddChildElement(aRootElement, aDocument, KScomoState, scomoState.c_str() );

	AddChildElement(aRootElement, aDocument, KGlobalId, aComponent.iComponentDetails.GlobalId().c_str() );

	WriteComponentVersion(aRootElement, aDocument, aComponent.iComponentDetails.iVersion);
	
	XercesString isOriginVerified = Int64ToXercesString(aComponent.iComponentDetails.iOriginVerified);
	AddChildElement(aRootElement, aDocument, KOriginVerified, isOriginVerified.c_str() );

	XercesString isHidden = IntegerToXercesString(aComponent.iComponentDetails.iIsHidden);
	AddChildElement(aRootElement, aDocument, KHidden, isHidden.c_str() );

	WriteComponentLocalizables(aRootElement, aDocument, aComponent.iComponentLocalizables);

	WriteComponentProperties(aRootElement, aDocument, aComponent.iComponentProperties);

	WriteComponentFiles(aRootElement, aDocument, aComponent.iComponentFiles);

	WriteComponentDependencies(aRootElement, aDocument, aComponent.iComponentDependency);

	WriteAppRegInfo(aRootElement, aDocument, aComponent.iApplicationRegistrationInfo);

	}

void CXmlGenerator::WriteComponentVersion
				(
				DOMElement* aRootElement, 
				DOMDocument* aDocument, 
				XmlDetails::TScrPreProvisionDetail::TComponentDetails::TVersion aVersion
				)
	{
	DOMElement*  version = aDocument->createElement(KVersion);
	aRootElement->appendChild(version);
	version->setAttribute( KMajor, aVersion.MajorVersion().c_str() );
	version->setAttribute( KMinor, aVersion.MinorVersion().c_str() );
	version->setAttribute( KBuild, aVersion.BuildVersion().c_str() );
	}

void CXmlGenerator::WriteComponentLocalizables
					(	
						DOMElement* aRootElement, DOMDocument* aDocument, 
						const std::vector<ComponentLocalizable>& aComponentLocalizable 
					)
	{
	std::vector<ComponentLocalizable>::const_iterator compLocIter;
	for( compLocIter = aComponentLocalizable.begin() ; compLocIter != aComponentLocalizable.end() ; ++compLocIter)
		{
		DOMElement* newRoot = AddTag(aRootElement, aDocument, KComponentLocalizable);
		XercesString locale = IntegerToXercesString(compLocIter->iLocale);


		AddChildElement(newRoot,aDocument, KComponentLocalizable_Locale, locale.c_str());
		AddChildElement(newRoot,aDocument, KComponentLocalizable_Name, compLocIter->Name().c_str());
		AddChildElement(newRoot,aDocument, KComponentLocalizable_Vendor, compLocIter->Vendor().c_str());
		}
	}

void CXmlGenerator::WriteComponentProperties	
					( 
						DOMElement* aRootElement, DOMDocument* aDocument, 
						const std::vector<ComponentProperty>& aComponentProperties 
					)
	{
	std::vector<ComponentProperty>::const_iterator compPropIter;
	for( compPropIter = aComponentProperties.begin() ; compPropIter != aComponentProperties.end() ; ++compPropIter)
		{
		
		DOMElement* compPropRoot = AddTag(aRootElement, aDocument, KComponentProperty);
		
		XercesString locale = IntegerToXercesString(compPropIter->iLocale);
		AddChildElement(compPropRoot,aDocument, KComponentProperty_Locale, locale.c_str());
		
		DOMElement* compPropValueRoot = AddTag(compPropRoot, aDocument, KComponentProperty_Value);
		
		XercesString isBinary = IntegerToXercesString(compPropIter->iIsStr8Bit);

		if(compPropIter->iIsIntValue)
			{
			AddChildElement(compPropValueRoot,aDocument, KComponentProperty_IntValue, compPropIter->Value().c_str());
			}
		else
			{
			AddChildElement(compPropValueRoot,aDocument, KComponentProperty_StrValue, compPropIter->Value().c_str());
			}
		
		
		AddChildElement(compPropRoot,aDocument, KComponentProperty_IsBinary, isBinary.c_str());

		compPropRoot->setAttribute(KName, compPropIter->Name().c_str());
		
		}
	}


/**
 * Writes Class TAppRegistrationInfo Info in XML. 
 */

void CXmlGenerator::WriteAppRegInfo	
					( 
						DOMElement* aRootElement, DOMDocument* aDocument, 
						const std::vector<AppRegistrationInfo>& aAppRegInfo 
					)
{
	std::vector<AppRegistrationInfo>::const_iterator compFileIter;
	for( compFileIter = aAppRegInfo.begin() ; compFileIter != aAppRegInfo.end() ; ++compFileIter)
	{
		DOMElement* compFileRoot = AddTag(aRootElement, aDocument, KApplicationRegistrationInfo);

		WriteAppAttributes(compFileRoot, aDocument, compFileIter->iApplicationAttribute);

		const std::vector<AppOpaqueDataType>& aAppOpaqueDataType = compFileIter->iOpaqueDataType;
		std::vector<AppOpaqueDataType>::const_iterator fileAppOpaqueDataType;

		for( fileAppOpaqueDataType = aAppOpaqueDataType.begin() ; fileAppOpaqueDataType != aAppOpaqueDataType.end() ; ++fileAppOpaqueDataType)
		{
			if(0 == fileAppOpaqueDataType->iServiceUid)
			{
				DOMElement* filePropValueRoot = AddTag(compFileRoot, aDocument, KOpaqueData);
				
				if(fileAppOpaqueDataType->iIsBinary)
				{
					std::string binStrData = Util::Base64Encode(fileAppOpaqueDataType->iOpaqueData);
					XercesString opaqueBinData = ConvertToXercesString(binStrData.c_str(),binStrData.length());
					AddChildElement(filePropValueRoot,aDocument, KData, opaqueBinData.c_str());
				}
				else
				{
					XercesString opaqueData = ConvertToXercesString(fileAppOpaqueDataType->iOpaqueData.c_str(),fileAppOpaqueDataType->iOpaqueData.length());
					AddChildElement(filePropValueRoot,aDocument, KData, opaqueData.c_str());
				}
				
				XercesString locale = IntegerToXercesString(fileAppOpaqueDataType->iLocale);
				AddChildElement(filePropValueRoot,aDocument, KOpaqueLocale, locale.c_str());

				XercesString iBinary = IntegerToXercesString(fileAppOpaqueDataType->iIsBinary);
				AddChildElement(filePropValueRoot,aDocument, KIsBinary, iBinary.c_str());
				
			}
		}

		for(vector<std::wstring>::iterator mimeiter = compFileIter->iFileOwnershipInfo.begin() ; mimeiter != compFileIter->iFileOwnershipInfo.end() ; ++mimeiter )
		{
			DOMElement* filePropRoot = AddTag(compFileRoot, aDocument, KFileOwnershipinfo);

			XercesString temp = WStringToXercesString(*mimeiter);
			AddChildElement(filePropRoot,aDocument, KFileName,  temp.c_str());
		}

		WriteAppServiceInfo(compFileRoot, aDocument, compFileIter->iApplicationServiceInfo, compFileIter->iOpaqueDataType);
		WriteAppLocalizableInfo(compFileRoot, aDocument, compFileIter->iApplicationLocalizableInfo);
	}
}

/**
 * Writes Class TAppLocalizableInfo Info in XML. 
 */

void CXmlGenerator::WriteAppLocalizableInfo	
					( 
						DOMElement* aRootElement, DOMDocument* aDocument, 
						const std::vector<AppLocalizableInfo>& aAppLocalizableInfo
					)
{
	std::vector<AppLocalizableInfo>::const_iterator filePropIter;
	for( filePropIter = aAppLocalizableInfo.begin() ; filePropIter != aAppLocalizableInfo.end() ; ++filePropIter)
	{
		DOMElement* filePropRoot = AddTag(aRootElement, aDocument, KApplicationLocalizableInfo);

		WriteAppLocalizableAttribute(filePropRoot, aDocument, filePropIter->iLocalizableAttribute);
		WriteAppLocalizableViewData(filePropRoot, aDocument, filePropIter->iViewData);
	}
}

/**
 * Writes Class TAppLocalizableAttribute Info in XML. 
 */

void CXmlGenerator::WriteAppLocalizableAttribute	
					( 
						DOMElement* aRootElement, DOMDocument* aDocument, 
						const std::vector<AppLocalizableAttribute>& aAppLocalizableAttribute 
					)
{
	std::vector<AppLocalizableAttribute>::const_iterator filePropIter;
	for( filePropIter = aAppLocalizableAttribute.begin() ; filePropIter != aAppLocalizableAttribute.end() ; ++filePropIter)
	{
		DOMElement* filePropRoot = AddTag(aRootElement, aDocument, KLocalizableAttribute);
		DOMElement* filePropValueRoot = AddTag(filePropRoot, aDocument, KLocalizableAttribute_Value);

		XercesString value = WStringToXercesString(filePropIter->iValue);
		if(filePropIter->iIsIntValue)
		{
			AddChildElement(filePropValueRoot, aDocument, KLocalizableAttribute_IntValue, value.c_str());
		}
		else
		{
			AddChildElement(filePropValueRoot, aDocument, KLocalizableAttribute_StrValue, value.c_str());
		}
		XercesString name = WStringToXercesString(filePropIter->iName);
		filePropRoot->setAttribute(KName, name.c_str());
	}
}

/**
 * Writes Class TAppViewData Info in XML. 
 */

 void CXmlGenerator::WriteAppLocalizableViewData 
					 ( 
						 DOMElement* aRootElement, DOMDocument* aDocument, 
						 const std::vector<AppViewData>& aAppViewData 
					 )
 {
	 std::vector<AppViewData>::const_iterator filePropIter;
	 for( filePropIter = aAppViewData.begin() ; filePropIter != aAppViewData.end() ; ++filePropIter)
	 {
		 DOMElement* filePropRoot = AddTag(aRootElement, aDocument, KViewData);
		 WriteAppLocalizableViewDataAttributes(filePropRoot, aDocument, filePropIter->iViewDataAttributes);
	 }
 }

 void CXmlGenerator::WriteAppLocalizableViewDataAttributes 
					 ( 
						 DOMElement* aRootElement, DOMDocument* aDocument, 
						 const std::vector<AppViewDataAttributes>& aAppViewDataAttributes 
					 )
 {
	 std::vector<AppViewDataAttributes>::const_iterator filePropIter;
	 for( filePropIter = aAppViewDataAttributes.begin() ; filePropIter != aAppViewDataAttributes.end() ; ++filePropIter)
	 {
		 DOMElement* filePropRoot = AddTag(aRootElement, aDocument, KViewDataAttribute);
		 DOMElement* filePropValueRoot = AddTag(filePropRoot, aDocument, KViewData_Value);
 
		 XercesString value = WStringToXercesString(filePropIter->iValue);
		 if(filePropIter->iIsIntValue)
		 {
			 AddChildElement(filePropValueRoot, aDocument, KViewData_IntValue, value.c_str());

		 }
		 else
		 {
			 AddChildElement(filePropValueRoot, aDocument, KViewData_StrValue, value.c_str());

		 }

		 XercesString name = WStringToXercesString(filePropIter->iName);
		 filePropRoot->setAttribute(KName, name.c_str());
	 }
 }

/**
 * Writes Class TAppServiceInfo Info in XML. 
 */

void CXmlGenerator::WriteAppServiceInfo	
					( 
						DOMElement* aRootElement, DOMDocument* aDocument, 
						const std::vector<AppServiceInfo>& aAppServiceInfo,
						const std::vector<AppOpaqueDataType>& aAppOpaqueData 
					)
{
	std::vector<AppServiceInfo>::const_iterator filePropIter;
	std::vector<AppOpaqueDataType>::const_iterator fileAppPropIter;
	for( filePropIter = aAppServiceInfo.begin() ; filePropIter != aAppServiceInfo.end() ; ++filePropIter)
	{
		DOMElement* filePropRoot = AddTag(aRootElement, aDocument, KApplicationServiceInfo);
	
		XercesString uid = IntegerToXercesString(filePropIter->iUid);
		AddChildElement(filePropRoot,aDocument, KUid, uid.c_str());


		for( fileAppPropIter = aAppOpaqueData.begin() ; fileAppPropIter != aAppOpaqueData.end() ; ++fileAppPropIter)
		{
			if(filePropIter->iUid == fileAppPropIter->iServiceUid)
			{
				if(!fileAppPropIter->iOpaqueData.empty())
				{
					DOMElement* filePropValueRoot = AddTag(filePropRoot, aDocument, KServiceOpaqueData);

					if(fileAppPropIter->iIsBinary)
					{
						std::string binStrData = Util::Base64Encode(fileAppPropIter->iOpaqueData);
						XercesString opaqueBinData = ConvertToXercesString(binStrData.c_str(),binStrData.length());
						AddChildElement(filePropValueRoot,aDocument, KServiceData, opaqueBinData.c_str());
					}
					else
					{
						XercesString opaqueData = ConvertToXercesString(fileAppPropIter->iOpaqueData.c_str(),fileAppPropIter->iOpaqueData.length());
						AddChildElement(filePropValueRoot,aDocument, KServiceData, opaqueData.c_str());
					}

					XercesString locale = IntegerToXercesString(fileAppPropIter->iLocale);
					AddChildElement(filePropValueRoot,aDocument, KServiceOpaqueLocale, locale.c_str());

					XercesString iBinary = IntegerToXercesString(fileAppPropIter->iIsBinary);
					AddChildElement(filePropValueRoot,aDocument, KServiceDataIsBinary, iBinary.c_str());
				}
			}
		}

		WriteAppDataType(filePropRoot, aDocument, filePropIter->iDataType);
	}
}

/**
 * Writes Class TDataType Info in XML. 
 */

void CXmlGenerator::WriteAppDataType	
					( 
						DOMElement* aRootElement, DOMDocument* aDocument, 
						const std::vector<AppDataType>& aAppDataType 
					)
{
	std::vector<AppDataType>::const_iterator filePropIter;
	for( filePropIter = aAppDataType.begin() ; filePropIter != aAppDataType.end() ; ++filePropIter)
	{
		DOMElement* filePropRoot = AddTag(aRootElement, aDocument, KApplicationDataType);

		XercesString priority = IntegerToXercesString(filePropIter->iPriority);
		AddChildElement(filePropRoot,aDocument, KPriority, priority.c_str());

		XercesString type = WStringToXercesString(filePropIter->iType);
		AddChildElement(filePropRoot, aDocument, KType, type.c_str());
	}
}

/**
 * Writes Class TApplicationAttribute Info in XML. 
 */

void CXmlGenerator::WriteAppAttributes	
					( 
						DOMElement* aRootElement, DOMDocument* aDocument, 
						const std::vector<ApplicationAttribute>& aAppAttributes 
					)
{
	std::vector<ApplicationAttribute>::const_iterator filePropIter;
	for( filePropIter = aAppAttributes.begin() ; filePropIter != aAppAttributes.end() ; ++filePropIter)
	{
		DOMElement* filePropRoot = AddTag(aRootElement, aDocument, KApplicationAttribute);

		DOMElement* filePropValueRoot = AddTag(filePropRoot, aDocument, KApplicationAttribute_Value);

		XercesString value = WStringToXercesString(filePropIter->iValue);
		if(filePropIter->iIsIntValue)
		{
			AddChildElement(filePropValueRoot, aDocument, KApplicationAttribute_IntValue, value.c_str());
		}
		else
		{
			AddChildElement(filePropValueRoot, aDocument, KApplicationAttribute_StrValue, value.c_str());
		}

		XercesString name = WStringToXercesString(filePropIter->iName);
		filePropRoot->setAttribute(KName, name.c_str());
	}
}

/**
 * Writes Class TApplicationProperty Info in XML. 
 */
void CXmlGenerator::WriteAppProperty	
					( 
						DOMElement* aRootElement, DOMDocument* aDocument, 
						const std::vector<AppProperty>& aAppProperty 
					)
{
	std::vector<AppProperty>::const_iterator filePropIter;
	for( filePropIter = aAppProperty.begin() ; filePropIter != aAppProperty.end() ; ++filePropIter)
	{
		DOMElement* filePropRoot = AddTag(aRootElement, aDocument, KApplicationProperty);

		XercesString locale = IntegerToXercesString(filePropIter->iLocale);
		AddChildElement(filePropRoot,aDocument, KLocale, locale.c_str());

		XercesString name = WStringToXercesString(filePropIter->iName);
		AddChildElement(filePropRoot,aDocument, KName, name.c_str());

		XercesString serviceUid = IntegerToXercesString(filePropIter->iServiceUid);
		AddChildElement(filePropRoot,aDocument, KServiceUid, serviceUid.c_str());

		XercesString intValue = IntegerToXercesString(filePropIter->iIntValue);
		AddChildElement(filePropRoot,aDocument, KIntValue, intValue.c_str());

		XercesString strValue = ConvertToXercesString(filePropIter->iStrValue.c_str(),filePropIter->iStrValue.length());
		AddChildElement(filePropRoot, aDocument, KStrValue, strValue.c_str());

		XercesString isStr8Bit = IntegerToXercesString(filePropIter->iIsStr8Bit);
		AddChildElement(filePropRoot,aDocument, KIsStr8Bit, isStr8Bit.c_str());
	}
}

void CXmlGenerator::WriteComponentFiles	
					( 
						DOMElement* aRootElement, DOMDocument* aDocument, 
						const std::vector<ComponentFile>& aComponentFiles 
					)
	{
	std::vector<ComponentFile>::const_iterator compFileIter;
	for( compFileIter = aComponentFiles.begin() ; compFileIter != aComponentFiles.end() ; ++compFileIter)
		{
		DOMElement* compFileRoot = AddTag(aRootElement, aDocument, KComponentFile);

		WriteFileProperties(compFileRoot, aDocument, compFileIter->iFileProperties);
		
		compFileRoot->setAttribute(KLocation, compFileIter->Location().c_str());
		}
	}

void CXmlGenerator::WriteFileProperties	
					( 
						DOMElement* aRootElement, DOMDocument* aDocument, 
						const std::vector<FileProperty>& aFileProperties 
					)
	{
	std::vector<FileProperty>::const_iterator filePropIter;
	for( filePropIter = aFileProperties.begin() ; filePropIter != aFileProperties.end() ; ++filePropIter)
		{
		DOMElement* filePropRoot = AddTag(aRootElement, aDocument, KFileProperty);
		DOMElement* filePropValueRoot = AddTag(filePropRoot, aDocument, KFileProperty_Value);

		if(filePropIter->iIsIntValue)
			{
			AddChildElement(filePropValueRoot, aDocument, KFileProperty_IntValue, filePropIter->Value().c_str());
			}
		else
			{
			AddChildElement(filePropValueRoot, aDocument, KFileProperty_StrValue, filePropIter->Value().c_str());
			}
		filePropRoot->setAttribute(KName, filePropIter->Name().c_str());
		}
	}


void CXmlGenerator::WriteComponentDependencies	
					( 
						DOMElement* aRootElement, 
						DOMDocument* aDocument, 
						const ComponentDependency& aComponentDependency 
					)
	{
	if( aComponentDependency.iDependentId.empty())
		return;

	std::vector<ComponentDependencyDetail> componentDependencyList = aComponentDependency.iComponentDependencyList;
	std::vector<ComponentDependencyDetail>::const_iterator compDepIter;

	DOMElement* compDepRoot = AddTag(aRootElement, aDocument, KComponentDependency);

	for( compDepIter = componentDependencyList.begin() ; compDepIter != componentDependencyList.end() ; ++compDepIter)
		{
		DOMElement* compDepListRoot = AddTag( compDepRoot, aDocument, KDependencyList );
		AddChildElement( compDepListRoot, aDocument, KSupplierId, compDepIter->SupplierId().c_str() );
		AddChildElement( compDepListRoot, aDocument, KFromVersion, compDepIter->FromVersion().c_str() );
		AddChildElement( compDepListRoot, aDocument, KToVersion, compDepIter->ToVersion().c_str() );
		}
	compDepRoot->setAttribute(KDependentId, aComponentDependency.DependentId().c_str());
	}


void CXmlGenerator::AddChildElement( DOMElement* aRootElement, DOMDocument* aDocument, const XMLCh* aElementName, const XMLCh* aTextValue )
	{
	DOMElement*  element = aDocument->createElement(aElementName);
	aRootElement->appendChild(element);

	DOMText* textValue = aDocument->createTextNode(aTextValue);
	element->appendChild(textValue);
	}

DOMElement* CXmlGenerator::AddTag( DOMElement* aRootElement, DOMDocument* aDocument, const XMLCh* aTagName)
	{
	DOMElement*  tagName = aDocument->createElement(aTagName);
	aRootElement->appendChild(tagName);

	return tagName;
	}


/**
 * Handles all warnings received while xml parsing. 
 */
bool SchemaDomErrorHandler::handleError(const DOMError& domError)
	{
	const XMLCh* message = domError.getMessage();	
	return false;
	}


