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
* CScrXmlParser - Used to retrieve details from xml files for creating database and 
* Software environment updates.
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#include "xmlparser.h"
#include "logs.h"
#include "exception.h"
#include "utf8_wrapper.h"
#include "util.h"
#include "utility.h"

#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>

#include <string>
#include <vector>

#include <memory>
#include <cstdio>

#ifndef _WIN32
#include <ctype.h>

//__ctype_b was removed from glibc. This is a workaround to fix the linking problem
extern "C"
    {
    const unsigned short int** __ctype_b()
        {
        return __ctype_b_loc();
        }
    }

#endif // _WIN32

XERCES_CPP_NAMESPACE_USE


//------------------------------------------------------------------------------------------------------------------------------
//											UTILITY FUNCTIONS
//------------------------------------------------------------------------------------------------------------------------------
int XercesStringToInteger(const XercesString& aWideStr)
	{
	int strLen = aWideStr.length();
	const XMLCh* source = aWideStr.c_str();	
  char* buffer = new char[strLen << 2];
  
	// Using a temp variable in place of buffer as ConvertUTF16toUTF8 modifies the source pointer passed.
	char* temp = buffer;	
	
	ConvertUTF16toUTF8(&source, aWideStr.c_str() + strLen, &temp,  temp + (strLen << 2));
	
	// Appending NUL to the converted buffer.
	*temp = 0;

	int value = 0;
	sscanf(buffer, "%d", &value);

	delete[] buffer;
	return value;
	}


// these function pointers are used to call appropriate release methods of XMLString
// present in the xerces library. 
typedef void (*releaseXmlChPtr) (XMLCh** buf);
typedef void (*releaseChPtr) (char** buf);

#ifdef _WIN32

// We need not do anything for WINDOWS, since XMLCh string(XercesString)
// and WString both are same and will be in UTF-16 encoding format.
#define XMLChToWString(aParameter) (aParameter)

#else
inline std::wstring XMLChToWString(const XercesString& aXercesString)
	{
	return XercesStringToWString (aXercesString);
	}
#endif // _WIN32


std::string ConvertOpaqueData(const std::string& opaqueData)
	{
	int iLength = 0;

	//Cardanility check. Values in the range 0-127 are stored in a single byte, 
	//128-16383 in two bytes and other values in 4 bytes.
	if(0x7f > opaqueData.length())
		iLength = 1;
	else
		iLength = 2;

	int iLen = (opaqueData.length()+iLength)*2;
	std::string sStr(iLen, '\0'); // Make room for characters

	if(1==iLength)
		{
		sStr[0]=opaqueData.length();
		sStr[1]=0xab;
		}
	else
		{
		sStr[0]=0x1;
		sStr[1]= ((opaqueData.length() & 0x3F00) >> 8);
		sStr[2]= (opaqueData.length() & 0xFF);
		sStr[3]=0xab;
		}

	char* temp = opaqueData.c_str();

	for(int i =iLength*2; i<iLen; i++)
		{
		sStr[i++] = *temp++;
		}
	sStr[i] = '\0';

	return sStr;
	}

DllExport CScrXmlParser::CScrXmlParser()
	{}

DllExport CScrXmlParser::~CScrXmlParser()
	{}

/**
	Initializes the xml framework for DOM parsing. Registers an error handler for retrieving
	all errors while xml parsing.
	Retrieves database creation statements from the xml file. The DTD for the same is 
	specified in the xml document.
	
	@param	aDbFile	name of the database file in xml format.

	@return list of database create statements in the form of a vector.
 */

DllExport std::vector<std::string>* CScrXmlParser::ParseDbSchema(const std::string& aDbFile)
	{
	std::vector<std::string>* dbSchema = NULL;
	
	try
		{
		LOGENTER("CScrXmlParser::GetDBSchema()");

		XMLPlatformUtils::Initialize();
		
		std::auto_ptr<xercesc::XercesDOMParser> configFileParser(new XercesDOMParser());

		ConfigDomParser(*configFileParser);
		// set the error handler for the xml parser
		std::auto_ptr<SchemaErrorHandler> errHandler(new SchemaErrorHandler());
		configFileParser->setErrorHandler(errHandler.get());

		fn_auto_ptr<releaseXmlChPtr, XMLCh> tagSQLQuery( &XMLString::release, XMLString::transcode("SQLQuery") );
		fn_auto_ptr<releaseXmlChPtr, XMLCh> dbFileName( &XMLString::release, XMLString::transcode(aDbFile.c_str()) );
		
		dbSchema = new std::vector<std::string>();
			
		LocalFileInputSource dbFile( dbFileName.get() );
		configFileParser->parse(dbFile);
		
		// no need to free this pointer - owned by the parent parser object
		DOMDocument* xmlDoc = configFileParser->getDocument();
		
		// Get the top-level element
		DOMElement* elementRoot = xmlDoc->getDocumentElement();
		
		// Retrieve the nodes for CreateDatabase.
		DOMNodeList* createDbNode = elementRoot->getChildNodes();
		const  XMLSize_t createDbNodeCount = createDbNode->getLength();

		// for each Transaction retrieve all tags
		for( XMLSize_t count = 0; count < createDbNodeCount; ++count )
			{
			DOMNode* currentNode = createDbNode->item(count);
			DOMElement* currentElement = static_cast< xercesc::DOMElement* >( currentNode );
			
			if( XMLString::equals(currentElement->getTagName(), tagSQLQuery.get()))
				{
				fn_auto_ptr<releaseChPtr, char> cleanupText( &XMLString::release, XMLString::transcode( currentElement->getTextContent()) );
				std::string query(cleanupText.get());
				dbSchema->push_back(query);
				}
			
			} // for
		}
	
		catch(XMLException& aXmlException)
			{
			delete dbSchema;
			XMLPlatformUtils::Terminate();
			const XMLCh* msg = aXmlException.getMessage();
			std::string msgStr = XMLString::transcode(msg);
			int errCode = aXmlException.getCode();
			throw CException(msgStr,errCode);
			}
		catch(CException& aXmlException)
			{
			delete dbSchema;
			XMLPlatformUtils::Terminate();
			throw aXmlException;
			}
		
	XMLPlatformUtils::Terminate(); 

	LOGEXIT("CScrXmlParser::GetDBSchema()");
	
	return dbSchema;
	}


/**
	Initializes the xml framework for DOM parsing. Registers an error handler for retrieving
	all errors while xml parsing.

	Retrieves software environment details from the xml file. The DTD for the same is 
	specified in the xml document.
	
	@param	aEnvDetailFile	name of the s/w environment file in xml format.

	@return list of details in the form of a vector.
 */

DllExport std::vector<XmlDetails::TScrEnvironmentDetails>* CScrXmlParser::GetEnvironmentDetails( const std::string aEnvDetailFile )
	{	
	
	std::vector<XmlDetails::TScrEnvironmentDetails>* environmentDetails = NULL;

	try
		{

		LOGENTER("CScrXmlParser::GetEnvironmentDetails()");
		
		XMLPlatformUtils::Initialize();
		
		std::auto_ptr<xercesc::XercesDOMParser> configFileParser(new XercesDOMParser());
		
		ConfigDomParser(*configFileParser);
		// set the error handler for the xml parser
		std::auto_ptr<SchemaErrorHandler> errHandler(new SchemaErrorHandler());
		configFileParser->setErrorHandler(errHandler.get());

		// Tags and attributes used in XML file.
		// Can't call transcode till after Xerces Initialize()
		fn_auto_ptr<releaseXmlChPtr, XMLCh> tagEnvironment( &XMLString::release, XMLString::transcode("Environment") );
		
		environmentDetails = new std::vector<XmlDetails::TScrEnvironmentDetails>();
		
		fn_auto_ptr<releaseXmlChPtr, XMLCh> envFileName( &XMLString::release, XMLString::transcode(aEnvDetailFile.c_str()) );
		LocalFileInputSource envFile( envFileName.get() );
		configFileParser->parse( envFile );
		
		// no need to free this pointer - owned by the parent parser object
		DOMDocument* xmlDoc = configFileParser->getDocument();
		
		// Get the top-level element which is SoftwareEnvironments.
		DOMElement* elementRoot = xmlDoc->getDocumentElement();
		
		// Retrieve the nodes for Environment.
		DOMNodeList* entryNodes = elementRoot->getChildNodes();
		const  XMLSize_t nodeCount = entryNodes->getLength();
		
		// for each Environment retrieve all tags
		for( XMLSize_t xx = 0; xx < nodeCount; ++xx )
			{
			DOMNode* currentNode = entryNodes->item(xx);
			DOMElement* currentElement = static_cast< xercesc::DOMElement* >( currentNode );
			
			// is not null and is an element
			if( XMLString::equals(currentElement->getTagName(), tagEnvironment.get()))
				{
				XmlDetails::TScrEnvironmentDetails scrEnvDetails = GetEnvironmentData(currentElement);
				environmentDetails->push_back(scrEnvDetails);
				}				
			}
		}
		catch(XMLException& aXmlException)
			{
			XMLPlatformUtils::Terminate();
			std::string msgStr = XMLString::transcode(aXmlException.getMessage());
			int errCode = aXmlException.getCode();
			throw CException(msgStr,errCode);
			}
		catch (CException& aXmlException)
			{
			XMLPlatformUtils::Terminate(); 
			delete environmentDetails;
			throw aXmlException;
			}

	XMLPlatformUtils::Terminate(); 

	LOGEXIT("CScrXmlParser::GetEnvironmentDetails()");

	return environmentDetails;
	}


