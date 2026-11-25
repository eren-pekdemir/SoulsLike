# M0-M1: Temel ve Hareket Planı

> Bu planı **sen** uyguluyorsun. Adımlar imza, değer ve test verir; gövdeleri sen yazarsın. Takıldığında veya bir görev bittiğinde Claude'a sor / review iste.

**Hedef:** Dark Knight'ı kılıçla, kameralı ve lock-on'lu 8 yön hareket eder hale getirmek.

**Mimari:** Karakter sınıfları ince kalır. Hesaplar (hız, hedef seçimi, kamera açısı) test edilebilir saf fonksiyonlarda, durum component'lerde, görsel karar AnimBP'de. GAS bu planda **yok**, M2'de girer.

**Teknoloji:** UE 5.8.4, C++, Enhanced Input, Automation Test, Rider.

**Tasarım:** `docs/superpowers/specs/2026-11-25-soulslike-vertical-slice-design.md`

## Ölçülen değerler (editör arayüzsüz çalıştırılarak okundu)

| Konu | Sonuç | Etkisi |
|---|---|---|
| Erkek DK iskeleti | Ölçülen kemikler Manny ile aynı (tek fark `hand_r` 0,11 cm) | Compatible Skeletons büyük olasılıkla yeter |
| Kadın DK iskeleti | `upperarm_r` −%14, `thigh_r` +%13, `calf_r` +%6 | Çift el tutuşu kayabilir, retarget gerekebilir |
| `weapon_r` | DK'da yok; `Attack_A_1` boyunca sabit | `hand_r` üstüne socket yeterli |
| Kılıç | Kabza orijinde, namlu +Y yönünde 114 cm | Socket dönüşü elle ayarlanır |
| Root motion | Bakılan tüm kliplerde **kapalı** | Saldırı/roll kopyalarında açılmalı |
| Hızlar (cm/sn) | Yürüme ileri 164, yan 150-180, geri 148. Koşma ileri 493, yan 323, geri 292 | Yöne göre hız sınırı şart |
| Mesafeler | `Attack_A_1` 168 cm, `Roll_F` 476 cm | M0 kontrol değeri |
| `_Combo` | `A_2_Combo` idle'dan farklı, `A_2` idle'a yakın başlıyor (tek kemik bakıldı) | Varsayım güçlendi, gözle bak |

