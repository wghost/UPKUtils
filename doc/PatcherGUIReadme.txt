PatcherGUI - a tool to install and maintain modifications for XCOM:EU and EW
============================================================================

The information below is intended for mod-users mostly. If you're a mod developer, please, consult
"PatchUPK_Readme.txt" file, included in the program archive. It contains all the information needed to create mod files for use with PatchUPK/PatcherGUI.


Program features:
=================
- Does not require installation: unpack and use.
- Has graphical interface.
- Can install mods for both XCOM:EU and EW.
- Maintains separate install logs for every game path.
- Writes backup files for every change it makes.
- Automatically reverts to latest backup if something goes wrong with install/uninstall process.
- Automatically creates uninstall files.
- Automatically unpacks game packages using Gildor's decompress program.
- Has an option to disable hash checks for XCOM:EU.
- Has an option to enable direct ini loading.
- Has an option to disable phoning home (ini re-downloading).


Quick overview:
===============
XCOM doesn't have official modding support, so we have a bit of a problem here. Actually, more than a bit.

To make some significant changes to the game, we need to alter its packed resources ("packages"). And those are compressed, "cooked" and hashed (for EU). Fortunately, the PC version can read decompressed packages and hash check can be disabled. But the problem of packages being "cooked" stays and raises a lot of questions about inter-patch and inter-mod compatibility.

First, each time Firaxis releases a new patch, those packages get "re-cooked" and all the prior mods, written for each specific patch, stop working.

Second, we can't simply distribute altered packages, as that would make it impossible to combine several mods into one game. So, to allow installation of multiple different mods, we have to distribute changed parts instead of full packages, which, again, raises inter-patch compatibility questions.

Trying to solve the problem of cooked packages, the XCOM modding community eventually came to the idea of distributing not a package itself, but rather a set of instructions on how to modify local user's packages - i.e. distributing a mod's source code, which can be made compatible to all the game versions.

This approach is a bit less user-friendly than a final GUI tool with sliders and check-boxes, but modularity and flexibility allows for easier mods creation, distribution and maintaining: anyone can contribute and the next patch will not ruin the majority of the hard work of prior mods!


Preparing your game for handling mods:
======================================
First mandatory thing you should do: make sure your XCOM game is installed outside the "C:\Program Files" (or "C:\Program Files (x86)" on 64-bit Systems) folder. The reason for this is that Windows (since Vista) considers the "Program Files" folder trees to be restricted areas and will revert all the changes any mod makes to a saved "shadow" copy. It's explained better in the "Installing Games on Windows Vista+" wiki article:
http://wiki.tesnexus.com/index.php/Installing_Games_on_Windows_Vista%2B

Second thing you should do is to switch off automatic updates for XCOM in Steam. Right-click XCOM in the Steam applications list and search it's properties for "updates" options. Note, that turning auto-updates off will not prevent Steam from downloading a newly released patch, but it will prevent silent updates running in the background and breaking your game.

Third thing to do is to stop the game from "phoning home". Each time you launch XCOM it "calls" to Firaxis' servers and downloads some "ini" files to prevent multiplayer cheating. If you're planning to make modifications to game "ini" files (like the "Arc Thrower in Pistol Slot" tweak), you need to block those server's IP addresses in the Windows hosts file. Instructions on how to do this can be found here:
http://wiki.tesnexus.com/index.php/Steam_and_mods#Modifying_hosts_file
Note that you will most probably need administrator access to do this. Also note that some anti-virus programs automatically protect the hosts file, so you'll probably need to temporarily turn off your anti-virus software.

For more information on how to install mods for XCOM, please, consult these wiki articles:
http://wiki.tesnexus.com/index.php/Basic_Guide_to_installing_mods
http://wiki.tesnexus.com/index.php/Steam_and_mods


Making a first step:
====================
Download the latest PatcherGUI archive and unpack it to any folder without locale-specific (i.e. "unicode") symbols in the path. "C:\" or "C:\XCOM-Mods-and-Utils" will do nicely.

Open the folder where you unpacked PatcherGUI and run "PatcherGUI.exe". You can create a desktop link to "PatcherGUI.exe" by the usual Windows' means if you want.

The very first line of the tool, which has a "Path to XCOM:EU or XCOM:EW" tool-tip (which appears when you hover your mouse over it for a few seconds), should point to your actual XCOM install folder. Press the corresponding "Browse" button (one with a "Set path to XCOM:EU or XCOM:EW" tool-tip) and select the "Path-to-SteamLibrary\SteamApps\common\XCom-Enemy-Unknown" folder if you're planning to install EU mods; and the "Path-to-SteamLibrary\SteamApps\common\XCom-Enemy-Unknown\XEW" folder if you're planning to install mods for EW.

("Path-to-SteamLibrary" is a placeholder for the actual path to your SteamLibrary folder location, for example, "D:\SteamLibrary".)

Now you're ready to install mods.


XCOM: Enemy Unknown — disabling hash check:
===========================================

