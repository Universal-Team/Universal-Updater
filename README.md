<p align="center">
	<a href="https://universal-team.net/projects/universal-updater.html"><img src="https://github.com/Universal-Team/Universal-Updater/blob/master/app/banner.png"></a><br>
	<b>A multiapp, JSON script-based updater for Nintendo 3DS</b><br>
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

## Features

Universal-Updater is a homebrew application for the Nintendo 3DS that is intended for easy set-up of other homebrew titles! Some features include:

- Integration with the Universal DB store, the easiest way to download all your homebrew titles
- A flexible and easily developable scripting system (with pre-made scripts included)
- Translations for multi-lingual users
- Customizable interface colors

## Compilation
### Setting up your enviromment

To build Universal-Updater from source, you will need to setup a system with devkitARM, libctru, 3ds-curl and 3ds-libarchive. Follow devkitPro's [Getting Started](https://devkitpro.org/wiki/Getting_Started) page to install pacman, then run `(sudo dkp-)pacman -S 3ds-dev 3ds-curl 3ds-libarchive`. You will also need [bannertool](https://github.com/Steveice10/bannertool/releases/latest) and [makerom](https://github.com/profi200/Project_CTR/releases/latest) in your PATH.

### Cloning the repo

To download the source you will need to clone it with submodules, this can be done by running
```
git clone --recursive https://github.com/Universal-Team/Universal-Updater.git
```
or if you've already cloned it running
```
git submodule update --init --recursive
```
to update all the submodules.

### Compiling

Simply run `make` in the Universal-Updater source directory.

## Screenshots

![](https://universal-team.net/images/universal-updater/script-browse-3.png)![](https://universal-team.net/images/universal-updater/script-browse-7.png)![](https://universal-team.net/images/universal-updater/scriptlist-selection.png)![](https://universal-team.net/images/universal-updater/mainMenu.png)

## Translations

Want to help to translate Universal-Updater? Then feel free to join us on Crowdin [here](https://crwd.in/universal-updater).
Is it a new Language which isn't available in Universal-Updater? Then join our [Discord Server](https://discord.gg/KDJCfGF) and ask Evie to add the language to crowdin.

## Credits

- [dlbeer](https://github.com/dlbeer) for [quirc](https://github.com/dlbeer/quirc) which i changed to C++.
- [FlagBrew](https://github.com/FlagBrew): For the QR Code Scanner code, which i reworked some bit.
- [Icons8](https://icons8.com/): For the icons.
- [PabloMK7](https://github.com/mariohackandglitch): For improving the Download code.