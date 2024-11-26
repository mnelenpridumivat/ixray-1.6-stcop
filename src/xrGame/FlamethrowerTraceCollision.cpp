#include "stdafx.h"
#include "FlamethrowerTraceCollision.h"
#include "CustomMonster.h"
#include "../xrphysics/iphworld.h"
#include "Flamethrower.h"
#include "../xrEngine/xr_collide_form.h"
#include "ai/stalker/ai_stalker.h"
#include "../xrParticles/psystem.h"
#include "debug_renderer.h"
#include "Level_Bullet_Manager.h"
#include "seniority_hierarchy_space.h"
#include "../xrParticles/psystem.h"
#include "../xrEngine/gamemtllib.h"

void FlamethrowerTrace::CPoint::UpdateAir(float delta_time)
{
	if(!IsCollided())
	{
		GravityVelocity += GravityAcceleration * delta_time;
		Fvector OldPos = PointPosition;
		PointPosition = (PointPosition + PointDirection * Velocity * delta_time) - Fvector{0.0f, GravityVelocity* delta_time, 0.0f};
#ifndef TEMPORARLY_REMOVE_FLAMETHROWER_LOGIC
		PointDirection = (PointPosition - OldPos).GetNormalizedCopy();
#endif
		collide::rq_results storage;
#ifndef TEMPORARLY_REMOVE_FLAMETHROWER_LOGIC
		collide::ray_defs RD(OldPos, PointPosition, CDB::OPT_FULL_TEST, collide::rqtBoth);
#endif
		TraceData data;
		data.TracedObj = this;
#ifndef TEMPORARLY_REMOVE_FLAMETHROWER_LOGIC
		if (Level().ObjectSpace.RayQuery(storage, RD, FlamethrowerTrace::CPoint::hit_callback, &data, FlamethrowerTrace::CPoint::test_callback, nullptr) && !data.Penetrate)
#endif
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
#ifndef TEMPORARLY_REMOVE_FLAMETHROWER_LOGIC
	collide::ray_defs RD(PointPosition-PointDirection, PointPosition, CDB::OPT_FULL_TEST, collide::rqtBoth);
#endif
	TraceData data;
	data.TracedObj = this;
#ifndef TEMPORARLY_REMOVE_FLAMETHROWER_LOGIC
	if (Level().ObjectSpace.RayQuery(storage, RD, FlamethrowerTrace::CPoint::hit_callback, &data, FlamethrowerTrace::CPoint::test_callback, nullptr))
#endif
	{
		HitPos = PointPosition + PointDirection * (data.HitDist - 1.0f);
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

	Fmatrix particles_pos = Fmatrix();
	particles_pos.c.set(AttachPoint->GetPosition());


	VERIFY(_valid(particles_pos));

	m_particles->SetXFORM(particles_pos);

	float interpTime = std::min(m_current_time / m_RadiusMaxTime, 1.0f);
	RadiusCurrent = m_RadiusMin + (m_RadiusMax - m_RadiusMin) * interpTime;
	clamp(RadiusCurrent, m_RadiusMin, m_RadiusMax);

	PAPI::pVector Size;
	Size.x = GetCurrentRadius() * m_RadiusCollisionCoeff.x;
	Size.y = GetCurrentRadius() * m_RadiusCollisionCoeff.y;
	Size.z = GetCurrentRadius() * m_RadiusCollisionCoeff.z;
#ifndef TEMPORARLY_REMOVE_FLAMETHROWER_LOGIC
	m_particle_size_handle.Set(Size);
#endif

	if (!IsCollided()&&AttachPoint->IsCollided())
	{
		m_State = ETraceState::AirToGround;
		VERIFY2(m_RadiusCollided > 0.01, "Too small RadiusCollided in flamethrower config!");
		RadiusOnCollide = RadiusCurrent;
		m_time_on_collide = m_current_time;
		m_particles->Stop(false);
		CParticlesObject::Destroy(m_particles);
		m_particles_ground = CParticlesObject::Create(*m_sFlameParticlesGround, false);
		m_particles_ground->Play(false);
#ifndef TEMPORARLY_REMOVE_FLAMETHROWER_LOGIC
		m_particle_alpha_handle = m_particles_ground->GetFloatHandle("AlphaHandle");
		m_particle_size_handle = m_particles_ground->GetVectorHandle("SizeHandle");
#endif
		particles_pos.c.set(AttachPoint->GetPosition());
		particles_pos.c.sub(CollidedParticlePivot);
		m_particles_ground->SetXFORM(particles_pos);
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
#ifndef TEMPORARLY_REMOVE_FLAMETHROWER_LOGIC
	if (m_particle_alpha_handle.IsValid()) {
		m_particle_alpha_handle.Set(AlphaValue);
	}
#endif
	RadiusCurrent = std::max(RadiusOnCollide, AlphaValue * m_RadiusCollided);
	PAPI::pVector Size;
	Size.x = RadiusCurrent * m_RadiusCollisionCollidedCoeff.x;
	Size.y = RadiusCurrent * m_RadiusCollisionCollidedCoeff.y;
	Size.z = RadiusCurrent * m_RadiusCollisionCollidedCoeff.z;
#ifndef TEMPORARLY_REMOVE_FLAMETHROWER_LOGIC
	if (m_particle_size_handle.IsValid()) {
		m_particle_size_handle.Set(Size);
	}
#endif

	auto Position = AttachPoint->GetPosition();
	Position.x -= CollidedParticlePivot.x * Size.x;
	Position.y -= CollidedParticlePivot.y * Size.y;
	Position.z -= CollidedParticlePivot.z * Size.z;
	Fmatrix particles_pos = Fmatrix();
	particles_pos.c.set(Position);
	m_particles_ground->SetXFORM(particles_pos);
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
	const float AlphaValue = 1.0f - std::pow(1.0f - interpTime, 2.0f);
#ifndef TEMPORARLY_REMOVE_FLAMETHROWER_LOGIC
	if(m_particle_alpha_handle.IsValid())
	{
		m_particle_alpha_handle.Set(AlphaValue);
	}
#endif
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
	//m_Velocity = pSettings->r_float(section, "Velocity");
	m_LifeTime = pSettings->r_float(section, "LifeTime");
	m_LifeTimeCollidedMax = pSettings->r_float(section, "LifeTimeCollided");
	m_FlameFadeTime = pSettings->r_float(section, "FlameFadeTime");
	//m_GravityAcceleration = pSettings->r_float(section, "GravityAcceleration");

	// flames
	m_sFlameParticles = pSettings->r_string(section, "flame_particles");
	m_sFlameParticlesGround = pSettings->r_string(section, "earth_flame_particles");
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
	m_particles = CParticlesObject::Create(*m_sFlameParticles, false);
#ifndef TEMPORARLY_REMOVE_FLAMETHROWER_LOGIC
	m_particle_alpha_handle = m_particles->GetFloatHandle("AlphaHandle");
	m_particle_size_handle = m_particles->GetVectorHandle("SizeHandle");
#endif
	m_particles->Play(false);
	RadiusCurrent = m_RadiusMin;
}

void FlamethrowerTrace::CCollision::Deactivate()
{
	m_State = ETraceState::Idle;
	m_current_time = 0.0f;
	m_time_on_collide = 0.0f;
	RadiusOnCollide = 0.0f;
	AttachPoint = nullptr;
#ifndef TEMPORARLY_REMOVE_FLAMETHROWER_LOGIC
	m_particle_alpha_handle.Reset();
#endif
	if (m_particles) {
		m_particles->Stop();
		CParticlesObject::Destroy(m_particles);
	}
	if (m_particles_ground) {
		m_particles_ground->Stop();
		CParticlesObject::Destroy(m_particles_ground);
	}
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

FlamethrowerTrace::CJoin::CJoin(CManager* Manager)
{
	this->Manager = Manager;
	CollisionRadiusMax = pSettings->r_float(Manager->GetSection(), "RadiusMax");
}

void FlamethrowerTrace::CJoin::Update(float DeltaTime)
{
	if(!AfterPrev)
	{
		AfterPrev = Next;
	}
	float ChunkDistance = Prev->GetPosition().distance_to(AfterPrev->GetPosition());
	int ChunksNum = std::ceil(Prev->GetPosition().distance_to(Next->GetPosition()) / CollisionRadiusMax);
	if(ChunkDistance > CollisionRadiusMax)
	{
		for(int i = IntermediatePoints.size(); i < ChunksNum; ++i)
		{
			auto NewAdded = Manager->LaunchPoint();
			IntermediatePoints.push_back(NewAdded);
			NewAdded->Activate();
		}
	}
	for(u32 i = 0; i < IntermediatePoints.size(); ++i)
	{
		float Alpha = static_cast<float>(i + 1) / (ChunksNum + 1);
		Fvector Pos = lerp(Prev->GetPosition(), Next->GetPosition(), Alpha);
		Fvector Dir = lerp(Prev->GetDirection(), Next->GetDirection(),Alpha).GetNormalizedCopy();
		if(!IntermediatePoints[i]->VerifySpawnPos(Pos, Dir, Pos))
		{
			IntermediatePoints[i]->SetState(ETraceState::AirToGround);
		}
		IntermediatePoints[i]->SetTransform(Pos, Dir);
		IntermediatePoints[i]->SetGravityVelocity(lerp(Prev->GetGravityVelocity(), Next->GetGravityVelocity(), Alpha));
		IntermediatePoints[i]->SetLastUpdateTime(lerp(Prev->GetLastUpdateTime(), Next->GetLastUpdateTime(), Alpha));
		IntermediatePoints[i]->SetCurrentTime(lerp(Prev->GetPointCurrentTime(), Next->GetPointCurrentTime(), Alpha));
	}
}

void FlamethrowerTrace::CJoin::SetBorders(CPoint* prev, CPoint* next)
{
	Prev = prev;
	Next = next;
}

void FlamethrowerTrace::CJoin::Deactivate()
{
	Prev = nullptr;
	AfterPrev = nullptr;
	Next = nullptr;
	IntermediatePoints.clear();
}

bool FlamethrowerTrace::CJoin::HasCollidedPoints(xr_vector<CPoint*>& points)
{
	points.clear();
	for(const auto& elem : IntermediatePoints)
	{
		if(elem->IsCollided())
		{
			points.push_back(elem);
		}
	}
	if(Prev->IsCollided())
	{
		points.push_back(Prev);
	}
	if (Next->IsCollided())
	{
		points.push_back(Next);
	}
	return !points.empty();
}

xr_vector<FlamethrowerTrace::CPoint*> FlamethrowerTrace::CJoin::GetAllPoints() const
{
	auto AllPoints = IntermediatePoints;
	AllPoints.push_back(Prev);
	AllPoints.push_back(Next);
	return AllPoints;
}

xr_vector<FlamethrowerTrace::CJoin*> FlamethrowerTrace::CManager::SplitJoin(CJoin* Join,
	const xr_vector<CPoint*>& CollidedPoints)
{
	xr_vector<CJoin*> NewCreatedJoins{};
	xr_vector<CPoint*> NotCollidedPoints{};
	for(const auto& elem : Join->GetAllPoints())
	{
		if(std::find(CollidedPoints.begin(), CollidedPoints.end(), elem) == CollidedPoints.end())
		{
			NotCollidedPoints.push_back(elem);
		}
	}
	for(int i = 0; i < static_cast<int>(NotCollidedPoints.size())-1; ++i)
	{
		auto JoinA = NotCollidedPoints[i];
		auto JoinB = NotCollidedPoints[i + 1];
		NewCreatedJoins.push_back(GetJoin(JoinA, JoinB));
	}
	return NewCreatedJoins;
}

FlamethrowerTrace::CManager::CManager(CFlamethrower* flamethrower) : m_flamethrower(flamethrower)
{

#ifdef DEBUG
#ifndef TEMPORARLY_REMOVE_FLAMETHROWER_LOGIC
	Level().BulletManager().MarkFlamethrowerTraceToDraw(this);
#endif
#endif
}

FlamethrowerTrace::CManager::~CManager()
{
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
	for (auto& elem : InactiveJoins) {
		xr_delete(elem);
	}
	for (auto& elem : ActiveJoins) {
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
#ifndef TEMPORARLY_REMOVE_FLAMETHROWER_LOGIC
	Overlapped.remove(smart_cast<CCustomMonster*>(O));
#endif
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
#ifndef TEMPORARLY_REMOVE_FLAMETHROWER_LOGIC
	for (int i = 0; i < StartNum; ++i) {
		auto NewCollision = xr_new<FlamethrowerTrace::CCollision>(this);
		NewCollision->Load(section);
		InactiveCollisions.push_back(NewCollision);
	}
#endif
}

void FlamethrowerTrace::CManager::save(NET_Packet& output_packet)
{
}

void FlamethrowerTrace::CManager::load(IReader& input_packet)
{
}

void FlamethrowerTrace::CManager::UpdateOverlaps(float DeltaTime)
{
	Fvector Center{};
	float Radius = 0.0f;
	uint16_t Num = 0;
	//Msg("InactiveCollisions num [%u]", InactiveCollisions.size());
	//Msg("ActiveCollisions num [%u]", ActiveCollisions.size());
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

void FlamethrowerTrace::CManager::UpdateJoins(float DeltaTime)
{
	if (ActiveJoins.empty())
	{
		return;
	}
	auto FirstElem = ActiveJoins.front();
	ActiveJoins.pop_front();
	ActiveJoins.push_back(FirstElem);
	auto CurrentElem = FirstElem;
	do
	{
		CurrentElem = ActiveJoins.front();
		ActiveJoins.pop_front();
		CurrentElem->Update(DeltaTime);
		if (CurrentElem->IsActive())
		{
			ActiveJoins.push_back(CurrentElem);
		}
		else
		{
			CurrentElem->Deactivate();
			InactiveJoins.push_back(CurrentElem);
		}
	} while (CurrentElem != FirstElem);
	if (ActiveJoins.empty())
	{
		return;
	}
	FirstElem = ActiveJoins.front();
	ActiveJoins.pop_front();
	ActiveJoins.push_back(FirstElem);
	xr_vector<CJoin*> CreatedJoins{};
	do
	{
		CurrentElem = ActiveJoins.front();
		ActiveJoins.pop_front();
		xr_vector<CPoint*> CollidedPoints{};
		if(CurrentElem->HasCollidedPoints(CollidedPoints))
		{
			for(auto& elem : SplitJoin(CurrentElem, CollidedPoints))
			{
				CreatedJoins.push_back(elem);
			}
			CurrentElem->Deactivate();
			InactiveJoins.push_back(CurrentElem);
		} else
		{
			ActiveJoins.push_back(CurrentElem);
		}
	} while (CurrentElem != FirstElem);
	for(const auto& elem : CreatedJoins)
	{
		ActiveJoins.push_back(elem);
	}
}

void FlamethrowerTrace::CManager::RegisterOverlapped(CCustomMonster* enemy)
{
	Overlapped.push_back(enemy);
}

void FlamethrowerTrace::CManager::UnregisterOverlapped(CCustomMonster* enemy)
{
#ifndef TEMPORARLY_REMOVE_FLAMETHROWER_LOGIC
	Overlapped.remove(enemy);
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
		if(StartPos.distance_to_sqr(LastLaunched->GetPosition()) <= m_RadiusMax* m_RadiusMax)
		{
			return nullptr;
		}
	}
	auto FirstPoint = LaunchPoint();
	auto FirstCollision = ActiveCollisions.front();
	if(LastLaunched && LastLaunched->IsActive())
	{
		ActiveJoins.push_back(GetJoin(LastLaunched, FirstPoint));
	}
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

FlamethrowerTrace::CJoin* FlamethrowerTrace::CManager::GetJoin(CPoint* Prev, CPoint* Next)
{
	CJoin* FirstJoin;
	if (InactiveJoins.empty())
	{
		FirstJoin = new CJoin(this);
	}
	else
	{
		FirstJoin = InactiveJoins.front();
		InactiveJoins.pop_front();
	}
	FirstJoin->SetBorders(Prev, Next);
	return FirstJoin;
}
