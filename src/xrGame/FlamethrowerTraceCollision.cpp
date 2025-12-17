#include "stdafx.h"
#include "FlamethrowerTraceCollision.h"
#include "CustomMonster.h"
#include "../xrphysics/iphworld.h"
#include "Flamethrower.h"
#include "../xrEngine/xr_collide_form.h"
#include "ai/stalker/ai_stalker.h"
#include "../Layers/xrRender/particle_core/psystem.h"
#include "debug_renderer.h"
#include "Level_Bullet_Manager.h"
#include "seniority_hierarchy_space.h"
#include "../xrEngine/gamemtllib.h"
#include "../Layers/xrRender/particle_core/particle_actions_collection.h"
#include "../xrCore/_vector3d_ext.h"

void FlamethrowerTrace::CPoint::UpdateAir(float delta_time)
{
	if(!IsCollided())
	{
		GravityVelocity += GravityAcceleration * delta_time;
		Fvector OldPos = PointPosition;
		PointPosition = (PointPosition + PointDirection * Velocity * delta_time) - Fvector{0.0f, GravityVelocity* delta_time, 0.0f};
		PointDirection = (PointPosition - OldPos).GetNormalizedCopy();
		collide::rq_results storage;
		collide::ray_defs RD(OldPos, PointPosition, CDB::OPT_FULL_TEST, collide::rqtBoth);
		TraceData data;
		data.TracedObj = this;
		if (Level().ObjectSpace.RayQuery(storage, RD, FlamethrowerTrace::CPoint::hit_callback, &data, FlamethrowerTrace::CPoint::test_callback, nullptr) && !data.Penetrate)
		{
			PointPosition = OldPos + PointDirection * data.HitDist;
			State = ETraceState::AirToGround;
			TimeOnCollide = CurrentTime;
			return;
		}
	}
	if(CurrentTime >= LifeTime)
	{
		TimeOnCollide = CurrentTime;
		State = ETraceState::End;
	}
}

void FlamethrowerTrace::CPoint::UpdateAirToGround(float delta_time)
{
	if((TimeOnCollide + RadiusCollidedInterpTime) - CurrentTime <= 0)
	{
		State = ETraceState::Ground;
	}
}

void FlamethrowerTrace::CPoint::UpdateGround(float delta_time)
{
	if(CurrentTime >= LifeTimeCollided)
	{
		TimeOnCollide = CurrentTime;
		State = ETraceState::End;
	}
}

void FlamethrowerTrace::CPoint::UpdateEnd(float delta_time)
{
	if( (TimeOnCollide+RadiusCollidedInterpTime)- CurrentTime <= 0)
	{
		State = ETraceState::Idle;
	}
}

BOOL FlamethrowerTrace::CPoint::hit_callback(collide::rq_result& result, LPVOID params)
{
	TraceData* pData = static_cast<TraceData*>(params);
	if (!result.O)
	{
		CDB::TRI const& triangle = *(Level().ObjectSpace.GetStaticTris() + result.element);
		SGameMtl* mtl = GMLib.GetMaterialByIdx(triangle.material);
		if (!fsimilar(mtl->fShootFactor, 0.0f)) // if not penetrate
		{
			pData->HitDist = result.range;
			return false;
		}
	}
	pData->Penetrate = true;
	return true;
}

BOOL FlamethrowerTrace::CPoint::test_callback(const collide::ray_defs& rd, CObject* object, LPVOID params)
{
	TraceData* pData = static_cast<TraceData*>(params);

	if (object)
	{
		CEntity* entity = smart_cast<CEntity*>(object);
		if (!entity)
		{
			return true;
		}
		if (entity->ID() == pData->TracedObj->Manager->GetParent()->H_Parent()->ID())
		{
			return false;
		}
	}
	return true;
}

FlamethrowerTrace::CPoint::CPoint(CManager* Manager)
{
	this->Manager = Manager;
	State = ETraceState::Idle;
	LifeTime = pSettings->r_float(Manager->GetSection(), "LifeTime");
	LifeTimeCollided = pSettings->r_float(Manager->GetSection(), "LifeTimeCollided");
	GravityAcceleration = pSettings->r_float(Manager->GetSection(), "GravityAcceleration");
	RadiusCollidedInterpTime = pSettings->r_float(Manager->GetSection(), "RadiusCollidedInterpTime");
	Velocity = pSettings->r_float(Manager->GetSection(), "Velocity");
}

void FlamethrowerTrace::CPoint::Activate()
{
	State = ETraceState::Air;
	LastUpdatedPos = PointPosition;
}

