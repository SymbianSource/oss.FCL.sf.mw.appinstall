DEF119363 Add signsis sanity checks 
===================================
In order to re-create eshell_hybrid.sis make use of the commands below. 
(The idea is that the actual exe in the sis file is substituted with a malicious one. testsignsis.pl then tests that this error is picked up by performing a check of the file hashes involved.)

m:\epoc32\tools\makesis -Xx eshell_readuserdata.pkg eshell_readuserdata.sis

hexdump eshell_readuserdata.sis > eshell_readuserdata_pkg.hex



m:\epoc32\tools\makesis -Xx eshell_caps.pkg eshell_caps.sis

hexdump eshell_caps.sis > eshell_caps_pkg.hex



hexdump eshell_readuserdata.exe > eshell_readuserdata.hex

hexdump eshell_caps.exe > eshell_caps.hex