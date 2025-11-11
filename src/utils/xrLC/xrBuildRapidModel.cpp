#include "StdAfx.h"
#include "Build.h"
#include "../../xrCore/Collision/xrCDB.h"

#include "../xrLC_Light/xrMU_Model.h"
#include "../xrLC_Light/xrMU_Model_Reference.h"
#include "../xrLC_Light/xrLC_GlobalData.h"
#include "../xrLC_Light/xrFace.h"

IC bool				FaceEqual(Face& F1, Face& F2)
{
	// Test for 6 variations
	if ((F1.v[0]==F2.v[0]) && (F1.v[1]==F2.v[1]) && (F1.v[2]==F2.v[2])) return true;
	if ((F1.v[0]==F2.v[0]) && (F1.v[2]==F2.v[1]) && (F1.v[1]==F2.v[2])) return true;
	if ((F1.v[2]==F2.v[0]) && (F1.v[0]==F2.v[1]) && (F1.v[1]==F2.v[2])) return true;
	if ((F1.v[2]==F2.v[0]) && (F1.v[1]==F2.v[1]) && (F1.v[0]==F2.v[2])) return true;
	if ((F1.v[1]==F2.v[0]) && (F1.v[0]==F2.v[1]) && (F1.v[2]==F2.v[2])) return true;
	if ((F1.v[1]==F2.v[0]) && (F1.v[2]==F2.v[1]) && (F1.v[0]==F2.v[2])) return true;
	return false;
}

void SaveUVM			(LPCSTR fname, xr_vector<b_rc_face>& vm)
{
	IWriter* W			= FS.w_open(fname);
	string256 tmp;
	// vertices
	for (u32 v_idx=0; v_idx<vm.size(); v_idx++){
		b_rc_face& rcf	= vm[v_idx];
		xr_sprintf			(tmp,"f %d %d [%3.2f,%3.2f]-[%3.2f,%3.2f]-[%3.2f,%3.2f]",rcf.dwMaterial,rcf.dwMaterialGame,
						rcf.t[0].x,rcf.t[0].y, rcf.t[1].x,rcf.t[1].y, rcf.t[2].x,rcf.t[2].y);
		W->w_string		(tmp);
	}
	FS.w_close	(W);
}
