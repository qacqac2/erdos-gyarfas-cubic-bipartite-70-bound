#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
constexpr int kMaximumSide = 34;
constexpr std::array<char, 8> kMagic{{'E', 'G', '7', '0', 'T', 'R', 'I', '1'}};
constexpr std::uint8_t kFlagC8 = 1U;
constexpr std::uint8_t kFlagC16 = 2U;
constexpr std::uint8_t kRejectC8 = 0x08U;
constexpr std::uint8_t kRejectC16 = 0x10U;
constexpr std::uint8_t kExpand = 0x20U;

struct Block {
    int point[3];
};
struct Counts {
    std::uint64_t states = 0;
    std::uint64_t attempted = 0;
    std::uint64_t structural = 0;
    std::uint64_t c8 = 0;
    std::uint64_t c16 = 0;
    std::uint64_t completions = 0;
};
struct Header {
    int side = 0;
    int orbit = 0;
    std::uint8_t flags = 0;
    Counts claimed;
};

[[noreturn]] void fail(const std::string& message) {
    throw std::runtime_error(message);
}
std::uint8_t read_byte(std::istream& input) {
    const int value = input.get();
    if (value == std::char_traits<char>::eof()) {
        fail("unexpected end of certificate");
    }
    return static_cast<std::uint8_t>(
        static_cast<unsigned char>(value));
}
std::uint64_t read_u64(std::istream& input) {
    std::uint64_t value = 0;
    for (int shift = 0; shift < 64; shift += 8) {
        value |= static_cast<std::uint64_t>(read_byte(input)) << shift;
    }
    return value;
}
Header read_header(std::istream& input) {
    for (char expected : kMagic) {
        if (read_byte(input) !=
            static_cast<std::uint8_t>(
                static_cast<unsigned char>(expected))) {
            fail("bad certificate magic");
        }
    }
    Header header;
    header.side = read_byte(input);
    header.orbit = read_byte(input);
    header.flags = read_byte(input);
    if (read_byte(input) != 0U) {
        fail("nonzero reserved header byte");
    }
    header.claimed.states = read_u64(input);
    header.claimed.attempted = read_u64(input);
    header.claimed.structural = read_u64(input);
    header.claimed.c8 = read_u64(input);
    header.claimed.c16 = read_u64(input);
    header.claimed.completions = read_u64(input);
    if (header.side < 7 || header.side > kMaximumSide) {
        fail("side size is outside the supported range");
    }
    if (header.orbit < 1 || header.orbit > 2) {
        fail("invalid root orbit");
    }
    if (header.flags != (kFlagC8 | kFlagC16)) {
        fail("triangle-root certificate has invalid flags");
    }
    return header;
}
bool counts_equal(const Counts& left, const Counts& right) {
    return left.states == right.states &&
           left.attempted == right.attempted &&
           left.structural == right.structural &&
           left.c8 == right.c8 &&
           left.c16 == right.c16 &&
           left.completions == right.completions;
}
class CertificateChecker {
  public:
    CertificateChecker(std::istream& input, const Header& header)
        : input_(input), side_(header.side), orbit_(header.orbit),
          flags_(header.flags) {}

