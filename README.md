<p align="center">
	<a href="https://universal-team.net/projects/universal-updater.html"><img src="https://github.com/Universal-Team/Universal-Updater/blob/master/app/banner.png"></a><br>
	<b>An easy to use app for installing and updating 3DS homebrew</b><br>
	<a href="https://discord.gg/KDJCfGF" style="padding-right: 5px;">
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

Universal-Updater is a homebrew application for the Nintendo 3DS with the intention to make downloading other homebrew simple and easy. No need to manually copy files or go through installation processes, as we do that for you.

## Features

- A store format with a concept similar to the Cydia Repositories
   - Default is [Universal-DB](https://db.universal-team.net)
   - Want to add your own? Go to settings, find "Select Unistore", hit the + icon and type the URL or hit the QR button and scan the QR code (if they have one)
- Customization in sorting and display
   - All sorting keys: "Title", "Author" & "Last Updated"
   - Direction could be Ascending or Descending
   - App display could be shown in either a Grid or Rows
- Translations for multi-lingual users
   - To contribute to translations, join our [Crowdin](https://crwd.in/universal-updater)
   - Request a new language on our [Discord Server](https://discord.gg/KDJCfGF)

## Compilation
### Setting up your enviromment

To build Universal-Updater from source, you will need to setup devkitARM with libctru, 3ds-curl and 3ds-libarchive. Follow devkitPro's [Getting Started](https://devkitpro.org/wiki/Getting_Started) page to install pacman, then run `(sudo dkp-)pacman -S 3ds-dev 3ds-curl 3ds-libarchive`. You will also need [bannertool](https://github.com/Steveice10/bannertool/releases/latest) and [makerom](https://github.com/profi200/Project_CTR/releases/latest) in your PATH. Once you clone the repo (and its submodules), simply run `make` in the source code directory.

## Screenshots

![](https://github.com/Universal-Team/Universal-Updater/blob/master/screenshots/Credits.png)![](https://github.com/Universal-Team/Universal-Updater/blob/master/screenshots/DirectorySelection.png)![](https://github.com/Universal-Team/Universal-Updater/blob/master/screenshots/DownloadList.png)![](https://github.com/Universal-Team/Universal-Updater/blob/master/screenshots/EntryInfo.png)![](https://github.com/Universal-Team/Universal-Updater/blob/master/screenshots/LanguageSelection.png)![](https://github.com/Universal-Team/Universal-Updater/blob/master/screenshots/ListStyle.png)![](https://github.com/Universal-Team/Universal-Updater/blob/master/screenshots/MarkMenu.png)![](https://github.com/Universal-Team/Universal-Updater/blob/master/screenshots/SearchMenu.png)![](https://github.com/Universal-Team/Universal-Updater/blob/master/screenshots/SettingsMenu.png)![](https://github.com/Universal-Team/Universal-Updater/blob/master/screenshots/SortMenu.png)![](https://github.com/Universal-Team/Universal-Updater/blob/master/screenshots/StoreSelection.png)![](https://github.com/Universal-Team/Universal-Updater/blob/master/screenshots/ToggleMenu.png)

## Credits

- [StackZ](https://github.com/SuperSaiyajinStackZ) - Lead developer, reworked quirc to C++
- [Pk11](https://github.com/Epicpkmn11) - Mockup Designer, Website Maintainer
- [NightScript](https://github.com/NightYoshi370) - Concept Creator & Planner
- [dlbeer](https://github.com/dlbeer) - Original developer of [quirc](https://github.com/dlbeer/quirc)
- [FlagBrew](https://github.com/FlagBrew): Original QR Code Scanner code
- [Icons8](https://icons8.com/): Icon Designer
- [PabloMK7](https://github.com/mariohackandglitch): Download Code Improvements