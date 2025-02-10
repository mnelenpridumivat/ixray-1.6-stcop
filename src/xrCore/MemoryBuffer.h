#pragma once
#include <minwindef.h>

constexpr u32 NET_PacketSizeLimit = 16 * 1024;

namespace MemoryBuffer {

	enum class VariableType : uint8_t {
		t_unspecified = 0,
		t_float,
		t_vec3,
		t_vec4,
		t_u64,
		t_u64_q32,
		t_u64_q16,
		t_u64_q8,
		t_s64,
		t_s64_q32,
		t_s64_q16,
		t_s64_q8,
		t_u32,
		t_u32_q16,
		t_u32_q8,
		t_s32,
		t_s32_q16,
		t_s32_q8,
		t_u16,
		t_u16_q8,
		t_s16,
		t_s16_q8,
		t_u8,
		t_s8,
		t_float_q16,
		t_float_q8,
		t_angle16,
		t_angle8,
		t_dir,
		t_sdir,
		t_stringZ,
		t_matrix,
		t_clientID,
		t_chunk_open8,
		t_chunk_close8,
		t_chunk_open16,
		t_chunk_close16,
	};

	static const xr_hash_map<VariableType, u8> VariablesSize = {
		{VariableType::t_unspecified, 0},
		{VariableType::t_float, sizeof(float)},
		{VariableType::t_vec3, sizeof(Fvector)},
		{VariableType::t_vec4, sizeof(Fvector4)},
		{VariableType::t_u64, sizeof(u64)},
		{VariableType::t_u64_q32, sizeof(u32)},
		{VariableType::t_u64_q16, sizeof(u16)},
		{VariableType::t_u64_q8, sizeof(u8)},
		{VariableType::t_s64, sizeof(s64)},
		{VariableType::t_s64_q32, sizeof(s32)},
		{VariableType::t_s64_q16, sizeof(s16)},
		{VariableType::t_s64_q8, sizeof(s8)},
		{VariableType::t_u32, sizeof(u32)},
		{VariableType::t_u32_q16, sizeof(u16)},
		{VariableType::t_u32_q8, sizeof(u8)},
		{VariableType::t_s32, sizeof(s32)},
		{VariableType::t_s32_q16, sizeof(s16)},
		{VariableType::t_s32_q8, sizeof(s8)},
		{VariableType::t_u16, sizeof(u16)},
		{VariableType::t_u16_q8, sizeof(u8)},
		{VariableType::t_s16, sizeof(s16)},
		{VariableType::t_s16_q8, sizeof(s8)},
		{VariableType::t_u8, sizeof(u8)},
		{VariableType::t_s8, sizeof(s8)},
		{VariableType::t_float_q16, sizeof(u16)},
		{VariableType::t_float_q8, sizeof(u8)},
		{VariableType::t_angle16, sizeof(u16)},
		{VariableType::t_angle8, sizeof(u8)},
		{VariableType::t_dir, sizeof(u16)},
		{VariableType::t_sdir, sizeof(u16) + sizeof(float)},
		{VariableType::t_stringZ, 0},
		{VariableType::t_matrix, sizeof(Fvector) * 4},
		{VariableType::t_clientID, sizeof(u32)},
		{VariableType::t_chunk_open8, sizeof(u8)},
		{VariableType::t_chunk_close8, 0},
		{VariableType::t_chunk_open16, sizeof(u16)},
		{VariableType::t_chunk_close16, 0}
	};

	class CVariable {
		xr_vector<BYTE> Value;
		u32 size;
	public:
		CVariable(const void* p, u32 size);
		CVariable(VariableType type, IReader& stream);
		void SetValue(const void* p, u32 size);
		void* GetPtr() { return &(*Value.begin()); }
		u32 GetSize() { return size; }
	};

	class IBufferBase {
	public:
		virtual void w_start() = 0;
		virtual u32 w_pos() = 0;
		virtual void w(const void* p, u32 count, VariableType type = VariableType::t_unspecified) = 0;
		virtual void r_start() = 0;
		virtual u32 r_pos() = 0;
		virtual void r(void* p, u32 count, VariableType type = VariableType::t_unspecified) = 0;
		virtual void* find(u32 pos) = 0;
		virtual bool r_eof() = 0;
		virtual void r_seek(u32 pos) = 0;
		virtual void r_advance(u32 count) = 0;
		virtual void* get_raw() = 0;
		virtual void write_data(IWriter& writer) = 0;
		virtual void read_data(IReader& reader) = 0;
	};

	class CBufferSimple : public IBufferBase {
		BYTE	data[NET_PacketSizeLimit];
		u32		count = 0;
		u32		r_position = 0;
	public:

		// Inherited via IMemoryBufferBase
		virtual void w_start() override;
		virtual u32 w_pos() override;
		virtual void w(const void* p, u32 count, VariableType type) override;
		virtual void r_start() override;
		virtual u32 r_pos() override;
		virtual void r(void* p, u32 count, VariableType type) override;
		virtual void* find(u32 count) override;
		virtual bool r_eof() override;
		virtual void r_seek(u32 pos) override;
		virtual void r_advance(u32 count) override;
		virtual void* get_raw() override;
		virtual void write_data(IWriter& writer) override;
		virtual void read_data(IReader& reader) override;
	};

	class CBufferComplex : public IBufferBase {
		using Content = xr_pair<VariableType, xr_unique_ptr<CVariable>>;
		xr_vector<xr_pair<u32, Content>> Variables;
		u32		count = 0;
		u32		r_position = 0;
		u32		r_pos_internal = 0;

		u32 SearchInternalPos(u32 pos);

	public:

		// Inherited via IMemoryBufferBase
		virtual void w_start() override;
		virtual u32 w_pos() override;
		virtual void w(const void* p, u32 count, VariableType type) override;
		virtual void r_start() override;
		virtual u32 r_pos() override;
		virtual void r(void* p, u32 count, VariableType type) override;
		virtual void* find(u32 count) override;
		virtual bool r_eof() override;
		virtual void r_seek(u32 pos) override;
		virtual void r_advance(u32 count) override;
		virtual void* get_raw() override;
		virtual void write_data(IWriter& writer) override;
		virtual void read_data(IReader& reader) override;

	};
}