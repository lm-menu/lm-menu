______________________________________________________________________

 Lunar Magic : Menu/Text Localization Plugin
 Version 1.0
 September ??, 2015

 This plugin does not work with Windows XP, please consider update to
 at least Windows 7.
______________________________________________________________________

 CONTENTS
______________________________________________________________________

 1. Credits
 2. Introduction
 3. Configuration
 4. Debugging
 5. Legal Notice

______________________________________________________________________


 PROGRAMMING:           Jixun (?)

 Origional Author:      FuSoYa (Defender of Relm)

 DEVELOPMENT TOOLS:     Visual Studio 2015, Windows 10

______________________________________________________________________

 2. Introduction
______________________________________________________________________

 This is a plugin mainly focused on modify Lunar Magic's memory on the
 fly, to be able to change some of its text to a different language.
 
 You will need to install V-CRT 2015 for debug version to work, you can
 get it in: https://www.microsoft.com/download/details.aspx?id=48145
 
______________________________________________________________________

 3. Configuration
______________________________________________________________________

 All configurable items are located in "lm_menu.ini". You can just edit
 it with your notepad.
 
 All translation dictionary file are located in "lm_menu.txt", in
 PLAIN TEXT, UTF-8 no BOM.
 
 For dialog translation, please extract it from "Lunar Magic" then build
 a resource-only dll, and finally place it here.
 
 Or you can just rename "Lunar Magic.exe" to "lm_res.dll" and have fun
 with it.

______________________________________________________________________

 4. Debugging
______________________________________________________________________

 Backup and delete "lm_menu.dll", then rename "lm_dbg.dll" to
 "lm_menu.dll".
 
 Now you'll be in debug mode, where you can find missing translation
 from the console output or the file called "lm_menu.log".

______________________________________________________________________

 5. Legal Notice
______________________________________________________________________

 The Lunar Magic Localization Plugin (hereafter referred to as the
 "Software") is not official or supported by FuSoYa, Lunarian Concepts,
 Nintendo or any other commercial entity.

 The Software is provided AS IS, and its use is at your own risk.
 Anyone mentioned in this document will not be held liable for any
 damages, direct or otherwise, arising from its use or presence, blah
 blah blah.
 
 Document layout was ripped off Lunar Magic's readme.
 
 You can do whatever you want to them, just remember to not include
 any part of Lunar Magic or any ROM when you redistribute it.
 
______________________________________________________________________
