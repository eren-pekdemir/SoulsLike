// Projedeki bütün gameplay tag'leri tek yerde.
// Tag'i metin yerine bu sabitlerle kullan: yanlış yazılırsa derleme hatası verir.

#pragma once

#include "NativeGameplayTags.h"

namespace SLGameplayTags
{
	// Durumlar
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Attacking);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dodging);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Invulnerable);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Window_Cancel);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dead);

	// Yetenek türleri
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Dodge);

	// Olaylar
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Input_LightAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Input_HeavyAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Combo_Handoff);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_HitReact);

	// Veri: hasar miktarını effect'e taşımak için (SetByCaller)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Damage);
}
