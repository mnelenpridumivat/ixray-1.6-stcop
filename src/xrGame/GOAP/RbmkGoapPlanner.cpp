#include "RbmkGoapPlanner.h"

#include "ai/monsters/state.h"


FRbmkGoapProperty::FRbmkGoapProperty(): Owner(nullptr)
{
}

FRbmkGoapProperty::~FRbmkGoapProperty()
{
}

bool FRbmkGoapProperty::GetProperty() const
{
	return false;
}

FRbmkGoapPropertyConst::FRbmkGoapPropertyConst(): Value(false)
{
}

bool FRbmkGoapPropertyConst::GetProperty() const
{
	return Value;
}

bool FRbmkGoapPropertyMember::GetProperty() const
{
	VERIFY(Value);
	if(Value)
	{
		return *Value == Equality;
	}
	return false;
}

FRbmkGoapAction::FRbmkGoapAction(const shared_str& InName):Owner(nullptr),Name(InName)
{
}

FRbmkGoapAction::~FRbmkGoapAction()
{
}

void FRbmkGoapAction::Active()
{
}

void FRbmkGoapAction::Update()
{
}

void FRbmkGoapAction::End()
{
}

FRbmkGoapAction& FRbmkGoapAction::AddCondition(const shared_str& Name, bool Value)
{
	Conditions.Add(Name,Value);
	Owner->NeedRefresh = true;
	return *this;
}

FRbmkGoapAction& FRbmkGoapAction::AddEffect(const shared_str& Name, bool Value)
{
	Effects.Add(Name,Value);
	Owner->NeedRefresh = true;
	return *this;
}

void FRbmkGoapParameters::Add(const shared_str& Name, bool Value)
{
	const auto Iterator = std::lower_bound(Parameters.begin(),Parameters.end(),Name,[](const FRbmkGoapParameter&Left, const shared_str&Right){return Left.Name._get()<Right._get();});
	if(Iterator != Parameters.end()&&Iterator->Name == Name)
	{
		
		InvalidHash |= Iterator->Value != Value;
		Iterator->Value = Value;
		return;
	}
	Parameters.insert(Iterator,{Name,Value});
	InvalidHash = true;

}

void FRbmkGoapParameters::Add(const FRbmkGoapParameters& InParameters)
{
	for(const FRbmkGoapParameter &Parameter:InParameters.Parameters)
	{
		Add(Parameter.Name,Parameter.Value);
	}
}

void FRbmkGoapParameters::Add(const FRbmkGoapParameters& Parameters, FRbmkGoapPlanner* ValuesFrom)
{
	auto Iterator = Parameters.Parameters.begin();
	const auto IteratorEnd = Parameters.Parameters.end();
	auto PropertiesIterator = ValuesFrom->CacheProperties.begin();
	auto PropertiesIteratorEnd = ValuesFrom->CacheProperties.end();
	while(PropertiesIterator != PropertiesIteratorEnd && Iterator != IteratorEnd)
	{
		if(Iterator->Name < PropertiesIterator->first->Name)
		{
			ValuesFrom->AddCacheProperty(Iterator->Name);
			PropertiesIterator = std::lower_bound(ValuesFrom->CacheProperties.begin(),ValuesFrom->CacheProperties.end(),Iterator->Name,
			[](const std::pair<FRbmkGoapProperty*,bool>&Left, const  shared_str&Right)
			{
				return Left.first->Name._get() < Right._get();
			});
			PropertiesIteratorEnd = ValuesFrom->CacheProperties.end();
		}
		else if(PropertiesIterator->first->Name < Iterator->Name)
		{
			++PropertiesIterator;
		}
		else 
		{
			VERIFY(PropertiesIterator->first->Name == Iterator->Name);
			Add(Iterator->Name,PropertiesIterator->second);
			++Iterator;
			++PropertiesIterator;
		}
	}
}