If you're planning to install EU mods, you need to disable hash checks, otherwise the game won't load your modified packages. To do so, click "Tools -> Disable hash check" in PatcherGUI main menu. If game path is correct (see "Making a first step" section above), program will patch XComGame.exe and you'll see "Success!" message. If you'll see an error message, read this file again and make sure you've followed all the instructions carefully.

You need to do this only once.

Note: "Disable hash check" option disables hash check for the four most commonly used packages: core.upk, engine.upk, xcomgame.upk and xcomstrategygame.upk. If a specific mod uses other package, you need to disable it separately via "Advanced -> Disable hash..." option in main menu.


Installing a mod:
=================
Download a mod's archive and unpack it. It's not required, but recommended to unpack all the mods into a "PatcherGUI\Mods" folder. Each mod file is a plain text file, which usually contains human-readable description at the top followed by a bunch of utility-specific commands.

Run PatcherGUI and use a second "Browse" button (one with "Open mod file" tool-tip) to browse for a file you just unpacked. PatcherGUI will load it and display its contents in it's simple text viewer/editor.

If you haven't yet set a path to your XCOM:EU or XCOM:EW folder, do it now (see "Making a first step" section above).

If the mod doesn't have any user-editable variables, press the "Apply" button to install it. If mod provides some configurable options, consult the mod's "Readme" file on how to set it up properly.

If installation was successful, close PatcherGUI and run the game. If an error message appears, read this file and the mod instructions carefully and try to repeat the installation. If it still fails, report a bug (see "Reporting bugs and asking questions" section later in this file).

If you're installing a mod for EU, you need to disable hash check (see "XCOM: Enemy Unknown — disabling hash check" section above).


Uninstalling a mod:
===================
PatcherGUI automatically creates "uninstall files" for all the installed mods. You can find the uninstall file in the same folder where the mod file is located. Each uninstall file is automatically named "name_of_the_original_mod_file.uninstall.txt".

Multiple subsequent installations of the same mod will create multiple subsequent uninstall files: "name_of_the_original_mod_file.uninstall1.txt",
"name_of_the_original_mod_file.uninstall2.txt",
and so on. The very first uninstall file ("name_of_the_original_mod_file.uninstall.txt") contains vanilla code, and the others - previous modded code. So, to completely uninstall a mod: use the very first uninstall file; and to undo only the last changes to that mod, use the most recent uninstall file.


Using a log:
============
PatcherGUI writes an install log for each of your game folders (you'll have separate install logs for EU and EW). You may access that log by pressing the "Show log" button.

The log window contains info on the current install path and a list of all installed mods for that path (names of the corresponding mod files). For each of the listed mods you can load it's installer or uninstaller file into PatcherGUI by pressing the corresponding "Load Installer" or "Load Uninstaller" button.

When you install a mod, it's automatically added to the install list. When you uninstall a mod, it's automatically removed from the list.


Cleaning things up:
===================
If you want to revert to the vanilla game, go to Steam, right-click XCOM and search for "verify game cache" under "game properties". Steam will check all the game files and re-download "broken" (i.e. modded) ones.

To clean up PatcherGUI you need to manually cleanup the "PatcherGUI\Backup" and "PatcherGUI\Logs" folder. The "Backup" folder contains all the modified files backups and the "Logs" folder contains install logs for all the game folders.


Enabling ini loading:
=====================

Since v.5.0 PatcherGUI can alter game executable to enable direct loading of DefaultGameCore.ini and DefaultLoadouts.ini. Click "Tools -> Enable ini loading" in program main menu. If game path is correct (see "Making a first step" section above), program will patch current game executable and you'll see "Success!" message. If you'll see an error message, read this file again and make sure you've followed all the instructions carefully.

You need to do it once for each install path (EU and EW).


Disabling phoning home:
=======================

Since v.5.2 PatcherGUI can disable phoning home (to stop ini files re-downloading and re-writing) by altering game executable. Click "Tools -> Disable phoning home" in program main menu. If game path is correct (see "Making a first step" section above), program will patch current game executable and you'll see "Success!" message. If you'll see an error message, read this file again and make sure you've followed all the instructions carefully.

You need to do it once for each install path (EU and EW).


Reporting bugs and asking questions:
====================================
For Nexus mods you can report bugs and ask questions in the mod-specific topic which can be accessed via the "POSTS" link on the Nexus Download page.

If you want to report a bug or ask a question about the UPKUtils utility itself, use the UPKUtils Nexus forum topic:
http://forums.nexusmods.com/index.php?showtopic=1253992

Click "Tools -> Show debug log" in program main menu to open program debug log. This log contains the output stream of all the utilities involved in a patching process. If something goes wrong, this information can be extremely helpful, so, please, copy and paste it into your bugreport.


Acknowledgements:
=================
Eliot van Uytfanghe for UE Explorer
Gildor for Unreal Engine Package Decompressor (decompress.exe)
bokauk for XCOM ToolBoks
Amineri for UPKModder
Bertilsson, Drakous79, JohnnyLump and all NexusForums authors for helpful info and discussions
dubiousintent for NexusWiki articles and helpful advices

All trademarks, mentioned herein, are properties of their respective owners.
