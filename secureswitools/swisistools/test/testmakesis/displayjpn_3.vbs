Dim fso 

Set fso = CreateObject("Scripting.FileSystemObject")

if (fso.FileExists("ゎわこんァア龐龑.SIS")) then
	fso.DeleteFile "ゎわこんァア龐龑.SIS ", true
End If

if (fso.FileExists("ゎわこんァア龐龑.pkg")) then
	fso.DeleteFile "ゎわこんァア龐龑.pkg ", true
End If
if (fso.FileExists("\epoc32\winscw\c\tswi\passed.txt")) then
	fso.DeleteFile "\epoc32\winscw\c\tswi\passed.txt", true
End If

