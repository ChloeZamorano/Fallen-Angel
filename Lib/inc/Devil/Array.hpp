#pragma once
#include "pch.hpp"

namespace fln
{
	template<typename T, typename SizeT>
	class Array
	{
	public:
		Array() = delete;

		inline Array(SizeT count) :
			p_Data(new u8[sizeof(SizeT) + count * sizeof(T)])
		{
			*reinterpret_cast<SizeT*>(p_Data.get()) = count;
		}

		inline Array(T* raw, SizeT count) :
			p_Data(new u8[sizeof(SizeT) + count * sizeof(T)])
		{
			*reinterpret_cast<SizeT*>(p_Data.get()) = count;
			memcpy(p_Data.get() + sizeof(SizeT), raw, count * sizeof(T));
		}

		inline Array(std::initializer_list<T> list) :
			p_Data(new u8[sizeof(SizeT) + list.size() * sizeof(T)])
		{
			*reinterpret_cast<SizeT*>(p_Data.get()) = list.size();
			memcpy(p_Data.get() + sizeof(SizeT), list.begin(), list.size() * sizeof(T));
		}

		inline Array(const Array<T, SizeT>& other) :
			p_Data(new u8[other.Width()])
		{
			memcpy(other.Raw().get(), p_Data.get(), other.Width());
		}

		inline const std::shared_ptr<u8[]>& Raw() const { return p_Data; }

		inline SizeT Count() const { return *reinterpret_cast<SizeT*>(p_Data.get()); }
		inline SizeT Size() const { return Count() * sizeof(T); }
		inline SizeT Width() const { return sizeof(SizeT) + Count(); }

		inline T* Begin() { return reinterpret_cast<T*>(p_Data.get() + sizeof(SizeT)); }
		inline T* End() { return Begin() + Count(); }
		inline T* Last() { return End() - 1; }
		inline const T* Begin() const { return reinterpret_cast<const T*>(p_Data.get() + sizeof(SizeT)); }
		inline const T* End() const { return Begin() + Count(); }
		inline const T* Last() const { return End() - 1; }

		inline void swap(Array<T, SizeT>& other)
		{
			p_Data.swap(other.p_Data);
		}

		inline T& operator[](SizeT index)
		{
			#ifndef DEPLOY
				if (index < 0 || index >= Count())
					throw std::out_of_range(
						"fln::Array::operator[]: Index out of array bounds.");
			#endif
			return reinterpret_cast<T*>(p_Data.get() + sizeof(SizeT))[index];
		}

		inline const T& operator[](SizeT index) const
		{
			#ifndef DEPLOY
				if (index < 0 || index >= Count())
					throw std::out_of_range(
						"fln::Array::operator[]: Index out of array bounds.");
			#endif
			return reinterpret_cast<const T*>(p_Data.get() + sizeof(SizeT))[index];
		}

		inline Array<T, SizeT>& operator=(const Array<T, SizeT>& other)
		{
			if (this != &other)
				p_Data = other.p_Data;
			return *this;
		}

		inline bool operator==(const Array<T, SizeT>& other) const
		{
			if (Count() != other.Count())
				return false;
			
			if(p_Data.get() == other.p_Data.get())
				return true;

			return
				ADLER32(Begin(), Size()) ==
				ADLER32(other.Begin(), other.Size());
		}
	
		inline bool operator!=(const Array<T, SizeT>& other) const
		{
			return !(*this == other);
		}

	private:
		std::shared_ptr<u8[]> p_Data;
	};
}