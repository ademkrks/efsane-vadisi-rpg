# Efsane Vadisi: Zindanın Sırrı

Terminalde oynanan, C++17 ile yazılmış bir rogue-lite RPG. Karakterini seç, zindana in, canavarları öldür, loot topla, kasabaya dön, güçlen, tekrar in.

## Özellikler

- 6 farklı sınıf: Savaşçı, Büyücü, Hırsız, Paladin, Okçu ve gizli bir sınıf olan Nekromancer (belirli şartlarda açılıyor)
- Sıra tabanlı savaş sistemi, kritik vuruşlar ve zehir/yanma/kanama/donma gibi durum etkileri
- 5 farklı bölge: Zindan, Lav Mağarası, Büyülü Orman, Ölü Şehir, Kar Kalesi
- Normal, Elite, Mini-Boss, Boss ve Gizli Boss düşman tipleri, her biri kendi davranış yapay zekasıyla (agresif, savunmacı, taktikçi, berserker vb.)
- Envanter ve ekipman sistemi, nadirlik seviyeleri (Yaygın'dan Efsanevi'ye)
- Demirci, simyacı ve taverna gibi kasaba mekanları
- Yetenek ağacı, görev panosu, başarımlar ve evcil hayvan sistemi
- Arena modu ve tavernada oynanabilen mini oyunlar (zar düellosu, kart tahmini, parmak tuzağı)
- Ruh Taşı sistemi ile ölüm sonrası kalıcı ilerleme (meta-progression)
- Oyunu kaydetme/yükleme desteği
- Renkli terminal arayüzü (ANSI escape kodları) ve can/mana çubukları

## Derleme

**MinGW / GCC:**
```bash
g++ -std=c++17 -O2 -o oyun.exe main.cpp
```

**MSVC:**
```bash
cl /std:c++17 /O2 /EHsc main.cpp /Fe:oyun.exe
```

## Çalıştırma

Windows:
```bash
oyun.exe
```

Linux / macOS:
```bash
g++ -std=c++17 -O2 -o oyun main.cpp
./oyun
```

> Not: Renklerin ve emoji karakterlerinin düzgün görünmesi için terminalinizin UTF-8 ve ANSI escape kodlarını desteklemesi gerekiyor. Windows'ta modern bir terminal (Windows Terminal, PowerShell 7+) önerilir.

## Nasıl Oynanır

1. Yeni oyun başlat, karakterine bir isim ve sınıf seç
2. Kasabadan zindana in, düşmanlarla savaş, altın ve eşya topla
3. Kazandığın altınla demirci ve simyacıdan ekipman/iksir al
4. Yetenek ağacında karakterini geliştir, görevleri tamamla
5. Öldüğünde bile kazandığın Ruh Taşlarıyla bir sonraki karakterine kalıcı avantajlar sağla

## Gereksinimler

- C++17 destekleyen bir derleyici (GCC, Clang veya MSVC)
- Standart kütüphane dışında ek bağımlılık yok

## Lisans

Bu proje kişisel bir eğlence/öğrenme çalışmasıdır. Kullanmak, değiştirmek ve üzerine geliştirmek serbesttir.
