#ifndef SUBO_BRANCH_MIX_H
#define SUBO_BRANCH_MIX_H

#include "uint256.h"

// Argon2d's memory-hardness (see argon2_scramble.h) already hurts GPU SIMT
// throughput because the *addresses* it reads depend on prior data, so
// different lanes in a warp/wavefront wander off to different memory
// locations. This step goes further and makes *which instruction runs*
// depend on prior data too: on a GPU, every thread in a warp is normally
// forced to execute in lockstep - if thread A takes an XOR and thread B
// (working on a different nonce, hence different running state) needs a
// SUBTRACT at the same point, the warp can't do both at once. It has to
// run every branch that *any* thread in the warp wants, masking off the
// threads that don't need it, and add up the cost of all of them. A CPU
// core just executes its own branch and moves on - no such penalty.
//
// Deliberately built from plain add/sub/xor/rotate, not from anything with
// dedicated hardware (like AES-NI) that would give one platform a
// structural edge over another.
inline void BranchMix(uint512& state)
{
    static const int nRounds = 128;
    static const int nWords = 8; // 8 * 64 bits = 512

    uint64_t w[nWords];
    for (int k = 0; k < nWords; k++)
        w[k] = state.GetUint64(k);

    for (int round = 0; round < nRounds; round++) {
        const int i = round % nWords;
        const int j = (round + 3) % nWords; // fixed offset, distinct from i for every round
        const int branch = (int)(w[i] & 0x7);

        switch (branch) {
            case 0: w[i] ^= w[j]; break;
            case 1: w[i] += w[j]; break;
            case 2: w[i] -= w[j]; break;
            case 3: w[i] = (w[i] << 13) | (w[i] >> (64 - 13)); break;
            case 4: w[i] ^= (w[j] << 7) | (w[j] >> (64 - 7)); break;
            case 5: w[i] += (w[j] ^ (uint64_t)round); break;
            case 6: w[i] = (w[i] ^ w[j]) + (uint64_t)round; break;
            default: w[i] -= (w[j] << 3) | (w[j] >> (64 - 3)); break;
        }
    }

    unsigned char* out = state.begin();
    for (int k = 0; k < nWords; k++) {
        uint64_t v = w[k];
        for (int b = 0; b < 8; b++) {
            out[k * 8 + b] = (unsigned char)(v & 0xff);
            v >>= 8;
        }
    }
}

#endif // SUBO_BRANCH_MIX_H