void FlamethrowerTrace::CPoint::Update(float DeltaTime)
{
	VERIFY(State != ETraceState::MAX);
	switch (State)
	{
	case ETraceState::Idle:
		{
			return;
		}
	case ETraceState::Air:
		{
		UpdateAir(DeltaTime);
			break;
		}
	case ETraceState::AirToGround:
		{
		UpdateAirToGround(DeltaTime);
		break;
		}
	case ETraceState::Ground:
		{
		UpdateGround(DeltaTime);
			break;
		}
	case ETraceState::End:
		{
		UpdateEnd(DeltaTime);
			break;
		}
	}
	LastUpdateTime = CurrentTime;
	CurrentTime += DeltaTime;
}

bool FlamethrowerTrace::CPoint::VerifySpawnPos(const Fvector& Position, const Fvector& Direction, Fvector& HitPos)
{
	collide::rq_results storage;
	collide::ray_defs RD(Position-Direction, Position, CDB::OPT_FULL_TEST, collide::rqtBoth);
	TraceData data;
	data.TracedObj = this;
	if (Level().ObjectSpace.RayQuery(storage, RD, FlamethrowerTrace::CPoint::hit_callback, &data, FlamethrowerTrace::CPoint::test_callback, nullptr))
	{
		HitPos = Position+Direction * (data.HitDist - 1.0f);
		return false;
	}
	return true;
}

void FlamethrowerTrace::CPoint::Deactivate()
{
	CurrentTime = 0.0f;
	GravityVelocity = 0.0f;
	TimeOnCollide = 0.0f;
	LastUpdateTime = 0.0f;
	PointPosition = {};
	LastUpdatedPos = {};
	PointDirection = {};
}

void FlamethrowerTrace::CCollision::Update_Air(float DeltaTime)
{
	float interpTime = std::min(m_current_time / m_RadiusMaxTime, 1.0f);
	RadiusCurrent = m_RadiusMin + (m_RadiusMax - m_RadiusMin) * interpTime;
	clamp(RadiusCurrent, m_RadiusMin, m_RadiusMax);

	if (!IsCollided()&&AttachPoint->IsCollided())
	{
		m_State = ETraceState::AirToGround;
		VERIFY2(m_RadiusCollided > 0.01, "Too small RadiusCollided in flamethrower config!");
		RadiusOnCollide = RadiusCurrent;
		m_time_on_collide = m_current_time;
		return;
	}
	if(m_current_time >= m_LifeTime)
	{
		m_State = ETraceState::End;
	}
}

void FlamethrowerTrace::CCollision::Update_AirToGround(float DeltaTime)
{
	float interpTime = (m_current_time - m_time_on_collide) / m_RadiusCollidedInterpTime;
	if(interpTime >= 1.0f)
	{
		m_State = ETraceState::Ground;
		interpTime = 1.0f;
	}
	const float AlphaValue = 1.0f -std::pow(1.0f - interpTime, 2.0f);
	RadiusCurrent = std::max(RadiusOnCollide, AlphaValue * m_RadiusCollided);
}

void FlamethrowerTrace::CCollision::Update_Ground(float DeltaTime)
{
	if (m_current_time >= m_LifeTimeCollidedMax)
	{
		m_current_time = 0;
		m_State = ETraceState::End;
	}
}

void FlamethrowerTrace::CCollision::Update_End(float DeltaTime)
{
	const float interpTime = 1.0f - (m_current_time / m_RadiusCollidedInterpTime);
	if(interpTime <= 0)
	{
		Deactivate();
		return;
	}
}

