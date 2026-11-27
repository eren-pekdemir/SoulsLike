# Saldırı Kataloğu (iki el, ölçülmüş)

Kaynak: `RPG_Animations_Pack/Anims/TwoHand_Base` ve `TwoHand_Up`, erkek Dark Knight iskeletinde arayüzsüz editörle ölçüldü (2026-11-26).

- **İleri:** root motion ile gidilen mesafe, cm.
- **Vuruş:** kılıç ucunun en hızlı olduğu an, sn. Hasar penceresi bunun etrafında açılır; kesin değeri montajda gözle ayarla.
- **Geçiş:** bir saldırıdan sonrakinin `_Combo` klibine geçilecek an, sn. Bu anda iki poz **birebir aynı** (fark 0,0°). Paketin kendi birleşik kliplerinden (`Attack_A_Combo_124` gibi) çıkarıldı.

## `_Combo` klipleri ne?
Tüm saldırılar idle pozunda başlayıp idle pozunda biter. `_Combo` sürümü aynı saldırının **hazırlık kısmı kesilmiş** hâli: önceki saldırının ortasındaki pozdan başlar. Kombo şöyle kurulur: ilk saldırı geçiş anına gelince, sıradakinin `_Combo` klibine geçilir.

## Base duruş (alçak)

### Tek saldırılar
| Klip | Süre | İleri | Vuruş | Not |
|---|---|---|---|---|
| Attack_A_1 | 1.83 | 169 | 0.53 | Kombo açılışı |
| Attack_A_2 | 1.67 | 105 | 0.54 | |
| Attack_A_3 | 1.83 | 127 | 0.60 | |
| Attack_A_4 | 2.33 | 94 | 0.98 | Yavaş, ağır |
| Attack_A_5 | 2.00 | 193 | 0.43 | En hızlı kılıç ucu |
| Attack_A_6 | 2.00 | 202 | 0.43 | |
| Attack_B_1 | 1.83 | 159 | 0.50 | |
| Attack_B_2 | 2.00 | 224 | 0.43 | |
| Attack_B_3 | 2.00 | 200 | 0.57 | |
| Attack_B_4 | 1.83 | 145 | 0.47 | |
| Attack_B_5 | 2.00 | 272 | 0.47 | |
| Attack_C_1 | 1.63 | 0 | 0.54 | Yerinde |
| Attack_C_3 | 1.47 | 0 | 0.40 | Yerinde, hızlı |
| Attack_C_2 / C_4 | 1.17 | −41 / −58 | erken | **Geri adımlı** |
| Attack_Dash_1 | 2.33 | 430 | 0.81 | Koşu saldırısı |
| Jump_Attack_1/2 (+Hold, Air_Loop, End) | | | | Zıplama / dalış saldırısı |
| Skill_1 | 2.83 | 668 | 0.57, 1.15 | Hamleli iki vuruş |
| Skill_2 | 2.50 | 384 | 1.30 | Yavaş tek ağır vuruş |
| Skill_3 | 2.00 | 1082 | 0.30, 0.57 | Uzun atılma |
| Skill_4 | 3.33 | 650 | 1.21, 1.62 | |
| Skill_5 | 2.50 | 113 | 0.77 | |
| Skill_6 | 3.50 | 113 | 1.43 | En yavaş, şarjlı vuruş gibi |
| Skill_7 | 2.33 | 104 | 0.78 | |
| Skill_8 | 2.67 | 437 | 0.33, 0.80 | |
| Fatal_Attack_1..5 | 3.2–5.2 | ~0 | | İnfaz, karşılığı `Fatal_Hit_N` |
| W_Parry_A/B_1/2 | 0.50 | 0 | | Silahla parry |

### Kombo zincirleri (paketin tasarladığı)
| Zincir | Geçiş anları (önceki klipte) |
|---|---|
| **A: 1 → 2 → 4** | A_1 0.70 → A_2_Combo; A_2 0.77 → A_4_Combo |
| **A: 1 → 6 → 5 → 3** | A_1 0.60 → A_6_Combo; A_6 0.63 → A_5_Combo; A_5 0.50 → A_3_Combo |
| **B: 1 → 2 → 3 → 4 / 5** | B_1 0.53; B_2 0.50; B_3 0.77 (→4) veya 0.70 (→5) |
| **C: 1 → 2 → 1 → 2**, **3 → 4 → 3 → 4** | `C_2_To_1`, `C_4_To_3` geçiş klipleri hazır |

Geçiş anları **düz** klibin zamanı. Önceki saldırı zaten bir `_Combo` klibiyse, `_Combo` klibi baştan kısaldığı için şu farkı çıkar: A_2 0.24, A_3 0.26, A_4 0.63, A_5 0.27, A_6 0.07 sn.

Tasarlanmamış geçişler (ör. A_1 → A_3, A_4 → A_5) pozda 25–30° fark bırakır; 0,2 sn blend ile kabul edilebilir.

## Up duruş (yüksek)
Vuruşlar Base'den biraz geç (0,6–0,8 sn), hamleler daha uzun: daha ağır bir duruş.

| Zincir | Geçiş anları |
|---|---|
| **A: 1 → 2 → 3 → 4 / 5** | A_1 0.67; A_2 0.87; A_3 0.77 |
| **B: 1 → 2 → 3** | B_1 1.10; B_2 0.93 |
| **B: 1 → 4 → 5** | B_1 0.97; B_4 1.00 |

| Klip | Not |
|---|---|
| Skill_5_Start / Loop / End | **Basılı tutma (şarj) saldırısı**: başla, tuş basılıyken döngü, bırakınca bitir |
| Attack_Dash_1 | Koşu saldırısı |
| Jump_Attack (+Hold, Air_Loop, End) | Dalış |
| Fatal_Attack_1..5 | İnfaz |

## Duruş değiştirme
`Weapon_Change_To_Up_1/2` (Base'den Up'a) ve `Weapon_Change_To_Base_1/2` (Up'tan Base'e) geçiş animasyonları var. İki duruşun yürüme, koşu, roll, blok ve saldırı setleri ayrı ayrı mevcut.