void FRbmkGoapParameters::SubFromCache(FRbmkGoapPlanner* From)
{
	auto Iterator = Parameters.begin();
	const auto IteratorEnd = Parameters.end();
	auto PropertiesIterator = From->CacheProperties.begin();
	auto PropertiesIteratorEnd = From->CacheProperties.end();
	while(PropertiesIterator != PropertiesIteratorEnd && Iterator != IteratorEnd)
	{
		if(Iterator->Name < PropertiesIterator->first->Name)
		{
			++Iterator;
		}
		else if(PropertiesIterator->first->Name < Iterator->Name)
		{
			From->AddCacheProperty(Iterator->Name);
			PropertiesIterator = std::lower_bound(From->CacheProperties.begin(),From->CacheProperties.end(),Iterator->Name,
			[](const std::pair<FRbmkGoapProperty*,bool>&Left, const  shared_str&Right)
			{
				return Left.first->Name._get() < Right._get();
			});
			PropertiesIteratorEnd = From->CacheProperties.end();
		}
		else 
		{
			VERIFY(PropertiesIterator->first->Name == Iterator->Name);
			if(PropertiesIterator->second == Iterator->Value)
			{
				Iterator  = Parameters.erase(Iterator);
				InvalidHash = true;
			}
			else
			{
				++Iterator;
			}
			++PropertiesIterator;
		}
	}
}

void FRbmkGoapParameters::ResetHash()
{
	if(InvalidHash)
	{
		Hash = 0xFFFFFFFF;
		for(const FRbmkGoapParameter &Parameter:Parameters)
		{
			Hash^= Parameter.Name._get()->dwCRC;
			if(Parameter.Value)
			{
				Hash+=1;
			}
		}
		InvalidHash = false;
	}

}

bool FRbmkGoapParameters::operator<(const FRbmkGoapParameters& Right) const
{
	if(Parameters.size() != Right.Parameters.size())
	{
		return Parameters.size() < Right.Parameters.size();
	}

	if(Hash!=Right.Hash)
	{
		return Hash<Right.Hash;
	}
	for(int32_t i = 0;i<Parameters.size() ;i++)
	{
		if(Parameters[i].Name != Right.Parameters[i].Name)
		{
			return Parameters[i].Name < Right.Parameters[i].Name;
		}
		if(Parameters[i].Value != Right.Parameters[i].Value)
		{
			return Parameters[i].Value < Right.Parameters[i].Value;
		}
	}
	return false;
}

bool FRbmkGoapParameters::operator==(const FRbmkGoapParameters& Right) const
{
	if(Parameters.size() != Right.Parameters.size())
	{
		return false;
	}

	if(Hash != Right.Hash)
	{
		return false;
	}
	for(int32_t i = 0;i<Parameters.size() ;i++)
	{
		if(Parameters[i].Name != Right.Parameters[i].Name)
		{
			return false;
		}
		if(Parameters[i].Value != Right.Parameters[i].Value)
		{
			return false;
		}
	}
	return true;
}

bool FRbmkGoapParameters::NotContradict(const FRbmkGoapParameters& With, FRbmkGoapPlanner* AppendFrom, bool FromCache)
{
	auto WithIterator = With.Parameters.begin();
	const auto WithIteratorEnd = With.Parameters.end();
	auto Iterator = Parameters.begin();
	auto IteratorEnd = Parameters.end();
	auto PropertiesIterator = AppendFrom->Properties.begin();
	const auto PropertiesIteratorEnd = AppendFrom->Properties.end();


	while(WithIterator != WithIteratorEnd && Iterator != IteratorEnd)
	{
		if(Iterator->Name < WithIterator->Name)
		{
			++Iterator;
		}
		else if(WithIterator->Name < Iterator->Name)
		{
			VERIFY(PropertiesIterator != PropertiesIteratorEnd);
			if(PropertiesIterator->get()->Name<WithIterator->Name)
			{
				++PropertiesIterator;
				continue;
			}
			VERIFY(PropertiesIterator->get()->Name == WithIterator->Name);

			if(FromCache)
			{
				Iterator = Parameters.insert(Iterator,{WithIterator->Name,  AppendFrom->FindOrAddCacheProperty(PropertiesIterator->get())});
			}
			else
			{
				Iterator = Parameters.insert(Iterator,{WithIterator->Name,  PropertiesIterator->get()->GetProperty()});
			}

			IteratorEnd = Parameters.end();
			++PropertiesIterator;
			InvalidHash = true;
		}
		else 
		{
			VERIFY(WithIterator->Name == Iterator->Name);
			if(WithIterator->Value != Iterator->Value)
			{
				return false;
			}
			++WithIterator;
			++Iterator;
		}
	}

	if(Iterator == IteratorEnd&& WithIterator != WithIteratorEnd)
	{
		bool Result = true;
		while(WithIterator != WithIteratorEnd)
		{
			VERIFY(PropertiesIterator != PropertiesIteratorEnd);
			if(PropertiesIterator->get()->Name<WithIterator->Name)
			{
				++PropertiesIterator;
				continue;
			}
			VERIFY(PropertiesIterator->get()->Name == WithIterator->Name);

			if(FromCache)
			{
				Parameters.push_back({WithIterator->Name,AppendFrom->FindOrAddCacheProperty(PropertiesIterator->get())});
			}
			else
			{
				Parameters.push_back({WithIterator->Name,PropertiesIterator->get()->GetProperty()});
			}

			VERIFY(WithIterator->Name == Parameters.back().Name);
			if(WithIterator->Value != Parameters.back().Value)
			{
				Result = false;
			}
			++WithIterator;
			++PropertiesIterator;
		}
		InvalidHash = true;
		return Result;
	}
	return true;
}

