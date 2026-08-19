#include "argon2_scramble.h"
#include "argon2/argon2.h"

void MemoryHardScramble(const uint512& input, uint512& output)
{
    // Argon2d is data-dependent (memory addresses depend on prior data),
    // which causes divergent, unpredictable access patterns per lane -
    // this is cheap on a CPU core but hostile to GPU SIMT execution
    // (warp/wavefront threads diverge) and to ASIC pipelining.
    //
    // m_cost is deliberately large (256 MiB) so the working set exceeds
    // the L2/L3 cache of essentially all GPU cores and cannot be cheaply
    // replicated as on-die SRAM in an ASIC: an attacker is forced to pay
    // for one full DRAM-class memory channel *per parallel hash attempt*,
    // which collapses the parallelism advantage GPUs/ASICs normally have
    // over general-purpose CPUs (which already have abundant DRAM).
    //
    // parallelism is kept at 1 so the memory-hard step cannot itself be
    // split across independent lanes/cores within a single hash attempt.
    //
    // NOTE: these are consensus parameters. Changing them changes every
    // hash produced by HashSubo(), including the genesis block hash for
    // every network - mainnet/testnet/regtest genesis blocks must be
    // re-mined and the hardcoded hashes in chainparams.cpp updated before
    // any node built with this change can start.
    static const uint32_t t_cost = 2;
    static const uint32_t m_cost = 262144; // 256 MiB (KiB units)
    static const uint32_t parallelism = 1;
    static const unsigned char salt[12] = {'s','u','b','o','s','c','r','a','m','b','l','e'};

    argon2d_hash_raw(t_cost, m_cost, parallelism,
                     static_cast<const void*>(&input), 64,
                     salt, sizeof(salt),
                     static_cast<void*>(&output), 64);
}
