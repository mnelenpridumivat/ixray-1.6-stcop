#pragma once

namespace GameObject {
	enum ECallbackType {
		eTradeStart = u32(0),
		eTradeStop,
		eTradeSellBuyItem,
		eTradePerformTradeOperation,

		eZoneEnter,
		eZoneExit,
		eExitLevelBorder,
		eEnterLevelBorder,
		eDeath,

		ePatrolPathInPoint,

		eInventoryPda,
		eInventoryInfo,
		eArticleInfo,
		eTaskStateChange,
		eMapLocationAdded,

		eUseObject,

		eHit,

		eSound,

		eActionTypeMovement,
		eActionTypeWatch,
		eActionTypeRemoved,
		eActionTypeAnimation,
		eActionTypeSound,
		eActionTypeParticle,
		eActionTypeObject,

		eActorSleep,

		eHelicopterOnPoint,
		eHelicopterOnHit,
		eHelicopterOnSamHit,

		eOnItemTake,
		eOnItemDrop,

		eScriptAnimation,
		
		eTraderGlobalAnimationRequest,
		eTraderHeadAnimationRequest,
		eTraderSoundEnd,

		eInvBoxItemTake,
		eWeaponNoAmmoAvailable,

		eKeyPress,
		eKeyRelease,
		eKeyHold,
		eMouseMove,
		eMouseWheel,

		eShieldOn,
		eShieldOff,
		eJump,

		eOnWeaponFired,
		eOnWeaponJammed,
		eOnWeaponZoomIn,
		eOnWeaponZoomOut,
		eOnWeaponMagazineEmpty,

		eItemToBelt,
		eItemToSlot,
		eItemToRuck,
		eOnFootStep,

		//Dance Maniac
		eOnActorJump,

		eAttachVehicle,
		eDetachVehicle,
		eUseVehicle,

		eActorBeforeDeath,
		eActorHudAnimationEnd,

		eDangerousMaterialTouch,

		eDummy = u32(-1),
	};
};