bool FRbmkGoapParameters::NotContradict(const FRbmkGoapParameters& With) const
{
	auto WithIterator = With.Parameters.begin();
	const auto WithIteratorEnd = With.Parameters.end();

	auto Iterator = Parameters.begin();
	const auto IteratorEnd = Parameters.end();
	while(WithIterator != WithIteratorEnd && Iterator != IteratorEnd)
	{
		if(Iterator->Name < WithIterator->Name)
		{
			++Iterator;
		}
		else if(WithIterator->Name < Iterator->Name)
		{
			++WithIterator;
		}
		else 
		{
			VERIFY(WithIterator->Name == Iterator->Name);
			if(WithIterator->Value != Iterator->Value)
			{
				return false;
			}
			++WithIterator;
			++Iterator;
		}
	}
	return true;
}

bool FRbmkGoapParameters::AllMatch(const FRbmkGoapParameters& In) const
{
	auto InIterator = In.Parameters.begin();
	const auto InIteratorEnd = In.Parameters.end();

	auto Iterator = Parameters.begin();
	const auto IteratorEnd = Parameters.end();
	while(InIterator != InIteratorEnd && Iterator != IteratorEnd)
	{
		if(Iterator->Name < InIterator->Name)
		{
			++Iterator;
		}
		else if(InIterator->Name < Iterator->Name)
		{
			return false;
		}
		else 
		{
			VERIFY(InIterator->Name == Iterator->Name);
			if(InIterator->Value != Iterator->Value)
			{
				return false;
			}
			++InIterator;
			++Iterator;
		}
	}
	return InIterator == InIteratorEnd;
}


FRbmkGoapPlanner::FRbmkGoapPlanner(void* InOwner): Owner(InOwner), CurrentAction(nullptr)
{
}

FRbmkGoapPlanner::~FRbmkGoapPlanner() = default;

void FRbmkGoapPlanner::AddProperty(FRbmkGoapProperty* InProperty)
{
	
	InProperty->Owner = this;

	const bool Inserted = Names2Properties.contains(InProperty->Name);

	VERIFY(Inserted == false);
	if(!Inserted)
	{
		const auto InsertIterator = std::lower_bound(Properties.begin(),Properties.end(),InProperty->Name,
		[](const xr_unique_ptr<FRbmkGoapProperty>&Left, const shared_str&Right)
		{
			return Left->Name._get()<Right._get();
		});
		const auto ResultIterator = Properties.insert(InsertIterator,xr_unique_ptr<FRbmkGoapProperty>(InProperty));
		Names2Properties.insert({ResultIterator->get()->Name,ResultIterator->get()});
	}
}

void FRbmkGoapPlanner::RemoveAction(FRbmkGoapAction* InAction)
{
	if(InAction)
	{
		if(InAction == CurrentAction)
		{
			CurrentAction->End();
			CurrentAction = nullptr;
		}
		Names2Actions.erase(InAction->Name);


		auto Iterator = std::ranges::find_if(Actions,[InAction](const xr_unique_ptr<FRbmkGoapAction>& Element) { return Element.get() == InAction; });
		Actions.erase(Iterator);

		NeedRefresh = true;
	}
}


void FRbmkGoapPlanner::RemoveProperty(FRbmkGoapProperty* InProperty)
{
	if(InProperty)
	{
		Names2Properties.erase(InProperty->Name);

		auto Iterator = std::ranges::find_if(Properties,[InProperty](const xr_unique_ptr<FRbmkGoapProperty>& Element) { return Element.get() == InProperty; });
		Properties.erase(Iterator);

		NeedRefresh = true;
	}
}


