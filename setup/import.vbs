' VBScript source code

On Error resume next

If WScript.Arguments.Count <> 1 Then
	WScript.Echo "Usage import.vbs xxx.trans.dat"
	WScript.Quit 1
End If

scriptdir = Mid(WScript.ScriptFullName, 1, Len(Wscript.ScriptFullName) - Len(Wscript.ScriptName ))

' set up registry
'
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

clsid = "{36353903-2137-43FD-9AD6-40B65A96A839}"
''clsid = "{F4360876-E70B-4f23-B9BF-A6EBC1F20031}"
classes = "HKEY_CURRENT_USER\Software\Classes\"
doc = "Testing.WPhast.Document"
exepath = scriptdir & "..\src\AutoRelease\WPhast.exe"

Set Fso = CreateObject("Scripting.FileSystemObject")
exepath  = Fso.GetAbsolutePathName(exepath)

If (Not Fso.FileExists(exepath)) Then
	WScript.Echo "Can't find " & exepath & "."
	WScript.Quit 1
End If

Set Sh = CreateObject("WScript.Shell")

'''bCreateDoc = false
'''Sh.RegRead(classes & doc & "\CLSID\")
'''If Err <> 0 Then
'''  bCreateDoc = true
'''End if
bCreateDoc = true

If bCreateDoc Then
	' [HKEY_CURRENT_USER\Software\Classes\Testing.WPhast.Document]
	' @="Testing.WPhast.Document"
	'
	Sh.RegWrite classes & doc & "\", doc
End If

'''bCreateDocCLSID = false
'''Sh.RegRead(classes & doc & "\CLSID\")
'''If Err <> 0 Then
'''  bCreateDocCLSID = true
'''End if
bCreateDocCLSID = true

If bCreateDocCLSID Then
	' [HKEY_CURRENT_USER\Software\Classes\Testing.WPhast.Document\CLSID]
	' @="{36353903-2137-43FD-9AD6-40B65A96A839}"
	'
	Sh.RegWrite classes & doc & "\CLSID\", clsid
End If


'''bCreateClassCLSID = false
'''Sh.RegRead(classes & "\CLSID\" & clsid & "\")
'''If Err <> 0 Then
'''  bCreateClassCLSID = true
'''End if
bCreateClassCLSID = true

If bCreateClassCLSID Then
	' [HKEY_CURRENT_USER\Software\Classes\CLSID\{36353903-2137-43FD-9AD6-40B65A96A839}]
	' @="Testing.WPhast.Document"
	'
	Sh.RegWrite classes & "\CLSID\" & clsid & "\", doc
End If


'''bCreateLocalServer32 = false
'''Sh.RegRead(classes & "\CLSID\" & clsid & "\LocalServer32\")
'''If Err <> 0 Then
'''  bCreateLocalServer32 = true
'''End if
bCreateLocalServer32 = true

If bCreateLocalServer32 Then
	' [HKEY_CURRENT_USER\Software\Classes\CLSID\{36353903-2137-43FD-9AD6-40B65A96A839}\LocalServer32]
	' @="<scriptdir>..\src\AutoRelease\WPhast.exe
	'
	Sh.RegWrite classes & "\CLSID\" & clsid & "\LocalServer32\", exepath
End If


'''bCreateProgID = false
'''Sh.RegRead(classes & "\CLSID\" & clsid & "\ProgID\")
'''If Err <> 0 Then
'''  bCreateProgID = true
'''End if
bCreateProgID = true

If bCreateProgID Then
	' [HKEY_CURRENT_USER\Software\Classes\CLSID\{36353903-2137-43FD-9AD6-40B65A96A839}\ProgID]
	' @="Testing.WPhast.Document"
	'
	Sh.RegWrite classes & "\CLSID\" & clsid & "\ProgID\", doc
End If


' import/save files
'
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
On Error GoTo 0

Set trans_dat_file = Fso.GetFile(WScript.Arguments(0))
wphast_file = Mid(trans_dat_file, 1, Len(trans_dat_file) - Len(".trans.dat")) & ".wphast"

Set WPhast = WScript.CreateObject("Testing.WPhast.Document")

If WPhast.Import(trans_dat_file.Path) Then
	WPhast.SaveAs(wphast_file)
Else
	WScript.Echo "An error occured during import: " & trans_dat_file.Path
End If


' clean up reg
'
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
On Error resume next

If bCreateProgID Then
	' [-HKEY_CURRENT_USER\Software\Classes\CLSID\{36353903-2137-43FD-9AD6-40B65A96A839}\ProgID]
	'
	Sh.RegDelete classes & "\CLSID\" & clsid & "\ProgID\"
End If

If bCreateLocalServer32 Then
	' [-HKEY_CURRENT_USER\Software\Classes\CLSID\{36353903-2137-43FD-9AD6-40B65A96A839}\LocalServer32]
	'
	Sh.RegDelete classes & "\CLSID\" & clsid & "\LocalServer32\"
End If


If bCreateClassCLSID Then
	' [-HKEY_CURRENT_USER\Software\Classes\CLSID\{36353903-2137-43FD-9AD6-40B65A96A839}]
	'
	Sh.RegDelete classes & "\CLSID\" & clsid & "\"
End If

If bCreateDocCLSID Then
	' [-HKEY_CURRENT_USER\Software\Classes\Testing.WPhast.Document\CLSID]
	'
	Sh.RegDelete classes & doc & "\CLSID\"
End If

If bCreateDoc Then
	' [-HKEY_CURRENT_USER\Software\Classes\Testing.WPhast.Document]
	'
	Sh.RegDelete classes & doc & "\"
End If
