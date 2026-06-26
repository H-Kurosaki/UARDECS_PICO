# UARDECS_PICO
UARDECS is a library for implementing the "UECS" protocol, a Japanese communication standard for horticulture, on Raspberry Pi Pico. To use this library, you need to connect the W5500 as an Ethernet controller. An easier platform to implement is the Raspberry Pi Pico compatible W5500-EVB-Pico or W5500-EVB-Pico2

As of 2026/6/25, the required Ethernet library (MyEthernet2, a fork of Ethernet2) is bundled inside this library under `src/_MyEthernet2/`, so you no longer need to install any separate Ethernet library to compile. See LICENSE.md for the licensing of that bundled code.

UARDECSは日本の施設園芸用の通信規格である"UECS"準拠のプロトコルをRaspberry Pi Picoに実装するためのライブラリです。このライブラリを利用するにはイーサネットコントローラとしてW5500を接続する必要があります。より簡単に実装可能なプラットフォームはRaspberry Pi Pico互換機のW5500-EVB-PicoまたはW5500-EVB-Pico2です。

詳細なマニュアルは準備中です。

2024/4/22 EEPROMのエミュレーションに対応し設定データが不揮発性メモリに保存できるようになりました。

2024/5/9 サンプルプログラムの説明に抜けていた手順があったので修正しました。

2024/5/17 スキャンコマンドの応答に異常があったためEthernet2.hを使用するように変更しました。

2025/2/5 低頻度でフリーズする問題に対応するため独自に改修したMyEthernet2.hを使用するように変更しました。

2025/4/28 CCMに負数を受信したとき異常な数値に変換される問題を修正

2026/6/26 サンプルプログラムの誤字や間違った記述を修正しました。
          MyEthernet2ライブラリを同封するようにしました。これでイーサネット関連ライブラリを別途インストールしなくてもコンパイルできます。
          MyEthernet2は"https://github.com/adafruit/Ethernet2"
          をフォークして一部コードを修正したものを使用しており、src/_MyEthernet2/フォルダ以下に収録しています。当該フォルダのソースコードのライセンスはフォーク元のEthernet2(LGPL 2.1)に準拠します。詳細はLICENSE.mdを参照してください。