void FRbmkGoapPlanner::AddAction(FRbmkGoapAction* InAction)
{
	InAction->Owner = this;
	const bool Inserted = Names2Actions.contains(InAction->Name);
	VERIFY(Inserted == false);
	if(!Inserted)
	{
		Actions.emplace_back(xr_unique_ptr<FRbmkGoapAction>(InAction));
		Names2Actions.insert({Actions.back()->Name,Actions.back().get()});
	}
}

void FRbmkGoapPlanner::SetTarget(const shared_str& Name, bool Value)
{
	if(Target.Name != Name||Target.Value!=Value)
	{
		NeedRefresh = true;
	}
	Target.Name = Name;
	Target.Value = Value;
}

void FRbmkGoapPlanner::Update()
{
	if(!ActualCurrentAction())
	{
		if(!Search())
		{
			PrintError();
			return;
		}
		if(CurrentAction)
		{
			CurrentAction->End();
		}
		if(Path.empty())
		{
			return;
		}
		CurrentAction = Path.back();
		CurrentAction->Active();
	}

	CurrentAction->Update();
}

void FRbmkGoapPlanner::PrintError()
{
}

bool FRbmkGoapPlanner::FindOrAddCacheProperty( FRbmkGoapProperty* Property)
{
	VERIFY(Property);
	if(!Property)
	{
		return false;
	}
	auto InsertIterator = std::lower_bound(CacheProperties.begin(),CacheProperties.end(),Property,
	[](const std::pair<FRbmkGoapProperty*,bool>&Left, const  FRbmkGoapProperty*Right)
	{
		return Left.first->Name._get() < Right->Name._get();
	});

	if(InsertIterator == CacheProperties.end() || InsertIterator->first != Property)
	{
		InsertIterator = CacheProperties.insert(InsertIterator,{Property,Property->GetProperty()});
	}
	return InsertIterator->second;
}

void FRbmkGoapPlanner::AddCacheProperty(const shared_str& name)
{
	FindOrAddCacheProperty(Names2Properties[name]);
}


bool FRbmkGoapPlanner::ActualCurrentAction()
{
	if(CurrentAction == nullptr)
	{
		return false;
	}
	if(NeedRefresh)
	{
		NeedRefresh = false;
		return false;
	}
	FRbmkGoapParameters CheckCacheParameters;
	if(CheckCacheParameters.NotContradict(CurrentParameters,this,false))
	{
		return true;
	}

	return false;
}