FlamethrowerTrace::CCollision::CCollision(FlamethrowerTrace::CManager* Manager) : Manager(Manager)
{
	m_RadiusMin = pSettings->r_float(Manager->GetSection(), "RadiusMin");
	m_RadiusMax = pSettings->r_float(Manager->GetSection(), "RadiusMax");
	m_RadiusCollided = pSettings->r_float(Manager->GetSection(), "RadiusCollided");
	m_RadiusCollidedInterpTime = pSettings->r_float(Manager->GetSection(), "RadiusCollidedInterpTime");
	m_RadiusCollisionCoeff = pSettings->r_fvector3(Manager->GetSection(), "RadiusCollisionCoeff");
	m_RadiusCollisionCollidedCoeff = pSettings->r_fvector3(Manager->GetSection(), "RadiusCollisionCollidedCoeff");
	m_RadiusMaxTime = pSettings->r_float(Manager->GetSection(), "RadiusMaxTime");
	m_LifeTime = pSettings->r_float(Manager->GetSection(), "LifeTime");
	m_LifeTimeCollidedMax = pSettings->r_float(Manager->GetSection(), "LifeTimeCollided");
	m_FlameFadeTime = pSettings->r_float(Manager->GetSection(), "FlameFadeTime");
	CollidedParticlePivot = pSettings->r_fvector3(Manager->GetSection(), "CollidedParticlePivot");

	// flames
	m_sFlameParticles = pSettings->r_string(Manager->GetSection(), "flame_particles");
	m_sFlameParticlesGround = pSettings->r_string(Manager->GetSection(), "earth_flame_particles");
	m_particle_size_air_PE_name = pSettings->r_string(Manager->GetSection(), "air_flame_size_bind");
	m_particle_alpha_air_PE_name = pSettings->r_string(Manager->GetSection(), "air_flame_alpha_bind");
	m_particle_size_ground_PE_name = pSettings->r_string(Manager->GetSection(), "earth_flame_size_bind");
	m_particle_alpha_ground_PE_name = pSettings->r_string(Manager->GetSection(), "earth_flame_alpha_bind");
}

FlamethrowerTrace::CCollision::~CCollision()
{
	if (IsActive()) {
		Deactivate();
	}
}

void FlamethrowerTrace::CCollision::Load(LPCSTR section)
{
	m_RadiusMin = pSettings->r_float(section, "RadiusMin");
	m_RadiusMax = pSettings->r_float(section, "RadiusMax");
	m_RadiusCollided = pSettings->r_float(section, "RadiusCollided");
	m_RadiusCollidedInterpTime = pSettings->r_float(section, "RadiusCollidedInterpTime");
	m_RadiusCollisionCoeff = pSettings->r_fvector3(section, "RadiusCollisionCoeff");
	CollidedParticlePivot = pSettings->r_fvector3(section, "CollidedParticlePivot");
	m_RadiusCollisionCollidedCoeff = pSettings->r_fvector3(section, "RadiusCollisionCollidedCoeff");
	m_RadiusMaxTime = pSettings->r_float(section, "RadiusMaxTime");
	m_LifeTime = pSettings->r_float(section, "LifeTime");
	m_LifeTimeCollidedMax = pSettings->r_float(section, "LifeTimeCollided");
	m_FlameFadeTime = pSettings->r_float(section, "FlameFadeTime");

	// flames
	m_sFlameParticles = pSettings->r_string(section, "flame_particles");
	m_sFlameParticlesGround = pSettings->r_string(section, "earth_flame_particles");
	m_particle_size_air_PE_name = pSettings->r_string(section, "air_flame_size_bind");
	m_particle_alpha_air_PE_name = pSettings->r_string(section, "air_flame_alpha_bind");
	m_particle_size_ground_PE_name = pSettings->r_string(section, "earth_flame_size_bind");
	m_particle_alpha_ground_PE_name = pSettings->r_string(section, "earth_flame_alpha_bind");
}

void FlamethrowerTrace::CCollision::AttachToPoint(CPoint* point)
{
	AttachPoint = point;
}

inline CFlamethrower* FlamethrowerTrace::CCollision::GetParentWeapon() const
{
	return Manager->GetParent();
}

bool FlamethrowerTrace::CCollision::IsReadyToUpdateCollisions()
{
	//return true;
	if(m_State == ETraceState::Idle)
	{
		m_State = ETraceState::Air;
		return false;
	}
	float Dist = GetCurrentRadius()*0.8;
	if(IsCollided())
	{
		if(m_last_update_time > 0.2)
		{
			m_last_update_time = 0.0f;
			return true;
		}
		return false;
	}
	return true;
}

float FlamethrowerTrace::CCollision::GetCurrentRadius()
{
	return RadiusCurrent;
}

void FlamethrowerTrace::CCollision::SetCurrentLifeTime(const float Time)
{
	m_current_time = Time;
	float interpTime = std::min(m_current_time / m_RadiusMaxTime, 1.0f);
	RadiusCurrent = m_RadiusMin + (m_RadiusMax - m_RadiusMin) * interpTime;
}

void FlamethrowerTrace::CCollision::feel_touch_new(CObject* O)
{
	if (m_State == ETraceState::Idle) {
		return;
	}
	if (CCustomMonster* Casted = smart_cast<CCustomMonster*>(O)) {
		Manager->RegisterOverlapped(Casted);
	}
}