/**
	Retrieves details from the s/w environment xml file. It retrieves details specific
	to the tags as specfied in the DTD which is part of the xml file.
	
	@param	aElement		the current Environment tag being processed.
	@param	aParamDetails	pointer to the vector to be populated with the details.

 */
XmlDetails::TScrEnvironmentDetails CScrXmlParser::GetEnvironmentData( const DOMElement* aEnvironment)
	{
	LOGENTER("CScrXmlParser::GetEnvironmentData()");
	
	XmlDetails::TScrEnvironmentDetails scrEnvDetails;
	
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagSoftwareTypeNames( &XMLString::release, XMLString::transcode("SoftwareTypeNames") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagUniqueSoftwareTypeName( &XMLString::release, XMLString::transcode("UniqueSoftwareTypeName") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagLocalizableSwTypeName( &XMLString::release, XMLString::transcode("LocalizableSoftwareTypeName") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagSifPluginUid( &XMLString::release, XMLString::transcode("SifPluginUid") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagLauncherExecutable( &XMLString::release, XMLString::transcode("LauncherExecutable") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagCustomAccess( &XMLString::release, XMLString::transcode("CustomAccess") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagMIMEDetails( &XMLString::release, XMLString::transcode("MIMEDetails") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagMIMEType( &XMLString::release, XMLString::transcode("MIMEType") );
	
	DOMNodeList* swTypeNames = aEnvironment->getElementsByTagName(tagSoftwareTypeNames.get());
	DOMNode* swTypeNamesRoot	= swTypeNames->item(0);
	DOMElement* swTypeNamesNode = static_cast< xercesc::DOMElement* >( swTypeNamesRoot );
	
	DOMNodeList* uniqueSwTypeName = swTypeNamesNode->getElementsByTagName(tagUniqueSoftwareTypeName.get());
	const XMLCh* textContent = uniqueSwTypeName->item(0)->getTextContent();
	scrEnvDetails.iUniqueSoftwareTypeName = XMLChToWString(textContent);
	
	DOMNodeList* localizableSwTypeNames = swTypeNamesNode->getElementsByTagName(tagLocalizableSwTypeName.get());
	const  XMLSize_t localizableSwTypeNamesCount = localizableSwTypeNames->getLength();
	for( XMLSize_t count=0 ; count<localizableSwTypeNamesCount ; ++count )
		{
		DOMNode* localizableSwTypeNameRoot = localizableSwTypeNames->item(count);
		DOMElement* localizableSwTypeNameNode = static_cast< xercesc::DOMElement* >( localizableSwTypeNameRoot );
		XmlDetails::TScrEnvironmentDetails::TLocalizedSoftwareTypeName localizedSwTypeName = GetLocalizedSoftwareTypeName(localizableSwTypeNameNode);
		scrEnvDetails.iLocalizedSoftwareTypeNames.push_back(localizedSwTypeName);
		}
	
	DOMNodeList* sifPluginUid = aEnvironment->getElementsByTagName(tagSifPluginUid.get());
	textContent = (sifPluginUid->item(0))->getTextContent();
	fn_auto_ptr<releaseChPtr,char> sifPluginUidText(&XMLString::release, XMLString::transcode(textContent));
	int sifPluginUidValue = 0;
	sscanf(sifPluginUidText.get(),"%x",&sifPluginUidValue);
	scrEnvDetails.iSifPluginUid = sifPluginUidValue;
	
    DOMNodeList* launcherExecutable = aEnvironment->getElementsByTagName(tagLauncherExecutable.get());
    if(0 != launcherExecutable->getLength())
        {
		textContent = launcherExecutable->item(0)->getTextContent();
		#ifdef _WIN32
		fn_auto_ptr<releaseXmlChPtr, XMLCh> launcherExecutableText( &XMLString::release,textContent);
		const XMLCh* launcherExecutableValue = launcherExecutableText.get();
		scrEnvDetails.iLauncherExecutable = XMLChToWString(launcherExecutableValue);
		#else
		scrEnvDetails.iLauncherExecutable = XMLChToWString(textContent);
		#endif // _WIN32
		}
    DOMNodeList* customAccessList = aEnvironment->getElementsByTagName(tagCustomAccess.get());
	const  XMLSize_t customAccessDataCount = customAccessList->getLength();
	for( XMLSize_t count=0 ; count<customAccessDataCount ; ++count )
		{
		DOMNode* customAccessRoot = customAccessList->item(count);
		DOMElement* customAccessNode = static_cast< xercesc::DOMElement* >( customAccessRoot );
		XmlDetails::TScrEnvironmentDetails::TCustomAccessList customAccessData = GetCustomAccessList(customAccessNode);
		scrEnvDetails.iCustomAccessList.push_back(customAccessData);
		}
	
	DOMNodeList* mimeDetails = aEnvironment->getElementsByTagName(tagMIMEDetails.get());
	DOMNode* mimeDetailRoot	= mimeDetails->item(0);
	DOMElement* mimeDetailNode = static_cast< xercesc::DOMElement* >( mimeDetailRoot );
	DOMNodeList* mimes = mimeDetailNode->getElementsByTagName(tagMIMEType.get());
	const  XMLSize_t mimeCount = mimes->getLength();
	for( XMLSize_t count=0 ; count<mimeCount ; ++count )
		{
		const XMLCh* textContent = mimes->item(count)->getTextContent();
		std::wstring textString = XMLChToWString(textContent);
		scrEnvDetails.iMIMEDetails.push_back(textString);
		}
			
	LOGEXIT("CScrXmlParser::GetEnvironmentData()");
	
	return scrEnvDetails;
	}


XmlDetails::TScrPreProvisionDetail CScrXmlParser::GetPreProvisionDetails( const std::string aPreProvisionFile )	
	{
	XmlDetails::TScrPreProvisionDetail preProvisionDetail;
	try
		{

		LOGENTER("CScrXmlParser::GetPreProvisionDetails()");
		
		XMLPlatformUtils::Initialize();
		
		std::auto_ptr<xercesc::XercesDOMParser> configFileParser(new XercesDOMParser());
		
		ConfigDomParser(*configFileParser);
		// set the error handler for the xml parser
		std::auto_ptr<SchemaErrorHandler> errHandler(new SchemaErrorHandler());
		configFileParser->setErrorHandler(errHandler.get());

		// Tags and attributes used in XML file.
		// Can't call transcode till after Xerces Initialize()
		// root tag
		fn_auto_ptr<releaseXmlChPtr, XMLCh> tagComponent( &XMLString::release, XMLString::transcode("Component") );
		fn_auto_ptr<releaseXmlChPtr, XMLCh> preProvisionFileName( &XMLString::release, XMLString::transcode(aPreProvisionFile.c_str()) );
		
		LocalFileInputSource envFile( preProvisionFileName.get() );
		configFileParser->parse( envFile );
		
		// no need to free this pointer - owned by the parent parser object
		DOMDocument* xmlDoc = configFileParser->getDocument();
		
		// Get the top-level element which is Component.
		DOMElement* elementRoot = xmlDoc->getDocumentElement();
		
		fn_auto_ptr<releaseXmlChPtr, XMLCh> tagSoftwareTypeName( &XMLString::release, XMLString::transcode("SoftwareTypeName") );
	
		DOMNodeList* softwaretype = elementRoot->getElementsByTagName(tagSoftwareTypeName.get());
		const XMLCh* textContent = softwaretype->item(0)->getTextContent();
		preProvisionDetail.iSoftwareTypeName = XMLChToWString(textContent);
	
		// Retrieve the nodes for Component.
		DOMNodeList* componentNodes = elementRoot->getElementsByTagName(tagComponent.get());
		const  XMLSize_t nodeCount = componentNodes->getLength();
		
		
		// for each Component retrieve all tags
		for( XMLSize_t index = 0; index < nodeCount; ++index )
			{
			DOMNode* currentNode = componentNodes->item(index);
			DOMElement* currentElement = static_cast< xercesc::DOMElement* >( currentNode );
			
			preProvisionDetail.iComponents.push_back(GetPreProvisionData(currentElement));
			}
		}
		catch(XMLException& aXmlException)
			{
			XMLPlatformUtils::Terminate();
			std::string msgStr = XMLString::transcode(aXmlException.getMessage());
			int errCode = aXmlException.getCode();
			throw CException(msgStr,errCode);
			}
		catch (CException& aXmlException)
			{
			XMLPlatformUtils::Terminate(); 
			throw aXmlException;
			}

	XMLPlatformUtils::Terminate(); 

	return preProvisionDetail;
	}

XmlDetails::TScrPreProvisionDetail::TComponent CScrXmlParser::GetPreProvisionData( const DOMElement* aDOMElement)
	{
	LOGENTER("CScrXmlParser::GetPreProvisionData()");
	XmlDetails::TScrPreProvisionDetail::TComponent component;
	
	DOMNodeList* childNodes = aDOMElement->getChildNodes();
	const XMLSize_t nodeCount = childNodes->getLength();
	
	// tags contained in Component
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagComponentLocalizable( &XMLString::release, XMLString::transcode("ComponentLocalizable") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagComponentProperty( &XMLString::release, XMLString::transcode("ComponentProperty") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagComponentFile( &XMLString::release, XMLString::transcode("ComponentFile") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagComponentDetails( &XMLString::release, XMLString::transcode("ComponentDetails") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagComponentDependency( &XMLString::release, XMLString::transcode("ComponentDependency") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagApplicationRegistrationInfo( &XMLString::release, XMLString::transcode("ApplicationRegistrationInfo") );

	XmlDetails::TScrPreProvisionDetail::TComponentDetails componentDetails = GetComponentDetails(aDOMElement);
	component.iComponentDetails = componentDetails;

	// for each ComponentProperty retrieve all tags
	for( XMLSize_t index = 0; index < nodeCount; ++index )
		{
		DOMElement* currentElement = static_cast< xercesc::DOMElement* >( childNodes->item(index) );
		
		// is not null and is an element
		if( XMLString::equals(currentElement->getTagName(), tagComponentLocalizable.get()))
			{
			XmlDetails::TScrPreProvisionDetail::TComponentLocalizable componentLocalizable = GetComponentLocalizable(currentElement);
			component.iComponentLocalizables.push_back(componentLocalizable);
			}
		else if( XMLString::equals(currentElement->getTagName(), tagComponentProperty.get()))
			{
			XmlDetails::TScrPreProvisionDetail::TComponentProperty componentProperty = GetComponentProperty(currentElement);
			component.iComponentProperties.push_back(componentProperty);
			}
		else if( XMLString::equals(currentElement->getTagName(), tagComponentFile.get()))
			{
			XmlDetails::TScrPreProvisionDetail::TComponentFile componentFile = GetComponentFile(currentElement);
			component.iComponentFiles.push_back(componentFile);
			}
		else if( XMLString::equals(currentElement->getTagName(), tagComponentDependency.get()))
			{
			XmlDetails::TScrPreProvisionDetail::TComponentDependency componentDependency = GetComponentDependency(currentElement);
			component.iComponentDependency = componentDependency;
			}
		else if( XMLString::equals(currentElement->getTagName(), tagApplicationRegistrationInfo.get()))
			{
			XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo appRegistrationInfo = GetApplicationRegistrationInfo(currentElement);
			component.iApplicationRegistrationInfo.push_back(appRegistrationInfo);
			}
		}
	LOGEXIT("CScrXmlParser::GetPreProvisionData()");
	return component;
	}

XmlDetails::TScrPreProvisionDetail::TComponentLocalizable CScrXmlParser::GetComponentLocalizable(const DOMElement* aDOMElement)
	{
	LOGENTER("CScrXmlParser::GetComponentLocalizable()");
	// tags in ComponentLocalizable
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagComponentLocalizableLocale( &XMLString::release, XMLString::transcode("ComponentLocalizable_Locale") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagComponentLocalizableName( &XMLString::release, XMLString::transcode("ComponentLocalizable_Name") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagComponentLocalizableVendor( &XMLString::release, XMLString::transcode("ComponentLocalizable_Vendor") );
	
	DOMNodeList* locale = aDOMElement->getElementsByTagName(tagComponentLocalizableLocale.get());
	DOMNodeList* name	= aDOMElement->getElementsByTagName(tagComponentLocalizableName.get());
	DOMNodeList* vendor = aDOMElement->getElementsByTagName(tagComponentLocalizableVendor.get());

	XmlDetails::TScrPreProvisionDetail::TComponentLocalizable componentLocalizable;

	if(locale->getLength() != 0)
		{
		const XMLCh* textLocale = locale->item(0)->getTextContent();
		componentLocalizable.iLocale = XercesStringToInteger(textLocale);
		}
	
	const XMLCh* textName = name->item(0)->getTextContent();
	componentLocalizable.iName = XMLChToWString(textName);

	if(vendor->getLength() != 0)
		{
		const XMLCh* textVendor = vendor->item(0)->getTextContent();
		componentLocalizable.iVendor = XMLChToWString(textVendor);
		}
	LOGEXIT("CScrXmlParser::GetComponentLocalizable()");
	return componentLocalizable;
	
	}

XmlDetails::TScrPreProvisionDetail::TComponentProperty CScrXmlParser::GetComponentProperty(const DOMElement* aEnvironment)
	{
	LOGENTER("CScrXmlParser::GetComponentProperty()");
	// tags in ComponentProperty
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagComponentPropertyName( &XMLString::release, XMLString::transcode("Name") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagComponentPropertyLocale( &XMLString::release, XMLString::transcode("ComponentProperty_Locale") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagComponentPropertyValue( &XMLString::release, XMLString::transcode("ComponentProperty_Value") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagComponentPropertyIsBinary( &XMLString::release, XMLString::transcode("ComponentProperty_IsBinary") );
		
	// tags of ComponentProperty_Value
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagComponentPropertyIntValue( &XMLString::release, XMLString::transcode("ComponentProperty_IntValue") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagComponentPropertyStrValue( &XMLString::release, XMLString::transcode("ComponentProperty_StrValue") );
	
	// attribute - name
	DOMNamedNodeMap* attributes = aEnvironment->getAttributes();
	DOMNode* name = attributes->getNamedItem(tagComponentPropertyName.get());
		
	DOMNodeList* locale = aEnvironment->getElementsByTagName(tagComponentPropertyLocale.get());
	DOMNodeList* value = aEnvironment->getElementsByTagName(tagComponentPropertyValue.get());
	DOMNodeList* isBinary = aEnvironment->getElementsByTagName(tagComponentPropertyIsBinary.get());
	
	XmlDetails::TScrPreProvisionDetail::TComponentProperty componentProperty;
	
	const XMLCh* textName = name->getTextContent();
	componentProperty.iName	= XMLChToWString(textName);
	
	if(locale->getLength() != 0)
		{
		const XMLCh* textLocale = locale->item(0)->getTextContent();
		componentProperty.iLocale = XercesStringToInteger(textLocale);
		}

	DOMNodeList* childNodes = value->item(0)->getChildNodes();
	const XMLSize_t nodeCount = childNodes->getLength();

	for(int index = 0; index< nodeCount; ++index)
		{
		DOMElement* currentElement = static_cast< xercesc::DOMElement* >( childNodes->item(index) );
		if( XMLString::equals(currentElement->getTagName(), tagComponentPropertyIntValue.get()))
			{
			componentProperty.iIsIntValue = true;
			componentProperty.iValue = XMLChToWString(currentElement->getTextContent());
			break;
			}
		else if( XMLString::equals(currentElement->getTagName(), tagComponentPropertyStrValue.get()))
			{
			componentProperty.iIsIntValue = false;
			componentProperty.iValue = XMLChToWString(currentElement->getTextContent());
			break;
			}
		}
		
	if(isBinary->getLength() != 0)
		{
		const XMLCh* textIsBinary = isBinary->item(0)->getTextContent();
		componentProperty.iIsStr8Bit = XercesStringToInteger(textIsBinary);		
		}
	LOGEXIT("CScrXmlParser::GetComponentProperty()");
	return componentProperty;
	
	}

XmlDetails::TScrPreProvisionDetail::TComponentFile CScrXmlParser::GetComponentFile( const DOMElement* aDOMElement)
	{
	LOGENTER("CScrXmlParser::GetComponentFile()");
	XmlDetails::TScrPreProvisionDetail::TComponentFile componentFile;
	
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagFileProperty( &XMLString::release, XMLString::transcode("FileProperty") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagLocation( &XMLString::release, XMLString::transcode("Location") );	
	
	DOMNodeList* fileProperties = aDOMElement->getElementsByTagName(tagFileProperty.get());
	const XMLSize_t filePropCount = fileProperties->getLength();

	// for each ComponentFile retrieve all tags
	for( XMLSize_t index = 0; index < filePropCount; ++index )
		{
		DOMElement* currentFileProp = static_cast< xercesc::DOMElement* >( fileProperties->item(index) );
		
		XmlDetails::TScrPreProvisionDetail::TComponentFile::TFileProperty fileProperty = GetFileProperty(currentFileProp);
		componentFile.iFileProperties.push_back(fileProperty);
		}
	
	// attribute - location
	DOMNamedNodeMap* attributes = aDOMElement->getAttributes();
	DOMNode* location = attributes->getNamedItem(tagLocation.get());
	componentFile.iLocation = XMLChToWString(location->getTextContent());
	LOGEXIT("CScrXmlParser::GetComponentFile()");
	return componentFile;
	}

XmlDetails::TScrPreProvisionDetail::TComponentDependency CScrXmlParser::GetComponentDependency( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement)
	{
	LOGENTER("CScrXmlParser::GetComponentDependency()");
	XmlDetails::TScrPreProvisionDetail::TComponentDependency componentDependency;
	
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagDepList( &XMLString::release, XMLString::transcode("DependencyList") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagDependentId( &XMLString::release, XMLString::transcode("DependentId") );	
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagSupplierId( &XMLString::release, XMLString::transcode("SupplierId") );	
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagFromVersion( &XMLString::release, XMLString::transcode("FromVersion") );	
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagToVersion( &XMLString::release, XMLString::transcode("ToVersion") );	

	DOMNodeList* depList = aDOMElement->getElementsByTagName(tagDepList.get());
	const XMLSize_t depListCount = depList->getLength();

	// for each ComponentFile retrieve all tags
	for( XMLSize_t index = 0; index < depListCount; ++index )
		{
		XmlDetails::TScrPreProvisionDetail::TComponentDependency::TComponentDependencyDetail componentDependencyDetail;
		DOMElement* currentDepDetail = static_cast< xercesc::DOMElement* >( depList->item(index) );
		
		DOMNodeList* supplierId = currentDepDetail->getElementsByTagName(tagSupplierId.get());
		const XMLCh* textSupplierId = supplierId->item(0)->getTextContent();
		componentDependencyDetail.iSupplierId = XMLChToWString(textSupplierId);
		
		DOMNodeList* fromVersion = currentDepDetail->getElementsByTagName(tagFromVersion.get());
		const XMLCh* textFromVersion = fromVersion->item(0)->getTextContent();
		componentDependencyDetail.iFromVersion = XMLChToWString(textFromVersion);

		DOMNodeList* toVersion = currentDepDetail->getElementsByTagName(tagToVersion.get());
		const XMLCh* textToVersion = toVersion->item(0)->getTextContent();
		componentDependencyDetail.iToVersion = XMLChToWString(textToVersion);

		componentDependency.iComponentDependencyList.push_back(componentDependencyDetail);
		
		}
	
	// attribute - location
	DOMNamedNodeMap* attributes = aDOMElement->getAttributes();
	DOMNode* dependentId = attributes->getNamedItem(tagDependentId.get());
	componentDependency.iDependentId = XMLChToWString(dependentId->getTextContent());
	LOGEXIT("CScrXmlParser::GetComponentDependency()");
	return componentDependency;
	}

XmlDetails::TScrPreProvisionDetail::TComponentFile::TFileProperty 
	CScrXmlParser::GetFileProperty( const DOMElement* aDOMElement)
	{
	LOGENTER("CScrXmlParser::GetFileProperty()");
	// tag for FileProperty
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagFilePropertyName( &XMLString::release, XMLString::transcode("Name") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagFilePropertyValue( &XMLString::release, XMLString::transcode("FileProperty_Value") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagFilePropertyIsBinary( &XMLString::release, XMLString::transcode("FileProperty_IsBinary") );

	// tags for FileProperty_Value
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagFilePropertyIntValue( &XMLString::release, XMLString::transcode("FileProperty_IntValue") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagFilePropertyStrValue( &XMLString::release, XMLString::transcode("FileProperty_StrValue") );
	
	XmlDetails::TScrPreProvisionDetail::TComponentFile::TFileProperty fileProperty;

	// attribute - name
	DOMNamedNodeMap* attributes = aDOMElement->getAttributes();
	DOMNode* name = attributes->getNamedItem(tagFilePropertyName.get());
		
	DOMNodeList* value = aDOMElement->getElementsByTagName(tagFilePropertyValue.get());
	DOMNodeList* isBinary = aDOMElement->getElementsByTagName(tagFilePropertyIsBinary.get());
	
	const XMLCh* textName = name->getTextContent();
	fileProperty.iName	= XMLChToWString(textName);
		
	int valueLength = value->getLength();
	if(valueLength != 0)
		{
		for(int valIndex = 0; valIndex< valueLength; ++valIndex)
			{
			DOMElement* currentElement = static_cast< xercesc::DOMElement* >( value->item(valIndex) );
			DOMNodeList* childNodes = currentElement->getChildNodes();
			const XMLSize_t nodeCount = childNodes->getLength();
			for(int index = 0; index< nodeCount; ++index)
				{
				DOMElement* valueElement = static_cast< xercesc::DOMElement* >( childNodes->item(index) );
				if( XMLString::equals(valueElement->getTagName(), tagFilePropertyIntValue.get()))
					{
					fileProperty.iIsIntValue = true;
					fileProperty.iValue = XMLChToWString(currentElement->getTextContent());
					}
				else if( XMLString::equals(valueElement->getTagName(), tagFilePropertyStrValue.get()))
					{
					fileProperty.iIsIntValue = false;
					fileProperty.iValue = XMLChToWString(currentElement->getTextContent());
					}
				}
			}
		}
	LOGEXIT("CScrXmlParser::GetFileProperty()");
	return fileProperty;
	}
		
XmlDetails::TScrPreProvisionDetail::TComponentDetails 
	CScrXmlParser::GetComponentDetails( const DOMElement* aDOMElement)
	{
	LOGENTER("CScrXmlParser::GetComponentDetails()");
	// tags for ComponentDetails
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagRomApplication( &XMLString::release, XMLString::transcode("RomApplication") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagRemovable( &XMLString::release, XMLString::transcode("Removable") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagSize( &XMLString::release, XMLString::transcode("Size") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagScomoState( &XMLString::release, XMLString::transcode("ScomoState") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagGlobalId( &XMLString::release, XMLString::transcode("GlobalId") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagVersion( &XMLString::release, XMLString::transcode("Version") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagOriginVerified( &XMLString::release, XMLString::transcode("OriginVerified") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagHidden( &XMLString::release, XMLString::transcode("Hidden") );
	
	XmlDetails::TScrPreProvisionDetail::TComponentDetails componentDetails;
	
	DOMNodeList* romApplication	= aDOMElement->getElementsByTagName(tagRomApplication.get());
	DOMNodeList* removable	= aDOMElement->getElementsByTagName(tagRemovable.get());
	DOMNodeList* size		= aDOMElement->getElementsByTagName(tagSize.get());
	DOMNodeList* scomoState = aDOMElement->getElementsByTagName(tagScomoState.get());
	DOMNodeList* globalId = aDOMElement->getElementsByTagName(tagGlobalId.get());
	DOMNodeList* versionDetail	= aDOMElement->getElementsByTagName(tagVersion.get());
	DOMNodeList* originVerified	= aDOMElement->getElementsByTagName(tagOriginVerified.get());
	DOMNodeList* hidden	= aDOMElement->getElementsByTagName(tagHidden.get());
	
	if( romApplication->getLength() != 0)
		{
		LOGINFO("CScrXmlParser::GetComponentDetails()- rom app");
		const XMLCh* textRomApplication = romApplication->item(0)->getTextContent();
		componentDetails.iIsRomApplication = XercesStringToInteger(textRomApplication);
		}

	if( removable->getLength() != 0)
		{
		const XMLCh* textRemovable = removable->item(0)->getTextContent();
		componentDetails.iIsRemovable = XercesStringToInteger(textRemovable);
		}
	
	if( size->getLength() != 0)
		{
		const XMLCh* textSize = size->item(0)->getTextContent();
		componentDetails.iSize = XercesStringToInteger(textSize);
		}
	
	if( scomoState->getLength() != 0)
		{
		const XMLCh* textScomoState = scomoState->item(0)->getTextContent();
		componentDetails.iScomoState = XercesStringToInteger(textScomoState);
		}
	
	if( globalId->getLength() != 0)
		{
		const XMLCh* textGlobalId = globalId->item(0)->getTextContent();
		componentDetails.iGlobalId = XMLChToWString(textGlobalId);
		}

	if( versionDetail->getLength() != 0)
		{
		// tags for Version
		fn_auto_ptr<releaseXmlChPtr, XMLCh> tagMajor( &XMLString::release, XMLString::transcode("Major") );
		fn_auto_ptr<releaseXmlChPtr, XMLCh> tagMinor( &XMLString::release, XMLString::transcode("Minor") );
		fn_auto_ptr<releaseXmlChPtr, XMLCh> tagBuild( &XMLString::release, XMLString::transcode("Build") );
			
		DOMElement* versionElement = static_cast< xercesc::DOMElement* > (versionDetail->item(0));
		DOMNamedNodeMap* attributes = versionElement->getAttributes();
		DOMNode* major = attributes->getNamedItem(tagMajor.get());
		DOMNode* minor = attributes->getNamedItem(tagMinor.get());
		DOMNode* build = attributes->getNamedItem(tagBuild.get());

		componentDetails.iVersion.iMajor = XMLChToWString(major->getTextContent());
		componentDetails.iVersion.iMinor = XMLChToWString(minor->getTextContent());
		componentDetails.iVersion.iBuild = XMLChToWString(build->getTextContent());
		}
	
	if( originVerified->getLength() != 0)
		{
		const XMLCh* textOriginVerified = originVerified->item(0)->getTextContent();
		componentDetails.iOriginVerified = XercesStringToInteger(textOriginVerified);
		}
	
	if( hidden->getLength() != 0)
		{
		const XMLCh* textHidden = hidden->item(0)->getTextContent();
		componentDetails.iIsHidden = XercesStringToInteger(textHidden);
		}
	LOGEXIT("CScrXmlParser::GetComponentDetails()");
	return componentDetails;
	}

XmlDetails::TScrEnvironmentDetails::TLocalizedSoftwareTypeName 
	CScrXmlParser::GetLocalizedSoftwareTypeName(const DOMElement* aDOMElement)
	{
	LOGENTER("CScrXmlParser::GetLocalizedSoftwareTypeName()");
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagSwTypeNameLocale( &XMLString::release, XMLString::transcode("Locale") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagSwTypeNameValue( &XMLString::release, XMLString::transcode("Value") );
	
	DOMNamedNodeMap* attributes = aDOMElement->getAttributes();
	DOMNode* locale = attributes->getNamedItem(tagSwTypeNameLocale.get());
	DOMNode* name = attributes->getNamedItem(tagSwTypeNameValue.get());

	XmlDetails::TScrEnvironmentDetails::TLocalizedSoftwareTypeName localizedSwTypeName;

	const XMLCh* textLocale = locale->getTextContent();
	localizedSwTypeName.iLocale = XercesStringToInteger(textLocale);
	
	const XMLCh* textName = name->getTextContent();
	localizedSwTypeName.iName =  XMLChToWString(textName);
	LOGEXIT("CScrXmlParser::GetLocalizedSoftwareTypeName()");
	return localizedSwTypeName;
	}

XmlDetails::TScrEnvironmentDetails::TCustomAccessList 
	CScrXmlParser::GetCustomAccessList(const DOMElement* aDOMElement)
	{
	LOGENTER("CScrXmlParser::GetCustomAccessList()");
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagSecureId( &XMLString::release, XMLString::transcode("SecureId") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAccessMode( &XMLString::release, XMLString::transcode("AccessMode") );
	
	DOMNamedNodeMap* attributes = aDOMElement->getAttributes();
	DOMNode* secureId = attributes->getNamedItem(tagSecureId.get());
	DOMNode* accessMode = attributes->getNamedItem(tagAccessMode.get());
	
	XmlDetails::TScrEnvironmentDetails::TCustomAccessList customAccessList;

	fn_auto_ptr<releaseChPtr,char> textSecureId(&XMLString::release, XMLString::transcode(secureId->getTextContent()));
	int secureIdVal=0;	
	sscanf(textSecureId.get(),"%x",&secureIdVal);
	customAccessList.iSecureId = secureIdVal;
	
	const XMLCh* textAccessMode = accessMode->getTextContent();
	customAccessList.iAccessMode = XercesStringToInteger(textAccessMode);
	LOGEXIT("CScrXmlParser::GetCustomAccessList()");
	return customAccessList;
	}

XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo CScrXmlParser::GetApplicationRegistrationInfo(const DOMElement* aDOMElement)
	{
	LOGENTER("CScrXmlParser::GetApplicationRegistrationInfo()");
	// tags in ApplicationRegistrationInfo
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagApplicationRegistrationInfoAppAttribute( &XMLString::release, XMLString::transcode("ApplicationAttribute") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagApplicationRegistrationInfoOpaqueData ( &XMLString::release, XMLString::transcode("OpaqueData") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagApplicationRegistrationInfoFileOwnershipInfo( &XMLString::release, XMLString::transcode("FileOwnershipinfo") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagApplicationRegistrationInfoAppDataType( &XMLString::release, XMLString::transcode("ApplicationDataType") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagApplicationRegistrationInfoAppServiceInfo( &XMLString::release, XMLString::transcode("ApplicationServiceInfo") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagApplicationRegistrationInfoAppLocalizableInfo( &XMLString::release, XMLString::transcode("ApplicationLocalizableInfo") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagApplicationRegistrationInfoAppProperty( &XMLString::release, XMLString::transcode("ApplicationProperty") );

	DOMNodeList* appAttributes = aDOMElement->getElementsByTagName(tagApplicationRegistrationInfoAppAttribute.get());
	const XMLSize_t appAttributeCount = appAttributes->getLength();
	DOMNodeList* OpaqueData	= aDOMElement->getElementsByTagName(tagApplicationRegistrationInfoOpaqueData.get());
	const XMLSize_t OpaqueDataCount = OpaqueData->getLength();
	DOMNodeList* fileOwnershipInfos	= aDOMElement->getElementsByTagName(tagApplicationRegistrationInfoFileOwnershipInfo.get());
	const XMLSize_t fileOwnershipInfoCount = fileOwnershipInfos->getLength();
	DOMNodeList* appServiceInfos = aDOMElement->getElementsByTagName(tagApplicationRegistrationInfoAppServiceInfo.get());
	const XMLSize_t appServiceInfoCount = appServiceInfos->getLength();
	DOMNodeList* appLocalizableInfos	= aDOMElement->getElementsByTagName(tagApplicationRegistrationInfoAppLocalizableInfo.get());
	const XMLSize_t appLocalizableInfoCount = appLocalizableInfos->getLength();
	DOMNodeList* appProperties = aDOMElement->getElementsByTagName(tagApplicationRegistrationInfoAppProperty.get());
	const XMLSize_t appPropertyCount = appProperties->getLength();

	XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo appRegistrationInfo;

	// for each AppAttribute retrieve all tags
	for( XMLSize_t index = 0; index < appAttributeCount; ++index )
		{
		DOMElement* currentappAttribute = static_cast< xercesc::DOMElement* >( appAttributes->item(index) );
		
		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppAttribute appAttribute = GetAppAttribute(currentappAttribute);
		appRegistrationInfo.iApplicationAttribute.push_back(appAttribute);
		}

	// for each OpaqueData retrieve all tags
	for( XMLSize_t index = 0; index < OpaqueDataCount; ++index )
		{
		DOMElement* currentOpaqueData = static_cast< xercesc::DOMElement* >( OpaqueData->item(index) );
		
		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppProperty appProperty;
		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TOpaqueDataType opaqueDataType = GetOpaqueDataType(currentOpaqueData);

		appProperty.iLocale = opaqueDataType.iLocale;
		appProperty.iName = L"OpaqueData";
		appProperty.iIntValue = 0;
		appProperty.iIsStr8Bit = true;
		appProperty.iServiceUid = 0;

		if(opaqueDataType.iIsBinary == 1)
			{
			std::cout.flush();
			appProperty.iStrValue = Util::Base64Decode(opaqueDataType.iOpaqueData);
			}
		else
			{
			appProperty.iStrValue = ConvertOpaqueData(opaqueDataType.iOpaqueData);
			}
		
		appRegistrationInfo.iApplicationProperty.push_back(appProperty);
		}

	// for each FileOwnershipInfo retrieve all tags
	for( XMLSize_t index = 0; index < fileOwnershipInfoCount; ++index )
		{
		DOMElement* currentFileOwnershipInfo = static_cast< xercesc::DOMElement* >( fileOwnershipInfos->item(index) );

		std::wstring file = GetFileOwnershipInfo(currentFileOwnershipInfo);

		appRegistrationInfo.iFileOwnershipInfo.push_back(file);
		}

	// for each appServiceInfo retrieve all tags
	for( XMLSize_t index = 0; index < appServiceInfoCount; ++index )
		{
		DOMElement* currentappServiceInfo = static_cast< xercesc::DOMElement* >( appServiceInfos->item(index) );
		
		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppServiceInfo appServiceInfo = GetAppServiceInfo(currentappServiceInfo, appRegistrationInfo);
		appRegistrationInfo.iApplicationServiceInfo.push_back(appServiceInfo);
		}

	// for each appLocalizableInfo retrieve all tags
	for( XMLSize_t index = 0; index < appLocalizableInfoCount; ++index )
		{
		DOMElement* currentAppLocalizableInfo = static_cast< xercesc::DOMElement* >( appLocalizableInfos->item(index) );
		
		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo appLocalizableInfo = GetAppLocalizableInfo(currentAppLocalizableInfo);
		appRegistrationInfo.iApplicationLocalizableInfo.push_back(appLocalizableInfo);
		}

	// for each AppProperty retrieve all tags
	for( XMLSize_t index = 0; index < appPropertyCount; ++index )
		{
		DOMElement* currentAppProperty = static_cast< xercesc::DOMElement* >( appProperties->item(index) );
		
		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppProperty appProperty = GetAppProperty(currentAppProperty);
		appRegistrationInfo.iApplicationProperty.push_back(appProperty);
		}
	LOGEXIT("CScrXmlParser::GetApplicationRegistrationInfo()");
	return appRegistrationInfo;
	}

XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppAttribute 
	CScrXmlParser::GetAppAttribute( const DOMElement* aDOMElement)
	{
	LOGENTER("CScrXmlParser::GetAppAttribute()");
	// tags in AppAttribute
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAppAttributeName( &XMLString::release, XMLString::transcode("Name") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAppAttributeValue( &XMLString::release, XMLString::transcode("ApplicationAttribute_Value") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAppAttributeIsBinary( &XMLString::release, XMLString::transcode("ApplicationAttribute_IsBinary") );
		
	// tags of ComponentProperty_Value
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagApplicationAttributeIntValue( &XMLString::release, XMLString::transcode("ApplicationAttribute_IntValue") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagApplicationAttributeStrValue( &XMLString::release, XMLString::transcode("ApplicationAttribute_StrValue") );
	
	// attribute - name
	DOMNamedNodeMap* attributes = aDOMElement->getAttributes();
	DOMNode* name = attributes->getNamedItem(tagAppAttributeName.get());
		
	DOMNodeList* value = aDOMElement->getElementsByTagName(tagAppAttributeValue.get());
	DOMNodeList* isBinary = aDOMElement->getElementsByTagName(tagAppAttributeIsBinary.get());
	
	XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppAttribute appAttribute;
	
	const XMLCh* textName = name->getTextContent();
	appAttribute.iName	= XMLChToWString(textName);
	
	DOMNodeList* childNodes = value->item(0)->getChildNodes();
	const XMLSize_t nodeCount = childNodes->getLength();

	for(int index = 0; index< nodeCount; ++index)
		{
		DOMElement* currentElement = static_cast< xercesc::DOMElement* >( childNodes->item(index) );
		if( XMLString::equals(currentElement->getTagName(), tagApplicationAttributeIntValue.get()))
			{
			appAttribute.iIsIntValue = true;
			appAttribute.iValue = XMLChToWString(currentElement->getTextContent());
			break;
			}
		else if( XMLString::equals(currentElement->getTagName(), tagApplicationAttributeStrValue.get()))
			{
			appAttribute.iIsIntValue = false;
			appAttribute.iValue = XMLChToWString(currentElement->getTextContent());
			break;
			}
		}
		
	if(isBinary->getLength() != 0)
		{
		const XMLCh* textIsBinary = isBinary->item(0)->getTextContent();
		appAttribute.iIsStr8Bit = XercesStringToInteger(textIsBinary);
		}
	LOGEXIT("CScrXmlParser::GetAppAttribute()");
	return appAttribute;
	}

std::wstring CScrXmlParser::GetFileOwnershipInfo( const DOMElement* aDOMElement)
	{
	LOGENTER("CScrXmlParser::GetFileOwnershipInfo()");
	// tags in FileOwnershipInfo 
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagFileName( &XMLString::release, XMLString::transcode("FileName") );

	DOMNodeList* fileName = aDOMElement->getElementsByTagName(tagFileName.get());
	
	std::wstring file;

	if( fileName->getLength() != 0)
		{
		const XMLCh* fil = fileName->item(0)->getTextContent();
		file = XMLChToWString(fil);
		}

	LOGEXIT("CScrXmlParser::GetFileOwnershipInfo()");
	return file;
	}

XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TDataType 
	CScrXmlParser::GetDataType( const DOMElement* aDOMElement)
	{
	LOGENTER("CScrXmlParser::GetDataType()");
	// tags in DataType 
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagDataTypePriority( &XMLString::release, XMLString::transcode("Priority") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagDataType( &XMLString::release, XMLString::transcode("Type") );

	DOMNodeList* priority = aDOMElement->getElementsByTagName(tagDataTypePriority.get());
	DOMNodeList* type = aDOMElement->getElementsByTagName(tagDataType.get());
	
	XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TDataType dataType;

	if( priority->getLength() != 0)
		{
		const XMLCh* pri = priority->item(0)->getTextContent();
		dataType.iPriority = XercesStringToInteger(pri);
		}

	if( type->getLength() != 0)
		{
		const XMLCh* typ = type->item(0)->getTextContent();
		dataType.iType = XMLChToWString(typ);
		}
	LOGEXIT("CScrXmlParser::GetDataType()");
	return dataType;
	}

XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TOpaqueDataType 
	CScrXmlParser::GetOpaqueDataType( const DOMElement* aDOMElement)
	{
	LOGENTER("CScrXmlParser::GetOpaqueDataType()");
	// tags in OpaqueDataType 
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagData( &XMLString::release, XMLString::transcode("Data") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagOpaqueLocale( &XMLString::release, XMLString::transcode("OpaqueLocale") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagIsBinary( &XMLString::release, XMLString::transcode("IsBinary") );

	DOMNodeList* Data = aDOMElement->getElementsByTagName(tagData.get());
	DOMNodeList* OpaqueLocale = aDOMElement->getElementsByTagName(tagOpaqueLocale.get());
	DOMNodeList* isBinary = aDOMElement->getElementsByTagName(tagIsBinary.get());
	
	XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TOpaqueDataType opaqueDataType;

	if( OpaqueLocale->getLength() != 0)
		{
		const XMLCh* pri = OpaqueLocale->item(0)->getTextContent();
		opaqueDataType.iLocale = XercesStringToInteger(pri);
		}

	if( Data->getLength() != 0)
		{
		const XMLCh* typ = Data->item(0)->getTextContent();
		opaqueDataType.iOpaqueData = XMLString::transcode(typ);
		}
	
	if(isBinary->getLength() != 0)
		{
		const XMLCh* textIsBinary = isBinary->item(0)->getTextContent();
		opaqueDataType.iIsBinary = XercesStringToInteger(textIsBinary);		
		}

	LOGEXIT("CScrXmlParser::GetOpaqueDataType()");
	return opaqueDataType;
	}

XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TOpaqueDataType 
	CScrXmlParser::GetServiceOpaqueDataType( const DOMElement* aDOMElement)
	{
	LOGENTER("CScrXmlParser::GetServiceOpaqueDataType()");
	// tags in OpaqueDataType 
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagServiceData( &XMLString::release, XMLString::transcode("ServiceData") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagServiceOpaqueLocale( &XMLString::release, XMLString::transcode("ServiceOpaqueLocale") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagServiceDataIsBinary( &XMLString::release, XMLString::transcode("ServiceDataIsBinary") );

	DOMNodeList* ServiceData = aDOMElement->getElementsByTagName(tagServiceData.get());
	DOMNodeList* ServiceOpaqueLocale = aDOMElement->getElementsByTagName(tagServiceOpaqueLocale.get());
	DOMNodeList* ServiceDataIsBinary = aDOMElement->getElementsByTagName(tagServiceDataIsBinary.get());
	
	XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TOpaqueDataType opaqueDataType;

	if( ServiceOpaqueLocale->getLength() != 0)
		{
		const XMLCh* pri = ServiceOpaqueLocale->item(0)->getTextContent();
		opaqueDataType.iLocale = XercesStringToInteger(pri);
		}

	if( ServiceData->getLength() != 0)
		{
		const XMLCh* typ = ServiceData->item(0)->getTextContent();
		opaqueDataType.iOpaqueData = XMLString::transcode(typ);
		}

	if(ServiceDataIsBinary->getLength() != 0)
		{
		const XMLCh* textIsBinary = ServiceDataIsBinary->item(0)->getTextContent();
		opaqueDataType.iIsBinary = XercesStringToInteger(textIsBinary);		
		}

	LOGEXIT("CScrXmlParser::GetServiceOpaqueDataType()");
	return opaqueDataType;
	}

XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppServiceInfo 
	CScrXmlParser::GetAppServiceInfo( const DOMElement* aDOMElement, XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo& aAppRegistrationInfo )
	{
	LOGENTER("CScrXmlParser::GetAppServiceInfo()");
	// tags in AppServiceInfo
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAppServiceInfoUid( &XMLString::release, XMLString::transcode("Uid") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAppServiceAppProperty( &XMLString::release, XMLString::transcode("ServiceOpaqueData") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAppServiceInfoDataType( &XMLString::release, XMLString::transcode("ApplicationDataType") );

	DOMNodeList* uid = aDOMElement->getElementsByTagName(tagAppServiceInfoUid.get());

	DOMNodeList* ServiceOpaqueData = aDOMElement->getElementsByTagName(tagAppServiceAppProperty.get());
	const XMLSize_t ServiceOpaqueDataCount = ServiceOpaqueData->getLength();
	
	DOMNodeList* dataTypes = aDOMElement->getElementsByTagName(tagAppServiceInfoDataType.get());
	const XMLSize_t dataTypeCount = dataTypes->getLength();

	XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppServiceInfo appServiceInfo;

	if( uid->getLength() != 0)
		{
		const XMLCh* priority = uid->item(0)->getTextContent();
		appServiceInfo.iUid = XercesStringToInteger(priority);
		}

	// for each OpaqueData retrieve all tags
	for( XMLSize_t index = 0; index < ServiceOpaqueDataCount; ++index )
		{
		DOMElement* currentOpaqueData = static_cast< xercesc::DOMElement* >( ServiceOpaqueData->item(index) );
		
		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppProperty appProperty;
		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TOpaqueDataType opaqueDataType = GetServiceOpaqueDataType(currentOpaqueData);

		appProperty.iLocale = opaqueDataType.iLocale;
		appProperty.iName = L"OpaqueData";
		appProperty.iIntValue = 0;
		appProperty.iIsStr8Bit = true;
		appProperty.iServiceUid = appServiceInfo.iUid;
		if(opaqueDataType.iIsBinary == 1)
			{
			std::cout.flush();
			appProperty.iStrValue = Util::Base64Decode(opaqueDataType.iOpaqueData);
			}
		else
			{
			appProperty.iStrValue = ConvertOpaqueData(opaqueDataType.iOpaqueData);
			}
		aAppRegistrationInfo.iApplicationProperty.push_back(appProperty);
		}

	// for each DataType retrieve all tags
	for( XMLSize_t index = 0; index < dataTypeCount; ++index )
		{
		DOMElement* currentDataType = static_cast< xercesc::DOMElement* >( dataTypes->item(index) );
		
		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TDataType dataType = GetDataType(currentDataType);
		appServiceInfo.iDataType.push_back(dataType);
		}
	LOGEXIT("CScrXmlParser::GetAppServiceInfo()");
	return appServiceInfo;

	}

XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo 
	CScrXmlParser::GetAppLocalizableInfo( const DOMElement* aDOMElement)
	{
	LOGENTER("CScrXmlParser::GetAppLocalizableInfo()");
	// tags in AppLocalizableInfo  
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAppLocalizableInfoAttribute( &XMLString::release, XMLString::transcode("LocalizableAttribute") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAppLocalizableInfoViewData ( &XMLString::release, XMLString::transcode("ViewData") );
	
	DOMNodeList* localizableAttributes = aDOMElement->getElementsByTagName(tagAppLocalizableInfoAttribute.get());
	const XMLSize_t attributeCount = localizableAttributes->getLength();
	DOMNodeList* viewData = aDOMElement->getElementsByTagName(tagAppLocalizableInfoViewData.get());
	const XMLSize_t viewDataCount = viewData->getLength();

	XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo appLocalizableInfo;

	LOGINFO("for each AppLocalizableInfoAttribute retrieve all tags");
	// for each AppLocalizableInfoAttribute retrieve all tags
	for( XMLSize_t index = 0; index < attributeCount; ++index )
		{
		DOMElement* currentLocalizableAttribute = static_cast< xercesc::DOMElement* >( localizableAttributes->item(index) );
		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TLocalizableAttribute appLocalizableAttribute = GetAppLocalizableAttribute(currentLocalizableAttribute);
		appLocalizableInfo.iLocalizableAttribute.push_back(appLocalizableAttribute);
		}

	LOGINFO("for each AppLocalizableViewData retrieve all tags");
	// for each AppLocalizableViewData retrieve all tags
	
	for( XMLSize_t index = 0; index < viewDataCount; ++index )
		{
		DOMElement* currentLocalizableViewData = static_cast< xercesc::DOMElement* >( viewData->item(index) );
		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TViewData appLocalizableViewData = GetAppLocalizableViewData(currentLocalizableViewData);
		LOGINFO("push_back viewdata");
		appLocalizableInfo.iViewData.push_back(appLocalizableViewData);
		}

	LOGEXIT("CScrXmlParser::GetAppLocalizableInfo()");
	return appLocalizableInfo;
	}

XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TLocalizableAttribute 
	CScrXmlParser::GetAppLocalizableAttribute( const DOMElement* aDOMElement)
	{
	LOGENTER("CScrXmlParser::GetAppLocalizableAttribute()");
	// tags in AppLocalizableInfo  
	// tags in AppLocalizableInfoAttribute
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAppLocalizableInfoAttributeName( &XMLString::release, XMLString::transcode("Name") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAppLocalizableInfoAttributeValue( &XMLString::release, XMLString::transcode("LocalizableAttribute_Value") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAppLocalizableInfoAttributeIsBinary( &XMLString::release, XMLString::transcode("LocalizableAttribute_IsBinary") );
		
	// tags of AppLocalizableInfoAttribute_Value
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAppLocalizableInfoAttributeIntValue( &XMLString::release, XMLString::transcode("LocalizableAttribute_IntValue") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAppLocalizableInfoAttributeStrValue( &XMLString::release, XMLString::transcode("LocalizableAttribute_StrValue") );
	
	LOGINFO("attribute - name");
	// attribute - name
	DOMNamedNodeMap* attributes = aDOMElement->getAttributes();
	DOMNode* name = attributes->getNamedItem(tagAppLocalizableInfoAttributeName.get());
		
	DOMNodeList* value = aDOMElement->getElementsByTagName(tagAppLocalizableInfoAttributeValue.get());
	DOMNodeList* isBinary = aDOMElement->getElementsByTagName(tagAppLocalizableInfoAttributeIsBinary.get());
	
	XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TLocalizableAttribute appLocalizableAttribute;
	
	LOGINFO("name->getTextContent");
	const XMLCh* textName = name->getTextContent();
	appLocalizableAttribute.iName = XMLChToWString(textName);
	
	DOMNodeList* childNodes = value->item(0)->getChildNodes();
	const XMLSize_t nodeCount = childNodes->getLength();

	for(int index = 0; index< nodeCount; ++index)
		{
		DOMElement* currentElement = static_cast< xercesc::DOMElement* >( childNodes->item(index) );
		if( XMLString::equals(currentElement->getTagName(), tagAppLocalizableInfoAttributeIntValue.get()))
			{
			appLocalizableAttribute.iIsIntValue = true;
			appLocalizableAttribute.iValue = XMLChToWString(currentElement->getTextContent());
			break;
			}
		else if( XMLString::equals(currentElement->getTagName(), tagAppLocalizableInfoAttributeStrValue.get()))
			{
			appLocalizableAttribute.iIsIntValue = false;
			appLocalizableAttribute.iValue = XMLChToWString(currentElement->getTextContent());
			break;
			}
		}
		
	if(isBinary->getLength() != 0)
		{
		const XMLCh* textIsBinary = isBinary->item(0)->getTextContent();
		appLocalizableAttribute.iIsStr8Bit = XercesStringToInteger(textIsBinary);
		}

	LOGEXIT("CScrXmlParser::GetAppLocalizableAttribute()");
	return appLocalizableAttribute;
	}


XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TViewData 
	CScrXmlParser::GetAppLocalizableViewData( const DOMElement* aDOMElement)
	{
	LOGENTER("CScrXmlParser::GetAppLocalizableViewData()");
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAppLocalizableInfoViewDataAttribute ( &XMLString::release, XMLString::transcode("ViewDataAttribute") );

	DOMNodeList* viewDataAttr = aDOMElement->getElementsByTagName(tagAppLocalizableInfoViewDataAttribute.get());
	const XMLSize_t viewDataAttrCount = viewDataAttr->getLength();
	
	XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TViewData appViewData;

	for( XMLSize_t index = 0; index < viewDataAttrCount; ++index )
	{
		DOMElement* currentLocalizableViewData = static_cast< xercesc::DOMElement* >( viewDataAttr->item(index) );
		XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TViewData::TViewDataAttributes appLocalizableViewData = GetAppLocalizableViewDataAttributes(currentLocalizableViewData);
		LOGINFO("push_back viewdata");
		appViewData.iViewDataAttributes.push_back(appLocalizableViewData);
	}
	LOGEXIT("CScrXmlParser::GetAppLocalizableViewData()");
	return appViewData;
	}

XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TViewData::TViewDataAttributes 
	CScrXmlParser::GetAppLocalizableViewDataAttributes( const DOMElement* aDOMElement)
	{
	LOGENTER("CScrXmlParser::GetAppLocalizableViewDataAttributes()");
	// tags in AppLocalizableViewData
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAppLocalizableViewDataName( &XMLString::release, XMLString::transcode("Name") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAppLocalizableViewDataValue( &XMLString::release, XMLString::transcode("ViewData_Value") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAppLocalizableViewDataIsBinary( &XMLString::release, XMLString::transcode("ViewData_IsBinary") );
		
	// tags of AppLocalizableViewData_Value
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAppLocalizableViewDataIntValue( &XMLString::release, XMLString::transcode("ViewData_IntValue") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAppLocalizableViewDataStrValue( &XMLString::release, XMLString::transcode("ViewData_StrValue") );
		
	LOGINFO("attribute - name");
	// attribute - name
	DOMNamedNodeMap* attributes = aDOMElement->getAttributes();
	DOMNode* name = attributes->getNamedItem(tagAppLocalizableViewDataName.get());
		
	DOMNodeList* value = aDOMElement->getElementsByTagName(tagAppLocalizableViewDataValue.get());
	DOMNodeList* isBinary = aDOMElement->getElementsByTagName(tagAppLocalizableViewDataIsBinary.get());
	
	XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TViewData::TViewDataAttributes appLocalizableViewDataAttribute;
	
	LOGINFO("name->getTextContent");
	const XMLCh* textName = name->getTextContent();
	LOGINFO("assign name");
	appLocalizableViewDataAttribute.iName = XMLChToWString(textName);

	LOGINFO("getChildNodes()");
	DOMNodeList* childNodes = value->item(0)->getChildNodes();
	LOGINFO("childNodes->getLength()");
	const XMLSize_t nodeCount = childNodes->getLength();

	for(int index = 0; index< nodeCount; ++index)
		{
		DOMElement* currentElement = static_cast< xercesc::DOMElement* >( childNodes->item(index) );
		if( XMLString::equals(currentElement->getTagName(), tagAppLocalizableViewDataIntValue.get()))
			{
			LOGINFO("int value");
			appLocalizableViewDataAttribute.iIsIntValue = true;
			appLocalizableViewDataAttribute.iValue = XMLChToWString(currentElement->getTextContent());
			break;
			}
		else if( XMLString::equals(currentElement->getTagName(), tagAppLocalizableViewDataStrValue.get()))
			{
			LOGINFO("str value");
			appLocalizableViewDataAttribute.iIsIntValue = false;
			appLocalizableViewDataAttribute.iValue = XMLChToWString(currentElement->getTextContent());
			break;
			}
		}
		
	if(isBinary->getLength() != 0)
		{
		LOGINFO("bin value");
		const XMLCh* textIsBinary = isBinary->item(0)->getTextContent();
		appLocalizableViewDataAttribute.iIsStr8Bit = XercesStringToInteger(textIsBinary);
		}
	LOGEXIT("CScrXmlParser::GetAppLocalizableViewDataAttributes()");
	return appLocalizableViewDataAttribute;
	}


XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppProperty 
	CScrXmlParser::GetAppProperty( const DOMElement* aDOMElement)
	{
	LOGENTER("CScrXmlParser::GetAppProperty()");
	// tags in AppProperty
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAppPropertyLocale( &XMLString::release, XMLString::transcode("Locale") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAppPropertyName( &XMLString::release, XMLString::transcode("Name") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAppPropertyIntValue( &XMLString::release, XMLString::transcode("IntValue") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagAppPropertyStrValue( &XMLString::release, XMLString::transcode("StrValue") );

	DOMNodeList* locale = aDOMElement->getElementsByTagName(tagAppPropertyLocale.get());
	DOMNodeList* name = aDOMElement->getElementsByTagName(tagAppPropertyName.get());
	DOMNodeList* intvalue = aDOMElement->getElementsByTagName(tagAppPropertyIntValue.get());
	DOMNodeList* strvalue = aDOMElement->getElementsByTagName(tagAppPropertyStrValue.get());
	
	XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppProperty appProperty;

	if( locale->getLength() != 0)
		{
		const XMLCh* loc = locale->item(0)->getTextContent();
		appProperty.iLocale = XercesStringToInteger(loc);
		}

	if( name->getLength() != 0)
		{
		const XMLCh* nam = name->item(0)->getTextContent();
		appProperty.iName = XMLChToWString(nam);
		}

	if( intvalue->getLength() != 0)
		{
		const XMLCh* intval = intvalue->item(0)->getTextContent();
		appProperty.iIntValue = XercesStringToInteger(intval);
		}
	else if( strvalue->getLength() != 0)
		{
		const XMLCh* strval = strvalue->item(0)->getTextContent();
		appProperty.iStrValue = XMLString::transcode(strval);
		}

	appProperty.iIsStr8Bit = false;
	
	LOGEXIT("CScrXmlParser::GetAppProperty()");
	return appProperty;
	}

void CScrXmlParser::ConfigDomParser(xercesc::XercesDOMParser& aDomParser)
	{
	aDomParser.setValidationScheme( XercesDOMParser::Val_Always );
	aDomParser.setValidationSchemaFullChecking(true);
	}

/**
 * Handles all warnings received while xml parsing. 
 */
void SchemaErrorHandler::warning (const SAXParseException &exc)
	{
	std::string msg( XMLString::transcode(exc.getMessage()) );
	LOGWARN(msg);
	throw CException(msg,ExceptionCodes::EWarning);
	}

/**
 * Handles all errors received while xml parsing. 
 */
void SchemaErrorHandler::error (const SAXParseException &exc)
	{
	std::string msg( XMLString::transcode(exc.getMessage()) );
	resetErrors();
	LOGERROR(msg);
	throw CException(msg, ExceptionCodes::EParseError);
	}

/**
 * Handles all fatal errors received while xml parsing. 
 */
void SchemaErrorHandler::fatalError (const SAXParseException &exc)
	{
	std::string msg( XMLString::transcode(exc.getMessage()) );
	resetErrors();
	LOGERROR(msg);
	throw CException(msg,ExceptionCodes::EFatalError);
	}

/**
 * Called if an error occurs while reintializing the current xml handler. 
 */
void SchemaErrorHandler::resetErrors ()
	{
	std::string msg( "Resetting error handler object" );
	LOGINFO(msg);
	}


