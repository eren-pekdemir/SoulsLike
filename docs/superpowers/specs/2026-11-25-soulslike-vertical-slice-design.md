# SoulsLike Vertical Slice: Tasarım

Tarih: 2026-11-25. Durum: **onaylandı (2026-11-25)**.
Etiketler: **[Senin]** = senin söylediğin, **[Öneri]** = benim kararım, itiraz edebilirsin, **[Doğrula]** = editörde bakılması gereken varsayım.

## 1. Amaç

- **[Senin]** Düşmanlarla karşılaşılan, bir boss fight'ı olan souls-like; iyi VFX/SFX, güzel bir environment, sonunda kısa bir showcase videosu.
- **[Senin]** İzleyici stüdyolar; bu bir portföy işi.
- **[Senin]** Kodu sen yazıyorsun. Claude mimari, soru-cevap ve review için var.
- **[Öneri]** Başarı ölçütü: izleyen kişi ilk 10 saniyede dövüş hissini görür, kodu açan kişi 5 dakikada mimariyi anlar.

### Hedefler
1. Ağır, okunabilir, adil hissettiren yakın dövüş.
2. Bir normal düşman tipi ve iki fazlı bir boss.
3. Yeni saldırı eklemenin kod gerektirmediği, veri odaklı yapı.
4. 60-90 saniyelik video ve incelenebilir temiz kaynak kod.

### Hedef olmayanlar
Multiplayer, envanter, loot, seviye/stat sistemi, save/load, dünya haritası, birden fazla silah sınıfı, mobil.

### Kaçınılacaklar
- Çok sistem olup hiçbirinin iyi hissettirmemesi.
- Mevcut bir oyunun karakterini, bölümünü veya görünümünü taklit etmek. Kimlik özgün olacak.
- Satın alınmış asset'lerin herkese açık repoya konması.

## 2. Mevcut durum

| Konu | Bulgu |
|---|---|
| Engine | UE 5.8, boş C++ modülü, Enhanced Input ekli |
| Animasyon | Binlerce klip, hepsi Manny iskeletinde; root motion ve `InPlace` sürümleri ayrı |
| Karakter | Dark Knight erkek ve kadın, ayrı iskeletler, kılıç ayrı static mesh, kalkan yok |
| Eksik | VFX, SFX, environment, normal düşman mesh'i, AnimBP, blend space, montage, versiyon kontrolü |

Dikkat:
- Dark Knight iskeletlerinde `weapon_l/r` yok (doğrulandı); kılıç `hand_r` socket'ine bağlanır.
- Erkek iskeletin ölçülen kemikleri Manny ile aynı; kadınınki farklı (kalça, omuz, bacak), retarget gerekebilir.
- Bakılan kliplerde root motion kapalı; kullanılan saldırı/roll kopyalarında açılır.
- Pakette mızrak hareketlerinin bir kısmı `Sphere_` adıyla geçiyor (yazım hatası), aramada kaçırma.
- `_Combo` ekli kliplerin önceki vuruştan devam eden sürüm olduğunu varsayıyorum. **[Doğrula]**

## 3. Yaklaşımlar

| | Artı | Eksi |
|---|---|---|
| **A. GAS + StateTree (seçildi)** | İptal, maliyet ve durum kuralları hazır; portföyde güçlü | Öğrenme ve kurulum maliyeti |
| B. Özel component'ler + FSM | Hızlı başlar, tamamı senin | Durumlar arttıkça iptal/öncelik kuralları dağılır |
| C. GAS + Behavior Tree | En çok kaynak | Boss fazlarını durum olarak ifade etmek daha dolaylı |

Seçim: sen izleyiciyi "stüdyolar, portföy" olarak belirledin; bu durumda A'yı önermiştim.

## 4. Karar özeti [Öneri]

