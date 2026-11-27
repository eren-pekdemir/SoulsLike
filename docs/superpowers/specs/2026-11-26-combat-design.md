# Dövüş Tasarımı

Tarih: 2026-11-26. Durum: **onaylandı (2026-11-26)**.
`2026-11-25-soulslike-vertical-slice-design.md` dokümanının dövüş ve yapay zekâ bölümlerinin (6.4-6.11, 7, 8'in M3-M6'sı) yerine geçer. Mimari (GAS, StateTree, veri asset'leri) aynı kalır.

Etiketler: **[Senin]** senin söylediğin, **[Öneri]** benim kararım.

## 1. Hedef
- **[Senin]** Souls-like olmak şart değil; hedef iyi bir dövüş.
- **[Senin]** Ağır, her vuruşu hissedilen bir dövüş. Uzun kombo, havada tutma, anime havası yok.
- **[Senin]** Zorluk düşmanın **iyi oynamasından** gelsin, yüksek candan değil. Düşman blok ve parry yapar, hemen ölmez.
- **[Senin]** Aynı anda 1-2 düşman. Hepsi birden saldırmaz.
- **[Senin]** Düşman yere düşer, yanına gidip kılıcı saplarsın. "Yaptım" anı bu.
- **[Senin]** Savunmayı açma yolları: **ağır vuruşla blok kırma** ve **tekme**.
- **[Senin]** İyi bir yapay zekâ yazmak istiyorsun; portföyün yıldızı bu.
- **[Öneri]** Başarı ölçütü: aynı düşmanı ilk denemede kaybetmek, üçüncüde temiz yenmek ve neden kazandığını bilmek.

### Kaçınılacaklar
- Canı şişirilmiş, vurdukça bitmeyen düşmanlar.
- Oyuncunun göremediği bir sebeple kaybetmesi (tepki süresi sıfır yapay zekâ, telegraph'sız saldırı).
- Aynı anda iki düşmandan birden darbe yemek.

## 2. Çekirdek döngü
```text
Düşman savunuyor ──► Oyuncu açar (ağır vuruş / tekme / parry)
        ▲                         │
        │                         ▼
  Düşman toparlanır ◄── Sendeleme (kısa pencere)
                                  │ ağır vuruş
                                  ▼
                       Yere düşme ──► Bitirici (ölüm)
```

## 3. Üçgen: oyuncunun üç aracı
| Araç | Güçlü olduğu | Zayıf olduğu |
|---|---|---|
| **Hafif saldırı** | Hızlı, açık düşmanı cezalandırır | Bloklanır, **parry edilebilir** |
| **Ağır saldırı** | Bloğa büyük denge hasarı, parry edilemez | Yavaş, hazırlığı görünür, kaçılır ve cezalandırılır |
| **Tekme** | Bloklayan düşmanı **anında** sendeletir | Saldıran düşmana karşı işe yaramaz, kesilir |

Bu üçgen hem oyuncuya karar verdirir hem de yapay zekâya okuyacak bir şey verir: hafif saldırı yağdırana parry, ağır saldırının hazırlığına kaçış, pasif oyuncuya saldırı.

## 4. Kurallar [Öneri]
### Can
Düşman canı düşük: hafif 25, ağır 40, düşman canı 100. Açılmış bir düşman 3-4 vuruşta ölür. Zorluk, açmaktır.

### Denge (guard) barı, yalnızca düşmanlarda
- Max 100. Blokladığı hafif vuruş 15, ağır vuruş 50 düşürür.
- Vurulmadığı her 2 sn'de dolmaya başlar (20/sn).
- 0'a inerse **sendeleme**.

### Sendeleme
- 1,5 sn savunmasız.
- Bu sırada **ağır vuruş → yere düşme**. Hafif vuruşlar normal hasar verir.

### Tekme
- Stamina 25, kısa hazırlık, düşmana kök hareketle 0,5 m yaklaşır.
- Bloklayan düşmana: denge ne olursa olsun **anında sendeleme**.
- Saldırmayan, bloklamayan düşmana: kısa sarsılma, hasar yok.
- Hazırlıkta vurulursa kesilir.

### Yere düşme ve bitirici
- Düşman ~2,5 sn yerde kalır.
- Yanında (≤2 m) ve önünde etkileşim tuşu: **bitirici**. Can ne olursa olsun düşman ölür.
- Süre dolarsa kalkar, denge yarıdan başlar.

### Parry
- Oyuncu: düşman saldırısının vuruş anından 0,2 sn önceki pencerede parry → düşman **sendeler**.
- Düşman: oyuncunun **hafif** saldırısını parry edebilir → oyuncu geri sekiyor, düşmana karşı saldırı fırsatı.
- Ağır saldırı ve tekme parry edilemez.

### Oyuncu bloğu
Stamina harcar (hafif 15, ağır 35). Stamina biterse oyuncunun gardı kırılır.

## 5. Animasyonlar (ölçüldü)
| İhtiyaç | Klip | Not |
|---|---|---|
| Hafif zincir | Base `A: 1 → 2 → 4` | Geçişler birebir; `docs/combat/2026-11-26-attack-catalog.md` |
| Ağır saldırı | Up `Attack_B_1` ve `B_3`, ya da Base `Skill_2` | Vuruş 0,7-1,3 sn, ağır his; denenecek |
| Tekme | `Actions/Obj_Action/AS_Obj_Kick` | Alçak itme tekmesi, ayak 35 cm. Kollar kılıç pozunda değil: üst gövde iki el idle'ından katmanlanır. Kötü görünürse dışarıdan tekme animasyonu alınır. |
| Sendeleme | `Hit/Hit/Front/AS_Hit_F_*` | |
| Yere düşme | `AS_Hit_F_Down` | 0,5 sn'de yerde; kalkış kısmı ayrı section |
| Bitirici | `Fatal_Attack_5` + `Fatal_Hit_5_Die` | Kurban yerde başlar, kılıç 1 m öndeki yere saplanır. 5,2 sn uzun; hızlandırılabilir |
| Blok, parry | `TwoHand_Base/Block`, `Weapon_Parry` | Oyuncu ve düşman ortak |
| Düşman saldırıları | `TwoHand_Base/Up` setleri | Düşman oyuncuyla aynı seti kullanır |

## 6. Yapay zekâ
### Yapı
- **StateTree**: `Idle → Alert → Combat → Staggered → Down → Dead`.
- **Combat** içinde eylem **puanlanarak** seçilir: yaklaş, etrafında dön, saldır, blok, parry, geri çekil, tekme.

### Oyuncuyu okuma
- Oyuncunun ASC'sindeki gameplay tag'leri: `State.Attack.Light.Windup`, `State.Attack.Heavy.Windup`, `State.Kick`, `State.Blocking`.
- Düşman bunları **tepki süresi** sonra görür (0,2-0,4 sn), insan gibi.
- Yaptığı tepki her seferinde başarılı olmaz; başarı olasılığı kişiliğe bağlı.

### Kişilik (veri asset'i)
`Aggression`, `Patience`, `ParryChance`, `BlockChance`, `DodgeChance`, `ReactionTime`, `PunishWindow`. Aynı kod, farklı ayarlar: temkinli kılıç ustası, saldırgan haydut.

### Grup davranışı
- **Saldırı jetonu**: aynı anda tek düşman saldırır. Diğeri yan tarafa geçer, mesafe tutar, tehdit eder.
- Jetonu uzun tutan düşman sırasını bırakır.

### Adalet
- Her düşman saldırısında görünür bir hazırlık (en az 0,4 sn).
- Bir saniye içinde iki farklı düşmandan darbe gelmez.

## 7. Kilometre taşları (M2 sonrası yeniden sıralandı)
| # | İş | Bitti sayılır |
|---|---|---|
| M2 | Stamina, hafif zincir, ağır saldırı, dodge, input buffer | Kuklaya vurmak ağır ve tepkili hissettiriyor |
| M3 | Silah trace, hasar, hit reaction, hitstop, kamera sarsıntısı, ses | Her vuruş hissediliyor |
| M4 | Düşman bloğu, denge barı, tekme, sendeleme, yere düşme, bitirici; oyuncu bloğu ve parry | Kukla düşmanı açıp bitirebiliyorsun |
| M5 | Yapay zekâ: StateTree, puanlama, oyuncuyu okuma, kişilik, saldırı jetonu | İki düşmanla dövüş adil ve zor |
| M6 | Boss (kadın Dark Knight) | Fazlı, kendine özgü saldırılar |
| M7 | VFX, SFX, UI, environment | Video kalitesi |
| M8 | Video ve repo | |

## 8. Sonraya kalanlar
Büyüler, kılıç buff'ı (ateş/buz), ikinci duruş. Çekirdek iyi hissettirdikten sonra değerlendirilir.
