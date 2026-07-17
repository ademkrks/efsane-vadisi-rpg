# Efsane Vadisi: Zindanın Sırrı

Terminal tabanlı, C++17 ile yazılmış bir rogue-lite RPG. Karakterini seç, zindana in, canavarları öldür, loot topla, kasabaya dön, güçlen — ve bir gün Gizli Patronla yüzleş.

---

## Özellikler

### 🧙 6 Oynanabilir Sınıf
| Sınıf | Öne Çıkan Stat | Özellik |
|---|---|---|
| Savaşçı 🛡️ | Yüksek can / savunma | Dengeli başlangıç sınıfı |
| Büyücü 🔮 | Yüksek mana / büyü hasarı | Mana yönetimi önemli |
| Hırsız 🗡️ | Yüksek kritik şans | Altın ve drop bonusu |
| Paladin ✜️ | Yüksek savunma + orta mana | Grup ve destek odaklı |
| Okçu 🏹 | Yüksek kritik şans | Uzak mesafe avantajı |
| **Nekromancer 💀** | Yüksek mana | **Gizli sınıf — 3 zafer kazanınca açılır** |

### ⚔️ Savaş Sistemi
- Sıra tabanlı, aksiyon menüsü ile yönetilen dövüş
- **Durum etkileri:** Zehir 🟢, Yanma 🔥, Kanama 🩸, Donma ❄️
- **Combo sistemi:** üst üste vuruşlarla artan hasar bonusu
- **Kritik vuruşlar:** sınıfa göre değişen şans ve çarpan
- **Evcil hayvan:** savaşta yan saldırı yapar, sevgi puanıyla büyür
- **Yoldaş sistemi:** savaşta ek destek sağlar

### 👹 Düşman Tipleri & Yapay Zeka
| Tür | Çıkma Koşulu | AI Davranışı |
|---|---|---|
| Normal | Her katta | Agresif / Savunmacı / Taktikçi / Zehirci / Hilekâr / Berserker |
| Elite | Rastgele (%15) | Güçlendirilmiş normal düşman |
| Mini-Boss | Kat % 5 == 0 | Bölgeye özel, 1 iksir taşır |
| Boss | Kat % 10 == 0 (%90) | 3 fazlı yapay zeka, bölgeye özel patron |
| **Gizli Patron** 🔮 | Kat % 10 == 0 (**%10 şans**) | Boss'un **%50 daha güçlü** versiyonu, iksir kullanmaz |

Her Boss/Gizli Patron kendi bölge lore'una sahip:
- 🏰 **Zindan Lordu Elesthia** — Kor Nefes & Kaos Patlaması
- 🌋 **Ateş Tanrısı Ignar** — Meteor & Volkanik Patlama
- 🌿 **Orman Büyücüsü Sylvara** — Orman Gazabı
- 💀 **Liç Kral Morthaur** — Ölüm Çığlığı & Lanet Yağmuru
- 👑 **Karanlık Lord Malachar** — Kaos Enerjisi (final boss)

### 🗺️ 5 Bölge
| Bölge | Tema | Hakim Efekt |
|---|---|---|
| Antika Zindan 🏰 | Karanlık taş koridorlar | Kanama |
| Lav Mağaraları 🌋 | Yanardağ içleri | Yanma |
| Büyülü Orman 🌿 | Kadim orman | Zehir |
| Ölüler Şehri 💀 | Ölümsüz şehri | Zehir + Kanama |
| Karanlık Kale 🏯 | Son bölge | Yanma + Kanama |

### 💎 Ganimet & Eşya Sistemi
- **Nadirlik:** Yaygın → Sıradan → Nadir → Destansı → **Efsanevi**
- **Boss'lar:** %15 Efsanevi, %20 Destansı drop şansı
- **Gizli Patron:** 🔮 **garanti Efsanevi ganimet** (seviyeye ölçeklenen özel silah/zırh)
- **Özel eşya efektleri:** vampir, yanan, dondurucu, lanet, kahraman
- Silah ve zırh kuşanma, envanter yönetimi (8 slot)