| Konu | Karar | Neden |
|---|---|---|
| Oyuncu | Erkek Dark Knight, çift el kılıç, `TwoHand_Base` | Kalkan mesh'i yok; bu set saldırı, blok, parry, fatal ve roll içeriyor |
| Boss | Kadın Dark Knight, faz 1 `TwoHand_Up` | Oyuncudan farklı duruş, ilk bakışta ayrışır |
| Normal düşman | Dark Knight parçalarından (miğfersiz, farklı materyal) varyant, `Enemy_Attack` seti | Yeni mesh alana kadar eksik kalmaz |
| ASC yeri | Karakterin üstünde | Tek oyunculu, PlayerState'e gerek yok |
| Lock-on | Kendi component'imiz | 5.8'de TargetingSystem hâlâ Experimental klasöründe ve beta |
| Motion Warping | Kullan, tek component üzerinden | Beta; kullanım tek yerde toplansın |
| Hareket | Saldırı/roll root motion, yürüme/koşma `InPlace` | Saldırı mesafesi animasyondan, kontrol hissi koddan gelir |
| Kod düzeni | Tek modül, alan bazlı klasörler | Bu ölçekte çok modül gereksiz |

## 5. Proje kurulumu

Pluginler: `GameplayAbilities`, `StateTree`, `GameplayStateTree`, `MotionWarping`. Niagara zaten açık.

Modül bağımlılıkları: `GameplayAbilities`, `GameplayTags`, `GameplayTasks`, `AIModule`, `NavigationSystem`, `StateTreeModule`, `GameplayStateTreeModule`, `MotionWarping`, `Niagara`, `UMG`.
İhtiyaç doğdukça ekle; hepsini ilk gün ekleme. `Build.cs` içine `PublicIncludePaths.Add(ModuleDirectory)`.

```text
Source/SoulsLike/
  Core/       GameMode, PlayerController, gameplay tag tanımları
  Character/  CharacterBase, PlayerCharacter, EnemyCharacter, BossCharacter
  Abilities/  ASC, AttributeSet, GA'lar, damage execution
  Combat/     CombatComponent, AttackData, hit sonucu, lock-on
  Animation/  AnimInstance, notify state'ler
  AI/         AIController, StateTree task/condition'ları
  UI/         HUD, boss bar
```

İsim öneki: C++ `SL` (`ASLCharacterBase`). Kendi asset'lerin `Content/SoulsLike/` altında; satın alınan klasörlere dokunma, klipleri kopyalayıp montage üret.

Versiyon kontrolü: GitHub Desktop + git; `Source`, `Config`, `docs`, `.uproject` takip edilir, `Content` hariç. Böylece portföye asset sızdırmadan commit geçmişi gösterebilirsin. `Content` yedeği ayrıca çözülecek.

## 6. Temel sistemler

### 6.1 Attribute ve tag'ler

`USLAttributeSet`: Health, MaxHealth, Stamina, MaxStamina, Poise, MaxPoise; meta: IncomingDamage, IncomingPoiseDamage.

```text
Input.*        LightAttack, HeavyAttack, Dodge, Block, Parry, LockOn
Ability.*      Attack.Light, Attack.Heavy, Dodge, Block, Parry, Execution
State.*        Attacking, Dodging, Blocking, Dead, Staggered, Executing
State.Window.* IFrame, Parry, Cancel, Combo
Event.*        Hit, Parried, PoiseBroken, ExecutionStart, Death
Cue.*          Hit.Flesh, Hit.Metal, Parry, Block, PhaseChange
```

Stamina kuralları:
- Souls kuralı: stamina > 0 ise eylem başlar, 0'ın altına inmez. GAS varsayılanı `cost <= current` ister; `CheckCost` override edilir, clamp `PreAttributeChange`/`PostGameplayEffectExecute` içinde yapılır.
- Yenilenme: sonsuz, periyodik bir GE; `State.Attacking/Dodging/Blocking` varken durur.

### 6.2 Saldırı verisi

