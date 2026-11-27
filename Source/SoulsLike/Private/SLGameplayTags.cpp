#include "SLGameplayTags.h"

namespace SLGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(State_Attacking, "State.Attacking");
	UE_DEFINE_GAMEPLAY_TAG(State_Dodging, "State.Dodging");
	UE_DEFINE_GAMEPLAY_TAG(State_Invulnerable, "State.Invulnerable");
	UE_DEFINE_GAMEPLAY_TAG(State_Window_Cancel, "State.Window.Cancel");
	UE_DEFINE_GAMEPLAY_TAG(State_Dead, "State.Dead");

	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack, "Ability.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Dodge, "Ability.Dodge");

	UE_DEFINE_GAMEPLAY_TAG(Event_Input_LightAttack, "Event.Input.LightAttack");
	UE_DEFINE_GAMEPLAY_TAG(Event_Input_HeavyAttack, "Event.Input.HeavyAttack");
	UE_DEFINE_GAMEPLAY_TAG(Event_Combo_Handoff, "Event.Combo.Handoff");
	UE_DEFINE_GAMEPLAY_TAG(Event_HitReact, "Event.HitReact");

	UE_DEFINE_GAMEPLAY_TAG(Data_Damage, "Data.Damage");
	
	UE_DEFINE_GAMEPLAY_TAG(State_HitReact, "State.HitReact");
	
}
