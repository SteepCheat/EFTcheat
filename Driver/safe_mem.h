#pragma once
#include <vector>

namespace mem {

	uintptr_t unity_player_base = LI_MODULE_SAFE_(_("UnityPlayer.dll"));
	template <typename T>
	T read(DWORD64 address) {
		if (address > 0x40000 && (address + sizeof(T)) < 0x7FFFFFFF0000) {
			return *(T*)(address);
		}
		else {
			return T{};
		}
	}
	template <typename T>
	void write(DWORD64 address, T data) {
		if (address > 0x40000 && (address + sizeof(T)) < 0x7FFFFFFF0000) {
			*(T*)(address) = data;
		}
	}
}





template <typename Type>
Type ReadChain(uint64_t address, std::vector<uint64_t> chain)
{
	uint64_t current = address;
	for (int i = 0; i < chain.size() - 1; i++)
	{
		current = mem::read<uint64_t>(current + chain[i]);
	}
	return mem::read<Type>(current + chain[chain.size() - 1]);
}

uint64_t ReadChain_2(uint64_t base, const std::vector<uint64_t>& offsets) {
	uint64_t result = mem::read<uint64_t>(base + offsets.at(0));
	for (int i = 1; i < offsets.size(); i++) {
		result = mem::read<uint64_t>(result + offsets.at(i));
	}
	return result;
}







Vector3 GetPosition(uint64_t transform)
{
	auto transform_internal = mem::read<uint64_t>(transform + 0x10);

	if (!transform_internal)
		return Vector3(0, 0, 0);

	auto some_ptr = mem::read<uint64_t>(transform_internal + 0x38);
	auto index = mem::read<int32_t>(transform_internal + 0x38 + sizeof(uint64_t));
	if (!some_ptr)
		return Vector3();

	auto relation_array = mem::read<uint64_t>(some_ptr + 0x18);
	if (!relation_array)
		return Vector3();

	auto dependency_index_array = mem::read<uint64_t>(some_ptr + 0x20);
	if (!dependency_index_array)
		return Vector3();

	__m128i temp_0;
	__m128 xmmword_1410D1340 = { -2.f, 2.f, -2.f, 0.f };
	__m128 xmmword_1410D1350 = { 2.f, -2.f, -2.f, 0.f };
	__m128 xmmword_1410D1360 = { -2.f, -2.f, 2.f, 0.f };
	__m128 temp_1;
	__m128 temp_2;
	auto temp_main = mem::read<__m128>(relation_array + index * 48);
	auto dependency_index = mem::read<int32_t>(dependency_index_array + 4 * index);

	while (dependency_index >= 0) {
		auto relation_index = 6 * dependency_index;

		temp_0 = mem::read<__m128i>(relation_array + 8 * relation_index + 16);
		temp_1 = mem::read<__m128>(relation_array + 8 * relation_index + 32);
		temp_2 = mem::read<__m128>(relation_array + 8 * relation_index);

		__m128 v10 = _mm_mul_ps(temp_1, temp_main);
		__m128 v11 = _mm_castsi128_ps(_mm_shuffle_epi32(temp_0, 0));
		__m128 v12 = _mm_castsi128_ps(_mm_shuffle_epi32(temp_0, 85));
		__m128 v13 = _mm_castsi128_ps(_mm_shuffle_epi32(temp_0, -114));
		__m128 v14 = _mm_castsi128_ps(_mm_shuffle_epi32(temp_0, -37));
		__m128 v15 = _mm_castsi128_ps(_mm_shuffle_epi32(temp_0, -86));
		__m128 v16 = _mm_castsi128_ps(_mm_shuffle_epi32(temp_0, 113));
		__m128 v17 = _mm_add_ps(
			_mm_add_ps(
				_mm_add_ps(
					_mm_mul_ps(
						_mm_sub_ps(
							_mm_mul_ps(_mm_mul_ps(v11, xmmword_1410D1350), v13),
							_mm_mul_ps(_mm_mul_ps(v12, xmmword_1410D1360), v14)),
						_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v10), -86))),
					_mm_mul_ps(
						_mm_sub_ps(
							_mm_mul_ps(_mm_mul_ps(v15, xmmword_1410D1360), v14),
							_mm_mul_ps(_mm_mul_ps(v11, xmmword_1410D1340), v16)),
						_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v10), 85)))),
				_mm_add_ps(
					_mm_mul_ps(
						_mm_sub_ps(
							_mm_mul_ps(_mm_mul_ps(v12, xmmword_1410D1340), v16),
							_mm_mul_ps(_mm_mul_ps(v15, xmmword_1410D1350), v13)),
						_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v10), 0))),
					v10)),
			temp_2);

		temp_main = v17;
		dependency_index = mem::read<int32_t>(dependency_index_array + 4 * dependency_index);
	}

	return *reinterpret_cast<Vector3*>(&temp_main);
}




