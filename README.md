<p align="center">
	<a href="https://universal-team.net/projects/universal-updater.html"><img src="https://github.com/Universal-Team/Universal-Updater/blob/master/app/banner.png"></a><br>
	<b>An easy to use app for installing and updating 3DS homebrew</b><br>
	<a href="https://universal-team.net/discord" style="padding-right: 5px;">
		<img src="https://img.shields.io/badge/Discord%20Server-%23universal--updater-green.svg" alt="Discord Server">
	</a>
	<a href="https://gbatemp.net/threads/release-universal-updater-a-universally-good-updater.551824/" style="padding-left: 5px; padding-right: 5px;">
		<img src="https://img.shields.io/badge/GBAtemp-thread-blue.svg" height="20">
	</a>
	<a href="https://github.com/Universal-Team/Universal-Updater/actions?query=workflow%3A%22Build+Universal-Updater%22" style="padding-left: 5px; padding-right: 5px;">
		<img src="https://github.com/Universal-Team/Universal-Updater/workflows/Build%20Universal-Updater/badge.svg" height="20" alt="Build status on GitHub Actions">
	</a>
	<a title="Crowdin" target="_blank" href="https://crowdin.com/project/universal-updater"><img src="https://badges.crowdin.net/universal-updater/localized.svg" alt="Translation status on Crowdin"></a>
</p>

Universal-Updater is a homebrew application for the Nintendo 3DS with the intention to make downloading other homebrew simple and easy. No need to manually copy files or go through installation processes, as it does that all for you.

## Features

- A store format with a concept similar to the Cydia Repositories
   - The default is [Universal-DB](https://db.universal-team.net)
   - Want to add more? Go to settings, choose "Select Unistore", click the + icon and select one from the list, enter a URL, or scan a QR code
- Customization in sorting and display
   - Several sorting keys: "Title", "Author", and "Last Updated"
   - Direction can be Ascending or Descending
   - App display can be shown in either a grid or rows
- Searching and markings to make finding apps easy
- View screenshots and release notes before downloading
- Shortcuts for easily updating frequently updated apps when using the Homebrew Launcher
- Translations for users of many languages
   - To contribute to translations, join our [Crowdin project](https://crwd.in/universal-updater)
   - Request new languages on our [Discord server](https://universal-team.net/discord)


## Compilation
### Setting up your enviromment

To build Universal-Updater from source, you will need to setup devkitARM with libctru and 3ds-curl. Follow devkitPro's [Getting Started](https://devkitpro.org/wiki/Getting_Started) page to install pacman, then run `(sudo dkp-)pacman -S 3ds-dev 3ds-curl`. You will also need [bannertool](https://github.com/Steveice10/bannertool/releases/latest) and [makerom](https://github.com/profi200/Project_CTR/releases/latest) in your PATH. Once you clone the repo (and its submodules), simply run `make` in the source code directory.

## Screenshots

<details><summary>Screenshots</summary>

![](https://github.com/Universal-Team/Universal-Updater/blob/master/resources/Credits.png) ![](https://github.com/Universal-Team/Universal-Updater/blob/master/resources/DirectorySelection.png) ![](https://github.com/Universal-Team/Universal-Updater/blob/master/resources/DownloadList.png) ![](https://github.com/Universal-Team/Universal-Updater/blob/master/resources/EntryInfo.png) ![](https://github.com/Universal-Team/Universal-Updater/blob/master/resources/LanguageSelection.png) ![](https://github.com/Universal-Team/Universal-Updater/blob/master/resources/ListStyle.png) ![](https://github.com/Universal-Team/Universal-Updater/blob/master/resources/MarkMenu.png) ![](https://github.com/Universal-Team/Universal-Updater/blob/master/resources/SearchMenu.png) ![](https://github.com/Universal-Team/Universal-Updater/blob/master/resources/SettingsMenu.png) ![](https://github.com/Universal-Team/Universal-Updater/blob/master/resources/SortMenu.png) ![](https://github.com/Universal-Team/Universal-Updater/blob/master/resources/StoreSelection.png) ![](https://github.com/Universal-Team/Universal-Updater/blob/master/resources/AutoUpdateSettings.png) ![](https://github.com/Universal-Team/Universal-Updater/blob/master/resources/DirectorySettings.png)![](https://github.com/Universal-Team/Universal-Updater/blob/master/resources/GUISettings.png) ![](https://github.com/Universal-Team/Universal-Updater/blob/master/resources/3DS_Screenshot.png) ![](https://github.com/Universal-Team/Universal-Updater/blob/master/resources/Screenshot_Zoom0.png) ![](https://github.com/Universal-Team/Universal-Updater/blob/master/resources/Screenshot_Zoom1.png) ![](https://github.com/Universal-Team/Universal-Updater/blob/master/resources/Screenshot_Zoom2.png) ![](https://github.com/Universal-Team/Universal-Updater/blob/master/resources/Keyboard.png) ![](https://github.com/Universal-Team/Universal-Updater/blob/master/resources/Recommended_UniStores.png) ![](https://github.com/Universal-Team/Universal-Updater/blob/master/resources/ReleaseNotes.png)

</details>

## Credits

- [SuperSaiyajinStackZ](https://github.com/SuperSaiyajinStackZ) - Lead developer, reworked quirc to C++
- [Pk11](https://github.com/Epicpkmn11) - Mockup Designer, Website Maintainer
- [NightScript](https://github.com/NightYoshi370) - Concept Creator & Planner
- [dlbeer](https://github.com/dlbeer) - Original developer of [quirc](https://github.com/dlbeer/quirc)
- [FlagBrew](https://github.com/FlagBrew): Original QR Code Scanner code
- [Icons8](https://icons8.com/): Icon Designer
- [lvandeve](https://github.com/lvandeve): For [LodePNG](https://github.com/lvandeve/lodepng)
- [PabloMK7](https://github.com/mariohackandglitch): Download Code Improvements
