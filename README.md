
# BonCasLink

BonCasLink v1.10 ( 64bit 対応版・http://www2.wazoku.net/2sen/friioup/source/up1085.zip ) に同梱されているソースコードの Mirror

ビルド環境を Visual Studio 2019 (VS2019) に更新した事、パッチ ( [BonCasLink.diff.txt](http://www2.wazoku.net/2sen/dtvvup/source/BonCasLink.diff.txt) ) を当てた事、この README.md を作成した事、フォントを Meiryo UI に変更した事以外はオリジナルのままとなっている。  
以下のドキュメントは [Readme.txt](Readme.txt)・[Readme_x64.txt](Readme_x64.txt) を現状に合わせて一部改変し、Markdown 形式に書き換えたものである。  
オリジナルの [Readme.txt](Readme.txt)・[Readme_x64.txt](Readme_x64.txt) も参照されたい。

----

## BonCasLink Ver.1.10

## 概要

スマートカードリーダを LAN を経由で共有するためのシステムです。

## ダウンロード

64bit 版の BonCasLink を利用する場合は x64 フォルダ内の exe を使用してください。

[BonCasLink v1.10-patch](https://github.com/miraao/BonCasLink/releases/download/v1.10-patch/BonCasLink_v1.10-patch.zip)

## 64bit 対応版について

VS2019 を使って x64 でビルドしたモジュールです。

ビルド時にエラーとなった部分を一部修正しています。  
（ GWL_USERDATA -> GWLP_USERDATA、GetWindowLong -> GetWindowLongPtr など）  
他の改変は行っていません。  

動作には VC++2019 のランタイムが必要です。

問題があったり、公式が更新されるようなら削除します。

### 動作確認環境
- Server … WindowsXP SP3 x86
- Proxy … Windows7 x64
- EpgDataCap_Bon … 人柱版10.3

## 注意事項

- 一般的にスマートカードには個人を特定可能な情報が含まれている可能性があります。
- 本ソフトウェアは通信の暗号化を行わないためこのリスクを十分に考えた上でご使用ください。
- 本ソフトウェアの警告を無視し LAN 以外のネットワークにおいて使用した結果発生したいかなる損害も作者に責任を求めないこととします。
- 本ソフトウェアの動作には「Microsoft Visual C++ 2019 再頒布可能パッケージ」が必要です。
  - 32bit (x86) 版 … https://aka.ms/vs/16/release/VC_redist.x86.exe
  - 64bit (x64) 版 … https://aka.ms/vs/16/release/VC_redist.x64.exe

## ライセンスについて

- 本パッケージに含まれる全てのソースコード、バイナリについて著作権は一切主張しません。
- オリジナルのまま又は改変し、各自のソフトウェアに自由に添付、組み込むことができます。
- 但し GPL に従うことを要求しますのでこれらを行う場合はソースコードの開示が必須となります。
- このとき本ソフトウェアの著作権表示を行うかどうかは任意です。
- 通信プロトコルの変更及び追加を含むソースコードの改変は一切許可できません。
- 上記を禁止するいかなる根拠または拘束力も作者にはありませんがこれに反して、作成した改変物を配布する場合は下記を要求します。
- ソフトウェアの名称を「BonCasLink」以外に変更すること。
- 「拡張ツール中の人」の著作権表示を一切行わないこと。
- ソースコードの流用元の表示を一切行わないこと。
- ビルドに必要な環境
  - Microsoft Visual Studio 2019 以上　※ MFC が必要
  - Microsoft Windows SDK v6.0 以上

## 使用方法

1. スマートカードリーダが接続された PC で「BonCasServer」を起動します。
2. LAN に参加している他の PC で「BonCasProxy」を起動します。
3. タスクトレイのアイコンをクリックし、メニューから「クライアントの設定」をクリックします。
4. IP アドレスに「BonCasServer」を動作させている PC の IP アドレスを入力して OK をクリックします。
5. 「BonCasProxy」を動作させているPCでスマートカードを使用するアプリケーションを起動します。
   - 正常に接続が行われカードリーダの共有が行われればタスクトレイのアイコンが赤色に変化します。
   - ※ファイヤウォールやフィルタリングを使用している場合は共有に使用するポートを開放してください。

## サービス版について

「BonCasService.exe」を使用することでサーバを Windows サービスとして動作させることができます。

1. 「サービスインストール.bat」を実行してサービスに登録します。
2. 「サービス開始.bat」を実行するとサービスを開始することができます。
   - ※ PC 起動時にサービスを自動的に開始するにはサービスの管理でスタートアップの種類を「自動」に設定してください。
   - ※ポートの設定は BonCasService.ini を直接編集して設定してください。

## サポート、連絡先

- 連絡先 … 拡張ツール中の人 nakanohito@2sen.dip.jp
- 公式サイト … http://2sen.dip.jp/friio/

## 参考文献

- DINOP.COM … http://www.dinop.com/vc/service_wizard.html
- EZ-NET 研究室 … http://program.station.ez-net.jp/special/vc/atl/service_vc2003.asp

## 更新履歴

- **Ver.1.10**
  - 動的にスマートカードサービスを使用するアプリケーションにも対応した。
  - BonCasStub.dll の更新のみ
- **Ver.1.01**
  - サービス版を追加
  - BonCasServerの設定を即時反映するようにした。
- **Ver.1.00**
  - 初回リリース
