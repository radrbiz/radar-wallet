# Radar Wallet

Radar Wallet - A cold wallet GUI tool for Radar network.

## How to Build

Radar Wallet uses Qt5 to implement its GUI interface. To build it, you need latest Qt5 installed on your system.

Openssl 1.0.x is needed to support wallet data encryption. You need to install it by yourself. We offer a prebuild Windows version of openssl in this repository, which is compiled by Visual Studio 2017. You can use your own build if you want.

Use Qt Creator to open `RadarWallet.pro`, and run build.

A command line tool `radar-tool` is used to generate address and sign transactions, you can find it in this project - [radrbiz/radar-tool](https://github.com/radrbiz/radar-tool). Copy this tool to the same path of the executable binary - RadarWallet.

That's all. Enjoy using it.

## License

```
This file is part of Radar Wallet: http://www.radarlab.org
Copyright (c) 2015 - 2018 Radar Laboratory

Permission to use, copy, modify, and/or distribute this software for any
purpose  with  or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE  SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES  OF
MERCHANTABILITY  AND  FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY  SPECIAL ,  DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER  RESULTING  FROM  LOSS  OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION  OF  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
```