    Counts check() {
        install({{0, 1, 3}});
        install({{1, 2, 4}});
        install({{0, 2, 5}});
        Block first{};
        if (orbit_ == 1) {
            if (side_ < 7) {
                fail("triangle root orbit 1 is unavailable");
            }
            first = {{0, 4, 6}};
            introduced_ = 7;
        } else if (orbit_ == 2) {
            if (side_ < 8) {
                fail("triangle root orbit 2 is unavailable");
            }
            first = {{0, 6, 7}};
            introduced_ = 8;
        } else {
            fail("invalid triangle root orbit");
        }
        install(first);
        visit(1, 2, 4);
        return counts_;
    }
  private:
    std::istream& input_;
    int side_;
    int orbit_;
    std::uint8_t flags_;
    std::vector<Block> blocks_;
    std::array<int, kMaximumSide> degree_{};
    std::array<std::array<bool, kMaximumSide>, kMaximumSide> paired_{};
    int introduced_ = 6;
    Counts counts_;
    static int intersection(const Block& left, const Block& right) {
        int common = -1;
        int number = 0;
        for (int x : left.point) {
            for (int y : right.point) {
                if (x == y) {
                    common = x;
                    ++number;
                }
            }
        }
        if (number == 0) {
            return -1;
        }
        return number == 1 ? common : -2;
    }
    void install(const Block& block) {
        if (!(block.point[0] < block.point[1] &&
              block.point[1] < block.point[2])) {
            fail("attempted to install an unordered block");
        }
        for (int point : block.point) {
            if (point < 0 || point >= side_) {
                fail("block point is outside the side size");
            }
            if (degree_[point] >= 3) {
                fail("accepted branch exceeds degree three");
            }
        }
        for (int i = 0; i < 3; ++i) {
            for (int j = i + 1; j < 3; ++j) {
                if (paired_[block.point[i]][block.point[j]]) {
                    fail("accepted branch repeats a point pair");
                }
            }
        }
        blocks_.push_back(block);
        for (int point : block.point) {
            ++degree_[point];
        }
        for (int i = 0; i < 3; ++i) {
            for (int j = i + 1; j < 3; ++j) {
                paired_[block.point[i]][block.point[j]] = true;
                paired_[block.point[j]][block.point[i]] = true;
            }
        }
    }
    void uninstall(const Block& block) {
        if (blocks_.empty()) {
            fail("internal block-stack underflow");
        }
        for (int point : block.point) {
            --degree_[point];
        }
        for (int i = 0; i < 3; ++i) {
            for (int j = i + 1; j < 3; ++j) {
                paired_[block.point[i]][block.point[j]] = false;
                paired_[block.point[j]][block.point[i]] = false;
            }
        }
        blocks_.pop_back();
    }
    bool structurally_invalid(const Block& candidate,
                              int old_introduced) const {
        for (int point : candidate.point) {
            if (point < old_introduced && degree_[point] >= 3) {
                return true;
            }
        }
        for (int i = 0; i < 3; ++i) {
            for (int j = i + 1; j < 3; ++j) {
                const int x = candidate.point[i];
                const int y = candidate.point[j];
                if (x < old_introduced && y < old_introduced &&
                    paired_[x][y]) {
                    return true;
                }
            }
        }
        return false;
    }
    void check_c8_witness(const Block& candidate) {
        std::array<int, 3> id{};
        for (int& value : id) {
            value = read_byte(input_);
            if (value < 0 ||
                value >= static_cast<int>(blocks_.size())) {
                fail("C8 witness has an invalid block index");
            }
        }
        if (id[0] == id[1] || id[0] == id[2] ||
            id[1] == id[2]) {
            fail("C8 witness repeats an old block");
        }
        const std::array<int, 4> points{{
            intersection(candidate, blocks_[id[0]]),
            intersection(blocks_[id[0]], blocks_[id[1]]),
            intersection(blocks_[id[1]], blocks_[id[2]]),
            intersection(blocks_[id[2]], candidate)}};
        std::set<int> distinct(points.begin(), points.end());
        if (*distinct.begin() < 0 || distinct.size() != 4) {
            fail("invalid C8 witness");
        }
    }
    void check_c16_witness(const Block& candidate) {
        if ((flags_ & kFlagC16) == 0U) {
            fail("C16 record appears when C16 pruning is disabled");
        }
        const int pair_code = read_byte(input_);
        if (pair_code < 0 || pair_code > 2) {
            fail("C16 witness has an invalid endpoint code");
        }
        constexpr int endpoint[3][2] = {{0, 1}, {0, 2}, {1, 2}};
        const int start = candidate.point[endpoint[pair_code][0]];
        const int finish = candidate.point[endpoint[pair_code][1]];
        std::array<int, 7> ids{};
        std::set<int> old_blocks;
        for (int& id : ids) {
            id = read_byte(input_);
            if (id < 0 || id >= static_cast<int>(blocks_.size())) {
                fail("C16 witness has an invalid block index");
            }
            if (!old_blocks.insert(id).second) {
                fail("C16 witness repeats an old block");
            }
        }
        std::array<int, 8> points{};
        points[0] = start;
        for (int i = 0; i < 6; ++i) {
            points[i + 1] =
                intersection(blocks_[ids[i]], blocks_[ids[i + 1]]);
        }
        points[7] = finish;
        if (std::find(std::begin(blocks_[ids[0]].point),
                      std::end(blocks_[ids[0]].point), start) ==
            std::end(blocks_[ids[0]].point)) {
            fail("C16 witness does not begin at its stated endpoint");
        }
        if (std::find(std::begin(blocks_[ids[6]].point),
                      std::end(blocks_[ids[6]].point), finish) ==
            std::end(blocks_[ids[6]].point)) {
            fail("C16 witness does not end at its stated endpoint");
        }
        std::set<int> distinct(points.begin(), points.end());
        if (*distinct.begin() < 0 || distinct.size() != 8) {
            fail("C16 witness is not a simple length-14 path");
        }
    }
    void visit(int point, int last_q, int last_r) {
        ++counts_.states;
        while (point < introduced_ && degree_[point] == 3) {
            ++point;
            last_q = -1;
            last_r = -1;
        }
        if (point >= introduced_) {
            if (static_cast<int>(blocks_.size()) == introduced_) {
                ++counts_.completions;
                fail("certificate expands to a complete configuration");
            }
            return;
        }
        if (static_cast<int>(blocks_.size()) >= side_) {
            return;
        }
        const int old_introduced = introduced_;
        std::vector<int> possible;
        for (int q = point + 1; q < old_introduced; ++q) {
            if (degree_[q] < 3 && !paired_[point][q]) {
                possible.push_back(q);
            }
        }
        if (old_introduced < side_) {
            possible.push_back(old_introduced);
        }
        if (old_introduced + 1 < side_) {
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
                    (q < last_q ||
                     (q == last_q && r <= last_r))) {
                    continue;
                }
                ++counts_.attempted;
                const Block candidate{{point, q, r}};
                if (structurally_invalid(candidate,
                                         old_introduced)) {
                    ++counts_.structural;
                    continue;
                }
                const std::uint8_t record = read_byte(input_);
                if (record == kRejectC8) {
                    check_c8_witness(candidate);
                    ++counts_.c8;
                    continue;
                }
                if (record == kRejectC16) {
                    check_c16_witness(candidate);
                    ++counts_.c16;
                    continue;
                }
                if (record != kExpand) {
                    fail("unknown certificate record");
                }
                int next_introduced = old_introduced;
                if (q == old_introduced ||
                    r == old_introduced) {
                    ++next_introduced;
                }
                if (r == old_introduced + 1) {
                    ++next_introduced;
                }
                const int saved_introduced = introduced_;
                introduced_ = next_introduced;
                install(candidate);
                visit(point, q, r);
                uninstall(candidate);
                introduced_ = saved_introduced;
            }
        }
    }
};
void print_counts(const Counts& counts) {
    std::cout << "states " << counts.states << '\n'
              << "attempted " << counts.attempted << '\n'
              << "structural " << counts.structural << '\n'
              << "C8 " << counts.c8 << '\n'
              << "C16 " << counts.c16 << '\n'
              << "expanded " << (counts.states - 1) << '\n'
              << "solutions " << counts.completions << '\n';
}

}  // namespace
int main(int argc, char** argv) {
    try {
        if (argc != 2) {
            fail("usage: verify_triangle_certificate CERTFILE|-");
        }
        std::ifstream file;
        std::istream* input = &std::cin;
        if (std::string(argv[1]) != "-") {
            file.open(argv[1], std::ios::binary);
            if (!file) {
                fail("cannot open certificate");
            }
            input = &file;
        }
        const Header header = read_header(*input);
        CertificateChecker checker(*input, header);
        const Counts actual = checker.check();
        if (!counts_equal(actual, header.claimed)) {
            fail("header counts do not match the reconstructed proof");
        }
        if (input->get() != std::char_traits<char>::eof()) {
            fail("trailing bytes after proof stream");
        }
        if (actual.completions != 0) {
            fail("certificate contains a completion");
        }
        std::cout << "VERIFIED EG70TRI1-UNIVERSAL"
                  << " side=" << header.side
                  << " orbit=" << header.orbit << '\n';
        print_counts(actual);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "VERIFICATION FAILED: "
                  << error.what() << '\n';
        return 1;
    }
}
