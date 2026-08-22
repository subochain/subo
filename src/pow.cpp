// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "pow.h"

#include "arith_uint256.h"
#include "chain.h"
#include "chainparams.h"
#include "primitives/block.h"
#include "uint256.h"
#include "util.h"

#include <algorithm>
#include <math.h>

unsigned int static SuboRetarget(const CBlockIndex* pindexLast, const CBlockHeader* pblock, const Consensus::Params& params)
{
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);

    // regtest only: always mine at a fixed, minimum difficulty for test tooling.
    if (params.fPowNoRetargeting)
        return pindexLast ? pindexLast->nBits : bnPowLimit.GetCompact();

    // Bootstrap: no history yet, mine at the easiest possible target.
    if (!pindexLast || pindexLast->nHeight < 1)
        return bnPowLimit.GetCompact();

    // --- base retarget: short weighted-average window ----------------------
    // The old DarkGravityWave used a 24-block window with up to a 3x swing
    // per step. That's fine for a hash that solves in milliseconds, but
    // HashSubo's 32 MiB Argon2d step means realistic hashrate for a single
    // miner is more like tens of H/s - a handful of easy early blocks can
    // push the target 3x harder on every following block for a dozen-plus
    // blocks before it finds the real equilibrium, and nothing can correct
    // an overshoot faster than "wait for the next block to be found", which
    // may not happen for a very long time once it overshoots the real
    // hashrate by a couple of orders of magnitude. A much shorter window
    // settles in a handful of blocks instead of dozens, and the emergency
    // easing below is the real backstop if it still overshoots.
    static const int64_t nWindow = 6;
    int64_t nBlocks = std::min<int64_t>(nWindow, pindexLast->nHeight);

    // powLimit here is ~2^255 (deliberately huge so genesis mining under
    // Argon2 stays cheap - see chainparams.cpp) - about as large as an
    // arith_uint256 can hold. That leaves essentially no headroom for the
    // classic DGW-style running average (bnAvg*i + bnTarget)/(i+1): with
    // bnAvg near powLimit, multiplying by i overflows the 256-bit type and
    // wraps to a effectively-arbitrary value, silently producing a wildly
    // wrong (often far too hard) target instead of an error. Use the
    // equivalent incremental-mean form instead (avg += (x-avg)/n), which
    // only ever adds/subtracts/divides - never multiplies a near-max value
    // - so it can't overflow regardless of how close to powLimit it is.
    const CBlockIndex* pindex = pindexLast;
    arith_uint256 bnPastTargetAvg;
    for (int64_t i = 1; i <= nBlocks; i++) {
        arith_uint256 bnTarget = arith_uint256().SetCompact(pindex->nBits);
        if (i == 1) {
            bnPastTargetAvg = bnTarget;
        } else if (bnTarget >= bnPastTargetAvg) {
            bnPastTargetAvg += (bnTarget - bnPastTargetAvg) / (i + 1);
        } else {
            bnPastTargetAvg -= (bnPastTargetAvg - bnTarget) / (i + 1);
        }
        if (i != nBlocks) pindex = pindex->pprev;
    }

    int64_t nActualTimespan = pindexLast->GetBlockTime() - pindex->GetBlockTime();
    int64_t nTargetTimespan = nBlocks * params.nPowTargetSpacing;

    // Tighter per-step clamp than the old 1/3..3x, so a burst of easy
    // blocks can't overshoot the real equilibrium by orders of magnitude
    // before there's been a chance to feel out the real hashrate.
    if (nActualTimespan < nTargetTimespan / 2)
        nActualTimespan = nTargetTimespan / 2;
    if (nActualTimespan > nTargetTimespan * 2)
        nActualTimespan = nTargetTimespan * 2;

    // Same overflow hazard as above: dividing first keeps the intermediate
    // value well clear of the 256-bit ceiling before scaling it back up,
    // where multiplying first could overflow when bnPastTargetAvg is close
    // to powLimit (losing a few low bits of precision here is irrelevant
    // for a security parameter this coarse).
    arith_uint256 bnNew = bnPastTargetAvg;
    bnNew /= nTargetTimespan;
    bnNew *= nActualTimespan;

    // --- emergency easing: don't make miners wait out an overshoot --------
    // Everything above can only react once a block has actually been
    // found. If the chain is stuck because a retarget overshot, no amount
    // of block history fixes that by itself - so also look at how long
    // it's been since the tip, using the *candidate* block's own
    // timestamp. Every node re-derives that same timestamp identically
    // once a block claiming it is actually mined, so this stays fully
    // deterministic and consensus-safe; it isn't "wall clock right now" on
    // whichever node happens to be checking. A getblocktemplate call
    // refreshes this timestamp to the current time on every request (see
    // CreateNewBlock()), so a miner polling for work sees the eased target
    // directly, without needing to know anything changed.
    if (pblock) {
        static const int64_t nGraceSeconds = 180; // 3 minutes
        int64_t nGap = (int64_t)pblock->nTime - pindexLast->GetBlockTime();
        if (nGap > nGraceSeconds) {
            int64_t nOverdue = nGap - nGraceSeconds;
            int64_t nHalvings = nOverdue / params.nPowTargetSpacing + 1;
            if (nHalvings > 32) nHalvings = 32; // plenty to reach powLimit well before this
            // One bit at a time, bailing out to powLimit the moment another
            // doubling would meet or exceed it - <<= nHalvings directly
            // would overflow (and silently wrap to something wrong rather
            // than clamp) once bnNew gets close to the 256-bit ceiling.
            for (int64_t s = 0; s < nHalvings; s++) {
                if (bnNew >= (bnPowLimit >> 1)) { bnNew = bnPowLimit; break; }
                bnNew <<= 1;
            }
        }
    }

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    return SuboRetarget(pindexLast, pblock, params);
}

