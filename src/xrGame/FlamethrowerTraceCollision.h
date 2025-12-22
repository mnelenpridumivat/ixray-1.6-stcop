#pragma once

#include "StdAfx.h"

#include "../xrCore/_stl_extensions.h"
#include "../xrEngine/feel_touch.h"
#include "../xrCore/_types.h"
//#include "../../xrParticles/particle_param_handle.h"
#include "../xrCore/Save/SaveObject.h"

class CBulletManager;

/*namespace PAPI
{
	class pVector;
}*/

class CParticlesObject;
class CFlamethrower;
class ENGINE_API CObject;
class CCustomMonster;

namespace FlamethrowerTrace
{
	class CManager;

	enum class ETraceState
	{
		Idle,
		Air,
		AirToGround,
		Ground,
		End,
		MAX
	};

	/*struct STraceConstants
	{
		CManager* Manager = nullptr;
		float LifeTime = 0.0f;
		float LifeTimeCollided = 0.0f;
		float Velocity = 0.0f;
		float GravityAcceleration = 0.0f;
		float m_FlameFadeTime = 0.0f; 
		float m_RadiusMin = 0.0f;
		float m_RadiusMax = 0.0f;
		float m_RadiusMaxTime = 0.0f;
		float m_RadiusCollided = 0.0f;
		float m_RadiusCollidedInterpTime = 0.0f;
	};

	class CTrace
	{
		
	};*/
	
	class CPoint
	{
		friend ISaveObject& operator<<(ISaveObject& Object, CPoint& Data);

		struct TraceData {
			CPoint* TracedObj = nullptr;
			float HitDist;
			bool Penetrate = false;
		};

		CManager* Manager = nullptr; //--
		ETraceState State = ETraceState::MAX;
		Fvector PointPosition{};
		Fvector LastUpdatedPos{};
		Fvector PointDirection{};
		float LifeTime = 0.0f; //--
		float LifeTimeCollided = 0.0f; //--
		float Velocity = 0.0f; //--
		float GravityVelocity = 0.0f;
		float GravityAcceleration = 0.0f; //--
		float CurrentTime = 0.0f;
		float LastUpdateTime = 0.0f;
		float TimeOnCollide = 0.0f;
		float RadiusCollidedInterpTime = 0.0f;

		void UpdateAir(float delta_time);
		void UpdateAirToGround(float delta_time);
		void UpdateGround(float delta_time);
		void UpdateEnd(float delta_time);

		static BOOL	hit_callback(collide::rq_result& result, LPVOID params);
		static BOOL test_callback(const collide::ray_defs& rd, CObject* object, LPVOID params);
	
	public:
		CPoint() = default;
		CPoint(CManager* Manager);

		void SetManager(CManager* Manager) { VERIFY(!Manager); this->Manager = Manager; }

		void Activate();
		void Update(float DeltaTime);
		bool VerifySpawnPos(const Fvector& Position, const Fvector& Direction, Fvector& HitPos);
		void Deactivate();
		const Fvector& GetPosition() const { return PointPosition; }
		const Fvector& GetDirection() const { return PointDirection; }
		void SetTransform(Fvector PointPosition, Fvector PointDirection)
		{
			this->PointPosition = PointPosition;
			this->PointDirection = PointDirection;
		}

		void SetState(ETraceState trace_state) { State = trace_state; }
		float GetGravityVelocity() const { return GravityVelocity; }
		float GetLastUpdateTime() const { return LastUpdateTime; }
		float GetPointCurrentTime() const { return CurrentTime; }
		void SetGravityVelocity(float GravityVelocity) { this->GravityVelocity = GravityVelocity; }
		void SetLastUpdateTime(float LastUpdateTime) { this->LastUpdateTime = LastUpdateTime; }
		void SetCurrentTime(float CurrentTime) { this->CurrentTime = CurrentTime; }
		bool IsActive() const { return State != ETraceState::Idle; }
		ETraceState GetState() const { return State; }
		bool IsCollided() const { return State == ETraceState::AirToGround || State == ETraceState::Ground; }
	};
	
	ISaveObject& operator<<(ISaveObject& Object, CPoint& Data);

