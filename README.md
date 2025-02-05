# UARDECS_PICO
UARDECS is a library for implementing the "UECS" protocol, a Japanese communication standard for horticulture, on Raspberry Pi Pico. To use this library, you need to connect the W5500 as an Ethernet controller. In addition, the MyEthernet2(https://github.com/H-Kurosaki/MyEthernet2 ) library must be installed with this library. An easier platform to implement is the Raspberry Pi Pico compatible W5500-EVB-Pico

UARDECSは日本の施設園芸用の通信規格である"UECS"準拠のプロトコルをRaspberry Pi Picoに実装するためのライブラリです。このライブラリを利用するにはイーサネットコントローラとしてW5500を接続する必要があります。さらにこのライブラリを使うときは一緒にMyEthernet2ライブラリ(https://github.com/H-Kurosaki/MyEthernet2 )をインストールして下さい。より簡単に実装可能なプラットフォームはRaspberry Pi Pico互換機のW5500-EVB-Picoです。

詳細なマニュアルは準備中です。

2024/4/22 EEPROMのエミュレーションに対応し設定データが不揮発性メモリに保存できるようになりました。

2024/5/9 サンプルプログラムの説明に抜けていた手順があったので修正しました。

2024/5/17 スキャンコマンドの応答に異常があったためEthernet2.hを使用するように変更しました。

2025/2/5　低頻度でフリーズする問題に対応するため独自に改修したMyEthernet2.hを使用するように変更しました。
