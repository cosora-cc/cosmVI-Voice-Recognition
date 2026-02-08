# cosmVI
![UE Edition](https://img.shields.io/badge/Unreal%20Engine-5.6--5.7-0e1128?logo=unrealengine&logoColor=white)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![X (formerly Twitter) Follow](https://img.shields.io/twitter/follow/cosora_dev?style=social)](https://x.com/cosora_cc)



#### [日本語 README](README.md)

---
Translated using machine translation.

## What is it?
　cosmVI is a simple speech recognition plugin for Unreal Engine. It supports both C++ and Blueprint.
It's built with the motto “Easy to understand, easy to use.” You can easily recognize speech in specific languages.
All models used are licensed under **Apache 2.0**.

## Features
- Lightweight. It's essentially a wrapper around the VOSK library, so it runs efficiently. Currently, all models are small models.
- Thread-safe.

## Installation

### 1. Download

**Supported versions: Unreal Engine 5.6~5.7**

**[Booth](https://cosora.booth.pm/items/7958007)** (Following my store account means you'll get notifications for updates, and it keeps me motivated!)

This is the packaged version. If you can package it yourself, cloning this repository and packaging it works just as well.

### Supported Platforms
If you can package the plugin yourself, we recommend cloning this GitHub repository and packaging it in your own environment.
| Windows | macOS | Linux | iOS | Android |
| :---: | :---: | :---: | :---: | :---: |
| ✅ | ❌ | ✅ | ❌ | ❌ |


### Pre-Packaged Plugin Version and Platform Compatibility Chart
| UE Version | Windows | macOS | Linux | iOS | Android |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **5.7** | ✅ | ❌ | ✅ | ❌ | ❌ |
| **5.6** | ✅ | ❌ | ✅ | ❌ | ❌ |
| **5.5** | ❌ | ❌ | ❌ | ❌ | ❌ |
| **5.4** | ❌ | ❌ | ❌ | ❌ | ❌ |
| **5.3** | ❌ | ❌ | ❌ | ❌ | ❌ |
| **5.2** | ❌ | ❌ | ❌ | ❌ | ❌ |
| **5.1** | ❌ | ❌ | ❌ | ❌ | ❌ |
| **5.0** | ❌ | ❌ | ❌ | ❌ | ❌ |


Items marked with ✅ are distributed as packaged items via Booth.

### Supported Languages
| Japanese | English | French | German | Italian | Chinese | ... | Korean |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ... | ❌ |

※Korean couldn't be pushed to GitHub due to capacity issues (apparently files are limited to 100MB. I'll probably add it once the vosk model is updated and usable (if I remember)).

Recognition performance for the languages themselves depends on vosk.
※To put it simply, it works just by swapping out the model.