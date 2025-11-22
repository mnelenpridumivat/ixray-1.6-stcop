#include "stdafx.h"
#include "Shell.h"

#include "Element.h"
#include "../Include/xrRender/Kinematics.h"
//#include "../xrEngine/bone.h"

using namespace physx;
using namespace xrPhysX;

namespace xrPhysX
{
    /*ICF bool no_physics_shape(const SBoneShape& shape)
    {
        return shape.type==SBoneShape::stNone||shape.flags.test(SBoneShape::sfNoPhysics);
    }

    bool ShapeIsPhysic(const SBoneShape& shape)
    {
        return !no_physics_shape( shape );
    }

    bool BoneHasPhysics(const IKinematics& kinematics, u16 bone_id)
    {
    	return kinematics.LL_GetBoneVisible( bone_id ) && ShapeIsPhysic(kinematics.GetBoneData(bone_id).get_shape());
    }

    bool VerifyPhysicsCollisionShapes(const IKinematics& kinematics)
    {
        u16 nbb = kinematics.LL_BoneCount();
        for(u16 i = 0; i < nbb; ++i )
        {
            if(BoneHasPhysics(kinematics, i))
            {
                return true;
            }
        }
        return false;
    }

	IC bool check_obb_sise(const Fobb& obb)
    {
    	return (!fis_zero(obb.m_halfsize.x,EPS_L)||
			!fis_zero(obb.m_halfsize.y,EPS_L)||
			!fis_zero(obb.m_halfsize.z,EPS_L));
    }*/
}

void Wrappers::CShell::build_FromKinematics(IKinematics& kinematics)
{
    m_kinematics = &kinematics;
	bool vis_check = false;
    AddElementRecursive(nullptr,kinematics.LL_GetBoneRoot(),Fidentity,0,&vis_check);
}

