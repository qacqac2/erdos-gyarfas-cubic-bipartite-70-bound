#ifndef ERDOS_GYARFAS_TRANSCRIPT_SHA256_HPP
#define ERDOS_GYARFAS_TRANSCRIPT_SHA256_HPP
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>

namespace transcript_audit {

class Sha256 {
  public:
    Sha256() = default;
    void update(const std::uint8_t* data, std::size_t size) {
        total_bytes_ += size;
        for (std::size_t i = 0; i < size; ++i) {
            buffer_[buffer_size_++] = data[i];
            if (buffer_size_ == buffer_.size()) {
                transform(buffer_.data());
                buffer_size_ = 0;
            }
        }
    }

    std::array<std::uint8_t, 32> digest() const {
        Sha256 copy = *this;
        return copy.finalize();
    }
    std::string hex_digest() const {
        static constexpr char digits[] = "0123456789abcdef";
        const auto bytes = digest();
        std::string result(64, '0');
        for (std::size_t i = 0; i < bytes.size(); ++i) {
            result[2 * i] = digits[bytes[i] >> 4];
            result[2 * i + 1] = digits[bytes[i] & 0x0f];
        }
        return result;
    }
  private:
    std::array<std::uint32_t, 8> state_{
        0x6a09e667U, 0xbb67ae85U, 0x3c6ef372U, 0xa54ff53aU,
        0x510e527fU, 0x9b05688cU, 0x1f83d9abU, 0x5be0cd19U,
    };
    std::array<std::uint8_t, 64> buffer_{};
    std::size_t buffer_size_ = 0;
    std::uint64_t total_bytes_ = 0;
    static constexpr std::array<std::uint32_t, 64> constants_{
        0x428a2f98U, 0x71374491U, 0xb5c0fbcfU, 0xe9b5dba5U,
        0x3956c25bU, 0x59f111f1U, 0x923f82a4U, 0xab1c5ed5U,
        0xd807aa98U, 0x12835b01U, 0x243185beU, 0x550c7dc3U,
        0x72be5d74U, 0x80deb1feU, 0x9bdc06a7U, 0xc19bf174U,
        0xe49b69c1U, 0xefbe4786U, 0x0fc19dc6U, 0x240ca1ccU,
        0x2de92c6fU, 0x4a7484aaU, 0x5cb0a9dcU, 0x76f988daU,
        0x983e5152U, 0xa831c66dU, 0xb00327c8U, 0xbf597fc7U,
        0xc6e00bf3U, 0xd5a79147U, 0x06ca6351U, 0x14292967U,
        0x27b70a85U, 0x2e1b2138U, 0x4d2c6dfcU, 0x53380d13U,
        0x650a7354U, 0x766a0abbU, 0x81c2c92eU, 0x92722c85U,
        0xa2bfe8a1U, 0xa81a664bU, 0xc24b8b70U, 0xc76c51a3U,
        0xd192e819U, 0xd6990624U, 0xf40e3585U, 0x106aa070U,
        0x19a4c116U, 0x1e376c08U, 0x2748774cU, 0x34b0bcb5U,
        0x391c0cb3U, 0x4ed8aa4aU, 0x5b9cca4fU, 0x682e6ff3U,
        0x748f82eeU, 0x78a5636fU, 0x84c87814U, 0x8cc70208U,
        0x90befffaU, 0xa4506cebU, 0xbef9a3f7U, 0xc67178f2U,
    };
    static std::uint32_t rotate_right(std::uint32_t value, int shift) {
        return (value >> shift) | (value << (32 - shift));
    }
    void transform(const std::uint8_t* chunk) {
        std::array<std::uint32_t, 64> words{};
        for (int i = 0; i < 16; ++i) {
            words[i] =
                (static_cast<std::uint32_t>(chunk[4 * i]) << 24) |
                (static_cast<std::uint32_t>(chunk[4 * i + 1]) << 16) |
                (static_cast<std::uint32_t>(chunk[4 * i + 2]) << 8) |
                static_cast<std::uint32_t>(chunk[4 * i + 3]);
        }
        for (int i = 16; i < 64; ++i) {
            const std::uint32_t s0 =
                rotate_right(words[i - 15], 7) ^
                rotate_right(words[i - 15], 18) ^
                (words[i - 15] >> 3);
            const std::uint32_t s1 =
                rotate_right(words[i - 2], 17) ^
                rotate_right(words[i - 2], 19) ^
                (words[i - 2] >> 10);
            words[i] = words[i - 16] + s0 + words[i - 7] + s1;
        }
        std::uint32_t a = state_[0];
        std::uint32_t b = state_[1];
        std::uint32_t c = state_[2];
        std::uint32_t d = state_[3];
        std::uint32_t e = state_[4];
        std::uint32_t f = state_[5];
        std::uint32_t g = state_[6];
        std::uint32_t h = state_[7];
        for (int i = 0; i < 64; ++i) {
            const std::uint32_t sum1 =
                rotate_right(e, 6) ^ rotate_right(e, 11) ^ rotate_right(e, 25);
            const std::uint32_t choice = (e & f) ^ ((~e) & g);
            const std::uint32_t temporary1 =
                h + sum1 + choice + constants_[i] + words[i];
            const std::uint32_t sum0 =
                rotate_right(a, 2) ^ rotate_right(a, 13) ^ rotate_right(a, 22);
            const std::uint32_t majority = (a & b) ^ (a & c) ^ (b & c);
            const std::uint32_t temporary2 = sum0 + majority;
            h = g;
            g = f;
            f = e;
            e = d + temporary1;
            d = c;
            c = b;
            b = a;
            a = temporary1 + temporary2;
        }

        state_[0] += a;
        state_[1] += b;
        state_[2] += c;
        state_[3] += d;
        state_[4] += e;
        state_[5] += f;
        state_[6] += g;
        state_[7] += h;
    }
    std::array<std::uint8_t, 32> finalize() {
        const std::uint64_t bit_length = total_bytes_ * 8;
        buffer_[buffer_size_++] = 0x80;
        if (buffer_size_ > 56) {
            while (buffer_size_ < 64) buffer_[buffer_size_++] = 0;
            transform(buffer_.data());
            buffer_size_ = 0;
        }
        while (buffer_size_ < 56) buffer_[buffer_size_++] = 0;
        for (int i = 7; i >= 0; --i) {
            buffer_[buffer_size_++] =
                static_cast<std::uint8_t>(bit_length >> (8 * i));
        }
        transform(buffer_.data());
        std::array<std::uint8_t, 32> result{};
        for (std::size_t i = 0; i < state_.size(); ++i) {
            result[4 * i] = static_cast<std::uint8_t>(state_[i] >> 24);
            result[4 * i + 1] = static_cast<std::uint8_t>(state_[i] >> 16);
            result[4 * i + 2] = static_cast<std::uint8_t>(state_[i] >> 8);
            result[4 * i + 3] = static_cast<std::uint8_t>(state_[i]);
        }
        return result;
    }
};
enum class Outcome : std::uint8_t {
    degree_or_pair = 0,
    c8 = 1,
    c16 = 2,
    accepted = 3,
};

class SearchTranscript {
  public:
    void begin(std::uint8_t side_size, std::uint8_t root_orbit) {
        static constexpr std::array<std::uint8_t, 5> magic{
            'E', 'G', 'T', 'R', 1,
        };
        sha_.update(magic.data(), magic.size());
        const std::array<std::uint8_t, 2> header{side_size, root_orbit};
        sha_.update(header.data(), header.size());
    }
    void state(std::uint8_t point, std::uint8_t introduced,
               int last_q, int last_r, std::uint8_t number_of_blocks) {
        const std::array<std::uint8_t, 6> record{
            'S', point, introduced, encode_optional(last_q),
            encode_optional(last_r), number_of_blocks,
        };
        sha_.update(record.data(), record.size());
        ++states_;
    }
    void block(std::uint8_t a, std::uint8_t b, std::uint8_t c) {
        const std::array<std::uint8_t, 3> record{a, b, c};
        sha_.update(record.data(), record.size());
    }