`USLAttackData` (DataAsset): montage, hasar, poise hasarı, stamina maliyeti, hit tipi, sonraki combo, warp mesafesi, blok edilebilir mi, parry edilebilir mi, cue tag'leri, AI için min/max mesafe, ağırlık ve cooldown.

Oyuncu ve AI aynı veriyi ve aynı `GA_MeleeAttack` sınıfını kullanır. AI saldırıyı tag ve event payload ile seçer.

### 6.3 Pencereler animasyonda

| Notify state | İş |
|---|---|
| `ANS_WeaponTrace` | Hasar trace'ini açar/kapar |
| `ANS_WindowTag` | Verilen `State.Window.*` tag'ini ekler/kaldırır (i-frame, parry, combo, cancel) |
| `ANS_Tracking` | Saldırı başındaki dönüş hızını sınırlar; kapanınca saldırı yön değiştiremez |
| Engine'in `MotionWarping` notify'ı | Hedefe hizalama |

Kural: gameplay zamanlaması için timer kullanılmaz. Notify state montage kesilince de `NotifyEnd` alır; yine de ability `EndAbility` içinde tag'leri temizler.

### 6.4 Input buffer

Basılan eylem 0,2 sn saklanır. `State.Window.Cancel` veya `Combo` açılınca ilk uygun eylem çalışır. Öncelik: dodge > parry > saldırı.

### 6.5 Vuruş tespiti

- Kılıç static mesh'inde `Trace_Start` ve `Trace_End` socket'leri.
- Her karede önceki ve şimdiki konum arasında sweep; hızlı saldırılar için alt adımlara bölünür.
- Vurulanlar saldırı başına bir kez sayılır. Custom trace channel: `Weapon`.

### 6.6 Hit çözümleme sırası

```text
Dead            -> yok say
State.Window.IFrame -> ıska
State.Executing     -> yok say
State.Window.Parry + önden + saldırı parry edilebilir -> Event.Parried
                       normal düşman: doğrudan Staggered
                       boss: 40 poise hasarı (tek parry ile düşmez)
State.Blocking + önden + saldırı blok edilebilir -> stamina düşer, 0 olursa guard break
Aksi            -> hasar + poise hasarı
Poise <= 0      -> Staggered, execution açık
Health <= 0     -> Dead
```

Blok, dodge, parry ve stagger tek yerde çözülür (`USLCombatComponent::ResolveHit`); sonuç `FSLHitResult` olarak döner. Kuralları tek fonksiyonda topla ki okunması kolay olsun.

Yön: vuruşun geldiği açı Front/Back/Left/Right'a ayrılır; `Hit_F/B/L/R` seçilir. Poise kırılınca `Hit_F_Down` gibi büyük tepkiler. Poise, 4 sn vurulmayınca dolar.

### 6.7 Execution

Yalnızca Staggered hedefte ve önden; backstab kapsam dışı. Oyuncu tetiklerse: `Fatal_Attack_N` ve aynı numaralı `Fatal_Hit_N`. Öldürüyorsa `_Die` sürümü. Saldıran Motion Warping ile hedefe göre sabit ofsete çekilir; ikisi de `State.Executing` alır, hasar görmez, montage bir event'te hasar uygular. Ofset klip başına veri. 4 numara iki varyantlı (`4_1`, `4_2`), onu sonra ekle.

### 6.8 Kamera, lock-on, animasyon

- Spring arm, lock-on'da hedef ve oyuncu arasında kadraj. Hedef seçimi: yarıçap içi, ekran merkezine yakınlık ve görüş hattı puanı.
- C++ `USLAnimInstance`, veri thread-safe güncellemede. Lock-on'da 8 yön blend space, serbestte ileri yön. Start/stop/turn klipleri ilk sürümde yok, M7'de cila.
- Full body montage slotu ve vuruş için additive slot.

### 6.9 Feedback

