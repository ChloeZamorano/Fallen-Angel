#pragma once
#include "pch.hpp"

#include "HE/HE2/Tag.hpp"

namespace fln::he2
{
	struct ObjectRef
	{
		u16 ID;
		u16 GroupID;
	};

	struct Object
	{
		u64 Padding;
		u64 ObjectTypeOffset;

		u64 ObjectNameOffset;
		ObjectRef ID;
		ObjectRef ParentID;

		f32 Position[3];
		f32 Rotation[3];

		f32 ChildPositionOffset[3];
		f32 ChildRotationOffset[3];

		u64 TagsOffsetTableOffset;
		u64 TagCount;
		u64 TagCapacity;
		u64 Padding1;

		u64 ParametersOffset;
	};

    class ObjectDescriptor
    {
    public:
        Object* m_Entry;
        u64* TagsOffsetTable;
        std::vector<TagDescriptor> Tags;
    };
}