// for DIFF_BTC only!
unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    if (params.fPowNoRetargeting)
        return pindexLast->nBits;

    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    LogPrintf("  nActualTimespan = %d  before bounds\n", nActualTimespan);
    if (nActualTimespan < params.nPowTargetTimespan/4)
        nActualTimespan = params.nPowTargetTimespan/4;
    if (nActualTimespan > params.nPowTargetTimespan*4)
        nActualTimespan = params.nPowTargetTimespan*4;

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnNew;
    arith_uint256 bnOld;
    bnNew.SetCompact(pindexLast->nBits);
    bnOld = bnNew;
    bnNew *= nActualTimespan;
    bnNew /= params.nPowTargetTimespan;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    /// debug print
    LogPrintf("GetNextWorkRequired RETARGET\n");
    LogPrintf("params.nPowTargetTimespan = %d    nActualTimespan = %d\n", params.nPowTargetTimespan, nActualTimespan);
    LogPrintf("Before: %08x  %s\n", pindexLast->nBits, bnOld.ToString());
    LogPrintf("After:  %08x  %s\n", bnNew.GetCompact(), bnNew.ToString());

    return bnNew.GetCompact();
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
        return error("CheckProofOfWork(): nBits below minimum work");

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return error("CheckProofOfWork(): hash doesn't match nBits");

    return true;
}

arith_uint256 GetBlockProof(const CBlockIndex& block)
{
    arith_uint256 bnTarget;
    bool fNegative;
    bool fOverflow;
    bnTarget.SetCompact(block.nBits, &fNegative, &fOverflow);
    if (fNegative || fOverflow || bnTarget == 0)
        return 0;
    // We need to compute 2**256 / (bnTarget+1), but we can't represent 2**256
    // as it's too large for a arith_uint256. However, as 2**256 is at least as large
    // as bnTarget+1, it is equal to ((2**256 - bnTarget - 1) / (bnTarget+1)) + 1,
    // or ~bnTarget / (nTarget+1) + 1.
    return (~bnTarget / (bnTarget + 1)) + 1;
}

int64_t GetBlockProofEquivalentTime(const CBlockIndex& to, const CBlockIndex& from, const CBlockIndex& tip, const Consensus::Params& params)
{
    arith_uint256 r;
    int sign = 1;
    if (to.nChainWork > from.nChainWork) {
        r = to.nChainWork - from.nChainWork;
    } else {
        r = from.nChainWork - to.nChainWork;
        sign = -1;
    }
    r = r * arith_uint256(params.nPowTargetSpacing) / GetBlockProof(tip);
    if (r.bits() > 63) {
        return sign * std::numeric_limits<int64_t>::max();
    }
    return sign * r.GetLow64();
}
