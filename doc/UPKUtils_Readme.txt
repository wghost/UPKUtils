-----------------------------------------------------------------------------------------------------------------
    UPK Utils documentation file
-----------------------------------------------------------------------------------------------------------------

UPK Utils are a set of Windows console applications for analysing and patching uncompressed XCOM packages (.upk).

Version 4.0, 04/24/2014

Wasteland Ghost aka wghost81 (wghost81@gmail.com).

-----------------------------------------------------------------------------------------------------------------
    ExtractNameLists
-----------------------------------------------------------------------------------------------------------------

This program is used to read package header and output the information to the console.

Usage:
ExtractNameLists UnpackedResourceFile.upk [/v]
    /v — verbose mode (optional parameter)
Examples:
ExtractNameLists.exe XComStrategyGame.upk
ExtractNameLists.exe XComStrategyGame.upk /v
    
To redirect output from console to text file type:
ExtractNameLists UnpackedResourceFile.upk > file_name.txt
Examples:
ExtractNameLists.exe XComStrategyGame.upk > XComStrategyGame.txt
ExtractNameLists.exe XComStrategyGame.upk /v > XComStrategyGame.txt

This will redirect output to file_name.txt. Error messages will still be output into console.

If package is compressed, program will try to read and output compression info.

-----------------------------------------------------------------------------------------------------------------
    FindObjectEntry
-----------------------------------------------------------------------------------------------------------------

This program is used to find specified object in specified package and output the information to the console.
Program will also try to deserialize object's data and output all the info available.

Usage:
FindObjectEntry UnpackedResourceFile.upk ObjectName [/d]
    ObjectName is a full object name: Owner.Owner...Name
    /d — dump object serialized data into binary file. File name will have Owner.Owner...Name.Type format.
        (optional parameter)
Example:
FindObjectEntry.exe XComStrategyGame.upk XGFacility_PsiLabs.TPsiTrainee

You can redirect output to file:
FindObjectEntry.exe XComStrategyGame.upk XGFacility_PsiLabs.TPsiTrainee > XGFacility_PsiLabs.TPsiTrainee.txt

FindObjectEntry will now try to deserialize unknown objects. It shouldn't crash in case of wrong guess about
object's data, but may output wrong or incomplete data sometimes. Anyway, it will output a warning about object's
type being unknown.

-----------------------------------------------------------------------------------------------------------------
    FindObjectByOffset
-----------------------------------------------------------------------------------------------------------------

This program is used to find an export object full name by specified offset.

Usage:
FindObjectByOffset UnpackedResourceFile.upk offset
    offset — file offset in bytes (dec or hex)

-----------------------------------------------------------------------------------------------------------------
    DeserializeAll
-----------------------------------------------------------------------------------------------------------------

This program is used to batch-deserialization of package Export Objects.

Usage:
DeserializeAll UnpackedResourceFile.upk [NameMask]
    NameMask - not yet real mask, but a substring in full name string (optional parameter)
    
Example:
DeserializeAll URB_PierA.upk
Will create a file URB_PierA.txt with package info and folder URB_PierA with subfolders for each owner and it's
objects.

DeserializeAll URB_PierA.upk TheWorld.PersistentLevel
Will deserialize TheWorld.PersistentLevel and it's objects.

The program is unstable and can sometimes crash, especially with map packages, as there is no info on most of the
map objects. But it is helpful in analysing TheWorld.PersistentLevel objects, as they are mostly archetypes and
contain Default Properties only.
    
-----------------------------------------------------------------------------------------------------------------
    MoveExpandFunction
-----------------------------------------------------------------------------------------------------------------

This program is used to move/expand specified function or to undo previous move/expand operation.

Usage:
MoveExpandFunction UnpackedResourceFile.upk FunctionName [NewFunctionSize or /u]
    FunctionName is a full object name: Owner.Owner...Name
    NewFunctionSize is a new function size in bytes (optional parameter)
    /u switches the program into undo mode (optional parameter)
    
Examples:
MoveExpandFunction.exe XComStrategyGame.upk XGStrategyAI.GetAltWeapon 300
MoveExpandFunction.exe XComStrategyGame.upk XGStrategyAI.GetAltWeapon /u

-----------------------------------------------------------------------------------------------------------------
    PatchUPK
-----------------------------------------------------------------------------------------------------------------

This program is used to apply package patches. For more information see PatchUPK_Readme.txt.

Usage:
PatchUPK modfile.txt [PATH_TO_UPK]
    modfile.txt — mod script (see PatchUPK_Readme.txt and PatchUPK_Mod_Example.txt)
    PATH_TO_UPK — path to folder, where packages are stored (optional parameter)


-----------------------------------------------------------------------------------------------------------------
    CompareUPK
-----------------------------------------------------------------------------------------------------------------

A program to compare packages. Useful for analysing patches. Slow. Really slow. :)

Usage:
CompareUPK OldPackage.upk NewPackage.upk

-----------------------------------------------------------------------------------------------------------------
    XComLZO
-----------------------------------------------------------------------------------------------------------------

An utility to pack/unpack data using LZO compression algorithm. Aimed to work with XCOM-style archives.

Usage: XComLZO p inputFileName
    or XComLZO u inputFileName
where p = pack and u = unpack

Useful to re-packing graphics and creating your very own tfc packages.

-----------------------------------------------------------------------------------------------------------------
    Acknowledgments
-----------------------------------------------------------------------------------------------------------------

Gildor, Antonio Cordero Balcazar, Eliot van Uytfanghe and the other Unreal Engine researchers for helpful
utilities and format info.