void Wrappers::CShell::AddElementRecursive(CElement* root_e, u16 id, Fmatrix global_parent, u16 element_number,
    bool* vis_check)
{
	/*const IBoneData& bone_data = m_kinematics->GetBoneData(u16(id));
	const SJointIKData& joint_data = bone_data.get_IK_data();

	Fmatrix fm_position = bone_data.get_bind_transform();
	fm_position.mulA_43(global_parent);

	// Set bone visible
	VisMask mask = m_kinematics->LL_GetBonesVisible();

	bool lvis_check = false;
	if (!mask.is(VisMask::GetBitMask(id), VisMask::GetChunkNumber(id)))
	{
		u16	num_children = bone_data.GetNumChildren();
		for (u16 i = 0; i < num_children; ++i)
		{
			AddElementRecursive(root_e, bone_data.GetChild(i).GetSelfID(), fm_position, element_number, &lvis_check);
		}

		return;
	}

	bool breakable = joint_data.ik_flags.test(SJointIKData::flBreakable) && root_e && !(no_physics_shape(bone_data.get_shape()) && joint_data.type == jtRigid);
	lvis_check = (check_obb_sise(bone_data.get_obb()));

	bool* arg_check = vis_check;
	if (breakable || !root_e)
	{
		arg_check = &lvis_check;
	}
	else
	{
		*vis_check = *vis_check || lvis_check;
	}

	//set true when if elemen created and added by this call
	bool element_added = false;
	u16 splitter_position = 0;
	u16 fracture_num = u16(-1);

	CElement* E = nullptr;
	CJoint* J = nullptr;

	if (!no_physics_shape(bone_data.get_shape()) || !root_e)
	{
		if (joint_data.type == jtRigid && root_e)
		{
			Fmatrix vs_root_position = root_e->mXFORM;
			vs_root_position.invert();
			vs_root_position.mulB_43(fm_position);

			E = root_e;
			if (breakable)
			{
				CPHFracture fracture;
				fracture.m_bone_id = id;
				R_ASSERT2(id < 64, "ower 64 bones in breacable are not supported");
				fracture.m_start_geom_num = E->numberOfGeoms();
				fracture.m_end_geom_num = u16(-1);
				fracture.m_start_el_num = u16(elements.size());
				fracture.m_start_jt_num = u16(joints.size());
				fracture.MassSetFirst(*(E->getMassTensor()));
				fracture.m_pos_in_element.set(vs_root_position.c);
				VERIFY(u16(-1) != fracture.m_start_geom_num);
				fracture.m_break_force = joint_data.break_force;
				fracture.m_break_torque = joint_data.break_torque;
				root_e->add_Shape(bone_data.get_shape(), vs_root_position);
				root_e->add_Mass(bone_data.get_shape(), vs_root_position, bone_data.get_center_of_mass(), bone_data.get_mass(), &fracture);

				fracture_num = E->setGeomFracturable(fracture);
			}
			else
			{
				root_e->add_Shape(bone_data.get_shape(), vs_root_position);
				root_e->add_Mass(bone_data.get_shape(), vs_root_position, bone_data.get_center_of_mass(), bone_data.get_mass());
			}
		}
		else
		{
			E = P_create_Element();
			E->m_SelfID = id;
			E->mXFORM.set(fm_position);
			E->SetMaterial(bone_data.get_game_mtl_idx());
			E->set_ParentElement(root_e);

			if (!no_physics_shape(bone_data.get_shape()))
			{
				E->add_Shape(bone_data.get_shape());
				E->setMassMC(bone_data.get_mass(), bone_data.get_center_of_mass());
			}

			element_number = u16(elements.size());
			add_Element(E);
			element_added = true;

			if (root_e)
			{
				J = BuildJoint(bone_data, root_e, E);
				if (J)
				{
					SetJointRootGeom(root_e, J);
					J->SetBoneID(id);
					add_Joint(J);
					if (breakable)
					{
						setEndJointSplitter();
						J->SetBreakable(joint_data.break_force, joint_data.break_torque);
					}
				}
			}
			if (m_spliter_holder)
			{
				splitter_position = u16(m_spliter_holder->m_splitters.size());
			}
		}
	}
	else
	{
		E = root_e;
	}

	if (!no_physics_shape(bone_data.get_shape()))
	{
		if (CODEGeom* added_geom = E->last_geom())
		{
			added_geom->set_bone_id(id);
			added_geom->set_shape_flags(bone_data.get_shape().flags);
		}
	}
#ifdef DEBUG
	if (E->last_geom())
		VERIFY(E->last_geom()->bone_id() != u16(-1));
#endif
	if (m_spliter_holder && E->has_geoms())
	{
		m_spliter_holder->AddToGeomMap(std::make_pair(id, E->last_geom()));
	}

	if (spGetingMap)
	{
		const BONE_P_PAIR_IT c_iter = spGetingMap->find(id);
		if (spGetingMap->end() != c_iter)
		{
			c_iter->second.joint = J;
			c_iter->second.element = E;
		}
	}

	u16	num_children = bone_data.GetNumChildren();
	for (u16 i = 0; i < num_children; ++i)
	{
		AddElementRecursive(E, bone_data.GetChild(i).GetSelfID(), fm_position, element_number, arg_check);
	}

	if (breakable)
	{
		if (joint_data.type == jtRigid)
		{
			CPHFracture& fracture = E->Fracture(fracture_num);
			fracture.m_bone_id = id;
			fracture.m_end_geom_num = E->numberOfGeoms();
			fracture.m_end_el_num = u16(elements.size());//just after this el = current+1
			fracture.m_end_jt_num = u16(joints.size());	 //current+1

		}
		else if (J)
		{
			J->JointDestroyInfo()->m_end_element = u16(elements.size());
			J->JointDestroyInfo()->m_end_joint = u16(joints.size());
		}
	}

	if (element_added && E->isBreakable())setElementSplitter(element_number, splitter_position);
#ifdef DEBUG
	bool bbb = lvis_check || (!breakable && root_e);
	if (!bbb)
	{
		IKinematics* K = m_pKinematics;

		Msg("all bones transform:--------");

		for (u16 ii = 0; ii < K->LL_BoneCount(); ++ii) {
			Fmatrix tr;

			tr = K->LL_GetTransform(ii);
			Msg("bone %s", K->LL_BoneName_dbg(ii));
			Log("bone_matrix ", tr);
		}
		Log("end-------");
	}

	if (dbg_obj != nullptr)
	{
		VERIFY3(bbb, dbg_obj->ObjectNameVisual(), "has breaking parts with no vertexes or size less than 1mm");//
	}
#endif*/
}

Wrappers::CShell* Wrappers::CShell::Create(const Interfaces::IShellHolder& holder, bool not_active_state)
{
    /*auto NewShell = new CShell();
	auto Kinematics = holder.ObjectKinematics();
    IVERIFY_M(Kinematics, "Can not create physics shell for object %s, model %s is not skeleton", holder.ObjectName(), holder.ObjectNameVisual());
    auto& KinematicsRef = *Kinematics;
    IVERIFY_M(VerifyPhysicsCollisionShapes(KinematicsRef), "Can not create physics shell for object %s, model %s has no physics collision shapes set", holder.ObjectName(), holder.ObjectNameVisual());
    IVERIFY_M(_valid(holder.ObjectXFORM()), "create physics shell: object matrix is not valid");
    
#ifdef DEBUG
    NewShell->m_dbg_obj = &holder;
#endif
    NewShell->build_FromKinematics(KinematicsRef);

    NewShell->set_PhysicsRefObject( obj );
    NewShell->mXFORM.set( obj->ObjectXFORM() );
    NewShell->Activate( not_active_state, not_set_bone_callbacks );//,
    //m_pPhysicsShell->SmoothElementsInertia(0.3f);
    NewShell->SetAirResistance();//0.0014f,1.5f*/
    
    return nullptr;
}