	class CCollision :
		public Feel::Touch
	{
		friend ISaveObject& operator<<(ISaveObject& Object, CCollision& Data);

		CManager* Manager;
		CPoint* AttachPoint;
		ETraceState m_State = ETraceState::Idle;
		float m_current_time = 0.0f;
		float m_time_on_collide = 0.0f;
		float RadiusCurrent;
		float RadiusOnCollide;

		shared_str m_sFlameParticles;
		shared_str m_sFlameParticlesGround;

		float m_last_update_time;

		float m_LifeTime = 0.0f; //--!
		float m_LifeTimeCollidedMax = 0.0f; //--!
		float m_FlameFadeTime = 0.0f; //--
		float m_RadiusMin = 0.0f; //--
		float m_RadiusMax = 0.0f; //--
		float m_RadiusMaxTime = 0.0f; //--
		float m_RadiusCollided = 0.0f; //--
		float m_RadiusCollidedInterpTime = 0.0f; //--
		Fvector m_RadiusCollisionCoeff{};
		Fvector m_RadiusCollisionCollidedCoeff{};
		Fvector CollidedParticlePivot{};

		shared_str m_particle_alpha_air_PE_name;
		shared_str m_particle_alpha_ground_PE_name;
		shared_str m_particle_size_air_PE_name;
		shared_str m_particle_size_ground_PE_name;

		struct FlamethrowerTraceData {
			CCollision* TracedObj = nullptr;
			float HitDist;
		};
		
		void Update_Air(float DeltaTime);
		void Update_AirToGround(float DeltaTime);
		void Update_Ground(float DeltaTime);
		void Update_End(float DeltaTime);

	public:
		CCollision() = default;
		CCollision(CManager* Manager);
		virtual ~CCollision();

		void SetManager(CManager* Manager) { VERIFY(!Manager); this->Manager = Manager; }

		inline CManager* GetParent() const { return Manager; }
		void AttachToPoint(CPoint* point);
		inline CFlamethrower* GetParentWeapon() const;

		inline bool IsActive() const { return m_State != ETraceState::Idle; }
		inline bool IsCollided() const { return m_State == ETraceState::AirToGround || m_State == ETraceState::Ground; }
		bool IsReadyToUpdateCollisions();
		float GetCurrentRadius();
		inline float GetCurrentLifeTime() const { return m_current_time; }
		inline float GetLastUpdateTime() const { return m_last_update_time; }
		inline ETraceState GetTraceState() const { return m_State; }

		void SetCurrentLifeTime(const float Time);
		void SetLastUpdateTime(const float Time) { m_last_update_time = Time; }
		void SetTraceState(const ETraceState State) { m_State = State; }

		void	feel_touch_new(CObject* O) override;
		void	feel_touch_delete(CObject* O) override;
		BOOL	feel_touch_contact(CObject* O) override;

		void Activate();
		void Deactivate();
		void Update(float DeltaTime);

		Fvector GetPosition();
	};
	
	ISaveObject& operator<<(ISaveObject& Object, CCollision& Data);
	
	class CManager :
		public Feel::Touch
	{
		friend ISaveObject& operator<<(ISaveObject& Object, CManager& Data);
	
	#ifdef DEBUG
		friend CBulletManager;
	#endif
	
		CFlamethrower* m_flamethrower;
		shared_str CollisionSection;
	
		using FOverlappedObjects = xr_vector<CCustomMonster*>;
		using FCollisions = xr_vector<CCollision*>;

		//DEFINE_VECTOR(CCustomMonster*, FOverlappedObjects, FOverlappedObjectsIt);
		//DEFINE_VECTOR(CCollision*, FCollisions, FCollisionsIt);
	
		FOverlappedObjects Overlapped;

		using CollisionTrace = xr_pair<xr_unique_ptr<CPoint>, xr_unique_ptr<CCollision>>;
		using CollisionTracePtr = CollisionTrace*;
		xr_deque<CollisionTracePtr> InactiveTraces;
		xr_deque<CollisionTracePtr> ActiveTraces;
	
		float m_RadiusMax = 0.0f;
		
		void SerializeElem(ISaveObject& Object, CollisionTrace& Elem);
	
	public:
	
	//#ifdef DEBUG
		CPoint* LastLaunched = nullptr;
	//#endif
	
		CManager(CFlamethrower* flamethrower);
		~CManager();
	
		void	feel_touch_new(CObject* O) override;
		void	feel_touch_delete(CObject* O) override;
		BOOL	feel_touch_contact(CObject* O) override;
	
		void Load(LPCSTR section);

		void save(NET_Packet& output_packet);
		void load(IReader& input_packet);
		void Serialize(ISaveObject& Object);

		void Update(float DeltaTime);
	
		void RegisterOverlapped(CCustomMonster* enemy);
		void UnregisterOverlapped(CCustomMonster* enemy);
		const FOverlappedObjects& GetOverlapped();
	
		inline CFlamethrower* GetParent() const { return m_flamethrower; }
	
		void OnShootingEnd();
	
		void LaunchTrace(const Fvector& StartPos, const Fvector& StartDir, bool Force = false);
	
		const shared_str& GetSection() { return CollisionSection; }
	};

	ISaveObject& operator<<(ISaveObject& Object, CManager& Data);

}
