#pragma once

#include "../xrCore/xr_deque.h"
#include "../xrCore/xr_defines.h"
#include "../xrEngine/feel_touch.h"
#include "../xrCore/_types.h"
#include "../../xrParticles/particle_param_handle.h"
#include "../xrCore/xr_smart_pointers.h"

class CBulletManager;

namespace PAPI
{
	class pVector;
}

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
	
	//DEFINE_DEQUE(CCollision*, FCollisionsDeque, FCollisionsDequeIt);
	
	class CPoint
	{

		struct TraceData {
			CPoint* TracedObj = nullptr;
			float HitDist;
			bool Penetrate = false;
		};

		CManager* Manager = nullptr;
		ETraceState State = ETraceState::MAX;
		Fvector PointPosition{};
		Fvector LastUpdatedPos{};
		Fvector PointDirection{};
		float LifeTime = 0.0f;
		float LifeTimeCollided = 0.0f;
		float Velocity = 0.0f;
		float GravityVelocity = 0.0f;
		float GravityAcceleration = 0.0f;
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
		CPoint(CManager* Manager);

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

	class CJoin
	{
		CManager* Manager = nullptr;
		CPoint* Prev = nullptr;
		CPoint* AfterPrev = nullptr;
		CPoint* Next = nullptr;
		xr_vector<CPoint*> IntermediatePoints;
		float CollisionRadiusMax = 0.0f;

	public:

		CJoin(CManager* Manager);

		void Update(float DeltaTime);
		void SetBorders(CPoint* prev, CPoint* next);
		void Deactivate();
		bool IsActive() const { return Prev && Next && Prev->GetState() != ETraceState::Idle && Next->GetState() != ETraceState::Idle; }
		bool HasCollidedPoints(xr_vector<CPoint*>& points);
		xr_vector<CPoint*> GetAllPoints() const;
	};

	class CCollision :
		public Feel::Touch
	{

		CManager* Manager;
		CPoint* AttachPoint;
		ETraceState m_State = ETraceState::Idle;
		float m_current_time = 0.0f;
		float m_time_on_collide = 0.0f;
		//Fmatrix XFORM;
		//Fmatrix invXFORM;
		//Fvector m_LastUpdatedPos;
		//Fvector m_position;
		//Fvector m_direction;
		float RadiusCurrent;
		float RadiusOnCollide;

		shared_str m_sFlameParticles;
		shared_str m_sFlameParticlesGround;

		float m_last_update_time;

		float m_LifeTime = 0.0f;
		float m_LifeTimeCollidedMax = 0.0f;
		float m_FlameFadeTime = 0.0f;
		float m_RadiusMin = 0.0f;
		float m_RadiusMax = 0.0f;
		float m_RadiusMaxTime = 0.0f;
		float m_RadiusCollided = 0.0f;
		float m_RadiusCollidedInterpTime = 0.0f;
		Fvector m_RadiusCollisionCoeff{};
		Fvector m_RadiusCollisionCollidedCoeff{};
		Fvector CollidedParticlePivot{};
		//float m_Velocity = 0.0f;
		//float m_GravityVelocity = 0.0f;
		//float m_GravityAcceleration = 0.0f;

		CParticlesObject* m_particles = nullptr;
		CParticlesObject* m_particles_ground = nullptr;

		PAPI::Handle<float> m_particle_alpha_handle;
		PAPI::Handle<PAPI::pVector> m_particle_size_handle;

		struct FlamethrowerTraceData {
			CCollision* TracedObj = nullptr;
			float HitDist;
		};

		//static BOOL	hit_callback(collide::rq_result& result, LPVOID params);
		//static BOOL test_callback(const collide::ray_defs& rd, CObject* object, LPVOID params);

		void UpdateParticles();
		void Update_Air(float DeltaTime);
		void Update_AirToGround(float DeltaTime);
		void Update_Ground(float DeltaTime);
		void Update_End(float DeltaTime);

	public:

		//#ifdef DEBUG
		//CCollision* Next = nullptr;
		//CCollision* Prev = nullptr;
		//#endif

		CCollision(CManager* Manager);
		~CCollision();

		void Load(LPCSTR section);

		inline CManager* GetParent() const { return Manager; }
		void AttachToPoint(CPoint* point);
		inline CFlamethrower* GetParentWeapon() const;

		inline bool IsActive() const { return m_State != ETraceState::Idle; }
		inline bool IsCollided() const { return m_State == ETraceState::AirToGround || m_State == ETraceState::Ground; }
		bool IsReadyToUpdateCollisions();
		//inline Fvector GetCurrentPosition() const { return m_position; }
		float GetCurrentRadius();
		//inline Fvector GetPosition() const { return m_position; }
		//inline Fvector GetDirection() const { return m_direction; }
		inline float GetCurrentLifeTime() const { return m_current_time; }
		inline float GetLastUpdateTime() const { return m_last_update_time; }
		//inline float GetGravityVelocity() const { return m_GravityVelocity; }
		inline ETraceState GetTraceState() const { return m_State; }

		//void SetTransform(const Fvector& StartPos, const Fvector& StartDir);
		void SetCurrentLifeTime(const float Time);
		void SetLastUpdateTime(const float Time) { m_last_update_time = Time; }
		//void SetGravityVelocity(const float Value) { m_GravityVelocity = Value; }
		void SetTraceState(const ETraceState State) { m_State = State; }

		void	feel_touch_new(CObject* O) override;
		void	feel_touch_delete(CObject* O) override;
		BOOL	feel_touch_contact(CObject* O) override;

		void Activate();
		void Deactivate();
		void Update(float DeltaTime);

		//bool VerifySpawnPosition(Fvector Pos, Fvector Dir, Fvector& HitPos);
		Fvector GetPosition();
	};
	
	class CManager :
		public Feel::Touch
	{
	
	#ifdef DEBUG
		friend CBulletManager;
	#endif
	
		CFlamethrower* m_flamethrower;
		shared_str CollisionSection;
	
		DEFINE_VECTOR(CCustomMonster*, FOverlappedObjects, FOverlappedObjectsIt);
		DEFINE_VECTOR(CCollision*, FCollisions, FCollisionsIt);
	
		FOverlappedObjects Overlapped;
		//FCollisionsDeque CollisionsPool;
		xr_deque<CCollision*> InactiveCollisions;
		xr_deque<CCollision*> ActiveCollisions;
		xr_deque<CPoint*> InactivePoints;
		xr_deque<CPoint*> ActivePoints;
		xr_deque<CJoin*> InactiveJoins;
		xr_deque<CJoin*> ActiveJoins;
		//FCollisionsDeque CollisionsDeque;
	
		float LifeTime;
		float LifeTimeGroundAddition;
		float m_RadiusMax = 0.0f;

		xr_vector<CJoin*> SplitJoin(CJoin* Join, const xr_vector<CPoint*>& CollidedPoints);
	
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
	
		void UpdateOverlaps(float DeltaTime);
		void UpdatePoints(float DeltaTime);
		void UpdateJoins(float DeltaTime);
	
		void RegisterOverlapped(CCustomMonster* enemy);
		void UnregisterOverlapped(CCustomMonster* enemy);
		const FOverlappedObjects& GetOverlapped();
	
		inline CFlamethrower* GetParent() const { return m_flamethrower; }
	
		void OnShootingEnd();
	
		CCollision* LaunchTrace(const Fvector& StartPos, const Fvector& StartDir, bool Force = false);
		CPoint* LaunchPoint();
		CCollision* LaunchCollision(CPoint* RootPoint);
		CJoin* GetJoin(CPoint* Prev, CPoint* Next);
	
		//void ExpandCollisions(CCollision* First, CCollision* Second);
		const shared_str& GetSection() { return CollisionSection; }
	};

}