GameplayCue ile VFX/SFX. Hitstop: iki karaktere kısa `CustomTimeDilation` (hafif 0,05 sn, ağır 0,10 sn). Kamera sarsıntısı, kılıç izi (Niagara ribbon), parry parlaması. GAS timer'ları dünya zamanındadır; hitstop sırasında gameplay pencerelerini timer ile ölçme.

### 6.10 Düşman AI

`ASLAIController` + `UStateTreeAIComponent` + perception.

```text
Idle -> Alert -> Combat { Approach, Strafe, Attack, Recover } -> Staggered -> Dead
```

Aynı anda saldıran sayısı bir saldırı jetonu ile 1'de tutulur; oyuncu adil hisseder.

### 6.11 Boss

- **Faz 1:** `TwoHand_Up` A/B setleri, yavaş, net telegraph.
- **Geçiş (%50 can):** dokunulmazlık, `Buff_Weapon`, `Cue.PhaseChange`, müzik değişimi.
- **Faz 2:** `TwoHand_Up` skill'leri eklenir, kısa recover.
- Seçim: mesafe, cooldown ve son kullanılanı cezalandıran ağırlıklı rastgele.
- Poise kırılması execution verir. Boss alanı, kapı ve boss bar. Oyuncu ölürse ekran kararır, karşılaşma baştan başlar (seviye yeniden yüklenir).

## 7. Başlangıç sayıları (hepsi data'da, ayarlanacak)

| | Oyuncu | Düşman | Boss |
|---|---|---|---|
| Health | 100 | 60 | 600 |
| Stamina | 100 | - | - |
| Poise | 30 | 25 | 100 |
| Hafif hasar / poise | 20 / 12 | 15 / 12 | 25 / 20 |
| Ağır hasar / poise | 38 / 28 | 25 / 25 | 40 / 40 |

Stamina maliyeti: hafif 15, ağır 28, dodge 20. Yenilenme 35/sn. Ağır saldırı için `Skill_*` klipleri kullanılacak; pakette `Heavy_Attack` adında klip yok.

## 8. Yol haritası ve bitiş ölçütleri

| # | İş | Bitti sayılır |
|---|---|---|
| M0 | Versiyon kontrolü, iskelet | `Attack_A_1` Dark Knight'ta kılıç elde, kol ve oran bozulmadan oynuyor |
| M1 | Hareket, kamera, lock-on | Lock-on'da 8 yön yürüme/koşma, kamera titremiyor |
| M2 | GAS, stamina, combo, dodge | 4'lü combo, buffer ve i-frame çalışıyor |
| M3 | Trace, hasar, poise, hit tepkisi, hitstop | **Kuklaya vurmak keyifli**; değilse M4'e geçme |
| M4 | Blok, parry, execution | Parry -> stagger -> execution akışı kesintisiz |
| M5 | Bir düşman | Jetonla iki düşmanlık karşılaşma adil |
| M6 | Boss, iki faz | Baştan sona yenilebiliyor, hatasız |
| M7 | VFX, SFX, UI, environment, cila | Video kalitesi |
| M8 | Video ve repo | 60-90 sn video, README ve mimari şeması |

M0'da iskelet kararı: önce Compatible Skeletons. Kol/kılıç kayarsa IK Retargeter ile yalnızca kullanılan ~150 klibi retarget et, hepsini değil.

## 9. Test

Otomatik test yok (kullanıcı kararı, 2026-11-25). Her şey oyun içinde denenir. Her kilometre taşında 30 sn\'lik kısa kayıt alınır.

## 10. Riskler

| Risk | Önlem |
|---|---|
| Oranlar/kemik farkı saldırıyı bozar | M0'ın tek işi bu |
| GAS'ta boğulma | M2'de 1 attribute set, 1 base ability; gerisi gerektikçe |
| Asset eksikliği | M6'ya kadar VFX/SFX ve environment paketini seç |
| Kapsam kayması | Hedef olmayanlar listesi |

## 11. Uygulayıcıya kalan

Sınıf içi detaylar, blend space ayarları, sayıların ince ayarı, klip seçimleri, UI görünümü.
