Testcase 1
==========
Update the writable SWI certstore

Test step 1a
============
Deliver a corrupt certstore.

Test step precondition
======================
ROM SWI certstore contains a certificate with TCB assigned capability
upon which the updater SIS package depends.

No writable SWI certstore exists

Test step postcondition 
=======================
C:\Resource\SwiCertstore\dat\00000001 exists.
It does not form part of the SWI certstore.

Test Results
============
Update to writable SWI certstore completes without error.
The Swicertstore.dll handles the corrupt update gracefully.


Test step 1b 
============
Deliver a new certstore.

Test step precondition
======================
ROM SWI certstore contains a certificate with TCB assigned capability
upon which the updater SIS package depends.

A corrupt writable SWI certstore exists.

Dependency on Test case 1. Test step 1a.

Test step postcondition 
=======================
C:\Resource\SwiCertstore\dat\00000001 does not exist.
C:\Resource\SwiCertstore\dat\00000002 exists.  It forms part of the
SWI certstore.

Test Results
============
The Swicertstore.dll reverts to the certstore in ROM for the purposes
of installing the updater.  Update to writable SWI certstore completes
without error.


Test step 2a 
============
Deliver an empty certstore.

Test step precondition
======================
ROM SWI certstore contains a certificate with TCB assigned capability
upon which the updater SIS package depends.

No writable SWI certstore exists

Test step postcondition 
=======================
C:\Resource\SwiCertstore\dat\00000001 exists.
It forms part of the SWI certstore.

Test Results
============
Update to writable SWI certstore completes without error.
The Swicertstore.dll handles the empty update gracefully.


Test step 2b 
============
Deliver a new certstore.

Test step precondition
======================
ROM SWI certstore contains a certificate with TCB assigned capability
that contains a key with which updater SIS package has been signed.

An empty writable SWI certstore exists.

Dependency on Test case 2. Test step 2a.

Test step postcondition 
=======================
C:\Resource\SwiCertstore\dat\00000001 does not exist.
C:\Resource\SwiCertstore\dat\00000002 exists.
00000002 forms part of the unified certstore.

Test Results
============
The Swicertstore.dll reverts to the certstore in ROM for the purposes
of installing the updater.  Update to writable SWI certstore completes
without error.




Testcase 2
==========
Attempt to overwrite the writable SWI certstore. 

Test step 1
===========
Attempt to overwrite C:\Resource\SwiCertstore\dat\00000001 by
delivering a text file in a signed SIS file

Test step precondition
======================
ROM SWI certstore contains a certificate upon which SIS package
depends.

C:\Resource\SwiCertstore\dat\00000001 does not exist

Test step postcondition 
=======================
C:\Resource\SwiCertstore\dat\00000001 does not exist

Test Results
============
Installation of the SIS file package fails to complete.


Test step 2
===========
Attempt to overwrite C:\Resource\SwiCertstore\tmp\afile by
delivering a text file in a signed SIS file

Test step precondition
======================
ROM SWI certstore contains a certificate upon which the SIS package
depends.

C:\Resource\SwiCertstore\tmp\afile does not exist

Test step postcondition
=======================
C:\Resource\SwiCertstore\tmp\afile does not exist

Test Results
============
Installation of the SIS file package fails to complete.


Test step 3
===========
Attempt to overwrite C:\Resource\SwiCertstore\afile by
delivering a text file in a signed SIS file

Test step precondition
======================
ROM SWI certstore contains a certificate upon which the SIS package
depends.

C:\Resource\SwiCertstore\afile does not exist

Test step postcondition
=======================
C:\Resource\SwiCertstore\afile does not exist

Test Results
============
Installation of the SIS file package fails to complete.




Testcase 3
==========
Attempt to remove the writable SWI certstore.

Test step 1
===========
Attempt to remove C:\Resource\SwiCertstore\dat\00000001 by
removing a text file when uninstalling a signed SIS file
using the FN (File Null) .pkg install option

