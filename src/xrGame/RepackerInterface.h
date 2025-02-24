#pragma once
#include "inventory_space.h"

class IRepackerInterface
{
public:

	// Repack current object with other. Returns true if still contains valid amount of content, false otherwise.
	virtual bool Repack(PIItem Other) = 0;

	// Returns true if contains valid amount of content, false otherwise.
	virtual bool IsValid() const = 0;
};
