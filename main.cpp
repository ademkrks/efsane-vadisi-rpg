/*
 * ╔══════════════════════════════════════════════════════════════╗
 * ║        EFSANE VADİSİ: ZİNDANIN SIRRI  —  v2.0              ║
 * ║        Terminal RPG  |  C++17                               ║
 * ╠══════════════════════════════════════════════════════════════╣
 * ║  Derleme (MinGW / GCC):                                     ║
 * ║    g++ -std=c++17 -O2 -o oyun.exe main.cpp                  ║
 * ║  Derleme (MSVC):                                            ║
 * ║    cl /std:c++17 /O2 /EHsc main.cpp /Fe:oyun.exe            ║
 * ╚══════════════════════════════════════════════════════════════╝
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <map>
#include <iomanip>
#include <chrono>
#include <thread>
#include <cmath>

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

// ═══════════════════════════════════════════════════════════════
//  §1  ANSI Renk Sabitleri
// ═══════════════════════════════════════════════════════════════
static const std::string RS  = "\033[0m";
static const std::string BD  = "\033[1m";
static const std::string DM  = "\033[2m";
static const std::string KR  = "\033[31m";
static const std::string YE  = "\033[32m";
static const std::string SR  = "\033[33m";
static const std::string MB  = "\033[34m";
static const std::string MJ  = "\033[35m";
static const std::string SY  = "\033[36m";
static const std::string GB  = "\033[37m";
static const std::string PKR = "\033[91m";
static const std::string PYE = "\033[92m";
static const std::string PSR = "\033[93m";
static const std::string PMB = "\033[94m";
static const std::string PMJ = "\033[95m";
static const std::string PCY = "\033[96m";
static const std::string PW  = "\033[97m";

// ═══════════════════════════════════════════════════════════════
//  §2  Yardımcı Fonksiyonlar
// ═══════════════════════════════════════════════════════════════
void terminalAyarla() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(h, &mode);
    SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}

int rng(int a, int b) {
    if (b <= a) return a;
    return a + (std::rand() % (b - a + 1));
}

// Gauss benzeri hasar üretimi — her saldırı farklı hissettirsin
int hasarUret(int temel) {
    if (temel <= 0) return 1;
    int v = std::max(1, temel / 4);
    return rng(temel - v, temel + v);
}

void temizle() { std::cin.clear(); std::cin.ignore(10000, '\n'); }

int guvenliOku(int minV = 0, int maxV = 99) {
    int v;
    while (true) {
        std::cin >> v;
        if (!std::cin.fail() && v >= minV && v <= maxV) return v;
        temizle();
        std::cout << PSR << "Geçersiz giriş, tekrar dene: " << RS << std::flush;
    }
}

void bekle() {
    std::cout << DM << "\n  [ ENTER ile devam et... ]" << RS << std::flush;
    temizle(); std::cin.get();
}

// Yüzde kontrolü
bool sansBasi(int yuzde) { return rng(0, 99) < yuzde; }

// Can çubuğu
std::string canBari(int cur, int maks, int len = 16) {
    if (maks <= 0) return "[" + std::string(len, '-') + "]";
    double r = std::max(0.0, std::min(1.0, (double)cur / maks));
    int dolu = (int)(r * len);
    std::string rk = (r > 0.6) ? PYE : (r > 0.3) ? PSR : PKR;
    std::string bar = "[" + rk;
    for (int i = 0; i < len; ++i) bar += (i < dolu) ? "\xe2\x96\x88" : "\xe2\x96\x91";
    bar += RS + "] " + BD + std::to_string(cur) + "/" + std::to_string(maks) + RS;
    return bar;
}

std::string manaBari(int cur, int maks) {
    if (maks <= 0) return "0";
    int len = 8;
    int dolu = (int)(std::max(0.0, std::min(1.0, (double)cur / maks)) * len);
    std::string bar = "[" + PCY;
    for (int i = 0; i < len; ++i) bar += (i < dolu) ? "\xe2\x96\xaa" : "\xc2\xb7";
    bar += RS + "] " + std::to_string(cur);
    return bar;
}

std::string hasarRenk(int d) {
    if (d >= 60) return PKR + BD + std::to_string(d) + RS;
    if (d >= 30) return KR  + std::to_string(d) + RS;
    if (d >= 15) return PSR + std::to_string(d) + RS;
    return GB + std::to_string(d) + RS;
}

void cizgi(int n = 60, char c = '-') {
    std::cout << SY;
    for (int i = 0; i < n; ++i) std::cout << c;
    std::cout << RS << "\n";
}
void dcizgi(int n = 60) {
    std::cout << BD + SY;
    for (int i = 0; i < n; ++i) std::cout << '=';
    std::cout << RS << "\n";
}

void baslik(const std::string& s, const std::string& renk = PCY) {
    dcizgi();
    std::cout << renk + BD;
    int bos = std::max(0, (60 - (int)s.size()) / 2);
    for (int i = 0; i < bos; ++i) std::cout << ' ';
    std::cout << s << RS << "\n";
    dcizgi();
}

// ═══════════════════════════════════════════════════════════════
//  §3  Enum'lar
// ═══════════════════════════════════════════════════════════════
enum class EsyaTuru  { Silah, Zirh };
enum class Nadir     { Common=0, Uncommon, Rare, Epic, Legendary };
enum class KarSinif  { Savasci=1, Buyucu, Hirsiz, Paladin, Okcu, Nekromancer };
enum class DusmanTur { Normal, Elite, MiniBoss, Boss, GizliBoss };
enum class Bolge     { Zindan=0, LavMag, BuyOrman, OlSehir, KarKale };
enum class Davranis  { Agresif, Savunmaci, Taktikci, Zehirci,
                       Berserker, Hilekâr, Destekci, BossAI };
enum class EfektTip  { YOK, Zehir, Yanma, Kanama, Donma };

// ═══════════════════════════════════════════════════════════════
//  §4  Temel Veri Yapıları
// ═══════════════════════════════════════════════════════════════
struct Esya {
    std::string ad    = "Bos";
    EsyaTuru    tur   = EsyaTuru::Silah;
    Nadir       nadir = Nadir::Common;
    int bHasar = 0, bSav = 0, bCan = 0, bMana = 0;
    int deger = 0, sevGer = 1, arti = 0;
    std::string ozel  = "";
};

std::string nadirRenk(Nadir n) {
    switch (n) {
        case Nadir::Uncommon:  return PYE;
        case Nadir::Rare:      return PMB;
        case Nadir::Epic:      return PMJ;
        case Nadir::Legendary: return PSR + BD;
        default:               return GB;
    }
}
std::string nadirEtiket(Nadir n) {
    switch (n) {
        case Nadir::Common:    return "[Yaygın]";
        case Nadir::Uncommon:  return "[Alışılmamış]";
        case Nadir::Rare:      return "[Nadir]";
        case Nadir::Epic:      return "[Epik]";
        case Nadir::Legendary: return "[Efsanevi]";
        default:               return "";
    }
}

struct Gorev {
    int id;
    std::string ad, aciklama;
    int hedef, mevcut;
    int aOdul, xOdul;
    bool tamam;
    std::string kriter; // "" = herhangi, "boss", "Goblin", "altin" vb.
};

struct Basarim {
    std::string ad, aciklama;
    bool kazanildi = false;
};

struct EvcilHayvan {
    std::string ad = "", tur = "";
    int seviye = 1, sevgi = 50;
    int bHasar = 0, bSav = 0;
    std::string yetenek = "";
    bool var = false;
};

struct Malzeme { std::string ad; int miktar = 0; };

struct MetaVeri {
    int  ruhTasi     = 0;
    int  olum        = 0;
    int  zafer       = 0;
    bool nekAcik     = false;
    bool efsaneviAcik= false;
    std::string mirasAd  = "";
    int  mirasHasar  = 0;
    int  mirasSav    = 0;

    void kaydet() const {
        std::ofstream f("meta.dat");
        if (!f) return;
        f << ruhTasi << " " << olum << " " << zafer << " "
          << (int)nekAcik << " " << (int)efsaneviAcik << "\n"
          << mirasAd << "\n" << mirasHasar << " " << mirasSav << "\n";
    }
    void yukle() {
        std::ifstream f("meta.dat");
        if (!f) return;
        int n, e;
        f >> ruhTasi >> olum >> zafer >> n >> e;
        nekAcik = (n != 0); efsaneviAcik = (e != 0);
        f.ignore();
        std::getline(f, mirasAd);
        f >> mirasHasar >> mirasSav;
    }
};

// ═══════════════════════════════════════════════════════════════
//  §5  Düşman — AI Sistemi
// ═══════════════════════════════════════════════════════════════
struct DusmanEylem {
    std::string isim;
    std::string telgraf;     // 1 tur önce gösterilen uyarı
    std::string mesaj;       // vuruş mesajı
    int         katsayi;     // hasar çarpanı × 100
    int         varyans;     // ek varyans puanı
    EfektTip    efekt        = EfektTip::YOK;
    int         efektSure    = 0;
    bool        iyilesme     = false;
    int         iyilestirYzd = 0; // maksCan'ın % kaçını iyileştirir
};

class Dusman {
public:
    std::string  ad, aciklama;
    int          can, maksCan, hasar, sav;
    int          xp, altin;
    DusmanTur    turu;
    Davranis     davranis;
    Bolge        bolge;

    int zehirSure = 0, yanmaSure = 0, kanamaSure = 0;
    const int zHas = 9, yHas = 12, kHas = 10;

    bool berserk       = false;
    int  turNo         = 0;
    int  aktifFaz      = 1;
    bool hazirlanma    = false;
    std::string bekleSaldiri;
    std::string oncekiEylem;
    int  tekrarSay     = 0;
    int  iksirSay      = 0;

    std::vector<DusmanEylem> eylemler;

    bool hayatta() const { return can > 0; }

    void dotHasari() {
        if (zehirSure > 0) {
            can -= zHas; --zehirSure;
            std::cout << MJ << "🟢 " << ad << " zehirden kıvrınıyor! -" << zHas << " can\n" << RS;
        }
        if (yanmaSure > 0) {
            can -= yHas; --yanmaSure;
            std::cout << PKR << "🔥 " << ad << " yanıyor! -" << yHas << " can\n" << RS;
        }
        if (kanamaSure > 0) {
            can -= kHas; --kanamaSure;
            std::cout << KR << "🩸 " << ad << " kanıyor! -" << kHas << " can\n" << RS;
        }
    }

    // ─── Ana AI Karar Motoru ──────────────────────────────────
    DusmanEylem eylemSec(int oyCan, int oyMaksCan,
                         bool oyZehirli, bool oyYaniyor,
                         int combo, KarSinif oySinif)
    {
        ++turNo;
        double hpOran  = (double)can / maksCan;
        double ohpOran = (double)oyCan / oyMaksCan;
        (void)ohpOran; (void)oySinif;

        // Berserker aktivasyonu
        if (davranis == Davranis::Berserker && hpOran < 0.3 && !berserk) {
            berserk = true;
            hasar   = (int)(hasar * 1.85);
            sav     = std::max(0, sav - 2);
            std::cout << PKR + BD << "\n\xf0\x9f\x92\xa2 " << ad
                      << " DELIRDI! Hasar 1.85x!\n" << RS;
        }

        // Boss faz geçişleri
        if (turu == DusmanTur::Boss || turu == DusmanTur::GizliBoss) {
            if (aktifFaz == 1 && hpOran < 0.60) {
                aktifFaz = 2;
                hasar += hasar / 4;
                dcizgi(55);
                std::cout << PKR + BD << "\xe2\x9a\xa1 " << ad
                          << " — FAZ 2! Saldırılar güçlendi!\n" << RS;
                dcizgi(55);
            }
            if (aktifFaz == 2 && hpOran < 0.30) {
                aktifFaz = 3;
                hasar    = (int)(hasar * 1.35);
                sav      = std::max(0, sav - 3);
                hazirlanma = true;
                bekleSaldiri = eylemler.empty() ? "" : eylemler.back().isim;
                dcizgi(55);
                std::cout << PKR + BD << "\xf0\x9f\x92\x80 " << ad
                          << " — FAZ 3! MUTLAK GÜÇ!\n" << RS;
                dcizgi(55);
            }
            // Her 5 turda büyük saldırı hazırlığı
            if (!hazirlanma && turNo % 5 == 4 && eylemler.size() >= 2) {
                hazirlanma   = true;
                bekleSaldiri = eylemler.back().isim;
                DusmanEylem d2;
                d2.isim    = "Hazırlanma";
                d2.telgraf = "\xe2\x9a\xa0\xef\xb8\x8f  " + ad + " " + bekleSaldiri + " için güç topluyor!";
                d2.mesaj   = "...";
                d2.katsayi = 0; d2.varyans = 0;
                return d2;
            }
        }

        // Hazırlık turu → büyük saldırı
        if (hazirlanma) {
            hazirlanma = false;
            for (auto& e : eylemler)
                if (e.isim == bekleSaldiri) return e;
        }

        // İyileşme
        if (iksirSay > 0 && hpOran < 0.35 &&
            (davranis == Davranis::Savunmaci || davranis == Davranis::Destekci)) {
            --iksirSay;
            int h = (int)(maksCan * 0.28);
            can = std::min(maksCan, can + h);
            std::cout << PYE << "\xf0\x9f\x92\x8a " << ad << " yarasarı sardı! +" << h << " can!\n" << RS;
            DusmanEylem ie;
            ie.isim = "İyileşme"; ie.katsayi = 0; ie.varyans = 0;
            ie.mesaj = ad + " toparlandı.";
            return ie;
        }

        // Taktikçi: oyuncu debuffsuzsa zehir öncelik
        if (davranis == Davranis::Taktikci && !oyZehirli && turNo % 3 == 1) {
            for (auto& e : eylemler)
                if (e.efekt == EfektTip::Zehir) return e;
        }

        // Zehirci: zehirsizken zehir önce
        if (davranis == Davranis::Zehirci && !oyZehirli) {
            for (auto& e : eylemler)
                if (e.efekt == EfektTip::Zehir || e.efekt == EfektTip::Yanma) return e;
        }

        // Hilekâr: yüksek comboya dodge
        if (davranis == Davranis::Hilekâr && combo >= 3 && sansBasi(40)) {
            DusmanEylem dg;
            dg.isim    = "Dodge";
            dg.mesaj   = ad + " sıçrayıp saldırıdan kaçtı!";
            dg.katsayi = 0; dg.varyans = 0;
            return dg;
        }

        // Zehirli yanıyorken oyuncuya fırsat saldırısı
        if (oyYaniyor && sansBasi(30)) {
            for (auto& e : eylemler)
                if (e.katsayi >= 130) return e;
        }

        // Aynı eylemi 3 kereden fazla yapma
        std::vector<DusmanEylem*> adaylar;
        for (auto& e : eylemler) {
            if (e.isim == oncekiEylem && tekrarSay >= 2) continue;
            adaylar.push_back(&e);
        }
        if (adaylar.empty())
            for (auto& e : eylemler) adaylar.push_back(&e);

        // Ağırlıklı seçim
        std::vector<int> agirl;
        int toplam = 0;
        for (auto* ep : adaylar) {
            int a = 10;
            if (davranis == Davranis::Agresif && ep->katsayi >= 120) a += 8;
            if (berserk) a += 6;
            if (davranis == Davranis::Taktikci && ep->efekt != EfektTip::YOK) a += 7;
            if (ep->isim == oncekiEylem) a = std::max(1, a - 5);
            agirl.push_back(a);
            toplam += a;
        }

        int r = rng(0, toplam - 1), kum = 0;
        for (size_t i = 0; i < adaylar.size(); ++i) {
            kum += agirl[i];
            if (r < kum) {
                if (adaylar[i]->isim == oncekiEylem) ++tekrarSay;
                else tekrarSay = 0;
                oncekiEylem = adaylar[i]->isim;
                return *adaylar[i];
            }
        }
        return eylemler[0];
    }
};

// ═══════════════════════════════════════════════════════════════
//  §6  Düşman Fabrikası (5 bölge × 30+ düşman + boss'lar)
// ═══════════════════════════════════════════════════════════════
Dusman dusmanYarat(int kat, DusmanTur turu, Bolge bolge) {
    Dusman d;
    d.turu  = turu;
    d.bolge = bolge;
    int sf  = kat;

    // ── BOSS ────────────────────────────────────────────────
    if (turu == DusmanTur::Boss || turu == DusmanTur::GizliBoss) {
        d.davranis  = Davranis::BossAI;
        d.iksirSay  = 2;
        switch (bolge) {
          case Bolge::Zindan:
            d.ad = "\xf0\x9f\x94\xa5 Zindan Lordu Elesthia";
            d.aciklama = "Yüzyıllardır bu zindanı yönetir.";
            d.maksCan = d.can = 210 + sf*28;
            d.hasar   = 20 + sf*4;  d.sav = 6 + sf;
            d.xp      = 220 + sf*50; d.altin = 160 + sf*40;
            d.eylemler = {
              {"Pençe","","Keskin pençeleri kaburganıza geçti!",100,22},
              {"Kor Nefes","\xe2\x9a\xa0\xef\xb8\x8f Derin nefes aldı...","Alevler sizi kucakladı!",145,30,EfektTip::Yanma,3},
              {"Karanlık Çığlık","\xe2\x9a\xa0\xef\xb8\x8f Çığlık için hazırlanıyor...","Ses dalgaları tüm vücudu sarstı!",175,25},
              {"Kaos Patlaması","\xe2\x9a\xa0\xef\xb8\x8f TÜM GÜÇLE HAZIRLANIYOR!","MUTLAK YOK OLUŞ!",300,45,EfektTip::Yanma,4}
            };
            break;
          case Bolge::LavMag:
            d.ad = "\xf0\x9f\x8c\x8b Ateş Tanrısı Ignar";
            d.aciklama = "Magma'dan doğmuş, soğuğu bilmez.";
            d.maksCan = d.can = 240 + sf*32;
            d.hasar   = 23 + sf*5; d.sav = 8 + sf;
            d.xp      = 260 + sf*60; d.altin = 190 + sf*50;
            d.eylemler = {
              {"Magma Yumruk","","Kızgın yumruk tene işledi!",108,20,EfektTip::Yanma,2},
              {"Lav Fışkırtma","\xe2\x9a\xa0\xef\xb8\x8f Yerden çatlaklar oluşuyor...","Lav fışkırdı!",155,35,EfektTip::Yanma,3},
              {"Meteor","\xe2\x9a\xa0\xef\xb8\x8f Gökyüzü kararıyor...","Meteor yağıyor!",195,30},
              {"Volkanik Patlama","\xe2\x9a\xa0\xef\xb8\x8f Zemin titriyor!","VOLKAN PATLAMASI!",305,50,EfektTip::Yanma,5}
            };
            break;
          case Bolge::BuyOrman:
            d.ad = "\xf0\x9f\x8c\xbf Orman Büyücüsü Sylvara";
            d.aciklama = "Ormanın özüyle bütünleşmiş.";
            d.maksCan = d.can = 200 + sf*26;
            d.hasar   = 18 + sf*4; d.sav = 5 + sf;
            d.xp      = 240 + sf*55; d.altin = 175 + sf*45;
            d.iksirSay = 3;
            d.eylemler = {
              {"Diken","","Zehirli dikenler saplandı!",92,15,EfektTip::Zehir,4},
              {"Kök Sarma","\xe2\x9a\xa0\xef\xb8\x8f Zemin hareketleniyor...","Kökler sarıldı!",125,20,EfektTip::Zehir,3},
              {"Spor Bulutu","\xe2\x9a\xa0\xef\xb8\x8f Sarı bulut beliriyor...","Zehirli spor!",105,25,EfektTip::Zehir,5},
              {"Orman Gazabı","\xe2\x9a\xa0\xef\xb8\x8f Ağaçlar titriyor!","DOĞANIN GAZABI!",270,40,EfektTip::Zehir,4}
            };
            break;
          case Bolge::OlSehir:
            d.ad = "\xf0\x9f\x92\x80 Liç Kral Morthaur";
            d.aciklama = "Binlerce yıllık ölümsüz lord.";
            d.maksCan = d.can = 230 + sf*30;
            d.hasar   = 22 + sf*5; d.sav = 7 + sf;
            d.xp      = 280 + sf*65; d.altin = 210 + sf*55;
            d.iksirSay = 0;
            d.eylemler = {
              {"Ruh Emme","","Ruhunuzun bir parçasını çekti!",108,15},
              {"Ölüm Dokunuşu","\xe2\x9a\xa0\xef\xb8\x8f Soğuk bir el hissettiniz...","Ölüm enerjisi kemiklere işledi!",145,30,EfektTip::Kanama,3},
              {"Lanet Yağmuru","\xe2\x9a\xa0\xef\xb8\x8f Gökyüzü morarıyor...","Lanet her yandan çarpıyor!",172,25,EfektTip::Zehir,3},
              {"Ölüm Çığlığı","\xe2\x9a\xa0\xef\xb8\x8f ÖLÜM GELİYOR!","MUTLAK LANET!",325,50,EfektTip::Kanama,5}
            };
            break;
          case Bolge::KarKale:
            d.ad = "\xf0\x9f\x91\x91 Karanlık Lord Malachar";
            d.aciklama = "Son savaş. Son umut...";
            d.maksCan = d.can = 310 + sf*38;
            d.hasar   = 27 + sf*6; d.sav = 11 + sf;
            d.xp      = 420 + sf*80; d.altin = 320 + sf*70;
            d.iksirSay = 3;
            d.eylemler = {
              {"Kılıç Fırtınası","","Kılıç darbelerinin altında ezildiniz!",112,20,EfektTip::Kanama,2},
              {"Karanlık Işın","\xe2\x9a\xa0\xef\xb8\x8f Kollarını açtı...","Karanlık enerji dağıttı!",162,30,EfektTip::Yanma,3},
              {"Rüzgar Kesişi","\xe2\x9a\xa0\xef\xb8\x8f Hava titredi...","Görünmez bıçaklar kesti!",185,35,EfektTip::Kanama,3},
              {"KAOS PATLAMASI","\xe2\x9a\xa0\xef\xb8\x8f TÜM GÜÇLE HAZIRLANIYOR!","KAOS ENERJİSİ SERBEST!",390,60,EfektTip::Yanma,5}
            };
            break;
        }

        // GizliBoss ek güçlendirmesi: %50 daha fazla can/hasar, özel isimlendirme
        if (turu == DusmanTur::GizliBoss) {
            d.maksCan = d.can = (int)(d.maksCan * 1.5);
            d.hasar   = (int)(d.hasar   * 1.5);
            d.sav     = (int)(d.sav     * 1.3);
            d.xp      = (int)(d.xp      * 2.0);
            d.altin   = (int)(d.altin   * 2.5);
            d.iksirSay = 0;   // Gizli patron iksir kullanmaz
            // İsmin başına gizli patron etiketi ekle
            d.ad      = "\xf0\x9f\x94\xae " + d.ad + " [Gizli Patron]";
            d.aciklama = "Efsanelerde bahsedilen, az kişinin yüzleştiği varlık.";
            // Her eylemin katsayısını %20 güçlendir
            for (auto& e : d.eylemler)
                e.katsayi = (int)(e.katsayi * 1.2);
        }

        return d;
    }

    // ── MİNİBOSS ────────────────────────────────────────────
    if (turu == DusmanTur::MiniBoss) {
        d.iksirSay = 1;
        int t = rng(0, 1);
        switch (bolge) {
          case Bolge::Zindan:
            if (t == 0) {
                d.ad = "\xf0\x9f\x94\xb1 Lanetli Muhafız Goran";
                d.davranis = Davranis::Savunmaci;
                d.eylemler = {
                  {"Kalkan Vuruşu","","Kalkan kenarıyla çeneni kırdı!",95,20},
                  {"Kılıç Seli","\xe2\x9a\xa0\xef\xb8\x8f Duruşunu değiştirdi...","Üst üste kılıç!",132,25,EfektTip::Kanama,2},
                  {"Savaş Kükremesi","","Moralini bozdu!",78,10}
                };
            } else {
                d.ad = "\xf0\x9f\x8e\xad Gölge Suikastçısı Vex";
                d.davranis = Davranis::Hilekâr;
                d.eylemler = {
                  {"Gizli Saldırı","","Gölgeden sırtına sapladı!",122,30,EfektTip::Kanama,2},
                  {"Zehirli Hançer","","Zehirli hançer bilekte!",92,15,EfektTip::Zehir,3},
                  {"Sis Bombası","","Siste kayboldunuz!",62,10}
                };
            }
            d.maksCan = d.can = 105 + sf*20; d.hasar = 17 + sf*4;
            d.sav = 5 + sf; d.xp = 105 + sf*30; d.altin = 85 + sf*25;
            break;
          case Bolge::LavMag:
            d.ad = "\xf0\x9f\x94\xa5 Magma Devi Groth";
            d.davranis = Davranis::Berserker;
            d.maksCan = d.can = 135 + sf*22; d.hasar = 19 + sf*5;
            d.sav = 6 + sf; d.xp = 125 + sf*35; d.altin = 95 + sf*28;
            d.eylemler = {
              {"Lav Yumruk","","Kızgın yumruk göğsünü deldi!",112,25,EfektTip::Yanma,2},
              {"Zemin Çatlağı","\xe2\x9a\xa0\xef\xb8\x8f Groth yere yumruk attı...","Yerden lav fışkırdı!",142,35,EfektTip::Yanma,3},
              {"Magma Dalga","\xe2\x9a\xa0\xef\xb8\x8f Dalga geliyor!","Magma dalgası sardı!",162,30,EfektTip::Yanma,4}
            };
            break;
          case Bolge::BuyOrman:
            d.ad = "\xf0\x9f\x95\xb7\xef\xb8\x8f Dev Örümcek Aracna";
            d.davranis = Davranis::Zehirci;
            d.maksCan = d.can = 115 + sf*18; d.hasar = 15 + sf*3;
            d.sav = 4 + sf; d.xp = 115 + sf*32; d.altin = 88 + sf*26;
            d.iksirSay = 0;
            d.eylemler = {
              {"Zehirli Isırık","","Zehirli dişler ete saplandı!",88,15,EfektTip::Zehir,4},
              {"Ağ Fırlatma","","Yapışkan ağa hapsoldunuz!",70,10},
              {"Asit Tükürük","\xe2\x9a\xa0\xef\xb8\x8f Aracna zehir topluyor...","Asit teni eritiyor!",122,20,EfektTip::Zehir,5}
            };
            break;
          case Bolge::OlSehir:
            d.ad = "\xf0\x9f\xa7\x9f Dev Vampir Voras";
            d.davranis = Davranis::Hilekâr;
            d.maksCan = d.can = 120 + sf*20; d.hasar = 16 + sf*4;
            d.sav = 5 + sf; d.xp = 118 + sf*33; d.altin = 98 + sf*30;
            d.eylemler = {
              {"Kan Emme","","Boynu ısırıp kan emdi!",102,20,EfektTip::Kanama,2},
              {"Yarasa Saldırı","\xe2\x9a\xa0\xef\xb8\x8f Voras dönüşüyor...","Yarasa sürüsü çullandı!",132,25},
              {"Hipnoz","\xe2\x9a\xa0\xef\xb8\x8f Gözlerine bakıyor...","Hipnozla donup kaldınız!",68,15,EfektTip::Donma,1}
            };
            break;
          case Bolge::KarKale:
            d.ad = "\xe2\x9a\x94\xef\xb8\x8f Kara Şövalye Dravak";
            d.davranis = Davranis::Savunmaci;
            d.maksCan = d.can = 145 + sf*25; d.hasar = 21 + sf*5;
            d.sav = 9 + sf; d.xp = 135 + sf*38; d.altin = 115 + sf*35;
            d.iksirSay = 2;
            d.eylemler = {
              {"Demirleme","","Ağır zırhlı darbe göğüsten patladı!",107,20},
              {"Alev Kılıcı","\xe2\x9a\xa0\xef\xb8\x8f Kılıç alevlendi...","Alevli kılıç yakıyor!",142,30,EfektTip::Yanma,3},
              {"Karanlık Kalkan","","Kalkan saldırıyı yansıttı!",82,15}
            };
            break;
        }
        return d;
    }

    // ── NORMAL DÜŞMANLAR ────────────────────────────────────
    struct NDSifresi {
        std::string ad, aciklama;
        Davranis    dav;
        int baseHas, baseSav, baseXp, baseAltin, baseCan;
        std::vector<DusmanEylem> eylemler;
    };
    std::vector<NDSifresi> havuz;

    switch (bolge) {
      case Bolge::Zindan:
        havuz = {
          {"🐛 Zindan Balçığı","Yapışkan ve asitli.",Davranis::Agresif,
           8,1,25,13,45,{
            {"Asit Temas","","Asit tenin içine işledi!",100,30,EfektTip::Zehir,1},
            {"Balçık Fırlat","","Yapışkan balçık gözleri kapattı!",80,20}}},
          {"👺 Sinsi Goblin","Küçük ama sinsi, zehir taşır.",Davranis::Zehirci,
           10,2,40,27,55,{
            {"Hızlı Yumruk","","Hızlı yumruk attı!",90,25},
            {"Zehirli Çakı","\xe2\x9a\xa0\xef\xb8\x8f Goblin çakısını ısırdı...","Zehirli çakı bilekte!",87,20,EfektTip::Zehir,3},
            {"Kaçarak Vur","","Geri çekilip saldırdı!",112,30}}},
          {"🪨 Vahşi Ork","Kaba kuvvet canavarı.",Davranis::Berserker,
           14,4,68,50,80,{
            {"Balta Darbesi","","Balta kemiğe çarptı!",117,25,EfektTip::Kanama,2},
            {"Vahşi Saldırı","","Çılgınca saldırdı!",132,35,EfektTip::Kanama,3},
            {"Gümbürtü","\xe2\x9a\xa0\xef\xb8\x8f Ork derin nefes aldı...","TÜM GÜCÜYLE vurdu!",162,40}}},
          {"⚔️ Zindan Muhafızı","Eski asker, taktik bilir.",Davranis::Savunmaci,
           12,5,58,44,70,{
            {"Kılıç Vuruşu","","Teknik kılıç darbesi!",100,20},
            {"Kalkan İtişi","","Kalkanla itildiniz!",70,15},
            {"İkili Kılıç","\xe2\x9a\xa0\xef\xb8\x8f Muhafız pozisyon aldı...","İkili kılıç kombinasyonu!",142,30,EfektTip::Kanama,2}}}
        };
        break;
      case Bolge::LavMag:
        havuz = {
          {"🦟 Ateş Böceği","Küçük ama yakıcı.",Davranis::Agresif,
           12,2,35,21,45,{
            {"Ateş Isırığı","","Alevli ısırık!",97,25,EfektTip::Yanma,2},
            {"Alev Fırlat","","Kor top fırlattı!",112,30,EfektTip::Yanma,1}}},
          {"🗿 Magma Golemi","Yavaş ama çok güçlü.",Davranis::Savunmaci,
           18,7,72,57,95,{
            {"Magma Yumruğu","","Yumruk derini yaktı!",122,20,EfektTip::Yanma,2},
            {"Kaya Fırlatma","\xe2\x9a\xa0\xef\xb8\x8f Golemi kaya kırıyor...","Dev kaya kafada patladı!",148,30},
            {"Lav Zırh","","Zırh saldırıyı yaktı!",82,10,EfektTip::Yanma,1}}},
          {"🦎 Alev Salamanderi","Hızlı ve yakıcı zehir püskürür.",Davranis::Zehirci,
           11,3,50,40,60,{
            {"Ateş Tükürük","","Kor alevler yüzü yaktı!",102,25,EfektTip::Yanma,3},
            {"Kuyruk Darbesi","","Kuyruğu kaldırdı!",92,20}}},
          {"💀 Yanmış Şövalye","Ölümsüz, sadece yıkım arar.",Davranis::Berserker,
           16,5,74,60,85,{
            {"Kor Kılıç","","Kızgın kılıç dağıttı!",127,30,EfektTip::Yanma,2},
            {"Alev Fırtınası","\xe2\x9a\xa0\xef\xb8\x8f Şövalye sallanıyor...","Alev kasırgası sardı!",162,35,EfektTip::Yanma,4},
            {"Çılgın Darbe","","Kontrolsüz saçma sapan vurdu!",142,40}}}
        };
        break;
      case Bolge::BuyOrman:
        havuz = {
          {"🕷️ Zehirli Örümcek","8 gözlü ölüm fısıldayıcısı.",Davranis::Zehirci,
           9,2,34,20,50,{
            {"Zehirli Isırık","","Zehirli dişler saplandı!",87,20,EfektTip::Zehir,3},
            {"Ağ Fırlatma","","Ağa sarıldınız!",70,15}}},
          {"🧚 Gece Perisi","Görünmez olabilir, büyü yapar.",Davranis::Hilekâr,
           13,3,54,42,62,{
            {"Büyü Saldırı","","Büyüsel patlama!",102,25},
            {"Görünmez Saldırı","\xe2\x9a\xa0\xef\xb8\x8f Peri kayboldu...","Arkadan saldırdı!",143,35},
            {"Uyku Tozu","","Uyku tozu gözleri kapattı!",60,10,EfektTip::Donma,1}}},
          {"🌿 Büyülü Kök","Zemine bağlı, zehir salar.",Davranis::Taktikci,
           11,4,48,37,65,{
            {"Kök Sarma","","Kökler sardı!",92,15,EfektTip::Zehir,2},
            {"Diken Fırlatma","","Dikenler saplandı!",102,20,EfektTip::Zehir,3},
            {"Spor Fışkırtma","\xe2\x9a\xa0\xef\xb8\x8f Kökler titriyor...","Zehirli spor!",82,20,EfektTip::Zehir,4}}},
          {"🐺 Orman Kurdu","Sürüyle avlar.",Davranis::Agresif,
           15,3,60,47,72,{
            {"Isırık","","Derin ısırık!",112,25,EfektTip::Kanama,2},
            {"Sürü Hücumu","\xe2\x9a\xa0\xef\xb8\x8f Kurt uluyordu...","Sürü çullandı!",142,30,EfektTip::Kanama,3},
            {"Pençe","","Pençe teni parçaladı!",95,20,EfektTip::Kanama,1}}}
        };
        break;
      case Bolge::OlSehir:
        havuz = {
          {"💀 İskelet Savaşçısı","Kemikten yapılmış, yorulmaz.",Davranis::Agresif,
           13,4,50,37,68,{
            {"Kemik Yumruk","","Kemik yumruk kafada patladı!",102,20},
            {"Kılıç Saldırı","","Eski kılıç kesti!",117,25,EfektTip::Kanama,2},
            {"İskelet Tırması","","Parmaklar teni tırmaladı!",92,30,EfektTip::Kanama,1}}},
          {"🧟 Mumya","Lanetli, yavaş ama güçlü.",Davranis::Savunmaci,
           16,6,67,55,82,{
            {"Lanet Dokunuşu","","Lanetli dokunuş!",107,20,EfektTip::Zehir,3},
            {"Bandaj Fırlatma","","Bandajlar sardı!",87,15,EfektTip::Donma,1},
            {"Ölüm Çığlığı","\xe2\x9a\xa0\xef\xb8\x8f Mumya inliyor...","Lanet enerji sardı!",142,30,EfektTip::Zehir,4}}},
          {"🧛 Vampir","Kanda yaşar, kanda ölür.",Davranis::Hilekâr,
           14,3,62,52,75,{
            {"Kan Emme","","Boynu ısırıp kan emdi!",112,20,EfektTip::Kanama,2},
            {"Yarasa Saldırı","","Yarasa sürüsü çullandı!",102,25},
            {"Hipnoz","\xe2\x9a\xa0\xef\xb8\x8f Gözlerine bakıyor...","Donup kaldınız!",72,15,EfektTip::Donma,1}}},
          {"👻 Hayalet","Görünmez, elle tutulamaz.",Davranis::Taktikci,
           12,8,57,47,70,{
            {"Ruh Geçişi","","Ruhunuzu çekti!",97,20},
            {"Lanet","\xe2\x9a\xa0\xef\xb8\x8f Hayalet fısıldıyor...","Lanet vücuda sindi!",112,25,EfektTip::Zehir,3},
            {"Korku","","Panik yarattı!",67,10}}}
        };
        break;
      case Bolge::KarKale:
        havuz = {
          {"⚔️ Kara Muhafız","Malachar'a adanmış.",Davranis::Savunmaci,
           18,7,74,62,88,{
            {"Kalkan Darbesi","","Kalkan kenarı çeneni kırdı!",102,20},
            {"Kılıç Fırtınası","\xe2\x9a\xa0\xef\xb8\x8f Muhafız duruşunu değiştirdi...","Kılıç darbesi yağdı!",142,30,EfektTip::Kanama,2},
            {"Demir Duvar","","Savunmaya geçip karşı saldırdı!",117,25}}},
          {"🔮 Kaos Büyücüsü","Gerçekliği büker.",Davranis::Taktikci,
           16,4,82,70,78,{
            {"Kaos Işını","","Kaotik enerji dağıttı!",122,30,EfektTip::Yanma,2},
            {"Büyü Sirküsü","\xe2\x9a\xa0\xef\xb8\x8f Eller çeviriyor...","Karanlık büyü her yandan!",148,35},
            {"Zaman Durumu","\xe2\x9a\xa0\xef\xb8\x8f Saat durdu...","Donup kaldınız!",92,20,EfektTip::Donma,2}}},
          {"🗡️ Karanlık Suikastçı","Gölgede, seste, ölümde.",Davranis::Hilekâr,
           20,5,87,74,82,{
            {"Gölge Adım","","Gölgeden boğaza sapladı!",132,30,EfektTip::Kanama,3},
            {"Zehirli İğne","","Zehirli iğne boyunda!",102,20,EfektTip::Zehir,4},
            {"Ölüm İzi","\xe2\x9a\xa0\xef\xb8\x8f Suikastçı kayboluyor...","Triple darbe!",168,35,EfektTip::Kanama,3}}}
        };
        break;
    }

    int idx = rng(0, (int)havuz.size() - 1);
    auto& h = havuz[idx];
    d.ad        = h.ad;
    d.aciklama  = h.aciklama;
    d.davranis  = h.dav;
    d.hasar     = h.baseHas + sf * 3;
    d.sav       = h.baseSav + sf / 2;
    d.maksCan   = d.can = h.baseCan + sf * 9 + rng(-8, 8);
    d.xp        = h.baseXp + sf * 13;
    d.altin     = h.baseAltin + sf * 11;
    d.eylemler  = h.eylemler;

    if (turu == DusmanTur::Elite) {
        d.ad        = "\xe2\xad\x90 " + d.ad;
        d.maksCan   = d.can = (int)(d.maksCan * 1.5f);
        d.hasar     = (int)(d.hasar * 1.3f);
        d.sav      += 3;
        d.xp        = (int)(d.xp * 1.5f);
        d.altin     = (int)(d.altin * 1.5f);
        d.iksirSay  = 1;
    }
    return d;
}

// ═══════════════════════════════════════════════════════════════
//  §7  Oyuncu Sınıfı
// ═══════════════════════════════════════════════════════════════
class Oyuncu {
public:
    std::string ad;
    KarSinif    sinif;
    int can, maksCan, mana, maksMana;
    int temelHas, sav;
    int seviye, xp, snXp;
    int altin;
    int iksir, panzehir;
    int yetenekPuani, pasifSev;
    bool aktifAcik;
    int kritSans;

    int zehirSure = 0, yanmaSure = 0, kanamaSure = 0;

    bool yoldasVar = false;
    std::string yoldasAd = "Yok";

    EvcilHayvan evcil;
    Esya silah, zirh;
    std::vector<Esya> envanter;
    int mEnvanter = 8;

    std::vector<Gorev>    gorevler;
    std::vector<Basarim>  basarimlar;
    std::vector<Malzeme>  malzemeler;

    int toplamOldurme = 0, toplamBossOldurme = 0;
    int toplamHasarVerilen = 0, toplamHasarAlinan = 0;
    int kesfedilenBolge = 0;
    int bossOldurme = 0;
    int arenaRekor  = 0;

    int isikItibar = 0, golgeItibar = 0, demirItibar = 0;
    int combo = 0, comboMax = 0;

    Oyuncu() : sinif(KarSinif::Savasci), can(100), maksCan(100),
               mana(50), maksMana(50), temelHas(10), sav(5),
               seviye(1), xp(0), snXp(100), altin(50),
               iksir(2), panzehir(1), yetenekPuani(0), pasifSev(0),
               aktifAcik(false), kritSans(10) {}

    Oyuncu(const std::string& isim, KarSinif s,
           const Esya& sw, const Esya& ar, const MetaVeri& meta)
        : ad(isim), sinif(s),
          can(0), maksCan(0), mana(0), maksMana(0),
          temelHas(0), sav(0),
          seviye(1), xp(0), snXp(100), altin(50),
          iksir(2), panzehir(1), yetenekPuani(0), pasifSev(0),
          aktifAcik(false), kritSans(10),
          silah(sw), zirh(ar)
    {
        switch (sinif) {
            case KarSinif::Savasci:     maksCan=155; maksMana=28;  temelHas=14; sav=7;  kritSans=10; break;
            case KarSinif::Buyucu:      maksCan=88;  maksMana=105; temelHas=7;  sav=2;  kritSans=5;  break;
            case KarSinif::Hirsiz:      maksCan=108; maksMana=52;  temelHas=17; sav=3;  kritSans=28; altin=90; break;
            case KarSinif::Paladin:     maksCan=135; maksMana=65;  temelHas=12; sav=10; kritSans=8;  break;
            case KarSinif::Okcu:        maksCan=112; maksMana=42;  temelHas=15; sav=4;  kritSans=22; break;
            case KarSinif::Nekromancer: maksCan=92;  maksMana=115; temelHas=9;  sav=3;  kritSans=14; altin=70; break;
        }
        can  = maksCan;
        mana = maksMana;

        if (!meta.mirasAd.empty() && meta.mirasHasar > 0) {
            Esya miras;
            miras.ad     = "\xf0\x9f\x8f\x9b\xef\xb8\x8f " + meta.mirasAd + " (Miras)";
            miras.nadir  = Nadir::Rare;
            miras.tur    = EsyaTuru::Silah;
            miras.bHasar = meta.mirasHasar;
            miras.bSav   = meta.mirasSav;
            miras.sevGer = 1;
            envanter.push_back(miras);
            std::cout << PSR << "\n\xf0\x9f\x8f\x9b\xef\xb8\x8f Miras eşyası: " << miras.ad
                      << " +" << miras.bHasar << " hasar envanterinize eklendi!\n" << RS;
        }

        basarimlariOlustur();
        gorevleriYenile();
    }

    void gorevleriYenile() {
        gorevler.clear();
        gorevler.push_back({1,"Zindan Temizliği \xf0\x9f\xaa\x93","5 canavar öldür.",5,0,90,100,false,""});
        gorevler.push_back({2,"Goblin Avcısı \xf0\x9f\x91\xba","5 Sinsi Goblin öldür.",5,0,130,140,false,"Goblin"});
        gorevler.push_back({3,"Boss Katili \xf0\x9f\x92\x80","Herhangi bir Boss'u yen.",1,0,320,380,false,"Boss"});
        gorevler.push_back({4,"Büyük Para \xf0\x9f\x92\xb0","500 Altın biriktir.",500,0,220,0,false,"altin"});
        gorevler.push_back({5,"Katliam \xf0\x9f\xa9\xb8","20 düşman öldür.",20,0,280,320,false,""});
    }

    void basarimlariOlustur() {
        basarimlar = {
            {"İlk Kan","İlk düşmanı öldür.",false},
            {"Keskin Kılıç","10 düşman öldür.",false},
            {"Katil","25 düşman öldür.",false},
            {"Efsanevi Savaşçı","50 düşman öldür.",false},
            {"Büyük Av","İlk Boss'u öldür.",false},
            {"Boss Avcısı","3 Boss öldür.",false},
            {"Zengin","1000 altın biriktir.",false},
            {"Koleksiyoncu","5 farklı eşya topla.",false},
            {"Evcil Sevgili","Bir evcil hayvan edin.",false},
            {"Kahraman","5. Seviyeye ulaş.",false},
            {"Usta","10. Seviyeye ulaş.",false},
            {"Kâşif","3 farklı bölgeyi keşfet.",false},
            {"Savaş Makinesi","1000 toplam hasar ver.",false},
            {"Hayatta Kalan","50 hasar al ve yaşa.",false},
            {"Arena Şampiyonu","Arenada 10 dalga geç.",false}
        };
    }

    void basarimKontrol(const std::string& tip) {
        auto ver = [&](int i) {
            if (i < (int)basarimlar.size() && !basarimlar[i].kazanildi) {
                basarimlar[i].kazanildi = true;
                std::cout << "\n" << PSR + BD
                          << "\xf0\x9f\x8f\x86 BAŞARIM: " << basarimlar[i].ad << RS
                          << "\n" << DM << "   " << basarimlar[i].aciklama << RS << "\n";
            }
        };
        if (tip == "oldurme") {
            if (toplamOldurme >= 1)  ver(0);
            if (toplamOldurme >= 10) ver(1);
            if (toplamOldurme >= 25) ver(2);
            if (toplamOldurme >= 50) ver(3);
        }
        if (tip == "boss") {
            if (bossOldurme >= 1) ver(4);
            if (bossOldurme >= 3) ver(5);
        }
        if (tip == "altin"  && altin >= 1000)          ver(6);
        if (tip == "esya"   && (int)envanter.size()>=5) ver(7);
        if (tip == "evcil")                             ver(8);
        if (tip == "seviye" && seviye >= 5)             ver(9);
        if (tip == "seviye" && seviye >= 10)            ver(10);
        if (tip == "bolge"  && kesfedilenBolge >= 3)   ver(11);
        if (tip == "hasar"  && toplamHasarVerilen>=1000)ver(12);
        if (tip == "hasar_al"&&toplamHasarAlinan>=50)   ver(13);
        if (tip == "arena"  && arenaRekor >= 10)        ver(14);
    }

    std::string sinifAdi() const {
        switch (sinif) {
            case KarSinif::Savasci:     return "Sava\xc5\x9f\xc3\xa7\xc4\xb1 \xf0\x9f\x9b\xa1\xef\xb8\x8f";
            case KarSinif::Buyucu:      return u8"B\u00FCy\u00FCC\u00FC \U0001F52E";
            case KarSinif::Hirsiz:      return "H\xc4\xb1rs\xc4\xb1z \xf0\x9f\x97\xa1\xef\xb8\x8f";
            case KarSinif::Paladin:     return "Paladin \xe2\x9c\x9c\xef\xb8\x8f";
            case KarSinif::Okcu:        return "Ok\xc3\xa7u \xf0\x9f\x8f\xb9";
            case KarSinif::Nekromancer: return "Nekromancer \xf0\x9f\x92\x80";
            default: return "?";
        }
    }

    int topHasar() const {
        int h = temelHas + silah.bHasar + silah.arti * 6;
        if (sinif == KarSinif::Hirsiz)     h += pasifSev * 7;
        if (sinif == KarSinif::Okcu)       h += pasifSev * 5;
        if (sinif == KarSinif::Nekromancer)h += pasifSev * 4;
        if (evcil.var) h += evcil.bHasar / 3;
        return h;
    }

    int topSav() const {
        int s = sav + zirh.bSav + zirh.arti * 4;
        if (sinif == KarSinif::Savasci) s += pasifSev * 5;
        if (sinif == KarSinif::Paladin) s += pasifSev * 4 + 3;
        if (evcil.var) s += evcil.bSav / 3;
        return s;
    }

    void xpKazan(int miktar) {
        xp += miktar;
        std::cout << PYE << "\xe2\x9c\xa8 [XP] +" << miktar << " ("
                  << xp << "/" << snXp << ")\n" << RS;
        while (xp >= snXp) {
            xp -= snXp;
            ++seviye;
            snXp = seviye * 115;
            ++yetenekPuani;
            switch (sinif) {
                case KarSinif::Savasci:     maksCan+=30; maksMana+=6;  temelHas+=4; sav+=3; break;
                case KarSinif::Buyucu:      maksCan+=14; maksMana+=30; temelHas+=2; sav+=1; break;
                case KarSinif::Hirsiz:      maksCan+=20; maksMana+=13; temelHas+=5; sav+=2; break;
                case KarSinif::Paladin:     maksCan+=24; maksMana+=16; temelHas+=3; sav+=4; break;
                case KarSinif::Okcu:        maksCan+=19; maksMana+=11; temelHas+=6; sav+=2; break;
                case KarSinif::Nekromancer: maksCan+=17; maksMana+=32; temelHas+=3; sav+=2; break;
            }
            can = maksCan; mana = maksMana;
            std::cout << "\n" << PSR + BD;
            std::cout << "\xf0\x9f\x8e\x89 SEV. " << seviye << "! +1 Yetenek Puanı!\n" << RS;
            basarimKontrol("seviye");
        }
    }

    void dotHasari() {
        if (zehirSure > 0) {
            int h = 8 + seviye;
            can -= h; --zehirSure;
            std::cout << MJ << "\xf0\x9f\xa4\xa2 [ZEHİR] -" << h << " can\n" << RS;
        }
        if (yanmaSure > 0) {
            int h = 11 + seviye;
            can -= h; --yanmaSure;
            std::cout << PKR << "\xf0\x9f\x94\xa5 [YANMA] -" << h << " can\n" << RS;
        }
        if (kanamaSure > 0) {
            int h = 9 + seviye;
            can -= h; --kanamaSure;
            std::cout << KR << "\xf0\x9f\xa9\xb8 [KANAMA] -" << h << " can\n" << RS;
        }
    }

    void iksirKullan() {
        if (iksir <= 0) { std::cout << KR << "\xe2\x9d\x8c İksir yok!\n" << RS; return; }
        --iksir;
        int h = 60 + seviye * 5;
        can = std::min(maksCan, can + h);
        std::cout << PYE << "\xf0\x9f\xa7\xaa \xc5\x9eifalı İksir! +" << h << " can\n" << RS;
    }

    void panzehirKullan() {
        if (panzehir <= 0) { std::cout << KR << "\xe2\x9d\x8c Panzehir yok!\n" << RS; return; }
        --panzehir;
        zehirSure = yanmaSure = kanamaSure = 0;
        std::cout << PYE << "\xf0\x9f\x9f\xa2 Panzehir! Tüm efektler temizlendi.\n" << RS;
    }

    void canYenile() {
        can = maksCan; mana = maksMana;
        zehirSure = yanmaSure = kanamaSure = 0;
    }

    void durumGoster() const {
        dcizgi();
        std::cout << BD + PCY << "  \xe2\x98\x85 " << ad << " — " << sinifAdi() << RS << "\n";
        cizgi();
        std::cout << "  Sev: " << BD << seviye << RS
                  << " | XP: " << xp << "/" << snXp
                  << " | Yetenek P.: " << BD + PSR << yetenekPuani << RS << "\n";
        std::cout << "  \xe2\x9d\xa4\xef\xb8\x8f Can:  " << canBari(can, maksCan) << "\n";
        std::cout << "  \xf0\x9f\x94\xb7 Mana: " << manaBari(mana, maksMana) << "/" << maksMana << "\n";
        cizgi();
        std::cout << "  \xe2\x9a\x94\xef\xb8\x8f Silah: " << nadirRenk(silah.nadir) << silah.ad << RS
                  << " [+" << silah.arti << "] — Hasar: " << silah.bHasar << "\n";
        std::cout << "  \xf0\x9f\x9b\xa1\xef\xb8\x8f Zırh:  " << nadirRenk(zirh.nadir) << zirh.ad << RS
                  << " [+" << zirh.arti << "] — Savunma: " << zirh.bSav << "\n";
        cizgi();
        std::cout << "  \xe2\x9a\x94\xef\xb8\x8f Top. Hasar: " << BD << topHasar() << RS
                  << " | \xf0\x9f\x9b\xa1\xef\xb8\x8f Top. Sav: " << BD << topSav() << RS
                  << " | \xf0\x9f\x8e\xaf Krit: %" << kritSans << "\n";
        std::cout << "  \xf0\x9f\xa7\xaa İksir: " << iksir
                  << " | \xf0\x9f\x9f\xa2 Panzehir: " << panzehir
                  << " | \xf0\x9f\x92\xb0 " << BD + PSR << altin << RS << " Altın\n";
        if (evcil.var) {
            std::cout << "  \xf0\x9f\x90\xbe Evcil: " << BD << evcil.ad << RS
                      << " (" << evcil.tur << " Sev." << evcil.seviye
                      << " Sevgi:" << evcil.sevgi << "/100)\n";
        }
        if (yoldasVar)
            std::cout << "  \xf0\x9f\x97\xa1\xef\xb8\x8f Yoldaş: " << BD << yoldasAd << RS << "\n";
        cizgi();
        std::cout << "  Toplam Öldürme: " << toplamOldurme
                  << " | Boss: " << bossOldurme
                  << " | Arena Rekor: " << arenaRekor << "\n";
        std::cout << "  Verilen Hasar: " << toplamHasarVerilen
                  << " | Alınan: " << toplamHasarAlinan << "\n";
        if (zehirSure||yanmaSure||kanamaSure) {
            std::cout << "  Efektler:";
            if (zehirSure)  std::cout << MJ << " Zehir(" << zehirSure << ")" << RS;
            if (yanmaSure)  std::cout << PKR << " Yanma(" << yanmaSure << ")" << RS;
            if (kanamaSure) std::cout << KR  << " Kanama(" << kanamaSure << ")" << RS;
            std::cout << "\n";
        }
        dcizgi();
    }
};

// ═══════════════════════════════════════════════════════════════
//  §8  Kaydet / Yükle
// ═══════════════════════════════════════════════════════════════
static void esyaYaz(std::ofstream& f, const Esya& e) {
    f << e.ad << "\n"
      << (int)e.tur << " " << (int)e.nadir << " "
      << e.bHasar << " " << e.bSav << " " << e.bCan << " " << e.bMana << " "
      << e.deger << " " << e.sevGer << " " << e.arti << "\n"
      << e.ozel << "\n";
}
static Esya esyaOku(std::ifstream& f) {
    Esya e;
    std::getline(f, e.ad);
    int t, n;
    f >> t >> n >> e.bHasar >> e.bSav >> e.bCan >> e.bMana
      >> e.deger >> e.sevGer >> e.arti;
    e.tur = (EsyaTuru)t; e.nadir = (Nadir)n;
    f.ignore();
    std::getline(f, e.ozel);
    return e;
}

bool oyunuKaydet(const Oyuncu& o) {
    std::ofstream f("kayit.txt");
    if (!f) return false;
    f << o.ad << "\n" << (int)o.sinif << "\n"
      << o.can << " " << o.maksCan << " " << o.mana << " " << o.maksMana << "\n"
      << o.temelHas << " " << o.sav << " " << o.seviye << " " << o.xp << " " << o.snXp << "\n"
      << o.altin << " " << o.iksir << " " << o.panzehir << "\n"
      << o.yetenekPuani << " " << o.pasifSev << " " << (int)o.aktifAcik << "\n"
      << o.zehirSure << " " << o.yanmaSure << " " << o.kanamaSure << "\n"
      << (int)o.yoldasVar << "\n" << o.yoldasAd << "\n"
      << o.toplamOldurme << " " << o.toplamBossOldurme << " " << o.bossOldurme << " "
      << o.kesfedilenBolge << " " << o.arenaRekor << "\n"
      << o.isikItibar << " " << o.golgeItibar << " " << o.demirItibar << "\n"
      << o.toplamHasarVerilen << " " << o.toplamHasarAlinan << "\n";

    esyaYaz(f, o.silah);
    esyaYaz(f, o.zirh);

    f << o.envanter.size() << "\n";
    for (auto& e : o.envanter) esyaYaz(f, e);

    f << o.gorevler.size() << "\n";
    for (auto& g : o.gorevler) f << g.mevcut << " " << (int)g.tamam << "\n";

    f << (int)o.evcil.var << "\n";
    if (o.evcil.var) {
        f << o.evcil.ad << "\n" << o.evcil.tur << "\n"
          << o.evcil.seviye << " " << o.evcil.sevgi << " "
          << o.evcil.bHasar << " " << o.evcil.bSav << "\n"
          << o.evcil.yetenek << "\n";
    }

    f << o.malzemeler.size() << "\n";
    for (auto& m : o.malzemeler) f << m.ad << "\n" << m.miktar << "\n";

    for (auto& b : o.basarimlar) f << (int)b.kazanildi << " ";
    f << "\n";
    return true;
}

bool oyunuYukle(Oyuncu& o) {
    std::ifstream f("kayit.txt");
    if (!f) return false;
    std::getline(f, o.ad);
    int s; f >> s; o.sinif = (KarSinif)s; f.ignore();
    f >> o.can >> o.maksCan >> o.mana >> o.maksMana; f.ignore();
    f >> o.temelHas >> o.sav >> o.seviye >> o.xp >> o.snXp; f.ignore();
    f >> o.altin >> o.iksir >> o.panzehir; f.ignore();
    int aktif;
    f >> o.yetenekPuani >> o.pasifSev >> aktif; o.aktifAcik = aktif != 0; f.ignore();
    f >> o.zehirSure >> o.yanmaSure >> o.kanamaSure; f.ignore();
    int yv; f >> yv; o.yoldasVar = yv != 0; f.ignore();
    std::getline(f, o.yoldasAd);
    f >> o.toplamOldurme >> o.toplamBossOldurme >> o.bossOldurme
      >> o.kesfedilenBolge >> o.arenaRekor; f.ignore();
    f >> o.isikItibar >> o.golgeItibar >> o.demirItibar; f.ignore();
    f >> o.toplamHasarVerilen >> o.toplamHasarAlinan; f.ignore();

    o.silah = esyaOku(f);
    o.zirh  = esyaOku(f);

    size_t n; f >> n; f.ignore(); o.envanter.clear();
    for (size_t i = 0; i < n; ++i) o.envanter.push_back(esyaOku(f));

    f >> n; f.ignore(); o.gorevleriYenile();
    for (size_t i = 0; i < n && i < o.gorevler.size(); ++i) {
        int m, t; f >> m >> t;
        o.gorevler[i].mevcut = m;
        o.gorevler[i].tamam  = (t != 0);
    }
    f.ignore();

    int ev; f >> ev; o.evcil.var = ev != 0; f.ignore();
    if (o.evcil.var) {
        std::getline(f, o.evcil.ad);
        std::getline(f, o.evcil.tur);
        f >> o.evcil.seviye >> o.evcil.sevgi >> o.evcil.bHasar >> o.evcil.bSav; f.ignore();
        std::getline(f, o.evcil.yetenek);
    }

    f >> n; f.ignore(); o.malzemeler.clear();
    for (size_t i = 0; i < n; ++i) {
        Malzeme m;
        std::getline(f, m.ad); f >> m.miktar; f.ignore();
        o.malzemeler.push_back(m);
    }

    o.basarimlariOlustur();
    for (size_t i = 0; i < o.basarimlar.size(); ++i) {
        int k = 0; f >> k;
        if (i < o.basarimlar.size()) o.basarimlar[i].kazanildi = (k != 0);
    }
    return true;
}

// ═══════════════════════════════════════════════════════════════
//  §9  Ganimet Sistemi
// ═══════════════════════════════════════════════════════════════
void ganimetDusur(Oyuncu& o, const Dusman& d) {
    int ag = (o.sinif == KarSinif::Hirsiz) ? (int)(d.altin * 1.5f) : d.altin;
    ag += rng(-ag / 4, ag / 4);
    o.altin += ag;
    std::cout << PSR << "\xf0\x9f\x92\xb0 +" << ag << " Altın!\n" << RS;
    o.basarimKontrol("altin");

    // Malzeme (%55)
    if (sansBasi(55)) {
        static const std::string mAd[] = {
            "\xf0\x9f\xa6\xb4 Kemik Tozu", "\xf0\x9f\x8c\xbf Zehir Otu",
            "\xf0\x9f\x92\x8e Ham Kristal", "\xf0\x9f\x94\xb4 Kan Taşı",
            "\xe2\x9a\x97\xef\xb8\x8f Mistik Toz", "\xf0\x9f\xaa\xa8 Demir Parçası",
            "\xf0\x9f\x94\xa5 Kor Taşı",    "\xf0\x9f\x8c\x91 Karanlık İp"
        };
        std::string secilenMalz = mAd[rng(0, 7)];
        bool bulundu = false;
        for (auto& m : o.malzemeler) {
            if (m.ad == secilenMalz) { ++m.miktar; bulundu = true; break; }
        }
        if (!bulundu) o.malzemeler.push_back({secilenMalz, 1});
        std::cout << SY << "\xe2\x9a\x97\xef\xb8\x8f Malzeme: " << secilenMalz << " x1\n" << RS;
    }

    // GizliBoss → garanti efsanevi ganimet (erken dön)
    if (d.turu == DusmanTur::GizliBoss) {
        static const std::string efkt[] = {"vampir","yanan","dondurucu","lanet","kahraman"};
        bool isSilah = sansBasi(50);
        Esya efs;
        efs.nadir  = Nadir::Legendary;
        efs.sevGer = std::max(1, o.seviye);
        if (isSilah) {
            efs.ad     = "\xf0\x9f\x94\xae Gizli Patron Kılıcı";
            efs.tur    = EsyaTuru::Silah;
            efs.bHasar = 40 + o.seviye * 3;
            efs.deger  = 800;
        } else {
            efs.ad     = "\xf0\x9f\x94\xae Gizli Patron Zırhı";
            efs.tur    = EsyaTuru::Zirh;
            efs.bSav   = 30 + o.seviye * 2;
            efs.deger  = 800;
        }
        efs.ozel = efkt[rng(0,4)];
        std::cout << "\n" << PSR + BD
                  << "\xf0\x9f\x94\xae [GİZLİ PATRON GANİMETİ] [Efsanevi] "
                  << efs.ad << "!\n" << RS;
        if ((int)o.envanter.size() < o.mEnvanter) {
            o.envanter.push_back(efs);
            std::cout << PYE << "\xf0\x9f\x8e\x92 Envantere eklendi.\n" << RS;
            o.basarimKontrol("esya");
        } else {
            std::cout << KR << u8"\u274C \u00C7anta dolu! E\u015Fya al\u0131namad\u0131.\n" << RS;
        }
        return;
    }

    // Eşya düşme şansı
    int sSans = ((int)d.turu >= 2) ? 72 : 35;
    if (o.sinif == KarSinif::Hirsiz) sSans = std::min(95, sSans + 20);
    if (!sansBasi(sSans)) return;

    // Nadir belirle
    int nr = rng(0, 99);
    Nadir nadir;
    // efsaneviAcik sonrası boss'lar da %15 şansla Legendary düşürür
    if      (d.turu == DusmanTur::GizliBoss)                         nadir = Nadir::Legendary;
    else if ((int)d.turu >= 3 && nr < 15)                            nadir = Nadir::Legendary;
    else if ((int)d.turu >= 3 && nr < 35)                            nadir = Nadir::Epic;
    else if (nr < 52)                                                 nadir = Nadir::Rare;
    else if (nr < 73)                                                 nadir = Nadir::Uncommon;
    else                                                              nadir = Nadir::Common;

    bool isSilah = sansBasi(50);
    struct ET { std::string ad; int has, sv, deg, sg; };
    static const ET silahlar[] = {
        {"Paslı Hançer",6,0,22,1}, {"Zindan Kılıcı",13,0,62,2},
        {"Runik Balta",19,0,125,3},{"Ejderha Dişi",26,0,255,4},
        {"Efsanevi Kılıç",36,0,510,5},{"Büyülü Asa",21,0,185,3},
        {"Gölge Hançer",23,0,205,4},{"Tanrısal Mızrak",31,0,410,5}
    };
    static const ET zirhlar[] = {
        {"Yıpranmış Zırh",0,4,16,1},{"Demir Göğüslük",0,11,57,2},
        {"Runik Kalkan",0,17,115,3},{"Ejder Derisi",0,25,245,4},
        {"Efsanevi Zırh",0,36,490,5},{"Büyücü Cübbesi",0,8,102,2},
        {"Gölge Deri",0,13,132,3},{"Kutsal Zırh",0,31,425,5}
    };

    const ET& temel = isSilah ? silahlar[rng(0,7)] : zirhlar[rng(0,7)];
    Esya esya;
    esya.ad    = temel.ad;
    esya.tur   = isSilah ? EsyaTuru::Silah : EsyaTuru::Zirh;
    esya.nadir = nadir;
    esya.sevGer = temel.sg;
    float nM = 1.f;
    switch (nadir) {
        case Nadir::Uncommon:  nM = 1.12f; break;
        case Nadir::Rare:      nM = 1.28f; break;
        case Nadir::Epic:      nM = 1.55f; break;
        case Nadir::Legendary: nM = 2.05f; break;
        default: break;
    }
    esya.bHasar = (int)(temel.has * nM);
    esya.bSav   = (int)(temel.sv  * nM);
    esya.deger  = (int)(temel.deg * nM);

    if (nadir == Nadir::Epic || nadir == Nadir::Legendary) {
        static const std::string efkt[] = {"vampir","yanan","dondurucu","lanet","kahraman"};
        esya.ozel = efkt[rng(0,4)];
    }

    std::cout << "\n" << nadirRenk(nadir) + BD
              << "\xf0\x9f\x8e\x81 [GANİMET] " << nadirEtiket(nadir) << " "
              << esya.ad << "!\n" << RS;

    if ((int)o.envanter.size() < o.mEnvanter) {
        o.envanter.push_back(esya);
        std::cout << PYE << "\xf0\x9f\x8e\x92 Envantere eklendi.\n" << RS;
        o.basarimKontrol("esya");
    } else {
        std::cout << KR << "\xe2\x9d\x8c Çanta dolu! Eşya alınamadı.\n" << RS;
    }
}

// ═══════════════════════════════════════════════════════════════
//  §10  Görev Güncelleme
// ═══════════════════════════════════════════════════════════════
void gorevGuncelle(Oyuncu& o, const Dusman& d) {
    for (auto& g : o.gorevler) {
        if (g.tamam) continue;
        bool sayildi = false;
        if (g.kriter.empty()) {
            sayildi = true;
        } else if (g.kriter == "Boss" &&
                   (d.turu == DusmanTur::Boss || d.turu == DusmanTur::GizliBoss)) {
            sayildi = true;
        } else if (d.ad.find(g.kriter) != std::string::npos) {
            sayildi = true;
        }
        if (sayildi) g.mevcut = std::min(g.hedef, g.mevcut + 1);
    }
    // Altın görevi güncelle
    for (auto& g : o.gorevler)
        if (!g.tamam && g.kriter == "altin")
            g.mevcut = std::min(g.hedef, o.altin);
}

// ═══════════════════════════════════════════════════════════════
//  §11  Savaş Sistemi
// ═══════════════════════════════════════════════════════════════
bool savas(Oyuncu& o, Dusman& d) {
    dcizgi(58);
    std::cout << PKR + BD << "  \xe2\x9a\x94\xef\xb8\x8f SAVAŞ: " << o.ad << " vs " << d.ad << "\n" << RS;
    if (!d.aciklama.empty())
        std::cout << DM << "  " << d.aciklama << RS << "\n";
    dcizgi(58);

    bool kalkanAktif = false;
    bool donmusMu    = false;

    while (o.can > 0 && d.can > 0) {
        // DoT hasarları
        o.dotHasari();
        if (o.can <= 0) break;
        d.dotHasari();
        if (!d.hayatta()) break;

        // Savaş durumu paneli
        cizgi(58);
        std::cout << "  \xf0\x9f\xa7\x99 " << BD << o.ad << RS << ": "
                  << canBari(o.can, o.maksCan) << "  MP:" << o.mana << "\n";
        std::cout << "  \xf0\x9f\x91\xb9 " << BD << d.ad << RS << ": "
                  << canBari(d.can, d.maksCan);
        if (d.berserk) std::cout << " " << PKR + BD << "[BERSERK!]" << RS;
        if (d.turu == DusmanTur::Boss || d.turu == DusmanTur::GizliBoss)
            std::cout << " " << PSR << "FAZ-" << d.aktifFaz << RS;
        std::cout << "\n";
        if (o.combo > 1)
            std::cout << "  \xf0\x9f\x94\xa5 Combo: " << PSR + BD << "x" << o.combo << RS << "\n";
        cizgi(58);

        // Donmuş tur
        if (donmusMu) {
            std::cout << PMB << "\xe2\x9d\x84\xef\xb8\x8f Donmuş durumdasınız! Bu tur atlıyorsunuz.\n" << RS;
            donmusMu = false;
            goto dusmanTuru;
        }

        // Oyuncu hamle seçimi
        {
            bool hamleYapildi = false;
            while (!hamleYapildi) {
                std::cout << "\n  " << PW << "1" << RS << ". Normal Saldırı \xe2\x9a\x94\xef\xb8\x8f"
                          << "  " << PW << "2" << RS << ". Yetenek/Büyü \xf0\x9f\x94\xae"
                          << "  " << PW << "3" << RS << ". Eşya \xf0\x9f\x8e\x92"
                          << "\n  Seçim: ";
                int sc = guvenliOku(1, 3);

                if (sc == 1) {
                    // Normal saldırı + varyans
                    int ham = hasarUret(o.topHasar());
                    bool krit = sansBasi(o.kritSans);
                    if (krit) {
                        float kM = (o.sinif == KarSinif::Hirsiz) ? 2.6f :
                                   (o.sinif == KarSinif::Okcu)   ? 2.3f : 2.0f;
                        ham = (int)(ham * kM);
                        std::cout << "\n" << PSR + BD << "\xf0\x9f\x92\xa5\xf0\x9f\x92\xa5 KRİTİK VURUŞ! \xf0\x9f\x92\xa5\xf0\x9f\x92\xa5\n" << RS;
                    }
                    // Efsanevi silah özel efekti
                    if (o.silah.ozel == "dondurucu" && sansBasi(25)) d.kanamaSure = 1;
                    if (o.silah.ozel == "yanan"      && sansBasi(35)) d.yanmaSure = std::max(d.yanmaSure, 2);
                    if (o.silah.ozel == "lanet"       && sansBasi(30)) d.zehirSure = std::max(d.zehirSure, 2);

                    int net = std::max(1, ham - d.sav);

                    // Combo sistemi
                    ++o.combo;
                    if (o.combo > o.comboMax) o.comboMax = o.combo;
                    if (o.combo >= 3) {
                        int cb = (o.combo - 2) * 6;
                        net += cb;
                        std::cout << PSR << "\xf0\x9f\x94\xa5 COMBO x" << o.combo << " +" << cb << " bonus!\n" << RS;
                    }

                    // Vampir efekti
                    if (o.silah.ozel == "vampir") {
                        int vheal = net / 4;
                        o.can = std::min(o.maksCan, o.can + vheal);
                        std::cout << PKR << "\xf0\x9f\xa9\xb8 [VAMPİR] +" << vheal << " can!\n" << RS;
                    }

                    d.can -= net;
                    o.toplamHasarVerilen += net;
                    o.basarimKontrol("hasar");

                    std::cout << "  \xe2\x9a\x94\xef\xb8\x8f " << o.ad << " \xe2\x86\x92 "
                              << hasarRenk(net) << " hasar!\n";
                    hamleYapildi = true;
                }
                else if (sc == 2) {
                    std::cout << "\n  1. Alev Topu \xf0\x9f\x94\xa5 [30 MP]    2. Kalkan \xf0\x9f\x9b\xa1\xef\xb8\x8f [15 MP]\n"
                              << "  3. Kutsal Şifa \xe2\x9c\xa8 [20 MP]";
                    if (o.aktifAcik) {
                        std::cout << "    4. [ULTİ] ";
                        switch (o.sinif) {
                            case KarSinif::Savasci:     std::cout << "Ejderha Kesişi \xf0\x9f\x90\x89 [25 MP]"; break;
                            case KarSinif::Buyucu:      std::cout << "Kıyamet Yağmuru \xe2\x98\x84\xef\xb8\x8f [40 MP]"; break;
                            case KarSinif::Hirsiz:      std::cout << "Suikast \xf0\x9f\x97\xa1\xef\xb8\x8f [30 MP]"; break;
                            case KarSinif::Paladin:     std::cout << "Kutsal İnfaz \xe2\x9c\x9c\xef\xb8\x8f [35 MP]"; break;
                            case KarSinif::Okcu:        std::cout << "Ölüm Yağmuru \xf0\x9f\x8f\xb9 [30 MP]"; break;
                            case KarSinif::Nekromancer: std::cout << "Ruh Patlaması \xf0\x9f\x92\x80 [45 MP]"; break;
                        }
                    }
                    std::cout << "\n  0. Geri\n  Seçim: ";
                    int ys = guvenliOku(0, 4);
                    if (ys == 0) continue;

                    bool kullanildi = false;

                    if (ys == 1) { // Alev Topu
                        if (o.mana < 30) { std::cout << KR << "\xe2\x9d\x8c Yetersiz MP!\n" << RS; }
                        else {
                            o.mana -= 30;
                            int mult = (o.sinif == KarSinif::Buyucu) ? 3 : 2;
                            int ham = hasarUret(o.topHasar() * mult);
                            int net = std::max(1, ham - d.sav / 2);
                            d.can -= net; d.yanmaSure = 3;
                            o.toplamHasarVerilen += net;
                            ++o.combo;
                            std::cout << PKR + BD << "\xf0\x9f\x94\xa5 [ALEV TOPU] "
                                      << hasarRenk(net) << " hasar + Yanma!\n" << RS;
                            kullanildi = true;
                        }
                    }
                    else if (ys == 2) { // Kalkan
                        if (o.mana < 15) { std::cout << KR << "\xe2\x9d\x8c Yetersiz MP!\n" << RS; }
                        else {
                            o.mana -= 15; kalkanAktif = true; o.combo = 0;
                            std::cout << PMB + BD << "\xf0\x9f\x9b\xa1\xef\xb8\x8f [KALKAN] Bir sonraki hasar yarılanacak!\n" << RS;
                            kullanildi = true;
                        }
                    }
                    else if (ys == 3) { // Şifa
                        if (o.mana < 20) { std::cout << KR << "\xe2\x9d\x8c Yetersiz MP!\n" << RS; }
                        else {
                            o.mana -= 20; o.combo = 0;
                            int h = 45 + o.seviye * 4;
                            o.can = std::min(o.maksCan, o.can + h);
                            std::cout << PYE + BD << "\xe2\x9c\xa8 [ŞİFA] +" << h << " can!\n" << RS;
                            kullanildi = true;
                        }
                    }
                    else if (ys == 4 && o.aktifAcik) {
                        bool yapildi = false;
                        switch (o.sinif) {
                            case KarSinif::Savasci:
                                if (o.mana >= 25) {
                                    o.mana -= 25;
                                    int h = hasarUret(o.topHasar() * 3);
                                    int net = std::max(1, h - d.sav);
                                    d.can -= net;
                                    int st = net / 2;
                                    o.can = std::min(o.maksCan, o.can + st);
                                    o.toplamHasarVerilen += net;
                                    std::cout << PKR + BD << "\xf0\x9f\x90\x89 [EJDERHA KESİŞİ] "
                                              << hasarRenk(net) << " + +" << st << " can!\n" << RS;
                                    yapildi = true;
                                }
                                break;
                            case KarSinif::Buyucu:
                                if (o.mana >= 40) {
                                    o.mana -= 40;
                                    int h = hasarUret(o.topHasar() * 4);
                                    d.can -= h; d.yanmaSure = 5;
                                    o.toplamHasarVerilen += h;
                                    std::cout << PKR + BD << "\xe2\x98\x84\xef\xb8\x8f [KIYAMET] "
                                              << hasarRenk(h) << " + 5 tur yanma!\n" << RS;
                                    yapildi = true;
                                }
                                break;
                            case KarSinif::Hirsiz:
                                if (o.mana >= 30) {
                                    o.mana -= 30;
                                    int h = hasarUret((int)(o.topHasar() * 3.5f));
                                    d.can -= h; // SAF hasar
                                    o.toplamHasarVerilen += h;
                                    std::cout << PKR + BD << "\xf0\x9f\x97\xa1\xef\xb8\x8f [SUİKAST] "
                                              << hasarRenk(h) << " SAF!\n" << RS;
                                    yapildi = true;
                                }
                                break;
                            case KarSinif::Paladin:
                                if (o.mana >= 35) {
                                    o.mana -= 35;
                                    int h = hasarUret((int)(o.topHasar() * 2.5f));
                                    int net = std::max(1, h - d.sav);
                                    d.can -= net;
                                    o.can = std::min(o.maksCan, o.can + 65);
                                    o.toplamHasarVerilen += net;
                                    std::cout << PYE + BD << "\xe2\x9c\x9c\xef\xb8\x8f [KUTSAL İNFAZ] "
                                              << hasarRenk(net) << " + +65 can!\n" << RS;
                                    yapildi = true;
                                }
                                break;
                            case KarSinif::Okcu:
                                if (o.mana >= 30) {
                                    o.mana -= 30;
                                    int h = hasarUret(o.topHasar() * 5);
                                    d.can -= h; d.kanamaSure = 5;
                                    o.toplamHasarVerilen += h;
                                    std::cout << KR + BD << "\xf0\x9f\x8f\xb9 [ÖLÜM YAĞMURU] "
                                              << hasarRenk(h) << " + 5 tur kanama!\n" << RS;
                                    yapildi = true;
                                }
                                break;
                            case KarSinif::Nekromancer:
                                if (o.mana >= 45) {
                                    o.mana -= 45;
                                    int h = hasarUret(o.topHasar() * 4);
                                    d.can -= h;
                                    int rc = h / 5;
                                    o.can = std::min(o.maksCan, o.can + rc);
                                    o.toplamHasarVerilen += h;
                                    std::cout << PMJ + BD << "\xf0\x9f\x92\x80 [RUH PATLAMASI] "
                                              << hasarRenk(h) << " + +" << rc << " ruh!\n" << RS;
                                    yapildi = true;
                                }
                                break;
                        }
                        if (!yapildi) std::cout << KR << "\xe2\x9d\x8c Yetersiz MP!\n" << RS;
                        kullanildi = yapildi;
                        if (yapildi) { ++o.combo; o.toplamHasarVerilen += 0; }
                    }
                    if (kullanildi) hamleYapildi = true;
                }
                else if (sc == 3) {
                    std::cout << "\n  1. İksir [" << o.iksir << "]   2. Panzehir [" << o.panzehir << "]   0. Geri\n  Seçim: ";
                    int es = guvenliOku(0, 2);
                    if      (es == 1 && o.iksir > 0)   { o.iksirKullan();    o.combo = 0; hamleYapildi = true; }
                    else if (es == 2 && o.panzehir > 0) { o.panzehirKullan(); o.combo = 0; hamleYapildi = true; }
                    else if (es == 0) continue;
                    else std::cout << KR << "\xe2\x9d\x8c Eşya yok!\n" << RS;
                }
            }
        }

        if (!d.hayatta()) break;

        // Yoldaş saldırısı
        if (o.yoldasVar && d.hayatta()) {
            int yh = hasarUret(10 + o.seviye * 2);
            d.can -= yh;
            std::cout << PYE << "\xf0\x9f\x90\xbe [YOLDAŞ] " << o.yoldasAd << ": "
                      << yh << " hasar!\n" << RS;
        }

        // Evcil hayvan
        if (o.evcil.var && d.hayatta()) {
            int eh = hasarUret(o.evcil.bHasar + o.evcil.seviye * 3);
            d.can -= eh;
            std::cout << PYE << "\xf0\x9f\x90\xbe [EVCİL] " << o.evcil.ad
                      << ": " << eh << " hasar!\n" << RS;
            o.evcil.sevgi = std::min(100, o.evcil.sevgi + 1);
        }

        if (!d.hayatta()) break;

        dusmanTuru:
        // Düşman hamlesi — AI
        DusmanEylem eylem = d.eylemSec(
            o.can, o.maksCan,
            o.zehirSure > 0, o.yanmaSure > 0,
            o.combo, o.sinif
        );

        if (!eylem.telgraf.empty())
            std::cout << "\n" << PSR << eylem.telgraf << RS << "\n";

        if (eylem.katsayi == 0) {
            if (eylem.mesaj != "..." && !eylem.mesaj.empty())
                std::cout << PYE << eylem.mesaj << RS << "\n";
            o.mana = std::min(o.maksMana, o.mana + 5);
            continue;
        }

        // Hasar: temel × katsayi/100 ± varyans
        int dHam = hasarUret((int)(d.hasar * eylem.katsayi / 100.0));
        dHam += rng(0, eylem.varyans);
        if (kalkanAktif) {
            dHam = std::max(1, dHam / 2);
            kalkanAktif = false;
            std::cout << PMB << "\xf0\x9f\x9b\xa1\xef\xb8\x8f Kalkan hasarı yarıladı!\n" << RS;
        }
        int dNet = std::max(1, dHam - o.topSav());
        o.can -= dNet;
        o.toplamHasarAlinan += dNet;
        o.basarimKontrol("hasar_al");
        o.combo = 0;

        std::cout << "  \xf0\x9f\x91\xb9 " << d.ad << ": " << eylem.mesaj
                  << " [" << hasarRenk(dNet) << " hasar]\n";

        // Efekt uygula
        if (eylem.efekt == EfektTip::Zehir)  { o.zehirSure  = eylem.efektSure; std::cout << MJ << "\xf0\x9f\xa4\xa2 Zehirlendi!\n" << RS; }
        if (eylem.efekt == EfektTip::Yanma)  { o.yanmaSure  = eylem.efektSure; std::cout << PKR << "\xf0\x9f\x94\xa5 Yandı!\n" << RS; }
        if (eylem.efekt == EfektTip::Kanama) { o.kanamaSure = eylem.efektSure; std::cout << KR << "\xf0\x9f\xa9\xb8 Kanıyor!\n" << RS; }
        if (eylem.efekt == EfektTip::Donma)  { donmusMu = true; std::cout << PMB << "\xe2\x9d\x84\xef\xb8\x8f Dondu!\n" << RS; }

        o.mana = std::min(o.maksMana, o.mana + 5);
    }

    // Savaş sonu
    if (!d.hayatta()) {
        std::cout << "\n" << PYE + BD;
        dcizgi(58);
        std::cout << "  \xf0\x9f\x8f\x86 ZAFER! " << d.ad << " yenildi!\n" << RS;
        dcizgi(58);
        ++o.toplamOldurme;
        if (d.turu == DusmanTur::Boss || d.turu == DusmanTur::GizliBoss) {
            ++o.toplamBossOldurme; ++o.bossOldurme;
            o.basarimKontrol("boss");
        }
        o.basarimKontrol("oldurme");
        o.xpKazan(d.xp + rng(-d.xp / 5, d.xp / 5));
        ganimetDusur(o, d);
        gorevGuncelle(o, d);
        return true;
    }

    std::cout << "\n" << PKR + BD;
    dcizgi(58);
    std::cout << "  \xf0\x9f\x92\x80 " << o.ad << " yenildi...\n" << RS;
    dcizgi(58);
    return false;
}

// ═══════════════════════════════════════════════════════════════
//  §12  Dinamik Olay Sistemi (20+ olay)
// ═══════════════════════════════════════════════════════════════
void olayTetikle(Oyuncu& o, int kat, Bolge bolge) {
    if (sansBasi(30)) return; // %70 oran oda olayı gerçekleşmez

    int tip = rng(0, 19);

    if (tip == 0) {
        std::cout << "\n\xe2\x9a\xa0\xef\xb8\x8f [TUZAK] *KLİK*... Gizli plakaya bastınız!\n";
        if (o.sinif == KarSinif::Hirsiz && sansBasi(55)) {
            std::cout << "\xf0\x9f\x97\xa1\xef\xb8\x8f [REFLEKS] Hırsız çevikliğiyle kaçtınız! (0 hasar)\n";
        } else {
            int h = std::max(5, (18 + kat * 3) - o.topSav());
            o.can -= h;
            std::cout << "\xf0\x9f\xa9\xb8 Oklar saplandı! -" << h << " can\n";
        }
    }
    else if (tip == 1) {
        std::cout << "\n\xf0\x9f\xa4\xa2 [TUZAK] Eski sandıktan zehirli gaz yayıldı!\n";
        if (o.sinif == KarSinif::Hirsiz && sansBasi(50)) {
            std::cout << "\xf0\x9f\x97\xa1\xef\xb8\x8f [REFLEKS] Gazdan kaçtınız!\n";
        } else {
            o.zehirSure = 3;
            std::cout << "\xf0\x9f\xa4\xa2 Zehir soludunuz! 3 tur zehir başladı.\n";
        }
    }
    else if (tip == 2) {
        std::cout << "\n\xe2\x9c\xa8 [KUTSALI ÇEŞME] Odanın ortasında parlayan pınar!\n";
        o.can  = std::min(o.maksCan,  o.can  + 40);
        o.mana = std::min(o.maksMana, o.mana + 25);
        std::cout << PYE << "\xf0\x9f\x92\x96 +40 Can, +25 Mana!\n" << RS;
    }
    else if (tip == 3) {
        int ag = 20 + kat * 6;
        o.altin += ag;
        std::cout << "\n\xf0\x9f\x92\x8e [GİZLİ HAZINE] Duvardaki gevşek taşın arkasında kese!\n";
        std::cout << PSR << "\xf0\x9f\x92\xb0 +" << ag << " Altın!\n" << RS;
    }
    else if (tip == 4) {
        std::cout << "\n\xf0\x9f\x94\xa5 [YANGIN] Odadan dumanlar yükseliyor!\n";
        int h = std::max(3, 12 - o.topSav() / 2);
        o.yanmaSure = 2;
        o.can -= h;
        std::cout << KR << "-" << h << " can + 2 tur yanma!\n" << RS;
    }
    else if (tip == 5) {
        std::cout << "\n\xf0\x9f\x97\xbf [GİZLİ ODA] Duvarda oyuk bir taş fark ettiniz!\n";
        std::cout << "1. İçeri gir (risk/ödül)  2. Geç\nSeçim: ";
        int sc = guvenliOku(1, 2);
        if (sc == 1) {
            o.basarimKontrol("gizem");
            if (sansBasi(60)) {
                Esya gizliEsya;
                gizliEsya.ad = "\xf0\x9f\x97\xbf Gizli Oda Sandığı";
                gizliEsya.tur = EsyaTuru::Silah;
                gizliEsya.nadir = Nadir::Rare;
                gizliEsya.bHasar = 15 + kat * 2;
                gizliEsya.deger  = 200;
                gizliEsya.sevGer = 1;
                if ((int)o.envanter.size() < o.mEnvanter) {
                    o.envanter.push_back(gizliEsya);
                    std::cout << PYE << "\xf0\x9f\x8e\x81 Nadide bir eşya buldunuz: " << gizliEsya.ad << "!\n" << RS;
                } else {
                    std::cout << KR << "\xe2\x9d\x8c Çanta dolu, eşya alınamadı!\n" << RS;
                }
            } else {
                std::cout << KR << "\xf0\x9f\x92\x80 Tuzaktı! Zehirli ok saplandı!\n" << RS;
                o.zehirSure = 4;
                o.can -= 20;
            }
        }
    }
    else if (tip == 6) {
        std::cout << "\n\xf0\x9f\xa7\x99 [BÜYÜLÜ YAZI] Duvarda eski bir yazıt!\n";
        if (o.sinif == KarSinif::Buyucu || o.sinif == KarSinif::Nekromancer) {
            o.mana = std::min(o.maksMana, o.mana + 35);
            std::cout << PCY << "+35 Mana! Büyücü olarak anlayabildiniz.\n" << RS;
        } else {
            std::cout << GB << "Yazıtı anlayamadınız ama gizemli bir huzur hissettiniz.\n" << RS;
            o.can = std::min(o.maksCan, o.can + 12);
        }
    }
    else if (tip == 7) {
        std::cout << "\n\xf0\x9f\x91\xa4 [NPC] Zindanda yaralı bir gezgin buluyorsunuz!\n";
        std::cout << "1. Yardım et (20 Altın harca)  2. Terk et\nSeçim: ";
        int sc = guvenliOku(1, 2);
        if (sc == 1 && o.altin >= 20) {
            o.altin -= 20;
            o.isikItibar += 5;
            std::cout << PYE << "Gezgin iyileşti ve size özel bir şişe verdi!\n" << RS;
            ++o.iksir;
            std::cout << "+1 İksir!\n";
        } else if (sc == 1) {
            std::cout << KR << "Yeterli altın yok...\n" << RS;
        } else {
            o.golgeItibar += 3;
            std::cout << DM << "Sessizce geçtiniz.\n" << RS;
        }
    }
    else if (tip == 8) {
        std::cout << "\n\xf0\x9f\xa6\x9e [MİSTİK HEYKELL] Kanatlı bir melek heykeli parlıyor!\n";
        std::cout << "1. Dua et  2. Kır (Efsanevi an şansı, ama lanet riski)\nSeçim: ";
        int sc = guvenliOku(1, 2);
        if (sc == 1) {
            std::cout << PYE << "İlahi ışık sizi sardı! Tüm efektler temizlendi, +20 can!\n" << RS;
            o.can = std::min(o.maksCan, o.can + 20);
            o.zehirSure = o.yanmaSure = o.kanamaSure = 0;
            o.isikItibar += 10;
        } else {
            std::cout << PSR << "Heykel paramparça! ";
            if (sansBasi(40)) {
                std::cout << "Efsanevi bir kılıç toprağa düştü!\n" << RS;
                Esya efs;
                efs.ad = "\xf0\x9f\x9f\xa0 Melek Kılıcı (Efsanevi)";
                efs.tur = EsyaTuru::Silah; efs.nadir = Nadir::Legendary;
                efs.bHasar = 28; efs.deger = 500; efs.sevGer = 3;
                efs.ozel   = "kahraman";
                if ((int)o.envanter.size() < o.mEnvanter) o.envanter.push_back(efs);
            } else {
                std::cout << "Lanetlendiniz! -30 can\n" << RS;
                o.can -= 30;
                o.kanamaSure = 3;
            }
        }
    }
    else if (tip == 9) {
        int ag = 8 + kat * 4;
        o.altin -= ag;
        if (o.altin < 0) o.altin = 0;
        std::cout << "\n\xf0\x9f\x92\x96 [LANET ODA] Bir ses altınınızı aldı! -" << ag << " Altın\n";
    }
    else if (tip == 10) {
        std::cout << "\n\xf0\x9f\x94\xae [BÜYÜ ÇEMBERI] Parlayan rünler!\n";
        if (sansBasi(50)) {
            std::cout << PYE << "Büyü çemberi sizi korudu! Sonraki 2 tur hasar -%30\n" << RS;
        } else {
            int h = 10;
            o.can -= h;
            std::cout << KR << "Rünler patladı! -" << h << " can\n" << RS;
        }
    }
    else if (tip >= 11 && tip <= 13) {
        // Bölgeye özel olaylar
        switch (bolge) {
            case Bolge::Zindan:
                std::cout << "\n\xf0\x9f\x96\xa4 [KEMİK YIĞINI] Eski bir kahraman zırhı!\n";
                if ((int)o.envanter.size() < o.mEnvanter) {
                    Esya zirh2; zirh2.ad = "Eski Zindan Zırhı"; zirh2.tur = EsyaTuru::Zirh;
                    zirh2.nadir = Nadir::Common; zirh2.bSav = 5; zirh2.deger = 30; zirh2.sevGer = 1;
                    o.envanter.push_back(zirh2);
                    std::cout << PYE << "Zırh bulundu!\n" << RS;
                }
                break;
            case Bolge::LavMag:
                std::cout << "\n\xf0\x9f\x8c\x8b [LAV HAVUZU] Yakın durmayın!\n";
                if (sansBasi(50)) { o.can -= 15; std::cout << KR << "-15 can!\n" << RS; }
                else { std::cout << PYE << "Güvenli geçiş!\n" << RS; }
                break;
            case Bolge::BuyOrman:
                std::cout << "\n\xf0\x9f\x8d\x84 [BİTKİ] Nadide şifalı bir bitki buldunuz!\n";
                o.can = std::min(o.maksCan, o.can + 25);
                std::cout << PYE << "+25 can!\n" << RS;
                break;
            case Bolge::OlSehir:
                std::cout << "\n\xf0\x9f\x92\x80 [RUHLAR] Eski ruhlar etrafınızda dönüyor!\n";
                if (o.sinif == KarSinif::Nekromancer) {
                    std::cout << PMJ << "Nekromancer olarak onları kontrol ediyorsunuz! +15 mana\n" << RS;
                    o.mana = std::min(o.maksMana, o.mana + 15);
                } else {
                    o.zehirSure = 2;
                    std::cout << KR << "Ruhlar sizi rahatsız ediyor! 2 tur zehir\n" << RS;
                }
                break;
            case Bolge::KarKale:
                std::cout << "\n\xe2\x9a\xa1 [KAOS ENERJİSİ] Duvarda parlayan rünler!\n";
                if (sansBasi(50)) {
                    o.temelHas += 2;
                    std::cout << PSR << "+2 kalıcı hasar!\n" << RS;
                } else {
                    o.can -= 18;
                    std::cout << PKR << "Kaos enerjisi çarptı! -18 can\n" << RS;
                }
                break;
        }
    }
    else if (tip == 14) {
        // Kara pazar
        if (sansBasi(15)) {
            std::cout << "\n\xf0\x9f\x8c\x91 [KARA PAZAR] Karanlık bir tüccar beliriverdi!\n";
            std::cout << "1. Efsanevi eşya al (300 Altın)  2. Ruh sat (+10 hasar, -20 maks can)  3. Geç\nSeçim: ";
            int sc = guvenliOku(1, 3);
            if (sc == 1 && o.altin >= 300) {
                o.altin -= 300;
                Esya kp; kp.ad = "\xf0\x9f\x8c\x91 Kara Pazar Silahı";
                kp.tur = EsyaTuru::Silah; kp.nadir = Nadir::Legendary;
                kp.bHasar = 30; kp.deger = 0; kp.sevGer = 1; kp.ozel = "vampir";
                if ((int)o.envanter.size() < o.mEnvanter) {
                    o.envanter.push_back(kp);
                    std::cout << PSR << "Karanlık güç elinizde!\n" << RS;
                }
            } else if (sc == 2) {
                o.temelHas += 10;
                o.maksCan   = std::max(10, o.maksCan - 20);
                o.can       = std::min(o.can, o.maksCan);
                std::cout << PKR << "Ruhunuzun bir parçasını sattınız! +10 hasar, -20 maks can\n" << RS;
                o.golgeItibar += 15;
            }
        }
    }
    else if (tip == 15) {
        std::cout << "\n\xf0\x9f\x90\xbe [EVCİL HAYVAN] Küçük bir yaratık size bakıyor...\n";
        if (!o.evcil.var) {
            std::cout << "1. Yanına al  2. Bırak\nSeçim: ";
            int sc = guvenliOku(1, 2);
            if (sc == 1) {
                static const std::string adlar[] = {"Kor","Gölge","Işık","Talon","Rüzgar","Duman","Kül"};
                static const std::string turler[] = {"Tilki","Kuzgun","Ejderha Yavrusu","Kedi","Baykuş"};
                o.evcil.var    = true;
                o.evcil.ad     = adlar[rng(0,6)];
                o.evcil.tur    = turler[rng(0,4)];
                o.evcil.bHasar = 8 + rng(0, 6);
                o.evcil.bSav   = rng(0, 3);
                o.evcil.sevgi  = 60;
                std::cout << PYE << "\xf0\x9f\x90\xbe " << o.evcil.ad << " (" << o.evcil.tur << ") yanınıza katıldı!\n" << RS;
                o.basarimKontrol("evcil");
            }
        } else {
            o.evcil.sevgi = std::min(100, o.evcil.sevgi + 10);
            std::cout << PYE << "\xf0\x9f\x90\xbe " << o.evcil.ad << " mutlulukla sizinle oynuyor! +10 sevgi\n" << RS;
        }
    }
    else if (tip == 16) {
        std::cout << "\n\xf0\x9f\x8f\x9b\xef\xb8\x8f [MİRAS IŞIĞI] Hayaletimsi bir siluet!...\n";
        std::cout << DM << "Eski bir kahramanın ruhu size küçük bir güç bırakıyor...\n" << RS;
        o.can = std::min(o.maksCan, o.can + 15);
        o.mana = std::min(o.maksMana, o.mana + 10);
    }
    else if (tip == 17) {
        if (sansBasi(40)) {
            std::cout << "\n\xf0\x9f\x8d\xba [TAVERNA] Küçük bir dinlenme noktası!\n";
            std::cout << "1. Dinlen (Ücretsiz! +30 can)  2. Devam et\nSeçim: ";
            int sc = guvenliOku(1, 2);
            if (sc == 1) {
                o.can = std::min(o.maksCan, o.can + 30);
                std::cout << PYE << "+30 can dinlendik!\n" << RS;
            }
        }
    }
    else if (tip == 18) {
        std::cout << "\n\xf0\x9f\x94\xae [BÜYÜCÜ RUHU] Eski büyücünün ruhu rehberlik ediyor!\n";
        if (o.mana < o.maksMana / 2) {
            o.mana = o.maksMana;
            std::cout << PCY << "Mana tamamen yenilendi!\n" << RS;
        } else {
            std::cout << PCY << "Yeni bir büyü ipucu öğrendiniz. (Hasar +5 bu savaşa)\n" << RS;
            o.temelHas += 1; // Küçük kalıcı bonus
        }
    }
    else {
        std::cout << "\n\xf0\x9f\x96\xa4 [SAKİN ODA] Karanlık ve sessiz bir oda... Hiçbir şey yok.\n";
    }
}

// ═══════════════════════════════════════════════════════════════
//  §13  Zindan Keşif Döngüsü
// ═══════════════════════════════════════════════════════════════
static const std::string bolgeAdlari[] = {
    "Antika Zindan \xf0\x9f\x8f\xb0",
    "Lav Mağaraları \xf0\x9f\x8c\x8b",
    "Büyülü Orman \xf0\x9f\x8c\xbf",
    "Ölüler Şehri \xf0\x9f\x92\x80",
    "Karanlık Kale \xf0\x9f\x8f\xaf"
};

void zindanKesfet(Oyuncu& o, MetaVeri& meta) {
    // Bölge seçimi
    baslik("ZINDAN GİRİŞİ");
    std::cout << "  Hangi bölgeye gireceksiniz?\n\n";
    int maxBolge = std::min(5, o.kesfedilenBolge + 1);
    for (int i = 0; i < maxBolge; ++i)
        std::cout << "  " << (i+1) << ". " << bolgeAdlari[i] << "\n";
    std::cout << "  0. Geri\n  Seçim: ";
    int bsec = guvenliOku(0, maxBolge);
    if (bsec == 0) return;
    Bolge secBolge = (Bolge)(bsec - 1);

    if (bsec > o.kesfedilenBolge) {
        o.kesfedilenBolge = bsec;
        o.basarimKontrol("bolge");
    }

    int aktifKat = 1;
    while (o.can > 0) {
        dcizgi();
        std::cout << PCY + BD << "  " << bolgeAdlari[(int)secBolge]
                  << "  —  Kat " << aktifKat << RS << "\n";
        dcizgi();

        olayTetikle(o, aktifKat, secBolge);

        if (o.can <= 0) {
            std::cout << "\n\xf0\x9f\x92\x80 Tehlikeler arasında yitip gittiniz...\n";
            break;
        }

        // Düşman türü — boss katında %10 ihtimalle Gizli Patron çıkar
        DusmanTur dTur = DusmanTur::Normal;
        if (aktifKat % 10 == 0) {
            if (sansBasi(10))
                dTur = DusmanTur::GizliBoss;
            else
                dTur = DusmanTur::Boss;
        } else if (aktifKat % 5  == 0) {
            dTur = DusmanTur::MiniBoss;
        } else if (sansBasi(15)) {
            dTur = DusmanTur::Elite;
        }

        // Gizli Patron duyurusu
        if (dTur == DusmanTur::GizliBoss) {
            dcizgi(58);
            std::cout << PSR + BD
                      << "  \xf0\x9f\x94\xae G\xc4\xb0ZL\xc4\xb0 PATRON H\xc4\xb0SSED\xc4\xb0L\xc4\xb0YOR...\n"
                      << "  Efsanelerde bahsedilen varl\xc4\xb1k kendini g\xc3\xb6steriyor!\n" << RS;
            dcizgi(58);
        }

        Dusman d = dusmanYarat(aktifKat, dTur, secBolge);

        bool zafer = savas(o, d);
        if (!zafer) {
            o.can = 0;
            // Meta-ilerleme: ölüm
            ++meta.olum;
            int ruhKazanimi = aktifKat * 2 + o.seviye;
            meta.ruhTasi += ruhKazanimi;
            std::cout << PMJ << "\n\xf0\x9f\x92\x8e +" << ruhKazanimi
                      << " Ruh Taşı kazandınız! (Toplam: " << meta.ruhTasi << ")\n" << RS;
            // Miras eşyası bırak
            if (!o.silah.ad.empty() && o.silah.ad != "Bos") {
                meta.mirasAd    = o.silah.ad;
                meta.mirasHasar = o.silah.bHasar / 2;
                meta.mirasSav   = 0;
                std::cout << PSR << "\xf0\x9f\x8f\x9b\xef\xb8\x8f " << o.ad << "'ın mirası: "
                          << meta.mirasAd << " bir sonraki kahramana kalıyor.\n" << RS;
            }
            meta.kaydet();
            break;
        }

        // Gizli Patron yenildi → efsaneviAcik bayrağını aç
        if (d.turu == DusmanTur::GizliBoss && !meta.efsaneviAcik) {
            meta.efsaneviAcik = true;
            meta.kaydet();
            std::cout << "\n" << PSR + BD
                      << u8"\U0001F52E EFSANEV\u0130 \u015EANS A\u00C7ILDI!\n"
                      << u8"  Art\u0131k efsanevi e\u015Fyalar daha s\u0131k d\u00FC\u015Fecek!\n" << RS;
        }

        ++meta.zafer;
        if (meta.zafer >= 3 && !meta.nekAcik) {
            meta.nekAcik = true;
            meta.kaydet();
            std::cout << "\n" << PMJ + BD
                      << "\xf0\x9f\x92\x80 G\xc4\xb0ZL\xc4\xb0 SINIF A\xc3\x87ILDI: NEKROMANCERl!\n" << RS;
        }

        std::cout << "\n  1. Bir Sonraki Kata İlerle \xf0\x9f\x9a\xaa\n"
                  << "  2. Kasabaya Geri Dön \xf0\x9f\x8f\x98\xef\xb8\x8f\n  Seçim: ";
        int sc = guvenliOku(1, 2);
        if (sc == 2) {
            std::cout << "\n\xf0\x9f\x8f\x98\xef\xb8\x8f Elde ettiğin ganimetlerle kasabaya döndün.\n";
            break;
        }
        ++aktifKat;
    }
}

// ═══════════════════════════════════════════════════════════════
//  §14  Envanter Yönetimi
// ═══════════════════════════════════════════════════════════════
void envanteriYonet(Oyuncu& o) {
    while (true) {
        baslik("SIRT ÇANTASI \xf0\x9f\x8e\x92");
        std::cout << "  Kapasite: " << o.envanter.size() << "/" << o.mEnvanter << "\n\n";
        if (o.envanter.empty()) {
            std::cout << "  Çantanız bomboş.\n";
        } else {
            for (size_t i = 0; i < o.envanter.size(); ++i) {
                auto& e = o.envanter[i];
                std::cout << "  " << (i+1) << ". " << nadirRenk(e.nadir)
                          << nadirEtiket(e.nadir) << " " << e.ad << RS
                          << " [+" << e.arti << "]";
                if (e.tur == EsyaTuru::Silah) std::cout << " Hasar+" << e.bHasar;
                else                           std::cout << " Sav+"   << e.bSav;
                if (!e.ozel.empty()) std::cout << " [" << e.ozel << "]";
                std::cout << " | Sev.Ger:" << e.sevGer << " | " << e.deger << " Altın\n";
            }
        }
        std::cout << "\n  0. Kapat\n  Kuşanmak için numara girin: ";
        int sc;
        std::cin >> sc;
        if (std::cin.fail()) { temizle(); continue; }
        if (sc == 0) break;
        if (sc < 1 || sc > (int)o.envanter.size()) continue;

        Esya& se = o.envanter[sc-1];
        if (o.seviye < se.sevGer) {
            std::cout << KR << "\xe2\x9d\x8c Seviye yetersiz! (Gerekli: " << se.sevGer << ")\n" << RS;
            bekle(); continue;
        }
        if (se.tur == EsyaTuru::Silah) {
            std::swap(o.silah, o.envanter[sc-1]);
            std::cout << PYE << "\xe2\x9a\x94\xef\xb8\x8f " << o.silah.ad << " kuşandı!\n" << RS;
        } else {
            std::swap(o.zirh, o.envanter[sc-1]);
            std::cout << PYE << "\xf0\x9f\x9b\xa1\xef\xb8\x8f " << o.zirh.ad << " giyildi!\n" << RS;
        }
        bekle();
    }
}

// ═══════════════════════════════════════════════════════════════
//  §15  Yetenek Ağacı
// ═══════════════════════════════════════════════════════════════
void yetenekAgaci(Oyuncu& o) {
    while (true) {
        baslik("YETENEK AĞACI \xf0\x9f\xa7\xac");
        std::cout << "  Mevcut Puan: " << BD + PSR << o.yetenekPuani << RS << "\n\n";

        switch (o.sinif) {
            case KarSinif::Savasci:
                std::cout << "  1. [PASİF] Demir Deri (Sev:" << o.pasifSev << "/3) — +5 Savunma/sev.\n";
                std::cout << "  2. [AKTİF] Ejderha Kesişi (" << (o.aktifAcik?"AÇIK":"KİLİTLİ — Sev.3+") << ")\n"
                          << "             3x hasar + hasar kadar can çal\n";
                break;
            case KarSinif::Buyucu:
                std::cout << "  1. [PASİF] Mana Akışı (Sev:" << o.pasifSev << "/3) — +30 Maks Mana/sev.\n";
                std::cout << "  2. [AKTİF] Kıyamet Yağmuru (" << (o.aktifAcik?"AÇIK":"KİLİTLİ — Sev.3+") << ")\n"
                          << "             4x hasar + 5 tur yanma\n";
                break;
            case KarSinif::Hirsiz:
                std::cout << "  1. [PASİF] Ölümcül Odak (Sev:" << o.pasifSev << "/3) — +7 Hasar/sev.\n";
                std::cout << "  2. [AKTİF] Suikast (" << (o.aktifAcik?"AÇIK":"KİLİTLİ — Sev.3+") << ")\n"
                          << "             3.5x SAF hasar (zırh yok sayılır)\n";
                break;
            case KarSinif::Paladin:
                std::cout << "  1. [PASİF] Kutsal Aura (Sev:" << o.pasifSev << "/3) — +4 Savunma/sev.\n";
                std::cout << "  2. [AKTİF] Kutsal İnfaz (" << (o.aktifAcik?"AÇIK":"KİLİTLİ — Sev.3+") << ")\n"
                          << "             2.5x hasar + +65 can yenileme\n";
                break;
            case KarSinif::Okcu:
                std::cout << "  1. [PASİF] Keskin Gözlem (Sev:" << o.pasifSev << "/3) — +5 Hasar/sev.\n";
                std::cout << "  2. [AKTİF] Ölüm Yağmuru (" << (o.aktifAcik?"AÇIK":"KİLİTLİ — Sev.3+") << ")\n"
                          << "             5x hasar + 5 tur kanama\n";
                break;
            case KarSinif::Nekromancer:
                std::cout << "  1. [PASİF] Karanlık Pakt (Sev:" << o.pasifSev << "/3) — +4 Hasar/sev.\n";
                std::cout << "  2. [AKTİF] Ruh Patlaması (" << (o.aktifAcik?"AÇIK":"KİLİTLİ — Sev.3+") << ")\n"
                          << "             4x hasar + hasar/5 can çal\n";
                break;
        }
        std::cout << "  0. Geri\n  Seçim: ";
        int sc;
        std::cin >> sc;
        if (std::cin.fail()) { temizle(); continue; }
        if (sc == 0) break;

        if (o.yetenekPuani <= 0) {
            std::cout << KR << "\xe2\x9d\x8c Yetenek puanı yok!\n" << RS;
            bekle(); continue;
        }
        if (sc == 1) {
            if (o.pasifSev < 3) {
                ++o.pasifSev; --o.yetenekPuani;
                if (o.sinif == KarSinif::Buyucu) { o.maksMana += 30; o.mana += 30; }
                std::cout << PYE << "\xe2\x9c\xa8 Pasif yetenek yükseltildi! (Sev." << o.pasifSev << ")\n" << RS;
            } else {
                std::cout << KR << "\xe2\x9d\x8c Zaten maks seviye!\n" << RS;
            }
        } else if (sc == 2) {
            if (!o.aktifAcik) {
                if (o.seviye >= 3) {
                    o.aktifAcik = true; --o.yetenekPuani;
                    std::cout << PKR + BD << "\xf0\x9f\x94\xa5 AKTİF YETENEK AÇILDI!\n" << RS;
                } else {
                    std::cout << KR << "\xe2\x9d\x8c En az 3. seviye gerekli!\n" << RS;
                }
            } else {
                std::cout << SR << "Zaten açık.\n" << RS;
            }
        }
        bekle();
    }
}

// ═══════════════════════════════════════════════════════════════
//  §16  Demirci
// ═══════════════════════════════════════════════════════════════
void demirciMenusu(Oyuncu& o) {
    while (true) {
        baslik("DEMİRCİ USTASI \xe2\x9a\x92\xef\xb8\x8f");
        std::cout << "  Altın: " << BD + PSR << o.altin << RS << "\n\n";
        std::cout << "  1. Silah Güçlendir (" << o.silah.ad << " [+" << o.silah.arti << "]) — 55 Altın\n";
        std::cout << "  2. Zırh Güçlendir  (" << o.zirh.ad  << " [+" << o.zirh.arti  << "]) — 45 Altın\n";
        std::cout << "  3. İksir Al (25 Altın)\n";
        std::cout << "  4. Panzehir Al (18 Altın)\n";
        std::cout << "  5. Büyük İksir Al (50 Altın, +100 can)\n";
        std::cout << "  0. Geri\n  Seçim: ";
        int sc;
        std::cin >> sc;
        if (std::cin.fail()) { temizle(); continue; }
        if (sc == 0) break;

        if      (sc == 1 && o.altin >= 55) { o.altin -= 55; ++o.silah.arti; std::cout << PYE << "\xe2\x9a\x92\xef\xb8\x8f Silah güçlendirildi! [+" << o.silah.arti << "]\n" << RS; }
        else if (sc == 2 && o.altin >= 45) { o.altin -= 45; ++o.zirh.arti;  std::cout << PYE << "\xe2\x9a\x92\xef\xb8\x8f Zırh güçlendirildi! [+"  << o.zirh.arti  << "]\n" << RS; }
        else if (sc == 3 && o.altin >= 25) { o.altin -= 25; ++o.iksir;     std::cout << PYE << "\xf0\x9f\xa7\xaa +1 İksir!\n" << RS; }
        else if (sc == 4 && o.altin >= 18) { o.altin -= 18; ++o.panzehir;  std::cout << PYE << "\xf0\x9f\x9f\xa2 +1 Panzehir!\n" << RS; }
        else if (sc == 5 && o.altin >= 50) {
            o.altin -= 50;
            o.can = std::min(o.maksCan, o.can + 100);
            std::cout << PYE << "\xf0\x9f\x92\x96 Büyük iksir! +100 can!\n" << RS;
        }
        else {
            std::cout << KR << "\xe2\x9d\x8c Yeterli altın yok!\n" << RS;
        }
        bekle();
    }
}

// ═══════════════════════════════════════════════════════════════
//  §17  Simyacı (Crafting)
// ═══════════════════════════════════════════════════════════════
void simyaciMenusu(Oyuncu& o) {
    while (true) {
        baslik("SİMYACI \xe2\x9a\x97\xef\xb8\x8f");
        std::cout << "  Malzemeleriniz:\n";
        if (o.malzemeler.empty()) {
            std::cout << "  (Henüz malzeme yok — düşmanlar düşürür)\n";
        } else {
            for (auto& m : o.malzemeler)
                std::cout << "  \xe2\x80\xa2 " << m.ad << " x" << m.miktar << "\n";
        }
        std::cout << "\n  Tarifler:\n";
        std::cout << "  1. Şifalı Merhem (2 Zehir Otu) → +1 İksir\n";
        std::cout << "  2. Güç İksiri (1 Kor Taşı + 1 Kan Taşı) → +3 Hasar (kalıcı)\n";
        std::cout << "  3. Savunma Tozu (2 Demir Parçası) → +2 Savunma (kalıcı)\n";
        std::cout << "  4. Zehir Bombası (1 Zehir Otu + 1 Kemik Tozu) → Sonraki sav. 4 tur zehir\n";
        std::cout << "  0. Geri\n  Seçim: ";
        int sc;
        std::cin >> sc;
        if (std::cin.fail()) { temizle(); continue; }
        if (sc == 0) break;

        auto malzBul = [&](const std::string& isim) -> Malzeme* {
            for (auto& m : o.malzemeler)
                if (m.ad == isim) return &m;
            return nullptr;
        };
        auto malzHarca = [&](const std::string& isim, int miktar) -> bool {
            auto* m = malzBul(isim);
            if (!m || m->miktar < miktar) return false;
            m->miktar -= miktar;
            if (m->miktar == 0)
                o.malzemeler.erase(std::remove_if(o.malzemeler.begin(), o.malzemeler.end(),
                    [&](const Malzeme& x){ return x.ad == isim && x.miktar == 0; }),
                    o.malzemeler.end());
            return true;
        };

        if (sc == 1) {
            if (malzHarca("\xf0\x9f\x8c\xbf Zehir Otu", 2)) {
                ++o.iksir;
                std::cout << PYE << "Şifalı merhem yapıldı! +1 İksir\n" << RS;
            } else std::cout << KR << "\xe2\x9d\x8c Yetersiz malzeme! (2x Zehir Otu gerekli)\n" << RS;
        }
        else if (sc == 2) {
            if (malzHarca("\xf0\x9f\x94\xa5 Kor Taşı", 1) && malzHarca("\xf0\x9f\x94\xb4 Kan Taşı", 1)) {
                o.temelHas += 3;
                std::cout << PKR << "+3 kalıcı hasar!\n" << RS;
            } else std::cout << KR << "\xe2\x9d\x8c Yetersiz malzeme!\n" << RS;
        }
        else if (sc == 3) {
            if (malzHarca("\xf0\x9f\xaa\xa8 Demir Parçası", 2)) {
                o.sav += 2;
                std::cout << PMB << "+2 kalıcı savunma!\n" << RS;
            } else std::cout << KR << "\xe2\x9d\x8c Yetersiz malzeme! (2x Demir Parçası)\n" << RS;
        }
        else if (sc == 4) {
            if (malzHarca("\xf0\x9f\x8c\xbf Zehir Otu", 1) && malzHarca("\xf0\x9f\xa6\xb4 Kemik Tozu", 1)) {
                o.zehirSure = std::max(o.zehirSure, 4);
                std::cout << MJ << "Zehir bombası kullanıldı! Zehir efekti aktif.\n" << RS;
            } else std::cout << KR << "\xe2\x9d\x8c Yetersiz malzeme!\n" << RS;
        }
        bekle();
    }
}

// ═══════════════════════════════════════════════════════════════
//  §18  Taverna Mini-Oyunları
// ═══════════════════════════════════════════════════════════════
void zarDüellosu(Oyuncu& o) {
    baslik("ZAR DÜELLOSU \xf0\x9f\x8e\xb2");
    std::cout << "  Bahis: 30 Altın | Paranız: " << o.altin << " Altın\n";
    std::cout << "  Kazanırsanız 2x, yüksek toplamla 3x!\n\n";
    if (o.altin < 30) { std::cout << KR << "\xe2\x9d\x8c Yeterli altın yok! (30 gerekli)\n" << RS; bekle(); return; }
    std::cout << "  Oyna? (1. Evet  0. Hayır): ";
    if (guvenliOku(0,1) == 0) return;
    o.altin -= 30;

    int oSkor = rng(1,6) + rng(1,6);
    int kSkor = rng(1,6) + rng(1,6);
    std::cout << "\n  Sizin zarlarınız: " << oSkor << "\n";
    std::cout << "  Krupiyenin zarları: " << kSkor << "\n\n";

    if (oSkor > kSkor) {
        int kazanc = (oSkor >= 10) ? 90 : 60;
        o.altin += kazanc;
        std::cout << PYE + BD << "  KAZANDINIZ! +" << kazanc << " Altın!\n" << RS;
    } else if (oSkor == kSkor) {
        o.altin += 30;
        std::cout << PSR << "  BERABERE! Paranız iade!\n" << RS;
    } else {
        std::cout << PKR << "  KAYBETTİNİZ! -30 Altın\n" << RS;
    }
    bekle();
}

void kartTahmini(Oyuncu& o) {
    baslik("KART TAHMİNİ \xf0\x9f\x83\x8f");
    std::cout << "  Bahis: 25 Altın | Paranız: " << o.altin << " Altın\n";
    std::cout << "  Kart rengini doğru tahmin et → 2x!\n\n";
    if (o.altin < 25) { std::cout << KR << "\xe2\x9d\x8c Yeterli altın yok!\n" << RS; bekle(); return; }
    std::cout << "  Oyna? (1. Evet  0. Hayır): ";
    if (guvenliOku(0,1) == 0) return;
    o.altin -= 25;

    std::cout << "\n  Tahmin: 1. Kırmızı  2. Siyah\n  Seçim: ";
    int tahmin = guvenliOku(1, 2);
    int gercek = rng(1, 2);
    std::cout << "  Kart: " << (gercek == 1 ? PKR + "KIRMIZI" : GB + "SİYAH") << RS << "\n\n";
    if (tahmin == gercek) {
        o.altin += 50;
        std::cout << PYE + BD << "  DOĞRU! +50 Altın!\n" << RS;
    } else {
        std::cout << PKR << "  YANLIŞ! -25 Altın\n" << RS;
    }
    bekle();
}

void parmakTuzagi(Oyuncu& o) {
    baslik("SAYI TAHMİNİ \xf0\x9f\xa4\x9e");
    std::cout << "  1-10 arası sayıyı doğru tahmin et!\n";
    std::cout << "  Doğru: +50 Altın  Yanlış: -15 Altın\n";
    std::cout << "  Paranız: " << o.altin << "\n\n";
    if (o.altin < 15) { std::cout << KR << "\xe2\x9d\x8c Yeterli altın yok!\n" << RS; bekle(); return; }
    std::cout << "  Tahmin (1-10): ";
    int tahmin = guvenliOku(1, 10);
    int gercek = rng(1, 10);
    std::cout << "  Doğru sayı: " << BD << gercek << RS << "\n\n";
    if (tahmin == gercek) {
        o.altin += 50;
        std::cout << PYE + BD << "  MÜKEMMELl! +50 Altın!\n" << RS;
    } else {
        o.altin = std::max(0, o.altin - 15);
        std::cout << PKR << "  Yanliş. -15 Altın\n" << RS;
    }
    bekle();
}

void tavernaMenusu(Oyuncu& o) {
    while (true) {
        baslik("TAVERNA \xf0\x9f\x8d\xba");
        std::cout << "  1. Zar Düellosu \xf0\x9f\x8e\xb2\n";
        std::cout << "  2. Kart Tahmini \xf0\x9f\x83\x8f\n";
        std::cout << "  3. Sayı Tahmini \xf0\x9f\xa4\x9e\n";
        std::cout << "  0. Geri\n  Seçim: ";
        int sc;
        std::cin >> sc;
        if (std::cin.fail()) { temizle(); continue; }
        if (sc == 0) break;
        if      (sc == 1) zarDüellosu(o);
        else if (sc == 2) kartTahmini(o);
        else if (sc == 3) parmakTuzagi(o);
    }
}

// ═══════════════════════════════════════════════════════════════
//  §19  Arena Modu
// ═══════════════════════════════════════════════════════════════
void arenaModu(Oyuncu& o) {
    baslik("ARENA: SONSUZ DALGA \xf0\x9f\x8f\x9f\xef\xb8\x8f");
    std::cout << "  Her dalga düşmanları öldür, hayatta kal!\n";
    std::cout << "  Her 5 dalgada ödül! Ölürsen sona erer.\n";
    std::cout << "  Rekorun: " << BD << o.arenaRekor << RS << " dalga\n\n";
    std::cout << "  Başla? (1. Evet  0. Hayır): ";
    if (guvenliOku(0,1) == 0) return;

    int dalgaNo = 0;
    int geciciCan  = o.can;
    int geciciMana = o.mana;

    while (o.can > 0) {
        ++dalgaNo;
        std::cout << "\n" << PSR + BD << "  ═══ DALGA " << dalgaNo << " ═══\n" << RS;

        DusmanTur dTur = DusmanTur::Normal;
        if (dalgaNo % 10 == 0) dTur = DusmanTur::Boss;
        else if (dalgaNo % 5 == 0) dTur = DusmanTur::MiniBoss;
        else if (sansBasi(20)) dTur = DusmanTur::Elite;

        Bolge aBolge = (Bolge)rng(0, 4);
        Dusman d = dusmanYarat(dalgaNo, dTur, aBolge);
        bool zafer = savas(o, d);

        if (!zafer) {
            std::cout << PKR << "\n  Arena'da " << dalgaNo-1 << ". dalgada yenildiniz!\n" << RS;
            break;
        }

        if (dalgaNo > o.arenaRekor) {
            o.arenaRekor = dalgaNo;
            o.basarimKontrol("arena");
        }

        if (dalgaNo % 5 == 0) {
            int odul = 40 * dalgaNo;
            o.altin += odul;
            o.can = std::min(o.maksCan, o.can + 35);
            std::cout << PYE + BD << "\n  \xf0\x9f\x8e\x81 DALGA ÖDÜLÜ: +" << odul << " Altın + +35 can!\n" << RS;
        }

        std::cout << "\n  Devam? (1. Evet  0. Kaç): ";
        if (guvenliOku(0,1) == 0) break;
    }

    o.can  = std::max(1, geciciCan);
    o.mana = geciciMana;
    std::cout << PSR << "\n  Arena'dan çıktınız. Rekor: " << o.arenaRekor << " dalga.\n" << RS;
    bekle();
}

// ═══════════════════════════════════════════════════════════════
//  §20  Görev Panosu
// ═══════════════════════════════════════════════════════════════
void gorevPanosu(Oyuncu& o) {
    baslik("GÖREV PANOSU \xf0\x9f\x93\x9c");
    for (auto& g : o.gorevler) {
        std::cout << "  [" << g.id << "] " << BD << g.ad << RS << "\n";
        std::cout << "      " << g.aciklama << "\n";
        std::cout << "      İlerleme: " << g.mevcut << "/" << g.hedef << " | ";
        if (g.tamam) {
            std::cout << PYE << "\xe2\x9c\x85 TAMAMLANDI\n" << RS;
        } else if (g.mevcut >= g.hedef) {
            g.tamam = true;
            o.altin += g.aOdul;
            if (g.xOdul > 0) o.xpKazan(g.xOdul);
            std::cout << PSR << "\xf0\x9f\x8c\x9f ÖDÜL ALINDI: +" << g.aOdul
                      << " Altın" << (g.xOdul ? ", +" + std::to_string(g.xOdul) + " XP" : "") << "\n" << RS;
        } else {
            std::cout << DM << "\xe2\x8f\xb3 Devam ediyor...\n" << RS;
        }
    }
    cizgi();
    std::cout << "  \xf0\x9f\x8f\x86 Başarımlar (" << std::count_if(o.basarimlar.begin(), o.basarimlar.end(),
                  [](const Basarim& b){ return b.kazanildi; }) << "/" << o.basarimlar.size() << "):\n";
    for (auto& b : o.basarimlar) {
        if (b.kazanildi) std::cout << "  " << PYE << "\xf0\x9f\x8f\x86 " << b.ad << RS << "\n";
        else             std::cout << "  " << DM  << "\xf0\x9f\x94\x92 " << b.ad << RS << "\n";
    }
    bekle();
}

// ═══════════════════════════════════════════════════════════════
//  §21  Meta-İlerleme Menüsü
// ═══════════════════════════════════════════════════════════════
void metaMenusu(Oyuncu& o, MetaVeri& meta) {
    baslik("RUH TAŞLARI \xf0\x9f\x92\x8e");
    std::cout << "  Mevcut Ruh Taşı: " << BD + PMJ << meta.ruhTasi << RS << "\n";
    std::cout << "  Toplam Ölüm: " << meta.olum << " | Zafer: " << meta.zafer << "\n\n";
    std::cout << "  Ruh Taşı Yatırımları:\n";
    std::cout << "  1. +10 Başlangıç Altını (5 Taş)\n";
    std::cout << "  2. +5 Başlangıç Can (3 Taş)\n";
    std::cout << "  3. +1 Başlangıç İksir (8 Taş)\n";
    std::cout << "  0. Geri\n  Seçim: ";
    int sc;
    std::cin >> sc;
    if (std::cin.fail()) { temizle(); return; }

    if (sc == 1 && meta.ruhTasi >= 5) {
        meta.ruhTasi -= 5; o.altin += 10;
        std::cout << PYE << "+10 Altın! (Kalıcı bu oyun)\n" << RS;
        meta.kaydet();
    } else if (sc == 2 && meta.ruhTasi >= 3) {
        meta.ruhTasi -= 3; o.maksCan += 5; o.can += 5;
        std::cout << PYE << "+5 Maks Can!\n" << RS;
        meta.kaydet();
    } else if (sc == 3 && meta.ruhTasi >= 8) {
        meta.ruhTasi -= 8; ++o.iksir;
        std::cout << PYE << "+1 İksir!\n" << RS;
        meta.kaydet();
    } else if (sc != 0) {
        std::cout << KR << "\xe2\x9d\x8c Yetersiz Ruh Taşı!\n" << RS;
    }
    bekle();
}

// ═══════════════════════════════════════════════════════════════
//  §22  Kasaba Ana Menüsü
// ═══════════════════════════════════════════════════════════════
void kasabaMenusu(Oyuncu& o, MetaVeri& meta) {
    while (o.can > 0) {
        dcizgi();
        std::cout << PCY + BD << "  \xf0\x9f\x8f\x98\xef\xb8\x8f KASABA  —  " << o.ad
                  << " [Sev." << o.seviye << "]  \xf0\x9f\x92\xb0 " << o.altin << " Alt.\n" << RS;
        cizgi();
        std::cout << "  \xe2\x9d\xa4\xef\xb8\x8f Can: " << canBari(o.can, o.maksCan, 12)
                  << "  \xf0\x9f\x94\xb7 Mana: " << o.mana << "/" << o.maksMana << "\n";
        cizgi();
        std::cout << "  1. \xf0\x9f\x8f\xb0 Zindana Gir         2. \xf0\x9f\x91\xa4 Karakter Durumu\n"
                  << "  3. \xf0\x9f\x8e\x92 Envanteri Yönet     4. \xf0\x9f\xa7\xac Yetenek Ağacı\n"
                  << "  5. \xe2\x9a\x92\xef\xb8\x8f Demirciye Git       6. \xe2\x9a\x97\xef\xb8\x8f Simyacıya Git\n"
                  << "  7. \xf0\x9f\x8d\xba Tavernaya Git       8. \xf0\x9f\x8f\x9f\xef\xb8\x8f Arena Modu\n"
                  << "  9. \xf0\x9f\x93\x9c Görev Panosu        10. \xf0\x9f\x92\x8e Ruh Taşları\n"
                  << "  11. \xf0\x9f\x9b\x8f\xef\xb8\x8f Han'da Dinlen (10 Altın, tam yenile)\n"
                  << "  12. \xf0\x9f\x92\xbe Oyunu Kaydet\n"
                  << "   0. \xe2\x9d\x8c Çık\n  Seçim: ";

        int sc;
        std::cin >> sc;
        if (std::cin.fail()) { temizle(); continue; }

        switch (sc) {
            case  1: zindanKesfet(o, meta);       break;
            case  2: o.durumGoster(); bekle();     break;
            case  3: envanteriYonet(o);            break;
            case  4: yetenekAgaci(o);              break;
            case  5: demirciMenusu(o);             break;
            case  6: simyaciMenusu(o);             break;
            case  7: tavernaMenusu(o);             break;
            case  8: arenaModu(o);                 break;
            case  9: gorevPanosu(o);               break;
            case 10: metaMenusu(o, meta);          break;
            case 11:
                if (o.altin >= 10) {
                    o.altin -= 10; o.canYenile();
                    std::cout << PYE << "\n\xf0\x9f\x9b\x8f\xef\xb8\x8f Derin uyku. Tüm can ve mana yenilendi!\n" << RS;
                } else {
                    std::cout << KR << "\xe2\x9d\x8c Yeterli altın yok! (10 gerekli)\n" << RS;
                }
                bekle(); break;
            case 12:
                if (oyunuKaydet(o)) {
                    std::cout << PYE << "\n\xf0\x9f\x92\xbe Oyun kaydedildi!\n" << RS;
                } else {
                    std::cout << KR << "\xe2\x9d\x8c Kayıt başarısız!\n" << RS;
                }
                bekle(); break;
            case  0:
                std::cout << "\nMaceradan ayrılıyorsunuz. Hoşça kalın, " << o.ad << "!\n";
                return;
            default:
                std::cout << KR << "Geçersiz seçim!\n" << RS; break;
        }
    }

    if (o.can <= 0) {
        std::cout << "\n" << PKR + BD;
        dcizgi();
        std::cout << "  \xf0\x9f\x92\x80 " << o.ad << "'ın macerası burada son buldu...\n" << RS;
        dcizgi();
    }
}

// ═══════════════════════════════════════════════════════════════
//  §23  Ana Ekran & Başlangıç
// ═══════════════════════════════════════════════════════════════
void anaBaslik() {
    std::cout << "\n" << PSR + BD;
    dcizgi(60);
    std::cout << "\n";
    std::cout << "      ███████╗███████╗███████╗ █████╗ ███╗   ██╗███████╗\n";
    std::cout << "      ██╔════╝██╔════╝██╔════╝██╔══██╗████╗  ██║██╔════╝\n";
    std::cout << "      █████╗  █████╗  ███████╗███████║██╔██╗ ██║█████╗  \n";
    std::cout << "      ██╔══╝  ██╔══╝  ╚════██║██╔══██║██║╚██╗██║██╔══╝  \n";
    std::cout << "      ███████╗██║     ███████║██║  ██║██║ ╚████║███████╗\n";
    std::cout << "      ╚══════╝╚═╝     ╚══════╝╚═╝  ╚═╝╚═╝  ╚═══╝╚══════╝\n";
    std::cout << "\n";
    std::cout << RS + PCY + BD;
    std::cout << "         V A D İ S İ :  Z İ N D A N I N  S I R R I\n";
    std::cout << "                      v 2.0  —  Terminal RPG\n";
    std::cout << RS;
    std::cout << "\n" << PSR + BD;
    dcizgi(60);
    std::cout << RS << "\n";
}

int main() {
    terminalAyarla();
    std::srand((unsigned int)std::time(nullptr));

    anaBaslik();

    // Meta-veri yükle
    MetaVeri meta;
    meta.yukle();

    if (meta.olum > 0 || meta.zafer > 0) {
        std::cout << DM << "  [Meta] Ruh Taşı: " << meta.ruhTasi
                  << " | Toplam Ölüm: " << meta.olum
                  << " | Zafer: " << meta.zafer << "\n" << RS;
        if (!meta.mirasAd.empty())
            std::cout << PSR << "  [Miras] " << meta.mirasAd << " sizi bekliyor!\n" << RS;
        std::cout << "\n";
    }

    std::cout << "  1. Yeni Oyun\n  2. Kayıtlı Oyunu Yükle\n  Seçim: ";
    int bSec = guvenliOku(1, 2);

    Esya bSilah; bSilah.ad = "Tahta Kılıç"; bSilah.tur = EsyaTuru::Silah;
    bSilah.nadir = Nadir::Common; bSilah.bHasar = 4; bSilah.deger = 5; bSilah.sevGer = 1;

    Esya bZirh; bZirh.ad = "Eski Kumaş Zırh"; bZirh.tur = EsyaTuru::Zirh;
    bZirh.nadir = Nadir::Common; bZirh.bSav = 2; bZirh.deger = 5; bZirh.sevGer = 1;

    Oyuncu kahraman;

    if (bSec == 2) {
        if (oyunuYukle(kahraman)) {
            std::cout << PYE << "\n\xf0\x9f\x92\xbe Kayıt yüklendi! Tekrar hoş geldin, "
                      << kahraman.ad << "!\n" << RS;
            bekle();
            kasabaMenusu(kahraman, meta);
            return 0;
        } else {
            std::cout << KR << "Kayıt bulunamadı. Yeni oyun başlıyor...\n" << RS;
        }
    }

    // Yeni oyun — isim
    std::string isim;
    std::cout << "\n  Kahramanının adını gir: ";
    temizle();
    std::getline(std::cin, isim);
    if (isim.empty()) isim = "Adsız Kahraman";

    // Sınıf seçimi
    baslik("SINIF SEÇİMİ");
    std::cout << "  1. Savaşçı \xf0\x9f\x9b\xa1\xef\xb8\x8f  — Yüksek Can/Savunma, Parry, Tank\n";
    std::cout << "  2. Büyücü \xf0\x9f\x94\xae   — Yüksek Mana, Alan Hasarı, Yanma\n";
    std::cout << "  3. Hırsız \xf0\x9f\x97\xa1\xef\xb8\x8f  — Kritik Vuruş, Zehir, Ganimet Bonusu\n";
    std::cout << "  4. Paladin \xe2\x9c\x9c\xef\xb8\x8f  — Şifa+Hasar, Kutsal, Denge\n";
    std::cout << "  5. Okçu \xf0\x9f\x8f\xb9    — Kanama, Zayıflatma, Yüksek Krit\n";
    if (meta.nekAcik)
        std::cout << "  6. Nekromancer \xf0\x9f\x92\x80 — Ruh Gücü, Karanlık Büyü [GİZLİ]\n";
    std::cout << "  Seçim: ";
    int sinifSec = guvenliOku(1, meta.nekAcik ? 6 : 5);
    KarSinif secilenSinif = (KarSinif)sinifSec;

    kahraman = Oyuncu(isim, secilenSinif, bSilah, bZirh, meta);

    std::cout << "\n" << PYE + BD;
    dcizgi();
    std::cout << "  \xe2\x9c\xa8 " << kahraman.ad << " — " << kahraman.sinifAdi()
              << " olarak macera başlıyor!\n" << RS;
    dcizgi();
    bekle();

    kasabaMenusu(kahraman, meta);
    return 0;
}