/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include "dumpswiregistrytool.h"
#ifdef _MSC_VER
#include <new>
#include <new.h>
#endif /* _MSC_VER */

#ifdef _MSC_VER
// House-keeping to allow ::new to throw rather than return NULL in MSVC++
int throwingHandler(size_t)
	{
	throw std::bad_alloc();
	return 0;
	}
#endif /* _MSC_VER */

class DumpRegistryTool tool;

//entry point
int main(int argc,char *argv[])
	{
#ifdef _MSC_VER
	_set_new_handler (throwingHandler);	// Make ::new throw rather than return NULL
#endif /* _MSC_VER */

	if(argc < 2)
		{
		Options::DisplayUsage();
		return 0;
		}
	try
		{
		auto_ptr<Options> options(new Options(argc , argv));
		tool.Run(*options);
		}
		
	catch (Exceptions aErr)
		{
		Options::DisplayError(aErr);
		return aErr;
		}

	catch (bad_alloc&)
		{
		cout << "Error Allocating Memory " << endl ;
		}

	return 0; 
	}
 
DumpRegistryTool::DumpRegistryTool()
	:iRegistryEntryValid(false) ,
	iFile("\\0") ,
	iFileMajorVersion (0) ,
	iFileMinorVersion (0) ,
	iPkgVendorLocalizedName (0) ,
	iPkgInstallType (0) ,
	iRomBasedPkg (0) ,
	iPkgPreinstalledStatus (0) ,	
	iPkgSignedStatus (0) ,
	iPkgTrust (0) ,	 
	iNotUsed (0) ,
	iTimeStampl (0) ,
	iTimeStamph (0)
	{
	iInstallType[0] = "Installation" ;
	iInstallType[1] = "Augmentation" ;
	iInstallType[2] = "PartialUpgrade" ;
	iInstallType[3] = "PralledApp" ;
	iInstallType[4] = "PralledPatch" ;
	
	iPackageTrust[0] = "SisPackageUnsignedOrSelfSigned" ;
	iPackageTrust[50] = "SisPackageValidationFailed" ;
	iPackageTrust[100] = "SisPackageCertificateChainNoTrustAnchor" ;
	iPackageTrust[200] = "SisPackageCertificateChainValidatedToTrustAnchor" ;
	iPackageTrust[300] = "SisPackageChainValidatedToTrustAnchorOCSPTransientError" ;
	iPackageTrust[400] = "SisPackageChainValidatedToTrustAnchorAndOCSPValid" ;
	iPackageTrust[500] = "SisPackageBuiltIntoRom" ;

	iValidationStatus[0] = "Unknown" ;
	iValidationStatus[10] = "Expired" ;
	iValidationStatus[20] = "Invalid" ;
	iValidationStatus[30] = "Unsigned" ;
	iValidationStatus[40] = "Validated" ;
	iValidationStatus[50] = "ValidatedToAnchor" ;
	iValidationStatus[60] = "PackageInRom" ;

	iRevocationStatus[0] = "Unknown2" ;
	iRevocationStatus[10] = "OcspNotPerformed" ;
	iRevocationStatus[20] = "OcspRevoked" ;
	iRevocationStatus[30] = "OcspUnknown" ;
	iRevocationStatus[40] = "OcspTransient" ;
	iRevocationStatus[50] = "OcspGood" ;
	}

/**
  Calls the ListFilesInDirectory(),which provides a vector containing all the .reg files present in the 
  package directory specified,and iterates this vector one by one and calls ExtractRegistry() ,to extract the information
  from that particular .reg file.
  */

void DumpRegistryTool::Run(const Options& aOptions)
	{
	std::vector<std::string> regFiles;
	//-p option specified
	if(aOptions.IsPkgUIDSpecified())
		{
		regFiles = OSInterface::ExtractFilesFromDirectory(aOptions.PkgUID(), "reg");
		}

	//-r option specified
	if(aOptions.IsRegistryEntrySpecified())
		{
		regFiles = OSInterface::ExtractFilesFromDirectory(aOptions.RegistryFilePath(), "reg");
		}

	for(vector<string>::const_iterator regIterator = regFiles.begin(); regIterator != regFiles.end(); regIterator++)
		{
		string file = *regIterator;
		const char* fileName = file.c_str();
		bool success = ExtractRegistry(fileName , aOptions);
		if(!success)
			{
			cout << "Package Information:" << endl << endl;
			cout << "Registry Entry :" << fileName <<" IS EMPTY " << endl;
			}
		}

	//if -r option is specified and the registry entry(either pkg name,pkg uid or vendor name is not specified)
	//display usage.
	if(!iRegistryEntryValid && (aOptions.IsRegistryEntrySpecified()))
		{
		cout << "Invalid Registry Entry Specified ." << endl << endl; 
		}
	}

