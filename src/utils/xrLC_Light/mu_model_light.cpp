#include "stdafx.h"
#include "xrFace.h"
#include "xrMU_Model.h"
#include "xrMU_Model_Reference.h"
#include "xrLC_GlobalData.h"
#include "mu_model_light.h"

#include "../xrForms/xrThread.h"
#include "../xrForms/CompilersUI.h"

#include "../../xrCore/xrSyncronize.h"

CThreadManager mu_materials;
CThreadManager mu_secondary;
 
xrCriticalSection csMUMAPS_LOCKS;

static xr_atomic_u32 ThreadTaskID = 0;

xr_atomic_u32 MUModelsProcessed = 0;
xr_vector<xrMU_Model*>* MUSortedPtr = nullptr;

// mu-light
 
class CMULight : public CThread
{
public:
	CMULight(u32 ID) : CThread(ID)
	{
		thMessages = FALSE;
	}

	virtual void Execute()
	{
		// Priority
		SetThreadPriority(Platform::GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
		Sleep(0);
		
		while (true)
		{
  			csMUMAPS_LOCKS.Enter();
			int ID = ThreadTaskID;

			if (ThreadTaskID >= inlc_global_data()->mu_refs().size())
			{
				csMUMAPS_LOCKS.Leave();
				break;
			}

			ThreadTaskID++;

			//if (ID % 512 == 0)
			Status("Models %d/%d", ID, inlc_global_data()->mu_refs().size());
			thProgress = (float(ID) / float(inlc_global_data()->mu_refs().size()));

			csMUMAPS_LOCKS.Leave();

			// Light references
//#ifndef DEBUG
//			FATAL("You forget to turn on lighting calculations in MU models!")
//#endif
			inlc_global_data()->mu_refs()[ID]->calc_lighting	();
			u32 LocMUModelsProcessed = ++MUModelsProcessed;
			Status("Processed %u/%u models", LocMUModelsProcessed, inlc_global_data()->mu_refs().size());
		}
	}
};

class CMULightCalculation : public CThread
{
public:
	CMULightCalculation(u32 ID) : CThread(ID)
	{
		thMessages = FALSE;
	}

	virtual void	Execute()
	{
 		// Priority
		SetThreadPriority(Platform::GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
		Sleep(0);
		const auto& mu_sorted = *MUSortedPtr;

		while (true)
		{
			csMUMAPS_LOCKS.Enter();

			int ID = ThreadTaskID;
 			if (ThreadTaskID >= mu_sorted.size())
			{
				csMUMAPS_LOCKS.Leave();
				break;
			}
 			ThreadTaskID++;
			// Light references
			mu_sorted[ID]->calc_materials();
			thProgress = (float(ThreadTaskID) / float(mu_sorted.size()));
			//if (ID%512 == 0)
			Status("Models %d/%d", ID, mu_sorted.size());
			csMUMAPS_LOCKS.Leave();
 
//#ifndef DEBUG
//			FATAL("You forget to turn on lighting calculations in MU models!")
//#endif
			mu_sorted[ID]->calc_lighting();
			u32 LocMUModelsProcessed = ++MUModelsProcessed;
			Status("Processed %u/%u models", LocMUModelsProcessed, mu_sorted.size());
		}
	}
};


#include "xrDeflectorLight_Packed.h"

void run_mu_light()
{
	// Priority
	SetThreadPriority(Platform::GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
	Sleep(0);

	auto mu_sorted = inlc_global_data()->mu_models();
	std::ranges::sort(mu_sorted, [](xrMU_Model* A, xrMU_Model* B){return A->m_faces.size() > B->m_faces.size();});
	MUSortedPtr = &mu_sorted;

	MUModelsProcessed = 0;
	ThreadTaskID = 0;
	for (u32 thID = 0; thID < gCompilerMode.ThreadsPerWork; thID++)
	{
		mu_materials.start(new CMULightCalculation(thID));
	}
 	mu_materials.wait(100); 

	// Light references
	MUModelsProcessed = 0;
#ifdef LCCUDA_BUILD
	if (gCompilerMode.CUDA)
	{
		GPUTaskinSystem.RestartALL();

		// Gathering
		int REF_INDEX = 0;
		for (auto& REF : inlc_global_data()->mu_refs())
		{
			AditionalData("REF LIGHT: %u/%u", REF_INDEX, inlc_global_data()->mu_refs().size());
			REF->calc_lighting_cuda_1();
			REF_INDEX++;
		}
		GPUTaskinSystem.LightPointPacked_MODELRun();
		
		// APPLY
		REF_INDEX = 0;
		for (auto& REF : inlc_global_data()->mu_refs())
		{
			AditionalData("REF LIGHT APPLY: %u/%u", REF_INDEX, inlc_global_data()->mu_refs().size());

			REF->calc_lighting_cuda_2();
			REF->calc_lighting_cuda_3();
			REF_INDEX++;
		}


		GPUTaskinSystem.RestartALL(); // ��������� ��� ��� ��������� ��������� 
	}
	else
#endif
	{
		ThreadTaskID = 0;
		for (u32 thID = 0; thID < gCompilerMode.ThreadsPerWork; thID++)
			mu_secondary.start(new CMULight(thID));
		mu_secondary.wait(100);
	}
	
	MUSortedPtr = nullptr;

}