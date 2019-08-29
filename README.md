# 古典モンテカルロ法の実行時間テスト

[![MIT License](http://img.shields.io/badge/license-MIT-blue.svg?style=flat)](LICENSE)

## 概要

三次元古典XY模型におけるWolff-Swendsen-Wangアルゴリズムの実行速度を測定する。

## 計算の内容

三次元古典XY模型の臨界点直上において、T_LOOPで指定された回数だけループを
実行した後、O_LOOPをMPIプロセスで割った回数だけループを実行する時間を測定する。

## ビルド方法

makefile.optというファイルを用意し、CCにC++コンパイラを、CPPFLAGSに
コンパイルオプションを、LDFLAGSにリンクオプションを定義する。
以下は設定例である。

```makefile
CC=mpicxx
CPPFLAGS=-O3 -std=c++11
LDFLAGS=-lmpi
```

ビルドすると`xy3d`というファイルが作成される。

## 実行方法

MPIプログラムとして実行する。引数は必要ない。
以下は実行例である。

```sh
$ mpirun -np 8 ./xy3d
MC benchmark: Version 1.00
L = 32
Procs = 8
Thermalize Loop = 1000
Total Samples = 153600
Observe Loops = 19200
T = 2.2016
Magnetization = 0.186431 +- 0.000611628
Binder Ratio = 0.883666 +- 0.00134068

Elapsed Time = 105.052
MCs/sec/procs = 182.767
Total MCs/sec = 1462.14
```

出力された「Total MCs/sec」が「一秒あたり何回サンプリングできたか」を示す値であり、大きいほどよい。

測定量はMagnetization、及びBinder Ratioであり、「測定量 +- 誤差」として表示されている。

## ベンチマーク方法

* プログラムは、原則としてディレクティブを追加すること以外の修正は認めない。
* MPIのプロセス数は自由に設定してかまわない。
* 1ノードにおいて実行すること。複数ノードにまたがっての実行は必要ない。
* プログラムの出力結果を全て報告すること。特に出力された「Magnetization」及び「Binder Ratio」が、実行例として挙げた数値と有効数字で二桁以上一致していること。

## ライセンス

This software is released under the MIT License, see [LICENSE](LICENSE).
