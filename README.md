UPKUtils
========

Set of utilities to work with UPK files


How to compile:
===============
Install gcc-c++ (g++) compiler, cmake, git and any wxGTK-devel v.2.9+ package available for your distribution repository. Aside from PatcherGUI, which isn't ready for Linux compilation yet, there is just one program which requires wxWidgets - DeserializeAll. Everything else should compile perfectly without wxGTK installed.

Clone github repo and compile UPKUtils project:

git clone https://github.com/wghost/UPKUtils.git
cd UPKUtils/build
cmake .
make

To compile XComLZO packer/unpacker:

cd UPKUtils/XComLZO/build
cmake .
make