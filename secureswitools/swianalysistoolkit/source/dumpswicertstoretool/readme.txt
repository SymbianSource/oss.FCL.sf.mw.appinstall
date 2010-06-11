DUMPSWICERTSTORETOOL
====================

DumpSWICertstoreTool is a tool capable of extacting information from one or more SWICertstore files containing
the root certificates used by software install.  


USAGE
------

The command line syntax is:

DumpSWICertstoretool  [-v] [-h] [-c] [-y] [ DatFile] [ Directory] 

where:

DatFile -  ROM Based swicertstore.dat located in  Z:\resource\swicertstore.dat(path of the dat file)

DirectoryPath - Refers to WritableCertstore or C:-Based  certstore located in c:\resource\swicertstore\dat\

-h | --help
Print this (help) message

-v | --version
version of DumpSWICertstore tool

-c | -- certificate
Extracts the certificates.

-y |--hidden option which the certificates in base64 encrypted format along with the capabilities and their mandatory status on the console.



Example Usage:
--------------


1. Command Line Input : When ROM Based dat file is specified.
   -------------------

dumswicertstoretool \epoc32\release\winscw\udeb\z\resource\swicertstore.dat

    Output:
   --------

 B:\epoc32\release\winscw\udeb\z\resource\swicertstore.dat:ROM Based SwiCertstore



[StatCA]

file=StatCA.der
capability= TCB
capability= CommDD
capability= PowerMgmt
capability= MultimediaDD
capability= ReadDeviceData
capability= WriteDeviceData
capability= DRM
capability= TrustedUI
capability= ProtServ
capability= DiskAdmin
capability= NetworkControl
capability= AllFiles
capability= SwEvent
capability= NetworkServices
capability= LocalServices
capability= ReadUserData
capability= WriteUserData
capability= Location
capability= SurroundingsDD
capability= UserEnvironment
application=SWInstall
application=SWInstallOCSP
Mandatory=0


[Root5CA]

file=Root5CA.der
capability= TCB
capability= ReadDeviceData
capability= WriteDeviceData
capability= DRM
capability= AllFiles
application=SWInstall
Mandatory=0


[SymbianTestDSACA]

file=SymbianTestDSACA.der
capability= NetworkServices
capability= LocalServices
capability= Location
application=SWInstall
application=SWInstallOCSP
Mandatory=0


[SymbianTestRSACA]

file=SymbianTestRSACA.der
capability= ReadUserData
capability= WriteUserData
application=SWInstall
application=SWInstallOCSP
Mandatory=0

Press any key to continue



2.  Command Line Input : When C:\Based dat file is specified.
    -------------------


  dumswicertstoretool \epoc32\winscw\c\reource\swicertstore\dat

 	Output:
   	-------


\epoc32\winscw\c\reource\swicertstore\dat : C:\ Based SwiCertstore


[SymbianTestRootCADiffSerial]

file=SymbianTestRootCADiffSerial.der
capability= ReadDeviceData
capability= WriteDeviceData
capability= DRM
capability= AllFiles
application=SWInstall
Mandatory=1


[Root5CA]

file=Root5CA.der
capability= ReadDeviceData
capability= WriteDeviceData
capability= DRM
capability= AllFiles
application=SWInstall
Mandatory=1


[SymbianTestRSACA]

file=SymbianTestRSACA.der
capability= TCB
capability= CommDD
application=SWInstall
Mandatory=0

Press any key to continue


3.  Command Line Input : When both C:\Based and Rom Based certstore are specified .
    -------------------


  dumswicertstoretool \epoc32\release\winscw\udeb\z\resource\swicertstore.dat \epoc32\winscw\c\reource\swicertstore\dat

 	Output:
   	-------

 Merged ROM Based and C:BASED Writable Certstore.(Shows which certificate belong to writable and ROM certstore respectively).


\epoc32\winscw\c\reource\swicertstore\dat :

[SymbianTestRootCADiffSerial]

file=SymbianTestRootCADiffSerial.der
capability= ReadDeviceData
capability= WriteDeviceData
capability= DRM
capability= AllFiles
application=SWInstall
Mandatory=1


[Root5CA]

file=Root5CA.der
capability= ReadDeviceData
capability= WriteDeviceData
capability= DRM
capability= AllFiles
application=SWInstall
Mandatory=1


[SymbianTestRSACA]

file=SymbianTestRSACA.der
capability= TCB
capability= CommDD
application=SWInstall
Mandatory=0


\epoc32\release\winscw\udeb\z\resource\swicertstore.dat :

[StatCA]

file=StatCA.der
capability= TCB
capability= CommDD
capability= PowerMgmt
capability= MultimediaDD
capability= ReadDeviceData
capability= WriteDeviceData
capability= DRM
capability= TrustedUI
capability= ProtServ
capability= DiskAdmin
capability= NetworkControl
capability= AllFiles
capability= SwEvent
capability= NetworkServices
capability= LocalServices
capability= ReadUserData
capability= WriteUserData
capability= Location
capability= SurroundingsDD
capability= UserEnvironment
application=SWInstall
application=SWInstallOCSP
Mandatory=0


[SymbianTestDSACA]

file=SymbianTestDSACA.der
capability= NetworkServices
capability= LocalServices
capability= Location
application=SWInstall
application=SWInstallOCSP
Mandatory=0

Press any key to continue