    void candidate(std::uint8_t point, std::uint8_t q, std::uint8_t r,
                   Outcome outcome) {
        const std::array<std::uint8_t, 5> record{
            'C', point, q, r, static_cast<std::uint8_t>(outcome),
        };
        sha_.update(record.data(), record.size());
        ++candidates_;
    }
    void terminal(std::uint8_t kind) {
        const std::array<std::uint8_t, 2> record{'T', kind};
        sha_.update(record.data(), record.size());
    }

    void leave() {
        const std::uint8_t record = 'E';
        sha_.update(&record, 1);
    }

    std::uint64_t states() const { return states_; }
    std::uint64_t candidates() const { return candidates_; }
    std::string hex_digest() const { return sha_.hex_digest(); }
  private:
    Sha256 sha_;
    std::uint64_t states_ = 0;
    std::uint64_t candidates_ = 0;

    static std::uint8_t encode_optional(int value) {
        return value < 0 ? 0xff : static_cast<std::uint8_t>(value);
    }
};
inline bool sha256_self_test() {
    Sha256 empty;
    if (empty.hex_digest() !=
        "e3b0c44298fc1c149afbf4c8996fb924"
        "27ae41e4649b934ca495991b7852b855") {
        return false;
    }
    Sha256 abc;
    static constexpr std::array<std::uint8_t, 3> input{'a', 'b', 'c'};
    abc.update(input.data(), input.size());
    return abc.hex_digest() ==
        "ba7816bf8f01cfea414140de5dae2223"
        "b00361a396177a9cb410ff61f20015ad";
}

}  // namespace transcript_audit
#endif
