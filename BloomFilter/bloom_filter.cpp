#include "pch.h"

#include <string>

namespace bloomfilter {
	class BloomFilter
	{
	public:
		explicit BloomFilter(int m, int n) : bits_(m) {
			k_ = static_cast<size_t>(m / n * 0.69); // 0.69 =~ ln(2)
			if (k_ < 1) k_ = 1;
			if (k_ > 30) k_ = 30;
			size_t bytes = (m + 7) / 8;
			if (bytes < 8) bytes = 8;
			dst.resize(bytes);
		};

		uint32_t BloomHash(const char* data, size_t n) {
			return Hash(data, n, 0xbc9f1d34);
		}


		inline uint32_t DecodeFixed32(const char* ptr) {
			const uint8_t* const buffer = reinterpret_cast<const uint8_t*>(ptr);

			// Recent clang and gcc optimize this to a single mov / ldr instruction.
			return (static_cast<uint32_t>(buffer[0])) |
				(static_cast<uint32_t>(buffer[1]) << 8) |
				(static_cast<uint32_t>(buffer[2]) << 16) |
				(static_cast<uint32_t>(buffer[3]) << 24);
		}

		uint32_t Hash(const char* data, size_t n, uint32_t seed) {
			// Similar to murmur hash
			const uint32_t m = 0xc6a4a793;
			const uint32_t r = 24;
			const char* limit = data + n;
			uint32_t h = seed ^ (n * m);

			// Pick up four bytes at a time
			while (data + 4 <= limit) {
				uint32_t w = DecodeFixed32(data);
				data += 4;
				h += w;
				h *= m;
				h ^= (h >> 16);
			}

			// Pick up remaining bytes
			switch (limit - data) {
			case 3:
				h += static_cast<uint8_t>(data[2]) << 16;
			case 2:
				h += static_cast<uint8_t>(data[1]) << 8;
			case 1:
				h += static_cast<uint8_t>(data[0]);
				h *= m;
				h ^= (h >> r);
				break;
			}
			return h;
		}

		void Add(std::string key) {
			char* array = &dst[0];
			std::cout << dst;
			uint32_t h = BloomHash(key.c_str(), key.size());
			const uint32_t delta = (h >> 17) | (h << 15);
			for (size_t i = 0; i < k_; i++) {
				const uint32_t bitpos = h % bits_;
				array[bitpos / 8] |= (1 << (bitpos % 8));
				h += delta;
			}
		}

		bool KeyMayMatch(std::string key) {
			char* array = &dst[0];
			uint32_t h = BloomHash(key.c_str(), key.size());
			const uint32_t delta = (h >> 17) | (h << 15);
			for (size_t i = 0; i < k_; i++) {
				const uint32_t bitpos = h % bits_;
				if ((array[bitpos / 8] & (1 << (bitpos % 8))) == 0) return false;
				h += delta;
			}
			return true;
		}

	private:
		size_t bits_;
		size_t k_;
		std::string dst;
	};
}