Test step precondition
======================
ROM SWI certstore contains a certificate upon which the SIS package
depends.

C:\Resource\SwiCertstore\dat\afile exists

Test step postcondition
=======================
C:\Resource\SwiCertstore\dat\afile has not been removed.

Test Results
============
Installation of the SIS file package fails to complete.


Test step 2
===========
Attempt to remove C:\Resource\SwiCertstore\tmp\afile by
removing a text file when uninstalling a signed SIS file
using the FN (File Null) .pkg install option

Test step precondition
======================
ROM SWI certstore contains a certificate upon which the SIS package
depends.

C:\Resource\SwiCertstore\tmp\afile exists

Test step postcondition
=======================
C:\Resource\SwiCertstore\tmp\afile has not been removed.

Test Results
============
Installation of the SIS file package fails to complete.


Test step 3
===========
Attempt to remove C:\Resource\SwiCertstore\afile by
removing a text file when uninstalling a signed SIS file
using the FN (File Null) .pkg install option

Test step precondition
======================
ROM SWI certstore contains a certificate upon which the SIS package
depends.

C:\Resource\SwiCertstore\afile exists

Test step postcondition
=======================
C:\Resource\SwiCertstore\afile exists and has not been removed.

Test Results
============
Installation of the SIS file package fails to complete.


Test step 4
===========
Attempt to remove C:\Resource\SwiCertstore\dat\ by
removing a directory when uninstalling a signed SIS file
using the FN (File Null) .pkg install option

Test step precondition
======================
ROM SWI certstore contains a certificate upon which the SIS package
depends.

C:\Resource\SwiCertstore\dat\ exists

Test step postcondition
=======================
C:\Resource\SwiCertstore\dat\ exists and has not been removed.

Test Results
============
Installation of the SIS file package fails to complete.


Test step 5
===========
Attempt to remove C:\Resource\SwiCertstore\tmp\ by
removing a directory when uninstalling a signed SIS file
using the FN (File Null) .pkg install option

Test step precondition
======================
ROM SWI certstore contains a certificate upon which the SIS package
depends.

C:\Resource\SwiCertstore\tmp\ exists

Test step postcondition
=======================
C:\Resource\SwiCertstore\tmp\ exists and has not been removed.

Test Results
============
Installation of the SIS file package fails to complete.


Test step 6
===========
Attempt to remove C:\Resource\SwiCertstore\ by
removing a directory when uninstalling a signed SIS file
using the FN (File Null) .pkg install option

Test step precondition
======================
ROM SWI certstore contains a certificate upon which the SIS package
depends.

C:\Resource\SwiCertstore\ exists

Test step postcondition
=======================
C:\Resource\SwiCertstore\ exists and has not been removed.

Test Results
============
Installation of the SIS file package fails to complete.


Test step 7
===========
Attempt to remove C:\Resource\SwiCertstore\* by
removing a directory when uninstalling a signed SIS file
using the FN (File Null) .pkg install option

Test step precondition
======================
ROM SWI certstore contains a certificate upon which the SIS package
depends.

C:\Resource\SwiCertstore\ exists

Test step postcondition
=======================
C:\Resource\SwiCertstore\ exists and has not been removed.

Test Results
============
Installation of the SIS file package fails to complete.





Testcase 4
==========
Ensure that capabilities assigned to certificates in the writable
certstore are enforced on sis file installation.


Test step 1
===========
Install SIS package which depends updon capabilities assigned to a
certificate in the writable SWI certstore.

Test step precondition
======================
Writable SWI certstore contains a certificate with an assigned
capability upon which a SIS package to be installed depends.

There is no certificate in the ROM certstore with the required
capability.

Test step postcondition
=======================
The SIS package has been installed

Test Results
============
The SIS package has been installed




Testcase 5
==========
Ensure that uninstallation of a SIS package works in the event that
the certificate upon which it depends has been removed from the
writabel SWI certstore.

