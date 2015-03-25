-----------------------------------------------------------------------------------------------------------------
    UPK Utils documentation file
-----------------------------------------------------------------------------------------------------------------

UPK Utils are a set of console applications for analysing and patching XCOM packages (.upk).

-----------------------------------------------------------------------------------------------------------------
    DecompressLZO
-----------------------------------------------------------------------------------------------------------------

An utility to decompress upk files using LZO compression algorithm.

Usage: DecompressLZO CompressedResourceFile.upk [DecompressedCompressedResourceFile.upk]

If DecompressedCompressedResourceFile.upk is not specified, decompressed package is saved to
CompressedResourceFile.upk.uncompr file.

Works with compressed and fully compressed packages.

-----------------------------------------------------------------------------------------------------------------
    ExtractNameLists
-----------------------------------------------------------------------------------------------------------------

The program reads package header (package information, name, import and export tables) and outputs the data
to the console.

Usage:
ExtractNameLists UnpackedResourceFile.upk [/v]
    /v — verbose mode (optional parameter)
Examples:
ExtractNameLists XComStrategyGame.upk
ExtractNameLists XComStrategyGame.upk /v
    
To redirect output from the console to a text file type:
ExtractNameLists UnpackedResourceFile.upk > file_name.txt
Examples:
ExtractNameLists XComStrategyGame.upk > XComStrategyGame.txt
ExtractNameLists XComStrategyGame.upk /v > XComStrategyGame.txt

This will redirect output to file_name.txt. Error messages will still be printed into the console.

If package is compressed, program will try to read and print compression info.

-----------------------------------------------------------------------------------------------------------------
    FindObjectEntry
-----------------------------------------------------------------------------------------------------------------

The program finds a specified object in a specified package and outputs the information to the console.
Program will also try to deserialize object's data and print all the data available.

Usage:
FindObjectEntry UnpackedResourceFile.upk ObjectName [/d]
    ObjectName is a full object name: Owner.Owner...Name
    /d — dump object serialized data into binary file. File name will have Owner.Owner...Name.Type format.
        (optional parameter)
Example:
FindObjectEntry XComStrategyGame.upk XGFacility_PsiLabs.TPsiTrainee

You can redirect output to a file:
FindObjectEntry XComStrategyGame.upk XGFacility_PsiLabs.TPsiTrainee > XGFacility_PsiLabs.TPsiTrainee.txt

FindObjectEntry will try to deserialize unknown objects. It shouldn't crash in case of wrong guess about
object's data, but may give wrong or incomplete data sometimes. It will give a warning about object's
type being unknown.

-----------------------------------------------------------------------------------------------------------------
    HexToPseudoCode
-----------------------------------------------------------------------------------------------------------------

Converts hex bytecode to PatchUPK/PatcherGUI pseudo-code. Works with Functions and States. Decompiled script is
directly usable by PatchUPK/PatcherGUI.

Usage:
HexToPseudoCode UnpackedResourceFile.upk ObjectName [/d]
    ObjectName is a full object name: Owner.Owner...Name
    /d — dump object serialized data into binary file. File name will have Owner.Owner...Name.Type format.
        (optional parameter)
Example:
HexToPseudoCode XComStrategyGame.upk XGFundingCouncil.UpdateSlingshotMission >  XGFundingCouncil.UpdateSlingshotMission.txt

This will create a file XGFundingCouncil.UpdateSlingshotMission.txt with decompiled code inside.

-----------------------------------------------------------------------------------------------------------------
    FindObjectByOffset
-----------------------------------------------------------------------------------------------------------------

The program finds an export object full name by specified offset.

Usage:
FindObjectByOffset UnpackedResourceFile.upk offset
    offset — file offset in bytes (dec or hex)

-----------------------------------------------------------------------------------------------------------------
    DeserializeAll
-----------------------------------------------------------------------------------------------------------------

The program performs batch-deserialization of all Export Objects inside a specified package.

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
    PatchUPK
-----------------------------------------------------------------------------------------------------------------

An utility to apply UPK patches. For more information see PatchUPK_Readme.txt.

Usage:
PatchUPK modfile.txt [PATH_TO_UPK]
    modfile.txt — mod script (see PatchUPK_Readme.txt and PatchUPK_Mod_Example.txt)
    PATH_TO_UPK — path to folder where packages are located (optional parameter)


-----------------------------------------------------------------------------------------------------------------
    MoveExpandFunction (Deprecated)
-----------------------------------------------------------------------------------------------------------------

The program performs move/expand operations for a specified function. Can undo previous move/expand operation.

Usage:
MoveExpandFunction UnpackedResourceFile.upk FunctionName [NewFunctionSize or /u]
    FunctionName is a full object name: Owner.Owner...Name
    NewFunctionSize is a new function size in bytes (optional parameter)
    /u switches the program into undo mode (optional parameter)
    
Examples:
MoveExpandFunction XComStrategyGame.upk XGStrategyAI.GetAltWeapon 300
MoveExpandFunction XComStrategyGame.upk XGStrategyAI.GetAltWeapon /u

-----------------------------------------------------------------------------------------------------------------
    CompareUPK
-----------------------------------------------------------------------------------------------------------------

An utility to compare packages. Useful for analysing patches. Slow. Really slow. :)

Usage:
CompareUPK OldPackage.upk NewPackage.upk

-----------------------------------------------------------------------------------------------------------------
    XComLZO
-----------------------------------------------------------------------------------------------------------------

An utility to pack/unpack raw data using LZO compression algorithm.

Usage: XComLZO p inputFileName
    or XComLZO u inputFileName
where p = pack and u = unpack

Useful to re-packing graphics and creating your very own tfc packages.

-----------------------------------------------------------------------------------------------------------------
    Acknowledgments
-----------------------------------------------------------------------------------------------------------------

Gildor, Antonio Cordero Balcazar, Eliot van Uytfanghe and the other Unreal Engine researchers for helpful
utilities and format info.
