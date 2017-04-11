## XNFSModFiles
Xanvier's NFS chunk resource file adder/replacer for BBX NFS games

# What is this?
This is an ASI loader plugin that allows the user to add custom chunks to their game and override them.
For example: you need to replace a texture in InGameB. Instead of replacing files, you can create your own chunk bundle (with a hex editor) or completely add in a modified bundle and load it, overriding InGameB.lzc

# Game compatibility
Current titles that are supported (with separate builds):
- Need for Speed: Underground (broken)
- GlobalVR's Need for Speed: Underground Arcade (same as above)
- Need for Speed: Underground 2
- Need for Speed: Most Wanted
- Need for Speed: Carbon
- Need for Speed: ProStreet
- Need for Speed: Undercover (not sure if it even works)

# Building
The included solution is made from VS2015, however, you should be able to build this with any Visual Studio or Dev C++.

- Open XNFSModFiles.sln
- Select the configuration of the game that you desire (Carbon is Release).
- Output file will be in the Release(-GameExt).

# Running
To run this, you need ThirteenAG's ASI loader, which you can get from the widescreen packs here: https://thirteenag.github.io/wfp#nfs
After installing the ASI loader (by putting dinput8.dll in game's root folder), simply copy the compiled .asi and the .ini into the scripts folder which is also located in the game's root directory.
