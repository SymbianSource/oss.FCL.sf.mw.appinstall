/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "util.h"

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

XERCES_CPP_NAMESPACE_USE

// these function pointers are used to call appropriate release methods of XMLString
// present in the xerces library. 
typedef void (*releaseXmlChPtr) (XMLCh** buf);
typedef void (*releaseChPtr) (char** buf);



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
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagInstallerSid( &XMLString::release, XMLString::transcode("InstallerSid") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagExecutionLayerSid( &XMLString::release, XMLString::transcode("ExecutionLayerSid") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagMIMEDetails( &XMLString::release, XMLString::transcode("MIMEDetails") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagMIMEType( &XMLString::release, XMLString::transcode("MIMEType") );
	
	DOMNodeList* swTypeNames = aEnvironment->getElementsByTagName(tagSoftwareTypeNames.get());
	DOMNode* swTypeNamesRoot	= swTypeNames->item(0);
	DOMElement* swTypeNamesNode = static_cast< xercesc::DOMElement* >( swTypeNamesRoot );
	
	DOMNodeList* uniqueSwTypeName = swTypeNamesNode->getElementsByTagName(tagUniqueSoftwareTypeName.get());
	const XMLCh* textContent = uniqueSwTypeName->item(0)->getTextContent();
	scrEnvDetails.iUniqueSoftwareTypeName = std::wstring(textContent);
	
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
	
	DOMNodeList* installerSid = aEnvironment->getElementsByTagName(tagInstallerSid.get());
	textContent = installerSid->item(0)->getTextContent();
	fn_auto_ptr<releaseChPtr,char> installerSidText(&XMLString::release, XMLString::transcode(textContent));
	int installerSidValue = 0;
	sscanf(installerSidText.get(),"%x",&installerSidValue);
	scrEnvDetails.iInstallerSid = installerSidValue;
	
	DOMNodeList* executionLayerSid = aEnvironment->getElementsByTagName(tagExecutionLayerSid.get());
	textContent = executionLayerSid->item(0)->getTextContent();
	fn_auto_ptr<releaseChPtr,char> executionLayerSidText(&XMLString::release, XMLString::transcode(textContent));
	int executionLayerSidValue = 0;
	sscanf(executionLayerSidText.get(),"%x",&executionLayerSidValue);
	scrEnvDetails.iExecutionLayerSid = executionLayerSidValue;
	
	DOMNodeList* mimeDetails = aEnvironment->getElementsByTagName(tagMIMEDetails.get());
	DOMNode* mimeDetailRoot	= mimeDetails->item(0);
	DOMElement* mimeDetailNode = static_cast< xercesc::DOMElement* >( mimeDetailRoot );
	DOMNodeList* mimes = mimeDetailNode->getElementsByTagName(tagMIMEType.get());
	const  XMLSize_t mimeCount = mimes->getLength();
	for( XMLSize_t count=0 ; count<mimeCount ; ++count )
		{
		const XMLCh* textContent = mimes->item(count)->getTextContent();
		scrEnvDetails.iMIMEDetails.push_back(textContent);
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
		preProvisionDetail.iSoftwareTypeName = std::wstring(textContent);
	
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
	XmlDetails::TScrPreProvisionDetail::TComponent component;
	
	DOMNodeList* childNodes = aDOMElement->getChildNodes();
	const XMLSize_t nodeCount = childNodes->getLength();
	
	// tags contained in Component
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagComponentLocalizable( &XMLString::release, XMLString::transcode("ComponentLocalizable") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagComponentProperty( &XMLString::release, XMLString::transcode("ComponentProperty") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagComponentFile( &XMLString::release, XMLString::transcode("ComponentFile") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagComponentDetails( &XMLString::release, XMLString::transcode("ComponentDetails") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagComponentDependency( &XMLString::release, XMLString::transcode("ComponentDependency") );

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
		}

	return component;
	}

XmlDetails::TScrPreProvisionDetail::TComponentLocalizable CScrXmlParser::GetComponentLocalizable(const DOMElement* aDOMElement)
	{
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
		componentLocalizable.iLocale = Util::WideCharToInteger(textLocale);
		}
	
	const XMLCh* textName = name->item(0)->getTextContent();
	componentLocalizable.iName = textName;

	if(vendor->getLength() != 0)
		{
		const XMLCh* textVendor = vendor->item(0)->getTextContent();
		componentLocalizable.iVendor = textVendor;
		}

	return componentLocalizable;
	
	}

XmlDetails::TScrPreProvisionDetail::TComponentProperty CScrXmlParser::GetComponentProperty(const DOMElement* aEnvironment)
	{
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
	componentProperty.iName	= textName;
	
	if(locale->getLength() != 0)
		{
		const XMLCh* textLocale = locale->item(0)->getTextContent();
		componentProperty.iLocale = Util::WideCharToInteger(textLocale);
		}

	DOMNodeList* childNodes = value->item(0)->getChildNodes();
	const XMLSize_t nodeCount = childNodes->getLength();

	for(int index = 0; index< nodeCount; ++index)
		{
		DOMElement* currentElement = static_cast< xercesc::DOMElement* >( childNodes->item(index) );
		if( XMLString::equals(currentElement->getTagName(), tagComponentPropertyIntValue.get()))
			{
			componentProperty.iIsIntValue = true;
			componentProperty.iValue = currentElement->getTextContent();
			break;
			}
		else if( XMLString::equals(currentElement->getTagName(), tagComponentPropertyStrValue.get()))
			{
			componentProperty.iIsIntValue = false;
			componentProperty.iValue = currentElement->getTextContent();
			break;
			}
		}
		
	if(isBinary->getLength() != 0)
		{
		const XMLCh* textIsBinary = isBinary->item(0)->getTextContent();
		componentProperty.iIsStr8Bit = Util::WideCharToInteger(textIsBinary);		
		}

	return componentProperty;
	
	}

XmlDetails::TScrPreProvisionDetail::TComponentFile CScrXmlParser::GetComponentFile( const DOMElement* aDOMElement)
	{
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
	componentFile.iLocation = location->getTextContent();
	
	return componentFile;
	}

XmlDetails::TScrPreProvisionDetail::TComponentDependency CScrXmlParser::GetComponentDependency( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement)
	{
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
		componentDependencyDetail.iSupplierId = std::wstring(textSupplierId);
		
		DOMNodeList* fromVersion = currentDepDetail->getElementsByTagName(tagFromVersion.get());
		const XMLCh* textFromVersion = fromVersion->item(0)->getTextContent();
		componentDependencyDetail.iFromVersion = std::wstring(textFromVersion);

		DOMNodeList* toVersion = currentDepDetail->getElementsByTagName(tagToVersion.get());
		const XMLCh* textToVersion = toVersion->item(0)->getTextContent();
		componentDependencyDetail.iToVersion = std::wstring(textToVersion);

		componentDependency.iComponentDependencyList.push_back(componentDependencyDetail);
		
		}
	
	// attribute - location
	DOMNamedNodeMap* attributes = aDOMElement->getAttributes();
	DOMNode* dependentId = attributes->getNamedItem(tagDependentId.get());
	componentDependency.iDependentId = dependentId->getTextContent();
	
	return componentDependency;
	}

XmlDetails::TScrPreProvisionDetail::TComponentFile::TFileProperty 
	CScrXmlParser::GetFileProperty( const DOMElement* aDOMElement)
	{
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
	fileProperty.iName	= textName;
		
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
					fileProperty.iValue = currentElement->getTextContent();
					}
				else if( XMLString::equals(valueElement->getTagName(), tagFilePropertyStrValue.get()))
					{
					fileProperty.iIsIntValue = false;
					fileProperty.iValue = currentElement->getTextContent();
					}
				}
			}
		}
	return fileProperty;
	}
		