### 🔮 Gizli Patron & Efsanevi Şans Sistemi
- Her boss katında **%10 ihtimalle** normal boss yerine Gizli Patron gelir
- Gizli Patron: tüm stat'lar ×1.5, hasar ×1.5, eylem katsayıları ×1.2
- Gizli Patronu yenince **Efsanevi Şans** kalıcı olarak açılır (`meta.dat`'a kaydedilir)
- Açıldıktan sonra normal Boss'lar da daha sık Efsanevi eşya bırakır

### 🏰 Kasaba Mekanları
- **Demirci:** Altınla silah/zırh satın al veya yükselt
- **Simyacı (Crafting):** Topladığın malzemelerden iksir ve özel eşya üret
- **Taverna:** 3 mini oyun — Zar Düellosu, Kart Tahmini, Parmak Tuzağı
- **Yetenek Ağacı:** kazanılan puanlarla aktif/pasif yetenek geliştirme
- **Görev Panosu:** aktif görevleri takip et, ödüller kazan
- **Arena:** sonsuz dalga modu, rekor kır
- **Meta-İlerleme Menüsü:** Ruh Taşı harcama ve kilit açma ekranı

### 🔄 Ölüm Sonrası Kalıcı İlerleme (Meta-Progression)
Ölseniz de her şeyi kaybetmezsiniz:

| Meta Özellik | Nasıl Kazanılır |
|---|---|
| **Ruh Taşı** 💎 | Her ölümde `kat × 2 + seviye` kadar |
| **Nekromancer Sınıfı** | Toplamda 3 zafer kazan |
| **Efsanevi Şans** 🔮 | Gizli Patronu ilk kez yen |
| **Miras Eşyası** | Öldüğünde silahın yarı statla bir sonraki kahramana geçer |

Tüm meta veriler `meta.dat` dosyasına otomatik kaydedilir.

### 🎲 Dinamik Olay Sistemi
Zindan katlarında her turda %30 ihtimalle rastgele olay tetiklenir (20+ farklı olay):
- Tuzaklar (ok tuzağı, zehirli gaz)
- Kutsal çeşme, gizli oda, gizli hazine
- NPC gezgin, mistik heykel, büyülü yazıt
- Kara pazar tüccarı (efsanevi eşya veya ruh satışı)
- Bölgeye özel olaylar (lav havuzu, orman bitkisi, kaos enerjisi...)

---

## Derleme

**MinGW / GCC (önerilen):**
```bash
g++ -std=c++17 -O2 -o efsane_vadisi.exe main.cpp
```

**MSVC:**
```bash
cl /std:c++17 /O2 /EHsc main.cpp /Fe:efsane_vadisi.exe
```

**Linux / macOS:**
```bash
g++ -std=c++17 -O2 -o efsane_vadisi main.cpp
./efsane_vadisi
```

> **Not:** Renklerin ve emoji karakterlerinin düzgün görünmesi için terminalinizin UTF-8 ve ANSI escape kodlarını desteklemesi gerekir. Windows'ta **Windows Terminal** veya **PowerShell 7+** önerilir.

---

## Nasıl Oynanır

1. Yeni oyun başlat → karakterine isim ve sınıf seç
2. Kasabadan zindana gir → bölge ve katı seç
3. Düşmanlarla savaş → altın, eşya ve malzeme topla
4. Kasabaya dön → demirci, simyacı, görev panosu, yetenek ağacı
5. Boss katlarında gözünü aç — **%10 ihtimalle Gizli Patron gelir!**
6. Gizli Patronu yen → kalıcı **Efsanevi Şans** açılır + garanti Efsanevi ganimet
7. Öldüğünde Ruh Taşları kazanırsın → bir sonraki karaktere meta avantaj

---

## Gereksinimler

- C++17 destekleyen derleyici (GCC 7+, Clang 5+, MSVC 2017+)
- Standart kütüphane dışında hiçbir bağımlılık yok
- Kayıt dosyası: `meta.dat` (otomatik oluşturulur, aynı dizinde)

---

## Lisans

Kişisel eğlence / öğrenme projesidir. Kullanmak, değiştirmek ve üzerine geliştirmek serbesttir.
