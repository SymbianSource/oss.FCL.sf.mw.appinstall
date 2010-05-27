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

#include "xmlgenerator.h"
#include "../common/exception.h"
#include "is_utils.h"

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

// constants used for writing to the xml file
std::wstring	PreProvisionInformation(L"PreProvisionInformation");
std::wstring	SoftwareTypeName(L"SoftwareTypeName");


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

		std::auto_ptr < XMLFormatTarget > outputFile( new LocalFileFormatTarget( aXmlFileName.c_str() ) );	
		
		const char* epocRoot = getenv("EPOCROOT");
		if(NULL == epocRoot)
			{
			throw std::runtime_error("EPOCROOT environment variable not specified.");
			}
		
		std::string epocRootStr(epocRoot); 

		std::string dtdFilePath = epocRootStr + "epoc32\\tools\\preprovision.dtd";
		
		fn_auto_ptr<releaseXmlChPtr, XMLCh> dtdPath( &XMLString::release, XMLString::transcode(dtdFilePath.c_str()) );
		DOMDocumentType* documentType = domImpl->createDocumentType(L"PreProvisionInformation",NULL, dtdPath.get());
		
		
		mem_fn_auto_ptr< releaseDOMDoc, DOMDocument* > domDocument( &XERCES_CPP_NAMESPACE::DOMDocument::release, domImpl->createDocument(	0, PreProvisionInformation.c_str(), documentType) );

		DOMElement* rootElement = domDocument->getDocumentElement();
		
		// SoftwareTypeName
		AddChildElement(rootElement, domDocument.get(), SoftwareTypeName.c_str(), aPreProvisionDetail.iSoftwareTypeName.c_str() );

		std::vector<XmlDetails::TScrPreProvisionDetail::TComponent>::const_iterator compIter;
		for(compIter = aPreProvisionDetail.iComponents.begin(); compIter != aPreProvisionDetail.iComponents.end() ; ++compIter)
			{
			DOMElement*  component = domDocument->createElement(L"Component");
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
	
	aDomWriter->setEncoding(L"UTF-16");
	}