void FlamethrowerTrace::CCollision::feel_touch_delete(CObject* O)
{
	if (CCustomMonster* Casted = smart_cast<CCustomMonster*>(O)) {
		Manager->UnregisterOverlapped(smart_cast<CCustomMonster*>(O));
	}
}

BOOL FlamethrowerTrace::CCollision::feel_touch_contact(CObject* O)
{
	return false;
}

void FlamethrowerTrace::CCollision::Activate()
{
	m_State = ETraceState::Air;
	RadiusCurrent = m_RadiusMin;
}

void FlamethrowerTrace::CCollision::Deactivate()
{
	m_State = ETraceState::Idle;
	m_current_time = 0.0f;
	m_time_on_collide = 0.0f;
	RadiusOnCollide = 0.0f;
}

void FlamethrowerTrace::CCollision::Update(float DeltaTime)
{
	switch (m_State)
	{
	case ETraceState::Idle:
		{
			return;
		}
	case ETraceState::Air:
		{
		Update_Air(DeltaTime);
			break;
		}
	case ETraceState::AirToGround:
		{
		Update_AirToGround(DeltaTime);
			break;
		}
	case ETraceState::Ground:{
			Update_Ground(DeltaTime);
			break;
		}
	case ETraceState::End:
		{
		Update_End(DeltaTime);
			break;
		}
	}
	m_current_time += DeltaTime;
	m_last_update_time += DeltaTime;

}

Fvector FlamethrowerTrace::CCollision::GetPosition()
{
	return AttachPoint->GetPosition();
}

FlamethrowerTrace::CManager::CManager(CFlamethrower* flamethrower) : m_flamethrower(flamethrower)
{

#ifdef DEBUG
	Level().BulletManager().MarkFlamethrowerTraceToDraw(this);
#endif
}

FlamethrowerTrace::CManager::~CManager()
{
#ifdef DEBUG
	Level().BulletManager().UnmarkFlamethrowerTraceToDraw(this);
#endif
	for (auto& elem : InactiveCollisions) {
		xr_delete(elem);
	}
	for (auto& elem : ActiveCollisions) {
		xr_delete(elem);
	}
	for (auto& elem : InactivePoints) {
		xr_delete(elem);
	}
	for (auto& elem : ActivePoints) {
		xr_delete(elem);
	}
}

void FlamethrowerTrace::CManager::feel_touch_new(CObject* O)
{
	Touch::feel_touch_new(O);
	Overlapped.push_back(smart_cast<CCustomMonster*>(O));
}

void FlamethrowerTrace::CManager::feel_touch_delete(CObject* O)
{
	Touch::feel_touch_delete(O);
	Overlapped.erase(std::ranges::find(Overlapped, smart_cast<CCustomMonster*>(O)));
}

BOOL FlamethrowerTrace::CManager::feel_touch_contact(CObject* O)
{
	if (CCustomMonster* enemy = smart_cast<CCustomMonster*>(O)) {
		VERIFY(ActiveCollisions.size() == ActivePoints.size());
		for (auto& elem : ActiveCollisions) {
			if (!elem->IsActive() || !elem->IsReadyToUpdateCollisions()) {
				continue;
			}
			float Radius = elem->GetCurrentRadius();
			if (elem->GetPosition().distance_to_sqr(enemy->Position()) < Radius*Radius)
			{
				return true;
			}
		}
	}
	return false;
}

void FlamethrowerTrace::CManager::Load(LPCSTR section)
{
	for (auto& elem : ActiveCollisions) {
		xr_delete(elem);
	}
	for (auto& elem : InactiveCollisions) {
		xr_delete(elem);
	}
	CollisionSection = section;
	ActiveCollisions.clear();
	InactiveCollisions.clear();
	int StartNum = pSettings->r_u16(section, "trace_collision_num_start");
	for (int i = 0; i < StartNum; ++i) {
		auto NewCollision = new FlamethrowerTrace::CCollision(this);
		NewCollision->Load(section);
		InactiveCollisions.push_back(NewCollision);
	}
	m_RadiusMax = pSettings->r_float(section, "RadiusMax");
}

void FlamethrowerTrace::CManager::save(NET_Packet& output_packet)
{
}

void FlamethrowerTrace::CManager::load(IReader& input_packet)
{
}

void FlamethrowerTrace::CManager::Update(float DeltaTime)
{
}

