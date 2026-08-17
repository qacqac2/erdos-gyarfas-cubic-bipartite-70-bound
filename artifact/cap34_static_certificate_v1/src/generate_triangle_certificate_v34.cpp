#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#ifndef SIDE
#define SIDE 34
#endif

namespace {

constexpr int kSide = SIDE;
static_assert(kSide >= 7 && kSide <= 34, "SIDE must be between 7 and 34");

constexpr std::array<char, 8> kMagic{{'E', 'G', '7', '0', 'T', 'R', 'I', '1'}};
constexpr std::uint8_t kFlagC8 = 1U;
constexpr std::uint8_t kFlagC16 = 2U;
constexpr std::uint8_t kRejectC8 = 0x08U;
constexpr std::uint8_t kRejectC16 = 0x10U;
constexpr std::uint8_t kExpand = 0x20U;

struct Block {
    int a;
    int b;
    int c;
};
struct Counts {
    std::uint64_t states = 0;
    std::uint64_t attempted = 0;
    std::uint64_t structural = 0;
    std::uint64_t c8 = 0;
    std::uint64_t c16 = 0;
    std::uint64_t completions = 0;
};
void write_byte(std::ostream& out, int value) {
    if (value < 0 || value > 255) {
        throw std::runtime_error("certificate byte out of range");
    }
    out.put(static_cast<char>(static_cast<unsigned char>(value)));
    if (!out) {
        throw std::runtime_error("failed while writing certificate");
    }
}

void write_u64(std::ostream& out, std::uint64_t value) {
    for (int shift = 0; shift < 64; shift += 8) {
        write_byte(out, static_cast<int>((value >> shift) & 0xffU));
    }
}
class CertificateGenerator {
  public:
    CertificateGenerator(std::ostream& output, bool test_c16)
        : out_(output), test_c16_(test_c16) {}

    Counts run(int orbit) {
        // Fixed Berge triangle: pairwise intersections 0,1,2 and
        // private points 3,4,5.
        add({0, 1, 3});
        add({1, 2, 4});
        add({0, 2, 5});
        Block first{};
        if (orbit == 1) {
            if (kSide < 7) {
                throw std::runtime_error("triangle orbit 1 requires SIDE >= 7");
            }
            first = {0, 4, 6};
            introduced_ = 7;
        } else if (orbit == 2) {
            if (kSide < 8) {
                throw std::runtime_error("triangle orbit 2 requires SIDE >= 8");
            }
            first = {0, 6, 7};
            introduced_ = 8;
        } else {
            throw std::runtime_error("invalid triangle-root orbit");
        }
        add(first);
        // {1,2,4} is the lexicographically first preinstalled block
        // through point 1 whose least point is 1.
        recurse(1, 2, 4);
        return counts_;
    }