void CXmlGenerator::WriteComponent( DOMElement* aRootElement, DOMDocument* aDocument, 
										const XmlDetails::TScrPreProvisionDetail::TComponent& aComponent, 
										int& aRomApplication
										)
	{

	if (aRomApplication) 
	{
		int isRomApplication = 1;
		std::wstring isRomApp = Utils::IntegerToWideString(isRomApplication);
		AddChildElement(aRootElement, aDocument, L"RomApplication", isRomApp.c_str());
	}
	
	std::wstring isRemovable = Utils::IntegerToWideString(aComponent.iComponentDetails.iIsRemovable);
	AddChildElement(aRootElement, aDocument, L"Removable", isRemovable.c_str() );
	
	std::wstring size = Utils::Int64ToWideString(aComponent.iComponentDetails.iSize);
	AddChildElement(aRootElement, aDocument, L"Size", size.c_str() );
	
	std::wstring scomoState = Utils::IntegerToWideString(aComponent.iComponentDetails.iScomoState);
	AddChildElement(aRootElement, aDocument, L"ScomoState", scomoState.c_str() );

	AddChildElement(aRootElement, aDocument, L"GlobalId", aComponent.iComponentDetails.iGlobalId.c_str() );

	WriteComponentVersion(aRootElement, aDocument, aComponent.iComponentDetails.iVersion);
	
	std::wstring isOriginVerified = Utils::IntegerToWideString(aComponent.iComponentDetails.iOriginVerified);
	AddChildElement(aRootElement, aDocument, L"OriginVerified", isOriginVerified.c_str() );

	std::wstring isHidden = Utils::IntegerToWideString(aComponent.iComponentDetails.iIsHidden);
	AddChildElement(aRootElement, aDocument, L"Hidden", isHidden.c_str() );

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
	DOMElement*  version = aDocument->createElement(L"Version");
	aRootElement->appendChild(version);
	version->setAttribute( L"Major", aVersion.iMajor.c_str() );
	version->setAttribute( L"Minor", aVersion.iMinor.c_str() );
	version->setAttribute( L"Build", aVersion.iBuild.c_str() );
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
		DOMElement* newRoot = AddTag(aRootElement, aDocument, L"ComponentLocalizable");
		std::wstring locale = Utils::IntegerToWideString(compLocIter->iLocale);
		AddChildElement(newRoot,aDocument, L"ComponentLocalizable_Locale", locale.c_str());
		AddChildElement(newRoot,aDocument, L"ComponentLocalizable_Name", compLocIter->iName.c_str());
		AddChildElement(newRoot,aDocument, L"ComponentLocalizable_Vendor", compLocIter->iVendor.c_str());
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
		
		DOMElement* compPropRoot = AddTag(aRootElement, aDocument, L"ComponentProperty");
		
		std::wstring locale = Utils::IntegerToWideString(compPropIter->iLocale);
		AddChildElement(compPropRoot,aDocument, L"ComponentProperty_Locale", locale.c_str());
		
		DOMElement* compPropValueRoot = AddTag(compPropRoot, aDocument, L"ComponentProperty_Value");
		
		std::wstring isBinary = Utils::IntegerToWideString(compPropIter->iIsStr8Bit);

		if(compPropIter->iIsIntValue)
			{
			AddChildElement(compPropValueRoot,aDocument, L"ComponentProperty_IntValue", compPropIter->iValue.c_str());
			}
		else
			{
			AddChildElement(compPropValueRoot,aDocument, L"ComponentProperty_StrValue", compPropIter->iValue.c_str());
			}
		
		
		AddChildElement(compPropRoot,aDocument, L"ComponentProperty_IsBinary", isBinary.c_str());

		compPropRoot->setAttribute(L"Name", compPropIter->iName.c_str());
		
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
		DOMElement* compFileRoot = AddTag(aRootElement, aDocument, L"ApplicationRegistrationInfo");

		WriteAppAttributes(compFileRoot, aDocument, compFileIter->iApplicationAttribute);

		const std::vector<AppOpaqueDataType>& aAppOpaqueDataType = compFileIter->iOpaqueDataType;
		std::vector<AppOpaqueDataType>::const_iterator fileAppOpaqueDataType;

		for( fileAppOpaqueDataType = aAppOpaqueDataType.begin() ; fileAppOpaqueDataType != aAppOpaqueDataType.end() ; ++fileAppOpaqueDataType)
		{
			if(0 == fileAppOpaqueDataType->iServiceUid)
			{
				DOMElement* filePropValueRoot = AddTag(compFileRoot, aDocument, L"OpaqueData");
				AddChildElement(filePropValueRoot,aDocument, L"Data", fileAppOpaqueDataType->iOpaqueData.c_str());
				std::wstring iLocale = Utils::IntegerToWideString(fileAppOpaqueDataType->iLocale);
				AddChildElement(filePropValueRoot,aDocument, L"OpaqueLocale", iLocale.c_str());
			}
		}

		for(vector<std::wstring>::iterator mimeiter = compFileIter->iFileOwnershipInfo.begin() ; mimeiter != compFileIter->iFileOwnershipInfo.end() ; ++mimeiter )
		{
			DOMElement* filePropRoot = AddTag(compFileRoot, aDocument, L"FileOwnershipinfo");

			AddChildElement(filePropRoot,aDocument, L"FileName",  mimeiter->c_str());
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
		DOMElement* filePropRoot = AddTag(aRootElement, aDocument, L"ApplicationLocalizableInfo");

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
		DOMElement* filePropRoot = AddTag(aRootElement, aDocument, L"LocalizableAttribute");
		DOMElement* filePropValueRoot = AddTag(filePropRoot, aDocument, L"LocalizableAttribute_Value");

		if(filePropIter->iIsIntValue)
		{
			AddChildElement(filePropValueRoot, aDocument, L"LocalizableAttribute_IntValue", filePropIter->iValue.c_str());
		}
		else
		{
			AddChildElement(filePropValueRoot, aDocument, L"LocalizableAttribute_StrValue", filePropIter->iValue.c_str());
		}
		filePropRoot->setAttribute(L"Name", filePropIter->iName.c_str());
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
		 DOMElement* filePropRoot = AddTag(aRootElement, aDocument, L"ViewData");
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
		 DOMElement* filePropRoot = AddTag(aRootElement, aDocument, L"ViewDataAttribute");
		 DOMElement* filePropValueRoot = AddTag(filePropRoot, aDocument, L"ViewData_Value");
 
		 if(filePropIter->iIsIntValue)
		 {
			 AddChildElement(filePropValueRoot, aDocument, L"ViewData_IntValue", filePropIter->iValue.c_str());
		 }
		 else
		 {
			 AddChildElement(filePropValueRoot, aDocument, L"ViewData_StrValue", filePropIter->iValue.c_str());
		 }
		 filePropRoot->setAttribute(L"Name", filePropIter->iName.c_str());
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
		DOMElement* filePropRoot = AddTag(aRootElement, aDocument, L"ApplicationServiceInfo");
	
		std::wstring iUid = Utils::IntegerToWideString(filePropIter->iUid);
		AddChildElement(filePropRoot,aDocument, L"Uid", iUid.c_str());

		for( fileAppPropIter = aAppOpaqueData.begin() ; fileAppPropIter != aAppOpaqueData.end() ; ++fileAppPropIter)
		{
			if(filePropIter->iUid == fileAppPropIter->iServiceUid)
			{
				if(!fileAppPropIter->iOpaqueData.empty())
				{
					DOMElement* filePropValueRoot = AddTag(filePropRoot, aDocument, L"ServiceOpaqueData");
					AddChildElement(filePropValueRoot,aDocument, L"ServiceData", fileAppPropIter->iOpaqueData.c_str());
					std::wstring iLocale = Utils::IntegerToWideString(fileAppPropIter->iLocale);
					AddChildElement(filePropValueRoot,aDocument, L"ServiceOpaqueLocale", iLocale.c_str());
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
		DOMElement* filePropRoot = AddTag(aRootElement, aDocument, L"ApplicationDataType");

		std::wstring iPriority = Utils::IntegerToWideString(filePropIter->iPriority);
		AddChildElement(filePropRoot,aDocument, L"Priority", iPriority.c_str());
		AddChildElement(filePropRoot, aDocument, L"Type", filePropIter->iType.c_str());
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
		DOMElement* filePropRoot = AddTag(aRootElement, aDocument, L"ApplicationAttribute");
		DOMElement* filePropValueRoot = AddTag(filePropRoot, aDocument, L"ApplicationAttribute_Value");

		if(filePropIter->iIsIntValue)
		{
			AddChildElement(filePropValueRoot, aDocument, L"ApplicationAttribute_IntValue", filePropIter->iValue.c_str());
		}
		else
		{
			AddChildElement(filePropValueRoot, aDocument, L"ApplicationAttribute_StrValue", filePropIter->iValue.c_str());
		}
		filePropRoot->setAttribute(L"Name", filePropIter->iName.c_str());
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
		DOMElement* filePropRoot = AddTag(aRootElement, aDocument, L"ApplicationProperty");


		std::wstring iLocale = Utils::IntegerToWideString(filePropIter->iLocale);
		AddChildElement(filePropRoot,aDocument, L"Locale", iLocale.c_str());

		AddChildElement(filePropRoot,aDocument, L"Name", filePropIter->iName.c_str());

		std::wstring iServiceUid = Utils::IntegerToWideString(filePropIter->iServiceUid);
		AddChildElement(filePropRoot,aDocument, L"ServiceUid", iServiceUid.c_str());

		std::wstring iIntValue = Utils::IntegerToWideString(filePropIter->iIntValue);
		AddChildElement(filePropRoot,aDocument, L"IntValue", iIntValue.c_str());

		AddChildElement(filePropRoot, aDocument, L"StrValue", filePropIter->iStrValue.c_str());

		std::wstring iIsStr8Bit = Utils::IntegerToWideString(filePropIter->iIsStr8Bit);
		AddChildElement(filePropRoot,aDocument, L"IsStr8Bit", iIsStr8Bit.c_str());

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
		DOMElement* compFileRoot = AddTag(aRootElement, aDocument, L"ComponentFile");

		WriteFileProperties(compFileRoot, aDocument, compFileIter->iFileProperties);
		
		compFileRoot->setAttribute(L"Location", compFileIter->iLocation.c_str());
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
		DOMElement* filePropRoot = AddTag(aRootElement, aDocument, L"FileProperty");
		DOMElement* filePropValueRoot = AddTag(filePropRoot, aDocument, L"FileProperty_Value");

		if(filePropIter->iIsIntValue)
			{
			AddChildElement(filePropValueRoot, aDocument, L"FileProperty_IntValue", filePropIter->iValue.c_str());
			}
		else
			{
			AddChildElement(filePropValueRoot, aDocument, L"FileProperty_StrValue", filePropIter->iValue.c_str());
			}
		filePropRoot->setAttribute(L"Name", filePropIter->iName.c_str());
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

	DOMElement* compDepRoot = AddTag(aRootElement, aDocument, L"ComponentDependency");

	for( compDepIter = componentDependencyList.begin() ; compDepIter != componentDependencyList.end() ; ++compDepIter)
		{
		DOMElement* compDepListRoot = AddTag( compDepRoot, aDocument, L"DependencyList" );
		AddChildElement( compDepListRoot, aDocument, L"SupplierId", compDepIter->iSupplierId.c_str() );
		AddChildElement( compDepListRoot, aDocument, L"FromVersion", compDepIter->iFromVersion.c_str() );
		AddChildElement( compDepListRoot, aDocument, L"ToVersion", compDepIter->iToVersion.c_str() );
		}
	compDepRoot->setAttribute(L"DependentId", aComponentDependency.iDependentId.c_str());
	}


void CXmlGenerator::AddChildElement( DOMElement* aRootElement, DOMDocument* aDocument, const wchar_t* aElementName, const wchar_t* aTextValue )
	{
	DOMElement*  element = aDocument->createElement(aElementName);
	aRootElement->appendChild(element);

	DOMText* textValue = aDocument->createTextNode(aTextValue);
	element->appendChild(textValue);
	}

DOMElement* CXmlGenerator::AddTag( DOMElement* aRootElement, DOMDocument* aDocument, const wchar_t* aTagName)
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
