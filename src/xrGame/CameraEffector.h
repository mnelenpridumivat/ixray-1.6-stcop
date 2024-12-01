#pragma once

//#include "../xrEngine/CameraManager.h"
#include "../xrEngine/Effector.h"
#include "../xrEngine/EffectorPP.h"

constexpr auto eStartEffectorID = 50;

constexpr auto effHit = (eStartEffectorID + 1);
constexpr auto effAlcohol = (eStartEffectorID + 2);
constexpr auto effFireHit = (eStartEffectorID + 3);
constexpr auto effExplodeHit = (eStartEffectorID + 4);
constexpr auto effNightvision = (eStartEffectorID + 5);
constexpr auto effPsyHealth = (eStartEffectorID + 6);
constexpr auto effControllerAura = (eStartEffectorID + 7);
constexpr auto effControllerAura2 = (eStartEffectorID + 8);
constexpr auto effBigMonsterHit = (eStartEffectorID + 9);
constexpr auto effActorDeath = (eStartEffectorID + 10);
constexpr auto effUseItem = (eStartEffectorID + 11);

constexpr auto effPoltergeistTeleDetectStartEffect = 2048;
// warning: ~50 constants after effPoltergeistTeleDetectStartEffect are reserved for poltergeists

constexpr auto effCustomEffectorStartID = 10000;
// warning: constants after effCustomEffectorStartID are reserved

constexpr auto eCEFall = ((ECamEffectorType)(cefNext + 1));
constexpr auto eCENoise = ((ECamEffectorType)(cefNext + 2));
constexpr auto eCEShot = ((ECamEffectorType)(cefNext + 3));
constexpr auto eCEZoom = ((ECamEffectorType)(cefNext + 4));
constexpr auto eCERecoil = ((ECamEffectorType)(cefNext + 5));
constexpr auto eCEBobbing = ((ECamEffectorType)(cefNext + 6));
constexpr auto eCEHit = ((ECamEffectorType)(cefNext + 7));
constexpr auto eCEUser = ((ECamEffectorType)(cefNext + 11));
constexpr auto eCEControllerPsyHit = ((ECamEffectorType)(cefNext + 12));
constexpr auto eCEVampire = ((ECamEffectorType)(cefNext + 13));
constexpr auto eCEPseudoGigantStep = ((ECamEffectorType)(cefNext + 14));
constexpr auto eCEMonsterHit = ((ECamEffectorType)(cefNext + 15));
constexpr auto eCEDOF = ((ECamEffectorType)(cefNext + 16));
constexpr auto eCEWeaponAction = ((ECamEffectorType)(cefNext + 17));
constexpr auto eCEActorMoving = ((ECamEffectorType)(cefNext + 18));