  private:
    std::ostream& out_;
    bool test_c16_;
    std::vector<Block> blocks_;
    std::array<std::vector<int>, kSide> incident_;
    std::array<int, kSide> degree_{};
    std::array<std::array<bool, kSide>, kSide> paired_{};
    int introduced_ = 6;
    Counts counts_;
    static int intersection(const Block& left, const Block& right) {
        const int x[3] = {left.a, left.b, left.c};
        const int y[3] = {right.a, right.b, right.c};
        int value = -1;
        int count = 0;
        for (int a : x) {
            for (int b : y) {
                if (a == b) {
                    value = a;
                    ++count;
                }
            }
        }
        if (count == 0) {
            return -1;
        }
        return count == 1 ? value : -2;
    }
    bool c8_witness(const Block& candidate,
                    std::array<std::uint8_t, 3>& witness) const {
        const int count = static_cast<int>(blocks_.size());
        for (int i = 0; i < count; ++i) {
            const int p0 = intersection(candidate, blocks_[i]);
            if (p0 < 0) {
                continue;
            }
            for (int j = 0; j < count; ++j) {
                if (j == i) {
                    continue;
                }
                const int p1 = intersection(blocks_[i], blocks_[j]);
                if (p1 < 0 || p1 == p0) {
                    continue;
                }
                for (int h = 0; h < count; ++h) {
                    if (h == i || h == j) {
                        continue;
                    }
                    const int p2 = intersection(blocks_[j], blocks_[h]);
                    if (p2 < 0 || p2 == p0 || p2 == p1) {
                        continue;
                    }
                    const int p3 = intersection(blocks_[h], candidate);
                    if (p3 < 0 || p3 == p0 || p3 == p1 || p3 == p2) {
                        continue;
                    }
                    witness = {static_cast<std::uint8_t>(i),
                               static_cast<std::uint8_t>(j),
                               static_cast<std::uint8_t>(h)};
                    return true;
                }
            }
        }
        return false;
    }
    bool path14_dfs(int current, int target, int used_blocks,
                    std::uint64_t seen_points, std::uint64_t seen_blocks,
                    std::array<std::uint8_t, 7>& witness) const {
        if (used_blocks == 7) {
            return current == target;
        }
        for (int block_id : incident_[current]) {
            const std::uint64_t block_bit = std::uint64_t{1} << block_id;
            if ((seen_blocks & block_bit) != 0U) {
                continue;
            }
            const Block& block = blocks_[block_id];
            const int next_points[3] = {block.a, block.b, block.c};
            for (int next : next_points) {
                if (next == current) {
                    continue;
                }
                const std::uint64_t point_bit = std::uint64_t{1} << next;
                if ((seen_points & point_bit) != 0U) {
                    continue;
                }
                if (next == target && used_blocks + 1 != 7) {
                    continue;
                }
                if (next != target && used_blocks + 1 == 7) {
                    continue;
                }
                witness[used_blocks] =
                    static_cast<std::uint8_t>(block_id);
                if (path14_dfs(next, target, used_blocks + 1,
                               seen_points | point_bit,
                               seen_blocks | block_bit, witness)) {
                    return true;
                }
            }
        }
        return false;
    }
    bool c16_witness(const Block& candidate, std::uint8_t& pair_code,
                     std::array<std::uint8_t, 7>& witness) const {
        const int points[3] = {candidate.a, candidate.b, candidate.c};
        int code = 0;
        for (int i = 0; i < 3; ++i) {
            for (int j = i + 1; j < 3; ++j, ++code) {
                const int start = points[i];
                const int target = points[j];
                if (path14_dfs(start, target, 0,
                               std::uint64_t{1} << start, 0U, witness)) {
                    pair_code = static_cast<std::uint8_t>(code);
                    return true;
                }
            }
        }
        return false;
    }
    void add(const Block& block) {
        const int id = static_cast<int>(blocks_.size());
        blocks_.push_back(block);
        const int points[3] = {block.a, block.b, block.c};
        for (int point : points) {
            ++degree_[point];
            incident_[point].push_back(id);
        }
        for (int i = 0; i < 3; ++i) {
            for (int j = i + 1; j < 3; ++j) {
                paired_[points[i]][points[j]] = true;
                paired_[points[j]][points[i]] = true;
            }
        }
    }
    void remove(const Block& block) {
        const int points[3] = {block.a, block.b, block.c};
        for (int point : points) {
            --degree_[point];
            incident_[point].pop_back();
        }
        for (int i = 0; i < 3; ++i) {
            for (int j = i + 1; j < 3; ++j) {
                paired_[points[i]][points[j]] = false;
                paired_[points[j]][points[i]] = false;
            }
        }
        blocks_.pop_back();
    }
    void recurse(int point, int last_q, int last_r) {
        ++counts_.states;
        while (point < introduced_ && degree_[point] == 3) {
            ++point;
            last_q = -1;
            last_r = -1;
        }
        // Treat kSide as a cap.  A completed configuration must be detected
        // as soon as every introduced point is cubic, even when fewer than
        // kSide points have been introduced.
        if (point >= introduced_) {
            if (static_cast<int>(blocks_.size()) == introduced_) {
                ++counts_.completions;
            }
            return;
        }
        if (static_cast<int>(blocks_.size()) >= kSide) {
            return;
        }
        const int old_introduced = introduced_;
        std::vector<int> possible;
        for (int q = point + 1; q < old_introduced; ++q) {
            if (degree_[q] < 3 && !paired_[point][q]) {
                possible.push_back(q);
            }
        }
        if (old_introduced < kSide) {
            possible.push_back(old_introduced);
        }
        if (old_introduced + 1 < kSide) {
            possible.push_back(old_introduced + 1);
        }
        for (std::size_t i = 0; i < possible.size(); ++i) {
            for (std::size_t j = i + 1; j < possible.size(); ++j) {
                const int q = possible[i];
                const int r = possible[j];
                if (r == old_introduced + 1 && q != old_introduced) {
                    continue;
                }
                if (last_q >= 0 &&
                    (q < last_q || (q == last_q && r <= last_r))) {
                    continue;
                }
                ++counts_.attempted;
                const Block candidate{point, q, r};
                const int points[3] = {point, q, r};
                bool invalid = false;
                for (int value : points) {
                    if (value < old_introduced && degree_[value] >= 3) {
                        invalid = true;
                    }
                }
                for (int a = 0; a < 3; ++a) {
                    for (int b = a + 1; b < 3; ++b) {
                        if (points[a] < old_introduced &&
                            points[b] < old_introduced &&
                            paired_[points[a]][points[b]]) {
                            invalid = true;
                        }
                    }
                }
                if (invalid) {
                    ++counts_.structural;
                    continue;
                }
                std::array<std::uint8_t, 3> c8{};
                if (c8_witness(candidate, c8)) {
                    ++counts_.c8;
                    write_byte(out_, kRejectC8);
                    for (std::uint8_t id : c8) {
                        write_byte(out_, id);
                    }
                    continue;
                }
                std::uint8_t pair_code = 0;
                std::array<std::uint8_t, 7> c16{};
                if (test_c16_ &&
                    c16_witness(candidate, pair_code, c16)) {
                    ++counts_.c16;
                    write_byte(out_, kRejectC16);
                    write_byte(out_, pair_code);
                    for (std::uint8_t id : c16) {
                        write_byte(out_, id);
                    }
                    continue;
                }
                write_byte(out_, kExpand);
                int next_introduced = old_introduced;
                if (q == old_introduced || r == old_introduced) {
                    ++next_introduced;
                }
                if (r == old_introduced + 1) {
                    ++next_introduced;
                }
                const int saved_introduced = introduced_;
                introduced_ = next_introduced;
                add(candidate);
                recurse(point, q, r);
                remove(candidate);
                introduced_ = saved_introduced;
            }
        }
    }
};
void write_header(std::ostream& out, int orbit, std::uint8_t flags,
                  const Counts& counts) {
    for (char value : kMagic) {
        write_byte(out, static_cast<unsigned char>(value));
    }
    write_byte(out, kSide);
    write_byte(out, orbit);
    write_byte(out, flags);
    write_byte(out, 0);
    write_u64(out, counts.states);
    write_u64(out, counts.attempted);
    write_u64(out, counts.structural);
    write_u64(out, counts.c8);
    write_u64(out, counts.c16);
    write_u64(out, counts.completions);
}
}  // namespace