bool FRbmkGoapPlanner::Search()
{
	FRbmkGoapParameters* StartParameters;
	FRbmkGoapParameters* TargetParameters = nullptr;
	
	auto GetParametersPtr = [this](FRbmkGoapParameters& From)
	{
	//	From.SubFromCache(this);
		From.ResetHash();

		const auto Iterator = std::lower_bound(CacheParameters.begin(),CacheParameters.end(), From,
		[](const xr_unique_ptr<FRbmkGoapParameters>&Left,const FRbmkGoapParameters&Right)
		{
			return *Left<Right;
		});

		if(Iterator != CacheParameters.end())
		{
			if(**Iterator == From)
			{
				return Iterator->get();
			}
		}

		FRbmkGoapParameters*NewGoapParameters =  new FRbmkGoapParameters(std::move(From));
		return CacheParameters.insert(Iterator,xr_unique_ptr<FRbmkGoapParameters>(NewGoapParameters))->get();
	};

	auto IsFinalNode = [this,&TargetParameters](const FRbmkGoapParameters& Node)
	{
		if(Node.AllMatch(*TargetParameters))
		{
			return true;
		}
		return false;
	};

	auto GetNeighbor = [this,&GetParametersPtr](const FRbmkGoapParameters* CurrentNode, const FRbmkGoapAction* Action,FRbmkGoapParameters*& OutParameters)->bool
	{
		if(CurrentNode->NotContradict(Action->Conditions))
		{
			/*FRbmkGoapParameters SearchCacheParameters;
			SearchCacheParameters.Add(*CurrentNode);
			if(SearchCacheParameters.NotContradict(Action->Conditions,this))
			{
				
			}*/
			FRbmkGoapParameters NewParameters;
			NewParameters.Add(*CurrentNode);
			NewParameters.Add(Action->Effects);
			OutParameters = GetParametersPtr(NewParameters);
			if(OutParameters==CurrentNode)
			{
				return false;
			}
			return true;
		}
		return false;
	};

	auto CalcCost = [this](FRbmkGoapParameters* Node1,FRbmkGoapParameters* Node2,FRbmkGoapAction* Action)
	{
		return 1;
	};

	auto DistanceNode = [this,&TargetParameters](FRbmkGoapParameters* CurrentNode)
	{
		return CurrentNode->NotContradict(*TargetParameters)?0:1;
	};

	TempPriorityNode.clear();
	TempCameFrom.clear();
	TempCostSoFar.clear();
	CacheParameters.clear();
	TempEdges.clear();
	Path.clear();
	CacheProperties.clear();

	{
		FRbmkGoapParameters NewTargetParameters;
		NewTargetParameters.Add(Target.Name,Target.Value);
		TargetParameters = GetParametersPtr(NewTargetParameters);
	}
	{
		FRbmkGoapParameters NewStartParameters;
		for(xr_unique_ptr<FRbmkGoapProperty>&Property :Properties)
		{
			NewStartParameters.Add(Property->Name,FindOrAddCacheProperty(Property.get()));
		}
		StartParameters = GetParametersPtr(NewStartParameters);
	}

	TempPriorityNode.push_back({0, StartParameters});
	TempCameFrom.insert({StartParameters, StartParameters});
	TempCostSoFar.insert( {StartParameters, 0});


	while (!TempPriorityNode.empty())
	{
		FRbmkGoapParameters* CurrentNode = TempPriorityNode.back().second;
		TempPriorityNode.pop_back();
		CurrentParameters.Parameters.clear();
		if (IsFinalNode(*CurrentNode))
		{
			FRbmkGoapParameters* NextNode = CurrentNode;
			while (NextNode != StartParameters)
			{
				Path.push_back(TempEdges[NextNode]);
				CurrentParameters.Add(Path.back()->Conditions,this);
				CurrentParameters.Add(Path.back()->Effects,this);
				NextNode = TempCameFrom[NextNode];
			}
			return true;
		}


		for (const xr_unique_ptr<FRbmkGoapAction>& Action : Actions )
		{
			FRbmkGoapParameters* Neighbor;
			if (!GetNeighbor(CurrentNode,Action.get(),Neighbor)) continue;

			int32_t NewCost = TempCostSoFar[CurrentNode] + CalcCost(CurrentNode, Neighbor,Action.get());
			auto TempCostSoFarIterator = TempCostSoFar.find(Neighbor);
			if ((TempCostSoFarIterator != TempCostSoFar.end() &&TempCostSoFarIterator->second > NewCost)|| (TempCostSoFarIterator == TempCostSoFar.end()))
			{
				const int32_t Distance = DistanceNode(Neighbor);

				if(TempCostSoFarIterator != TempCostSoFar.end())
				{
					TempCostSoFarIterator->second = NewCost; 
				}
				else
				{
					TempCostSoFar.insert({Neighbor,NewCost});
				}

				int32_t  Priority = NewCost + Distance;

				TempPriorityNode.insert(std::ranges::upper_bound(TempPriorityNode,
				                                                 std::pair<GraphEngineSpace::_solver_dist_type, FRbmkGoapParameters*>{Priority, Neighbor},
				                                                 [](const std::pair<GraphEngineSpace::_solver_dist_type, FRbmkGoapParameters*>& Left,const std::pair<GraphEngineSpace::_solver_dist_type, FRbmkGoapParameters*>& Right)
				                                                 {
					                                                 return Left.first > Right.first;
				                                                 }), 
				{Priority, Neighbor});
		
				auto TempCameFromIterator = TempCameFrom.find(Neighbor);
				const auto TempEdgesIterator = TempEdges.find(Neighbor);
				if(TempCameFromIterator!=TempCameFrom.end())
				{
					TempCameFromIterator->second = CurrentNode;
					TempEdgesIterator->second = Action.get();
				}
				else
				{
					TempCameFrom.insert({Neighbor,CurrentNode});
					TempEdges.insert({Neighbor,Action.get()});
				}
			}
		}
	}
	return false;
};