/**
  Extracts the particular registry contents.
  @param registry file
 */
bool DumpRegistryTool::ExtractRegistry(const char* aRegFile , const Options& aOptions)	
	{
	StreamReader reader(aRegFile);
	//check if file is empty
	bool isFileEmpty = reader.Initialise();
	if(isFileEmpty)
		{
		return false;
		}

	RegistryPackage package;
	RegistryToken token;
	token.ExtractRegistryToken(reader,package);
	ExtractPackageInformation(reader);

	unsigned int uid = package.PkgUID();
	char uidBuf[KLength];
	itoa(uid,uidBuf,KRadix);

	//if -p option is specified
	if(aOptions.IsPkgUIDSpecified())
		{
		cout << "Package Information:" << endl << endl;
		cout << "Registry File :" << aRegFile << endl << endl;
		package.DisplayPackage();
		token.DisplayRegistryToken();
		DisplayPackageInformation();
		}

	//if -r option sis specified.
	if(aOptions.IsRegistryEntrySpecified()) 
		{
		int packageUid = CompareUserAndFileSpecifiedData(aOptions.PkgUID(), uidBuf);
		int packageName = CompareUserAndFileSpecifiedData(aOptions.PkgName(), package.PackageName());
		int vendorName = CompareUserAndFileSpecifiedData(aOptions.VendorName(), package.Vendor());

		if(packageUid && packageName && vendorName)
			{
			iRegistryEntryValid = true;
			cout <<"Registry Controller Information:" << endl << endl;
			cout << "Registry Entry :" << aRegFile << endl << endl;
			package.DisplayPackage();
			unsigned int count = token.GetControllerCount();
			for(int i = 0 ; i < count ; i++)
				{
				token.ControllerInformation()->DisplayControllerInfo(i);
				}
			ExtractControllerInformation(reader);
			}
		}
	iFile.close();
	return true;
	}

/**
  Extracts the information about sis dependencies,sis embeds, sis file description,sis properties , 
  validation status and revocation status .
 */

void DumpRegistryTool::ExtractControllerInformation(StreamReader& aReader)
	{
	//list of depenedent SISX packages 
	unsigned int dependencyCount = aReader.ReadInt32(); 
	cout << "Number of Dependencies : " << dependencyCount << endl << endl;
	if(dependencyCount)
		{
		cout << "Dependencies Information : " << endl << endl;
		SISDependency* dependency = new SISDependency[dependencyCount];	
		for(int j = 0; j < dependencyCount; j++)
			{
			dependency[j].ExtractDependency(aReader);
			dependency[j].DisplayDependencyInfo();
			}
		delete [] dependency;
		}
	
	//list of embedded SISX files
	unsigned int embeddedCount = aReader.ReadInt32(); 
	cout<<"Number of Embedded Packages : " << embeddedCount << endl << endl;
	if(embeddedCount)
		{
		cout << "List Of Embedded  Packages:" << endl << endl;
		SISEmbeds* embed = new SISEmbeds[embeddedCount];	
		for(int i = 0; i < embeddedCount ; i++)
			{
			embed[i].ExtractEmbeds(aReader);
			embed[i].DisplayEmbeddedInfo();
			}
		delete [] embed;
		}
	
	//properties for the SISX package,is a key, value pair associated with a SISX package.
	unsigned int sisPropertyCount = aReader.ReadInt32();
	cout << "SISProperty Count:"<< sisPropertyCount << endl << endl;
	if(sisPropertyCount)
		{
		cout << "SIS Properties :" << endl << endl;
		SISProperty* property = new SISProperty[sisPropertyCount];
		for(int k = 0; k < sisPropertyCount; k++)
			{
			property[k].ExtractSISProperty(aReader);
			property[k].DisplaySISProperty();
			}
		delete [] property;
		}

	//information about files to be installed .
	unsigned int sisFileDescriptionCount = aReader.ReadInt32();
	cout << "Number Of File Descriptions :" << sisFileDescriptionCount << endl << endl;
	if(sisFileDescriptionCount)
		{
		cout << "SIS File Description:" << endl << endl;
		SISFileDescription* description = new SISFileDescription[sisFileDescriptionCount];	
		for(int l = 0; l < sisFileDescriptionCount; l++)
			{
			description[l].ExtractSISFileDescription(aReader);
			description[l].DisplaySISFileDescription(l);
			}
		delete [] description;
		}
	
	unsigned int statusOfValidation = aReader.ReadInt32();
	if(tool.iValidationStatus.find(statusOfValidation) !=tool.iValidationStatus.end())
		{
		cout<<"Validation Status : "<< iValidationStatus[statusOfValidation] << endl;
		}
	else
		{
		cout << "Validation Status : " << "Unknown" << endl;
		}

	unsigned int revocationStatus = aReader.ReadInt32();
	if(tool.iRevocationStatus.find(revocationStatus) != tool.iRevocationStatus.end())
		{
		cout << "Revocation Status : " << iRevocationStatus[revocationStatus] << endl;
		}
	else
		{
		cout << "Revocation Status : " << "Unknown" << endl;
		}

	}