int main(int argc, char** argv) {
    try {
        if (argc != 3) {
            throw std::runtime_error(
                "usage: generate_triangle_certificate OUTPUT (1|2)");
        }
        const std::string output_path = argv[1];
        const std::string mode = argv[2];
        int orbit = 0;
        const bool test_c16 = true;
        if (mode == "1" || mode == "2") {
            orbit = mode[0] - '0';
        } else {
            throw std::runtime_error("mode must be 1 or 2");
        }
        std::ofstream out(output_path,
                          std::ios::binary | std::ios::trunc);
        if (!out) {
            throw std::runtime_error("cannot create " + output_path);
        }

        const std::uint8_t flags =
            kFlagC8 | (test_c16 ? kFlagC16 : 0U);
        write_header(out, orbit, flags, Counts{});
        const auto start = std::chrono::steady_clock::now();
        CertificateGenerator generator(out, test_c16);
        const Counts counts = generator.run(orbit);
        const auto finish = std::chrono::steady_clock::now();
        const std::streamoff bytes = out.tellp();

        out.seekp(0);
        write_header(out, orbit, flags, counts);
        out.close();
        if (!out) {
            throw std::runtime_error("failed to finalize " + output_path);
        }
        const double seconds =
            std::chrono::duration<double>(finish - start).count();
        std::cout << "CERT EG70TRI1"
                  << " side=" << kSide
                  << " orbit=" << orbit
                  << " flags=" << static_cast<int>(flags)
                  << " states=" << counts.states
                  << " attempted=" << counts.attempted
                  << " structural=" << counts.structural
                  << " c8=" << counts.c8
                  << " c16=" << counts.c16
                  << " completions=" << counts.completions
                  << " bytes=" << bytes
                  << " seconds=" << seconds << '\n';
        return counts.completions == 0 ? 0 : 2;
    } catch (const std::exception& error) {
        std::cerr << "CERTIFICATE GENERATION FAILED: "
                  << error.what() << '\n';
        return 1;
    }
}