XmlDetails::TScrPreProvisionDetail::TComponentDetails 
	CScrXmlParser::GetComponentDetails( const DOMElement* aDOMElement)
	{
	// tags for ComponentDetails
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagRemovable( &XMLString::release, XMLString::transcode("Removable") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagSize( &XMLString::release, XMLString::transcode("Size") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagScomoState( &XMLString::release, XMLString::transcode("ScomoState") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagGlobalId( &XMLString::release, XMLString::transcode("GlobalId") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagVersion( &XMLString::release, XMLString::transcode("Version") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagOriginVerified( &XMLString::release, XMLString::transcode("OriginVerified") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagHidden( &XMLString::release, XMLString::transcode("Hidden") );
	
	XmlDetails::TScrPreProvisionDetail::TComponentDetails componentDetails;
	
	DOMNodeList* removable	= aDOMElement->getElementsByTagName(tagRemovable.get());
	DOMNodeList* size		= aDOMElement->getElementsByTagName(tagSize.get());
	DOMNodeList* scomoState = aDOMElement->getElementsByTagName(tagScomoState.get());
	DOMNodeList* globalId = aDOMElement->getElementsByTagName(tagGlobalId.get());
	DOMNodeList* versionDetail	= aDOMElement->getElementsByTagName(tagVersion.get());
	DOMNodeList* originVerified	= aDOMElement->getElementsByTagName(tagOriginVerified.get());
	DOMNodeList* hidden	= aDOMElement->getElementsByTagName(tagHidden.get());
	
	if( removable->getLength() != 0)
		{
		const XMLCh* textRemovable = removable->item(0)->getTextContent();
		componentDetails.iIsRemovable = Util::WideCharToInteger(textRemovable);
		}
	
	if( size->getLength() != 0)
		{
		const XMLCh* textSize = size->item(0)->getTextContent();
		componentDetails.iSize = Util::WideCharToInteger(textSize);
		}
	
	if( scomoState->getLength() != 0)
		{
		const XMLCh* textScomoState = scomoState->item(0)->getTextContent();
		componentDetails.iScomoState = Util::WideCharToInteger(textScomoState);
		}
	
	if( globalId->getLength() != 0)
		{
		const XMLCh* textGlobalId = globalId->item(0)->getTextContent();
		componentDetails.iGlobalId = textGlobalId;
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

		componentDetails.iVersion.iMajor = major->getTextContent();
		componentDetails.iVersion.iMinor = minor->getTextContent();
		componentDetails.iVersion.iBuild = build->getTextContent();
		}
	
	if( originVerified->getLength() != 0)
		{
		const XMLCh* textOriginVerified = originVerified->item(0)->getTextContent();
		componentDetails.iOriginVerified = Util::WideCharToInteger(textOriginVerified);
		}
	
	if( hidden->getLength() != 0)
		{
		const XMLCh* textHidden = hidden->item(0)->getTextContent();
		componentDetails.iIsHidden = Util::WideCharToInteger(textHidden);
		}
	
	return componentDetails;
	}

XmlDetails::TScrEnvironmentDetails::TLocalizedSoftwareTypeName 
	CScrXmlParser::GetLocalizedSoftwareTypeName(const DOMElement* aDOMElement)
	{
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagSwTypeNameLocale( &XMLString::release, XMLString::transcode("Locale") );
	fn_auto_ptr<releaseXmlChPtr, XMLCh> tagSwTypeNameValue( &XMLString::release, XMLString::transcode("Value") );
	
	DOMNamedNodeMap* attributes = aDOMElement->getAttributes();
	DOMNode* locale = attributes->getNamedItem(tagSwTypeNameLocale.get());
	DOMNode* name = attributes->getNamedItem(tagSwTypeNameValue.get());

	XmlDetails::TScrEnvironmentDetails::TLocalizedSoftwareTypeName localizedSwTypeName;

	const XMLCh* textLocale = locale->getTextContent();
	localizedSwTypeName.iLocale = Util::WideCharToInteger(textLocale);
	
	const XMLCh* textName = name->getTextContent();
	localizedSwTypeName.iName = textName;

	return localizedSwTypeName;
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