## Global kurallar
- Önek `SL`; kendi asset'lerin `Content/SoulsLike/` altında; satın alınan klasörlerdeki asset'leri değiştirme (Görev 2'deki iskelet ayarı tek istisna).
- `Content/` git'e girmez; yedeği ayrıca alınır (yöntem kararı açık).
- Mesh C++'ta yazılmaz, BP'de seçilir. Kod hangi Dark Knight'ın oyuncu olduğunu bilmez.

## Review odağı
1. Menzilde hedef yokken lock-on'a basmak: durum değişmez. (Görev 5 testi)
2. Kilitli hedef silinirse: kilit açılır, serbest harekete dönülür. (Görev 5 PIE)
3. Kamera ile hedef aynı noktadaysa: açı sıçramaz, NaN olmaz. (Görev 5 testi)
4. Klavyede çapraz basınca daha hızlı koşulmaz. (Görev 3 testi)
5. Tam geri yürürken ±180 sınırında titreme olmaz. (Görev 3 testi, Görev 4 PIE)

## Dosya haritası

```text
Source/SoulsLike/
  Character/SLCharacterBase, SLPlayerCharacter, SLEnemyCharacter
  Core/SLPlayerController, SLGameMode
  Animation/SLAnimInstance
  Combat/SLLockOnTargetInterface, SLLockOnComponent, SLLockOnMath
  Character/SLLocomotionMath
  Tests/SLLocomotionMathTests.cpp, SLLockOnMathTests.cpp
```

Komutlar (PowerShell, proje kökünde; editör kapalıyken):

```powershell
& "C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" SoulsLikeEditor Win64 Development -Project="$PWD\SoulsLike.uproject" -WaitMutex
```

```powershell
& "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "$PWD\SoulsLike.uproject" -ExecCmds="Automation RunTests SoulsLike" -testexit="Automation Test Queue Empty" -unattended -nullrhi -nosplash -stdout
```

---

### Görev 1: Versiyon kontrolü ve derleme (M0)

**Dosyalar:** Oluştur `.gitignore`. Değiştir `SoulsLike.Build.cs`, `SoulsLike.uproject`, `Config/DefaultEngine.ini`.

- [ ] `SoulsLike.uproject`: `AndroidFileServer` pluginini `"Enabled": false` yap. `DefaultEngine.ini` içindeki `[/Script/AndroidFileServerEditor...]` bölümünü sil; içinde otomatik üretilmiş bir `SecurityToken` var, public repoya girmesin.
- [ ] `Build.cs`: `PublicIncludePaths.Add(ModuleDirectory);` ekle, kullanmadığın yorumları sil.
- [ ] `.gitignore`:

```text
/*
!/.gitignore
!/.gitattributes
!/Source/
!/Config/
!/docs/
!/SoulsLike.uproject
```

- [x] Repo GitHub Desktop ile açıldı, proje köküne taşındı (2026-11-25); `.gitignore` hazır.
- [ ] Rider'da derle. Beklenen: hatasız, editör açılıyor.
- [ ] `git init`, `git status` ile **yalnızca** Source/Config/docs/uproject/.gitignore listelendiğini gör, sonra `chore: project setup` commit'i.

### Görev 2: İskelet kapısı (M0, kod yok)

Bu görev geçmeden 3'e başlama; burada çıkan sorun her şeyi etkiler. Karar (2026-11-25): oyuncu kadın, boss erkek. Riskli taraf kadın iskelet olduğu için önce o test edilir.

- [ ] `SK_DKF_Full` (oyuncu) iskeletinde Retarget Manager > Manage Compatible Skeletons listesine `SKM_Manny_Skeleton` ekle.
- [ ] Translation Retargeting: root `Animation`, pelvis `Animation Scaled`, kalanlar `Skeleton`.
- [ ] `hand_r` altına `WeaponSocket_R`: konum (−1,16, 1,32, −0,52), dönüş roll −11,6 / pitch 15,8 / yaw −8,5. `SM_DKF_Sword` önizlemesi ekle; kabza avuca oturana kadar dönüşü düzelt.
- [ ] `AS_2Hand_Base_Attack_A_1`'i `Content/SoulsLike/Animations/Player/` altına kopyala, kopyada `EnableRootMotion` aç, montage üret.
- [ ] Geçici bir Character BP'de mesh yaw −90, montage'ı Play Anim Montage ile oynat.
- [ ] Aynı kontrolü `SK_DKM_Full` (boss) ile yap; ölçümlere göre sorunsuz geçmesi beklenir, kadınla karşılaştırma referansın da bu olur.

**Geçti sayılır:** karakter ~168 cm ilerliyor; sol el kabzada; ayak zemine batmıyor; omuz/kol bozulmuyor.
**Kadın iskelet testi geçemezse, sırayla:** (1) yalnızca sol el kayıyorsa AnimBP'de sol el IK (kabza ucuna socket); (2) omuz, kalça veya ayak bozuksa IK Retargeter (Manny -> DKF), sadece kullanılan klipler; (3) retarget sonrası kılıç yolu bozulursa Görev 3'e geçmeden Claude ile konuş. Retarget seçilirse oyuncu klipleri `Content/SoulsLike/Animations/Player/` altına üretilir, root motion orada açılır ve ~168 cm ölçütü bacak oranına göre biraz değişebilir.

### Görev 3: Hareket matematiği ve oyuncu karakteri (M1)

**Üretir:**
```cpp
// SLLocomotionMath.h
enum class ESLGait : uint8 { Walk, Run };
namespace SLLocomotion
{
    FVector2D ClampMoveInput(FVector2D Input);              // uzunluk <= 1
    float GetMaxSpeed(ESLGait Gait, bool bStrafing, float DirectionDegrees);
}
```
Yürüme 165 (her yön). Koşma strafe değilken 490; strafe'te |yön| 0->90 arası 490->320, 90->180 arası 320->290 doğrusal.

- [ ] Testi yaz (`SoulsLike.Locomotion`):
```cpp
TestEqual(TEXT("walk"),   SLLocomotion::GetMaxSpeed(ESLGait::Walk, true, 90.f), 165.f);
TestEqual(TEXT("free"),   SLLocomotion::GetMaxSpeed(ESLGait::Run, false, 180.f), 490.f);
TestEqual(TEXT("side"),   SLLocomotion::GetMaxSpeed(ESLGait::Run, true, -90.f), 320.f);
TestEqual(TEXT("mid"),    SLLocomotion::GetMaxSpeed(ESLGait::Run, true, 45.f), 405.f);
TestEqual(TEXT("back"),   SLLocomotion::GetMaxSpeed(ESLGait::Run, true, 180.f), 290.f);
TestEqual(TEXT("wrap"),   SLLocomotion::GetMaxSpeed(ESLGait::Run, true, -180.f), 290.f);
TestTrue (TEXT("diag"),   SLLocomotion::ClampMoveInput({1.0, 1.0}).Size() <= 1.0 + UE_KINDA_SMALL_NUMBER);
TestEqual(TEXT("analog"), SLLocomotion::ClampMoveInput({0.5, 0.0}).X, 0.5);
```
  `IMPLEMENT_SIMPLE_AUTOMATION_TEST`, bayraklar `EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter`, dosya `#if WITH_DEV_AUTOMATION_TESTS` içinde.
- [ ] Fonksiyonlar boş dönerken çalıştır: FAIL. Yaz, çalıştır: PASS.
- [ ] `ASLCharacterBase : ACharacter` (abstract): mesh konum Z −90, yaw −90; `UStaticMeshComponent* WeaponMesh`, `WeaponSocket_R`'a bağlı, çarpışmasız; protected `ESLGait Gait = ESLGait::Run`.
- [ ] `ASLPlayerCharacter`: `USpringArmComponent* CameraBoom` (350, `bUsePawnControlRotation`, lag kapalı), `UCameraComponent* FollowCamera`. `bUseControllerRotationYaw=false`; serbestte `bOrientRotationToMovement=true`, dönüş hızı 540; acceleration 1500, braking 1800.
- [ ] Input: `UInputAction` `MoveAction` (Axis2D), `LookAction` (Axis2D), `WalkAction` (bool, basılıyken Walk), `LockOnAction` (bool); `UInputMappingContext* DefaultMappingContext`'i `ASLPlayerController::BeginPlay`'de local player subsystem'e ekle. `SetupPlayerInputComponent`'te null action için `ensureMsgf`.
- [ ] Tick'te `MaxWalkSpeed = SLLocomotion::GetMaxSpeed(...)`; yön `UKismetAnimationLibrary::CalculateDirection(Velocity, ActorRotation)` (`AnimGraphRuntime` private bağımlılık).
- [ ] `ASLGameMode`; BP'ler `BP_SLPlayer`, `BP_SLGameMode`, `IMC_Default`, `IA_*`; test haritası `Content/SoulsLike/Maps/L_Gym`. `GameDefaultMap`, `EditorStartupMap` ve `GlobalDefaultGameMode` bunlara baksın.
- [ ] PIE: kamera dönüyor, klavye ve gamepad ile hareket var, çapraz hız 490'ı geçmiyor (`showdebug`). Commit `feat: player locomotion`.

### Görev 4: AnimInstance ve AnimBP (M1)

**Üretir:** `USLAnimInstance` alanları (BlueprintReadOnly): `float GroundSpeed`, `float Direction`, `bool bIsMoving`, `bool bIsStrafing`, `bool bIsFalling`.

- [ ] `NativeUpdateAnimation`'da karakterden velocity, rotation, falling, strafe **kopyala**; hesapları `NativeThreadSafeUpdateAnimation`'da yap. `bIsMoving` eşiği 3 cm/sn. Strafe bilgisi `ASLCharacterBase::IsStrafing() const` (virtual, varsayılan false).
- [ ] `BS_2H_Walk` ve `BS_2H_Run` değil, tek `BS_2H_Locomotion`: X yön −180..180, Y hız 0..490, `InPlace` klipler. Hız satırları: 0 idle (`Stand_Idle_A_1`), 165 `Walk_A_*`, 490 `Run_A_*`. Yön eksenine Wrap Input aç. Yan için `_L90_A` / `_R90_A`, ±180'e aynı geri klip.
- [ ] Koşma yan/geri klipleri 490'dan yavaş olduğu için onları Y=320 (yan) ve Y=290 (geri) noktalarına koy; 490 satırına da aynı klipleri koy ki blend space dışına taşmasın.
- [ ] `ABP_SLPlayer`: Idle/Move state machine -> `DefaultSlot` -> çıktı. Root Motion Mode: Root Motion from Montages Only.
- [ ] PIE (strafe'i geçici olarak açıp): ayak kayması gözle kabul edilebilir, tam geri yürürken titreme yok. Commit `feat: locomotion anim instance`.

### Görev 5: Lock-on (M1)

**Üretir:**
```cpp
namespace SLLockOn
{
    struct FCandidate { float Distance; float AngleDegrees; bool bVisible; };
    struct FParams { float MaxDistance = 1500.f; float MaxAngle = 45.f; };
    int32 PickBest(TConstArrayView<FCandidate> Candidates, const FParams& Params);   // yoksa INDEX_NONE
    FRotator ComputeRotation(const FVector& From, const FVector& To, const FRotator& Current); // pitch [-25, 10]; 3B mesafe < 1 cm ise Current
}
// ISLLockOnTargetInterface
virtual FVector GetLockOnLocation() const = 0;
virtual bool IsLockOnAllowed() const = 0;
// USLLockOnComponent
void ToggleLockOn();
bool IsLockedOn() const;
AActor* GetTarget() const;
FSLOnLockOnChanged OnLockOnChanged; // yeni hedef veya nullptr
```
Puan = 0,7·(1 − açı/MaxAngle) + 0,3·(1 − mesafe/MaxDistance).

- [ ] Testi yaz (`SoulsLike.LockOn`):
```cpp
TestEqual(TEXT("none"),    SLLockOn::PickBest({}, {}), INDEX_NONE);
TestEqual(TEXT("hidden"),  SLLockOn::PickBest(MakeArrayView(&Hidden, 1), {}), INDEX_NONE);   // {500, 0, false}
TestEqual(TEXT("far"),     SLLockOn::PickBest(MakeArrayView(&Far, 1), {}), INDEX_NONE);      // {1600, 0, true}
TestEqual(TEXT("wide"),    SLLockOn::PickBest(MakeArrayView(&Wide, 1), {}), INDEX_NONE);     // {500, 60, true}
TestEqual(TEXT("centre"),  SLLockOn::PickBest(Two, {}), 1);                                  // {300,40,true}, {900,5,true}
TestEqual(TEXT("same"),    SLLockOn::ComputeRotation(P, P, Cur), Cur);
TestEqual(TEXT("clamp"),   SLLockOn::ComputeRotation({0,0,0}, {10,0,1000}, Cur).Pitch, 10.0);
```
- [ ] FAIL gör, yaz, PASS gör.
- [ ] `ASLEnemyCharacter : ASLCharacterBase, ISLLockOnTargetInterface`: konum göğüs (actor + Z 40), `IsLockOnAllowed` true (M3'te ölüm eklenir).
- [ ] `USLLockOnComponent`: hedef `TWeakObjectPtr<AActor>`; adaylar Pawn overlap, interface filtresi, açı kameradan, görüş `ECC_Visibility` trace. Tick grubu `TG_PrePhysics`. Kilit, hedef geçersiz / izin yok / 2000 cm'den uzak / 1,5 sn görünmez ise açılır. Control rotation `RInterpTo`, hız 8.
- [ ] Oyuncu: kilitte `bOrientRotationToMovement=false`, `bUseControllerDesiredRotation=true`; look girdisi kilitte yok sayılır; `IsStrafing()` kilit durumunu döner.
- [ ] PIE, `L_Gym`'de iki düşman ve bir duvar: merkezdeki seçiliyor; duvar arkasında 1,5 sn sonra açılıyor; hedefi Outliner'dan silince serbest harekete dönüyor; menzilde kimse yokken basınca hiçbir şey olmuyor; hedefin etrafında koşarken kamera titremiyor. Commit `feat: lock-on`.

### Görev 6: M1 kapanışı
- [ ] Derleme ve tüm testler PASS.
- [ ] 30 sn kayıt: serbest koşu, lock-on, 8 yön.
- [ ] Claude'dan review iste; kritik bulguları düzelt.
- [ ] `git tag m1`, GitHub Desktop ile push.

**M1 bitti:** lock-on'da 8 yön yürüme/koşma var, kamera titremiyor, kılıç elde doğru duruyor.
