VS2010を使ってx64でビルドしたモジュールです。

ビルド時にエラーとなった部分を一部修正しています。
（GWL_USERDATA -> GWLP_USERDATA、GetWindowLong -> GetWindowLongPtrなど）
他の改変は行っていません。

動作にはVC++2010のランタイムが必要です。

Microsoft Visual C++ 2010 再頒布可能パッケージ (x64)
http://www.microsoft.com/downloads/details.aspx?familyid=BD512D9E-43C8-4655-81BF-9350143D5867&displaylang=ja

問題があったり、公式が更新されるようなら削除します。


動作確認環境
Server:WindowsXP SP3 x86
Proxy:Windows7 x64
EpgDataCap_Bon（人柱版１０．３）
