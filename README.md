# cosmVI
![UE Version](https://img.shields.io/badge/Unreal%20Engine-5.7--5.7-0e1128?logo=unrealengine&logoColor=white)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![X (formerly Twitter) Follow](https://img.shields.io/twitter/follow/cosora_dev?style=social)](https://x.com/cosora_cc)



#### [English README](README_en.md)

---

## 一体なに？
　cosmVIは Unreal Engine 向けのシンプルな音声認識プラグインです。c++,BluePrintに対応しています。
「分かりやすく、使いやすい」をモットーに作っています。特定の言語の音声を簡単に認識させることができます。
使用しているモデルのライセンスは全て Apache 2.0 です。

## 特徴
- 軽量です。基本的にvoskライブラリをラッパしたものなので、軽量に動作します。現時点でモデルは全てsmallモデルです。
- スレッドセーフです。

## 導入方法

### 1. ダウンロード

### 対応プラットフォーム
　プラグインのパッケージ化ができるのなら、このgithubリポジトリをクローンして自分の環境でパッケージ化することをお勧めします。
| Windows | macOS | Linux | iOS | Android |
| :---: | :---: | :---: | :---: | :---: |
| ✅ | ❌ | ❌ | ❌ | ❌ |


### パッケージ化済みプラグインのバージョン、プラットフォーム対応表
※プラットフォームはどれか一つでも✅がついていないと使えません。
| UE Version | Windows | macOS | Linux | iOS | Android |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **5.7** | 🛠️ | ❌ | ❌ | ❌ | ❌ |
| **5.4** | ❌ | ❌ | ❌ | ❌ | ❌ |
| **5.3** | ❌ | ❌ | ❌ | ❌ | ❌ |
| **5.2** | ❌ | ❌ | ❌ | ❌ | ❌ |
| **5.1** | ❌ | ❌ | ❌ | ❌ | ❌ |
| **5.0** | ❌ | ❌ | ❌ | ❌ | ❌ |


✅がついている物はBoothにてパッケージ化したものを配布（無料）しています。🛠️は準備中です。❌はめどが立っていません。

### 対応言語
| 日本語 | 英語 | フランス語 | ドイツ語 | イタリア語 | 中国語 |
| :---: | :---: | :---: | :---: | :---: | :---: |
| ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |

言語自体への認識性能はvoskに依存しています。
※言ってしまえば、モデルを差し替えるだけで動作します。