//this function is to check whether the user specified values for registry entries matches the one specified in the .reg file.
int DumpRegistryTool::CompareUserAndFileSpecifiedData(const string& aUserSpecified , const string& aFileSpecified)
	{
	string::const_iterator userIter = aUserSpecified.begin();
	string::const_iterator fileIter = aFileSpecified.begin();

	int userSize = aUserSpecified.length();
	int size = aFileSpecified.length();

	//stop when either string end has reached
	while((userIter != aUserSpecified.end()) && (fileIter != aFileSpecified.end()))
		{
		if(::toupper(*userIter) != ::toupper(*fileIter))
			{
			return 0;
			}
		++userIter;
		++fileIter;
		}

	if(userSize == size)
		{
		return 1;
		}
	else
		{
		return 0;
		}
	}


/**
  This function extracts top level information about a package namely install type,whether it is rom based,trust status of package
 */
void DumpRegistryTool::ExtractPackageInformation(StreamReader& aReader)
	{
	iFileMajorVersion = aReader.ReadInt16();
	iFileMinorVersion = aReader.ReadInt16();
	iPkgVendorLocalizedName = aReader.ReadDescriptor();
	iPkgInstallType = aReader.ReadInt32(); 
	iRomBasedPkg = aReader.ReadInt32();				//is package ROM Based
	iPkgPreinstalledStatus = aReader.ReadInt32();	//is package preinstalled
	iPkgSignedStatus = aReader.ReadInt32();			//is package signed   
	iPkgTrust = aReader.ReadInt32();				//trust status of package 
	iNotUsed = aReader.ReadInt32(); 
	iTimeStampl = aReader.ReadInt32();
	iTimeStamph = aReader.ReadInt32();
	}


void DumpRegistryTool::DisplayPackageInformation()
	{
	cout << "File Major Version :" << iFileMajorVersion << endl;
	cout << "File Minor Version :" << iFileMinorVersion << endl;
	cout << "Package Vendor Localized Name :" << iPkgVendorLocalizedName << endl;
	if(tool.iInstallType.find(iPkgInstallType) != tool.iInstallType.end())
		{
		cout << "Package Install Type :" << tool.iInstallType[iPkgInstallType] << endl;
		}
	else
		{
		cout << "Package Install Type :" << "Unknown" << endl;
		}

	if(iRomBasedPkg)
		{
		cout << "Is Package in ROM :" << "Yes" << endl;
		}
	else
		{
		cout<<"Is Package in ROM :"<< "No" <<endl;
		}

		
	if(iPkgPreinstalledStatus)
		{
		cout << "Package Preinstalled :" << "Yes" << endl;
		}
	else
		{
		cout << "Package Preinstalled :" << "No" << endl;
		}
		
	if(iPkgSignedStatus)
		{
		cout << "Package Signed :" << "Yes" << endl;
		}
	else
		{
		cout << "Is Package Signed :" << "No" << endl;
		}
		
	if(tool.iPackageTrust.find(iPkgTrust) !=tool.iPackageTrust.end())
		{
		cout << "Package Trust :" << tool.iPackageTrust[iPkgTrust] << endl;
		}
	else
		{
		cout << "Package Trust :" << "Unknown" << endl << endl;
		}
	}


DumpRegistryTool::~DumpRegistryTool()
	{
	delete [] iPkgVendorLocalizedName;
	}
 



