#include "FortPawn.h"

#include "reboot.h"
#include "FortPlayerControllerAthena.h"

AFortWeapon* AFortPawn::EquipWeaponDefinition(UFortWeaponItemDefinition* WeaponData, const FGuid& ItemEntryGuid)
{
	static auto EquipWeaponDefinitionFn = FindObject<UFunction>("/Script/FortniteGame.FortPawn.EquipWeaponDefinition");

	FGuid TrackerGuid{};

	struct { UObject* Def; FGuid Guid; AFortWeapon* Wep; } params{ WeaponData, ItemEntryGuid };
	struct { UObject* Def; FGuid Guid; FGuid TrackerGuid; AFortWeapon* Wep; } S16_params{ WeaponData, ItemEntryGuid, TrackerGuid };
	struct { UObject* Def; FGuid Guid; FGuid TrackerGuid; bool bDisableEquipAnimation; AFortWeapon* Wep; } S17_params{ WeaponData, ItemEntryGuid, TrackerGuid, false };

	if (Fortnite_Version < 16)
	{
		this->ProcessEvent(EquipWeaponDefinitionFn, &params);
		return params.Wep;
	}
	else if (std::floor(Fortnite_Version) == 16)
	{
		this->ProcessEvent(EquipWeaponDefinitionFn, &S16_params);
		return S16_params.Wep;
	}
	else
	{
		this->ProcessEvent(EquipWeaponDefinitionFn, &S17_params);
		return S17_params.Wep;
	}

	return nullptr;
}

bool AFortPawn::PickUpActor(AActor* PickupTarget, UFortDecoItemDefinition* PlacementDecoItemDefinition)
{
	static auto fn = FindObject<UFunction>("/Script/FortniteGame.FortPawn.PickUpActor");
	struct { AActor* PickupTarget; UFortDecoItemDefinition* PlacementDecoItemDefinition; bool ReturnValue; } AFortPawn_PickUpActor_Params{ PickupTarget, PlacementDecoItemDefinition };
	this->ProcessEvent(fn, &AFortPawn_PickUpActor_Params);

	return AFortPawn_PickUpActor_Params.ReturnValue;
}

void AFortPawn::SetHealth(float NewHealth)
{
	static auto SetHealthFn = FindObject<UFunction>("/Script/FortniteGame.FortPawn.SetHealth");

	if (SetHealthFn)
		this->ProcessEvent(SetHealthFn, &NewHealth);
}

void AFortPawn::SetShield(float NewShield)
{
	static auto SetShieldFn = FindObject<UFunction>("/Script/FortniteGame.FortPawn.SetShield");

	if (SetShieldFn)
		this->ProcessEvent(SetShieldFn, &NewShield);
}

void AFortPawn::NetMulticast_Athena_BatchedDamageCuesHook(UObject* Context, FFrame* Stack, void* Ret)
{
	auto Pawn = (AFortPawn*)Context;
	auto Controller = Cast<AFortPlayerController>(Pawn->GetController());
	auto CurrentWeapon = Pawn->GetCurrentWeapon();
	auto WorldInventory = Controller ? Controller->GetWorldInventory() : nullptr;

	if (!WorldInventory || !CurrentWeapon)
		return NetMulticast_Athena_BatchedDamageCuesOriginal(Context, Stack, Ret);

	static auto AmmoCountOffset = CurrentWeapon->GetOffset("AmmoCount");
	auto AmmoCount = CurrentWeapon->Get<int>(AmmoCountOffset);

	WorldInventory->CorrectLoadedAmmo(CurrentWeapon->GetItemEntryGuid(), AmmoCount);

	return NetMulticast_Athena_BatchedDamageCuesOriginal(Context, Stack, Ret);
}

UClass* AFortPawn::StaticClass()
{
	static auto Class = FindObject<UClass>("/Script/FortniteGame.FortPawn");
	return Class;
}