Test step 1
===========
Install package that depends on certificate in the writable certstore.
Remove certificate and uninstall the package.

Test step precondition
======================
The SWI certstore contains a certificate upon which the SIS package to
be installed depends.

Test step postcondition
=======================
The certificate in the writable SWI certstore has been removed.  The
SIS package has been uninstalled.

Test Results
============
The SIS package has been uninstalled





Testcase 6
==========
Ensure that that a SIS file signed with a certificate in one of the
SWI certstores can be installed in the event that the certificate is
duplicated in the other SWI certstore.


Test step 1
===========
Install SIS package which has been signed with a certificate
duplicated in both the SWI certstores.

Test step precondition
======================
Both SWI certstores contain the certificate upon which the SIS package
to be installed depends.

Test step postcondition
=======================
The SIS package has been installed

Test Results
============
The SIS package has been installed




Testcase 7
==========
Ensure that a SIS file that has been signed multiple times can be
installed.


Test step 1
===========
Install SIS package which depends on certificates that reside in the
writable SWI certstore and the ROM certstore.

Test step precondition
======================
The writable SWI certstore and the ROM certstore each contain a
certificate upon which the SIS package to be installed depends.

These certificates do not form a chain and are not duplicates.

Test step postcondition
=======================
The SIS package has been installed

Test Results
============
The SIS package has been installed




Testcase 8
==========
Ensure that a SIS file signed with a certificate installed in the ROM
certstore can be installed in the presence of the writable SWI
certstore.


Test step 1
===========
Install SIS package which has been signed with a certificate in the
ROM certstore.

Test step precondition
======================
The ROM certstore contains a certificate upon which the SIS package to
be installed depends.  The writable SWI certstore does not contain a
certificate upon which the package depends.

Test step postcondition
=======================
The SIS package has been installed

Test Results
============
The SIS package has been installed



Testcase 9
==========
Ensure that a SIS file signed with a mandatory certificate in the
writable SWI certstore can be installed.


Test step 1
===========
Install SIS package which has been signed with a mandatory certificate
in the writable SWI certstore.

Test step precondition
======================
The writable SWI certstore contains a mandatory certificate upon which
the SIS package to be installed depends.  The ROM certstore does not.

Test step postcondition
=======================
The SIS package has been installed

Test Results
============
The SIS package has been installed





Testcase 10
===========
Ensure correct behavior of Swi certstore in the event that a newer
version of a certificate in the ROM certstore is delivered to the
writable certstore.  Certificate versions will be distinguished by
changing the version number and certificate expiry date.


Test step 1
===========
Install package that depends on a certificate version in the writable
certstore that has expired.

Test step precondition
======================
Both SWI certstores contain a certificate upon which the SIS package
to be installed depends.  The certificate version in the writable
certstore has expired.

Test step postcondition
=======================
The SIS package has not been installed

Test Results
============
The SIS package has not been installed


Test step 2
===========
Install package that depends on a certificate version in the writable
certstore where the version in the ROM certstore has expired.


Test step precondition
======================
Both SWI certstores contain a certificate upon which the SIS package
to be installed depends.  The certifacte version in the ROM certstore
has expired, the version in the writable SWI certstore is valid.

Test step postcondition
=======================
The SIS package has been installed

Test Results
============
The SIS package has been installed




Testcase 11
===========
Ensure that the writable SWI certstore takes part in certificate chain
validation


Test step 1
===========
Install package that depends on a root certificate in the writable
certstore.

Test step precondition
======================
File certstore contains an intermediate certificate that forms part of
chain rooted in the writable swi certstore.

Test step postcondition
=======================
The SIS package has been installed

Test Results
============
The SIS package has been installed




=======================================================================
Other potential test steps for test case 10
=======================================================================

Test with various lengths of certificate chains with some intermediate
certificates not in swicertstore.

Check that if intermediate cert is not supplied in packge that install
still works.  

Test what happens if an intermediate certificate is removed.

=======================================================================



 

