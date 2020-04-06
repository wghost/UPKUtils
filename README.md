UPKUtils
========

A set of utilities to work with UE3 cooked packages (u, upk, umap).

The utilities were originally made to work with XCOM:EU/EW games. Now they also support Batman: Arkham Asylum and partially support Batman: Arkham City (WIP).

XComLZO is an old helper tool and can be considered deprecated now.

ExportTexturesToDDS and ImportTexturesFromDDS are the new tools added to find and modify textures inside packages and the Textures.tfc file. They allow to permanently inject modified textures into an UE3 game, thus eliminating the need for TexMod.

The tools are locked to work with certain versions of the engine only. Simply unlocking the version is useless in most cases: due to the differences in how data are organized inside the packages using tools on an unsupported game will most probably lead to a crash.

The updated versions of the tools might not be working properly with PatcherGUI. Use the v7.3 sources released on Apr 10, 2015 for that purpose.


How to compile under Linux:
===========================

I did not have time to test the newest version of the tools under Linux. These instructions are for the older versions (v7.3 and below).

Install gcc-c++ (g++) compiler, meson, ninja, git and any wxGTK-devel v.2.9+ package available for your distribution repository.
There are some programs which requires wxWidgets:
 - DecompressLZO
 - DeserializeAll
 - ExportTexturesToDDS
 - ImportTexturesFromDDS

Everything else should compile perfectly without wxGTK installed.

Clone github repo and compile UPKUtils project:
```
git clone https://github.com/wghost/UPKUtils.git
meson build
cd build
ninja
```

To compile XComLZO packer/unpacker (deprecated):
```
cd UPKUtils/XComLZO/build
cmake .
make
```
