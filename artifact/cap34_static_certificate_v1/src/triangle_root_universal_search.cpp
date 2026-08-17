#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "transcript_sha256.hpp"
#ifndef SIDE
#define SIDE 29
#endif
namespace triangle_discovery {
constexpr int V = SIDE;
struct Block { int a, b, c; };
struct Counts {
    std::uint64_t states=0, attempted=0, pair=0, c8=0, c16=0, completions=0;
    Counts& operator+=(const Counts& x) {
        states+=x.states; attempted+=x.attempted; pair+=x.pair;
        c8+=x.c8; c16+=x.c16; completions+=x.completions; return *this;
    }
};
struct Branch {
    std::vector<Block> blocks;
    std::vector<int> incident[V];
    int degree[V]{};
    bool paired[V][V]{};
    int introduced=6;
    Counts count;
    transcript_audit::SearchTranscript transcript;
    static int unique_intersection(const Block& x, const Block& y) {
        int value=-1, number=0;
        int a[3]={x.a,x.b,x.c}, b[3]={y.a,y.b,y.c};
        for (int i=0;i<3;i++) for (int j=0;j<3;j++) if (a[i]==b[j]) {
            value=a[i]; ++number;
        }
        return number==1 ? value : (number==0 ? -1 : -2);
    }
    bool creates_C8(const Block& t) const {
        const int k=static_cast<int>(blocks.size());
        for (int i=0;i<k;i++) {
            int p0=unique_intersection(t,blocks[i]); if (p0<0) continue;
            for (int j=0;j<k;j++) if (j!=i) {
                int p1=unique_intersection(blocks[i],blocks[j]);
                if (p1<0 || p1==p0) continue;
                for (int h=0;h<k;h++) if (h!=i && h!=j) {
                    int p2=unique_intersection(blocks[j],blocks[h]);
                    if (p2<0 || p2==p0 || p2==p1) continue;
                    int p3=unique_intersection(blocks[h],t);
                    if (p3<0 || p3==p0 || p3==p1 || p3==p2) continue;
                    return true;
                }
            }
        }
        return false;
    }
    bool exact_path(int current, int target, int depth,
                    std::array<unsigned char,2*V>& seen) const {
        constexpr int needed=14; // C16 minus the two incidences of the new block
        if (depth==needed) return current==target;
        if (current==target) return false;
        if (current<V) {
            for (int id:incident[current]) {
                int next=V+id; if (seen[next]) continue;
                seen[next]=1;
                if (exact_path(next,target,depth+1,seen)) return true;
                seen[next]=0;
            }
        } else {
            const Block& e=blocks[current-V];
            int points[3]={e.a,e.b,e.c};
            for (int next:points) {
                if (seen[next]) continue;
                if (next==target && depth+1!=needed) continue;
                seen[next]=1;
                if (exact_path(next,target,depth+1,seen)) return true;
                seen[next]=0;
            }
        }
        return false;
    }
    bool creates_C16(const Block& t) const {
        int points[3]={t.a,t.b,t.c};
        for (int i=0;i<3;i++) for (int j=i+1;j<3;j++) {
            std::array<unsigned char,2*V> seen{};
            seen[points[i]]=1;
            if (exact_path(points[i],points[j],0,seen)) return true;
        }
        return false;
    }
    void add(Block e) {
        int id=static_cast<int>(blocks.size()); blocks.push_back(e);
        int x[3]={e.a,e.b,e.c};
        for (int p:x) { ++degree[p]; incident[p].push_back(id); }
        for (int i=0;i<3;i++) for (int j=i+1;j<3;j++)
            paired[x[i]][x[j]]=paired[x[j]][x[i]]=true;
    }
    void remove(Block e) {
        int x[3]={e.a,e.b,e.c};
        for (int p:x) { --degree[p]; incident[p].pop_back(); }
        for (int i=0;i<3;i++) for (int j=i+1;j<3;j++)
            paired[x[i]][x[j]]=paired[x[j]][x[i]]=false;
        blocks.pop_back();
    }
    void recurse(int point, int last_q, int last_r) {
        ++count.states;
        while (point<introduced && degree[point]==3) {
            ++point; last_q=last_r=-1;
        }
        transcript.state(
            static_cast<std::uint8_t>(point),
            static_cast<std::uint8_t>(introduced),
            last_q,last_r,static_cast<std::uint8_t>(blocks.size()));
        for (const Block& block:blocks) {
            transcript.block(
                static_cast<std::uint8_t>(block.a),
                static_cast<std::uint8_t>(block.b),
                static_cast<std::uint8_t>(block.c));
        }
        if (point>=introduced) {
            if (static_cast<int>(blocks.size())==introduced) {
                ++count.completions;
                transcript.terminal(0);
            } else {
                transcript.terminal(2);
            }
            transcript.leave();
            return;
        }
        if (static_cast<int>(blocks.size())>=V) {
            transcript.terminal(3);
            transcript.leave();
            return;
        }
        const int old=introduced;
        std::vector<int> possible;
        for (int q=point+1;q<old;q++)
            if (degree[q]<3 && !paired[point][q]) possible.push_back(q);
        if (old<V) possible.push_back(old);
        if (old+1<V) possible.push_back(old+1);
        for (std::size_t i=0;i<possible.size();i++) for (std::size_t j=i+1;j<possible.size();j++) {
            int q=possible[i], r=possible[j];
            if (r==old+1 && q!=old) continue;
            if (last_q>=0 && (q<last_q || (q==last_q && r<=last_r))) continue;
            ++count.attempted;
            Block e{point,q,r};
            bool invalid=false; int x[3]={point,q,r};
            for (int p:x) if (p<old && degree[p]>=3) invalid=true;
            for (int a=0;a<3;a++) for (int b=a+1;b<3;b++)
                if (x[a]<old && x[b]<old && paired[x[a]][x[b]]) invalid=true;
            if (invalid) {
                ++count.pair;
                transcript.candidate(
                    static_cast<std::uint8_t>(point),
                    static_cast<std::uint8_t>(q),
                    static_cast<std::uint8_t>(r),
                    transcript_audit::Outcome::degree_or_pair);
                continue;
            }
            if (creates_C8(e)) {
                ++count.c8;
                transcript.candidate(
                    static_cast<std::uint8_t>(point),
                    static_cast<std::uint8_t>(q),
                    static_cast<std::uint8_t>(r),
                    transcript_audit::Outcome::c8);
                continue;
            }
            if (creates_C16(e)) {
                ++count.c16;
                transcript.candidate(
                    static_cast<std::uint8_t>(point),
                    static_cast<std::uint8_t>(q),
                    static_cast<std::uint8_t>(r),
                    transcript_audit::Outcome::c16);
                continue;
            }
            transcript.candidate(
                static_cast<std::uint8_t>(point),
                static_cast<std::uint8_t>(q),
                static_cast<std::uint8_t>(r),
                transcript_audit::Outcome::accepted);
            int next=old;
            if (q==old || r==old) ++next;
            if (r==old+1) ++next;
            int saved=introduced; introduced=next;
            add(e); recurse(point,q,r); remove(e);
            introduced=saved;
        }
        transcript.leave();
    }
    Counts run_orbit(int orbit) {
        transcript.begin(static_cast<std::uint8_t>(V),
                         static_cast<std::uint8_t>(orbit));
        // Normalize a Berge triangle with intersection points 0,1,2
        // and private points 3,4,5.
        add({0,1,3}); add({1,2,4}); add({0,2,5});
        Block first{};
        if (orbit==1) {
            if (V<7) {
                transcript.terminal(4);
                transcript.leave();
                return count;
            }
            first={0,4,6}; introduced=7;
        } else if (orbit==2) {
            if (V<8) {
                transcript.terminal(4);
                transcript.leave();
                return count;
            }
            first={0,6,7}; introduced=8;
        } else {
            transcript.terminal(4);
            transcript.leave();
            return count;
        }
        add(first);
        // Point 0 is now complete.  The preinserted block {1,2,4}
        // is the lexicographically first block through 1 whose least
        // point is 1.
        recurse(1,2,4);
        return count;
    }
    Counts run_unreduced_root() {
        transcript.begin(static_cast<std::uint8_t>(V),0);
        add({0,1,3}); add({1,2,4}); add({0,2,5});
        recurse(0,-1,-1);
        return count;
    }
    bool transcript_matches_counts() const {
        return transcript.states()==count.states &&
               transcript.candidates()==count.attempted;
    }
};
}
int main(int argc, char** argv) {
    int first_orbit=1, last_orbit=2;
    const bool unreduced_root =
        argc==2 && std::strcmp(argv[1],"--unreduced-root")==0;
    if (argc==2) {
        if (!unreduced_root) {
            const int requested=std::atoi(argv[1]);
            if (requested<1 || requested>2) {
                std::fprintf(stderr,
                    "usage: %s [orbit: 1|2 | --unreduced-root]\n",argv[0]);
                return 64;
            }
            first_orbit=last_orbit=requested;
        }
    } else if (argc!=1) {
        std::fprintf(stderr,
            "usage: %s [orbit: 1|2 | --unreduced-root]\n",argv[0]);
        return 64;
    }
    const auto start=std::chrono::steady_clock::now();
    triangle_discovery::Counts total;
    if (unreduced_root) {
        triangle_discovery::Branch branch;
        total=branch.run_unreduced_root();
        if (!branch.transcript_matches_counts()) {
            std::fprintf(stderr,"transcript counter mismatch\n");
            return 70;
        }
        std::printf("UNREDUCED_ROOT states=%llu attempted=%llu pair=%llu c8=%llu c16=%llu completions=%llu\n",
          (unsigned long long)total.states,(unsigned long long)total.attempted,
          (unsigned long long)total.pair,(unsigned long long)total.c8,
          (unsigned long long)total.c16,(unsigned long long)total.completions);
        std::printf("TRANSCRIPT orbit=0 states=%llu candidates=%llu sha256=%s\n",
          (unsigned long long)branch.transcript.states(),
          (unsigned long long)branch.transcript.candidates(),
          branch.transcript.hex_digest().c_str());
        std::fflush(stdout);
    } else for (int orbit=first_orbit;orbit<=last_orbit;orbit++) {
        triangle_discovery::Branch branch;
        auto c=branch.run_orbit(orbit); total+=c;
        if (!branch.transcript_matches_counts()) {
            std::fprintf(stderr,"transcript counter mismatch in orbit %d\n",orbit);
            return 70;
        }
        std::printf("ORBIT %d states=%llu attempted=%llu pair=%llu c8=%llu c16=%llu completions=%llu\n",
          orbit,(unsigned long long)c.states,(unsigned long long)c.attempted,
          (unsigned long long)c.pair,(unsigned long long)c.c8,(unsigned long long)c.c16,
          (unsigned long long)c.completions);
        std::printf("TRANSCRIPT orbit=%d states=%llu candidates=%llu sha256=%s\n",
          orbit,(unsigned long long)branch.transcript.states(),
          (unsigned long long)branch.transcript.candidates(),
          branch.transcript.hex_digest().c_str());
        std::fflush(stdout);
    }
    double sec=std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();
    std::printf("TOTAL V=%d states=%llu attempted=%llu pair=%llu c8=%llu c16=%llu completions=%llu seconds=%.6f\n",
      triangle_discovery::V,(unsigned long long)total.states,(unsigned long long)total.attempted,
      (unsigned long long)total.pair,(unsigned long long)total.c8,(unsigned long long)total.c16,
      (unsigned long long)total.completions,sec);
    return total.completions==0 ? 0 : 2;
}