void FlamethrowerTrace::CManager::UpdateOverlaps(float DeltaTime)
{
	Fvector Center{};
	float Radius = 0.0f;
	uint16_t Num = 0;
	if(!ActiveCollisions.empty()){
		auto FirstElem = ActiveCollisions.front();
		ActiveCollisions.pop_front();
		ActiveCollisions.push_back(FirstElem);
		FlamethrowerTrace::CCollision* CurrentElem;
		do
		{
			CurrentElem = ActiveCollisions.front();

			CurrentElem->Update(DeltaTime);
			if (!CurrentElem->IsActive())
			{
				ActiveCollisions.pop_front();
				InactiveCollisions.push_back(CurrentElem);
			}
			else {
				Center += CurrentElem->GetPosition();
				++Num;

				ActiveCollisions.pop_front();
				ActiveCollisions.push_back(CurrentElem);
			}
		} while (CurrentElem != FirstElem);
	}
	if(!Num)
	{
		return;
	}
	Center /= Num;
	for (auto& elem : ActiveCollisions) {
		float Dist = Center.distance_to_sqr(elem->GetPosition());
		if(Dist > Radius)
		{
			Radius = Dist;
		}
	}
	feel_touch_update(Center, sqrt(Radius));
}

void FlamethrowerTrace::CManager::UpdatePoints(float DeltaTime)
{
	if(ActivePoints.empty())
	{
		return;
	}
	auto FirstElem = ActivePoints.front();
	ActivePoints.pop_front();
	ActivePoints.push_back(FirstElem);
	auto CurrentElem = FirstElem;
	do
	{
		CurrentElem = ActivePoints.front();
		ActivePoints.pop_front();
		CurrentElem->Update(DeltaTime);
		if(CurrentElem->IsActive())
		{
			ActivePoints.push_back(CurrentElem);
		}
		else
		{
			CurrentElem->Deactivate();
			InactivePoints.push_back(CurrentElem);
		}
	} while (CurrentElem != FirstElem);
}

void FlamethrowerTrace::CManager::RegisterOverlapped(CCustomMonster* enemy)
{
	Overlapped.push_back(enemy);
}

void FlamethrowerTrace::CManager::UnregisterOverlapped(CCustomMonster* enemy)
{
#ifndef TEMPORARLY_REMOVE_FLAMETHROWER_LOGIC
	Overlapped.erase(std::ranges::find(Overlapped, enemy));
#endif
}

const FlamethrowerTrace::CManager::FOverlappedObjects& FlamethrowerTrace::CManager::GetOverlapped()
{
	return Overlapped;
}

void FlamethrowerTrace::CManager::OnShootingEnd()
{
	LastLaunched = nullptr;
}

FlamethrowerTrace::CCollision* FlamethrowerTrace::CManager::LaunchTrace(const Fvector& StartPos, const Fvector& StartDir, bool Force)
{
	// Insert debug trace abort here if needed
	if(LastLaunched && LastLaunched->IsActive() && !Force)
	{
		auto dist = StartPos.distance_to_sqr(LastLaunched->GetPosition());
		auto RadiusSq = m_RadiusMax* m_RadiusMax;
		Msg("dist [%f], radius sq [%f]", dist, RadiusSq);
		if(dist <= RadiusSq)
		{
			return nullptr;
		}
	}
	auto FirstPoint = LaunchPoint();
	auto FirstCollision = ActiveCollisions.front();
	LastLaunched = FirstPoint;
	LastLaunched->SetTransform(StartPos, StartDir);
	FirstPoint->Activate();
	return FirstCollision;
}

FlamethrowerTrace::CPoint* FlamethrowerTrace::CManager::LaunchPoint()
{
	CPoint* FirstPoint;
	if(InactivePoints.empty())
	{
		FirstPoint = new CPoint(this);
	} else
	{
		FirstPoint = InactivePoints.front();
		InactivePoints.pop_front();
	}
	LaunchCollision(FirstPoint);
	ActivePoints.push_back(FirstPoint);
	return FirstPoint;
}

FlamethrowerTrace::CCollision* FlamethrowerTrace::CManager::LaunchCollision(CPoint* RootPoint)
{
	CCollision* FirstCollision;
	if (InactiveCollisions.empty())
	{
		FirstCollision = new CCollision(this);
	}
	else
	{
		FirstCollision = InactiveCollisions.front();
		InactiveCollisions.pop_front();
	}
	FirstCollision->AttachToPoint(RootPoint);
	FirstCollision->Activate();
	FirstCollision->SetTraceState(ETraceState::Air);
	ActiveCollisions.push_back(FirstCollision);
	return FirstCollision;
}

ISaveObject& FlamethrowerTrace::operator<<(ISaveObject& Object, CManager& Data)
{
	// TODO: insert return statement here
	return Object;
}
