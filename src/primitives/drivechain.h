// Copyright (c) 2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef DRIVECHAIN_H
#define DRIVECHAIN_H

#include "coins.h"
#include "pubkey.h"
#include "script/script.h"
#include "serialize.h"
#include "uint256.h"
#include "utilstrencodings.h"

#include <limits.h>
#include <string>

using namespace std;

// TODO move to chainparams?

// uint256 hash (sidechainid) of THIS sidechain
static const uint256 SIDECHAIN_ID = uint256S("0xca85db47c45dfccfa9f5562f7383c7b3fe1746017327371771ed3f70345b72d4");

// Sidechain deposit script (used by mainchain to lock funds to this chain)
static const CScript SIDECHAIN_DEPOSITSCRIPT = CScript() << OP_TRUE; // TODO UPDATE ON MAIN & HERE

// mnPGuDbviWX4xR72mUXqVAJJUjj68Vh7eG
// cQv4JP3XDivx3CjoSoe1dw2KtGr1KqLn7yLAe7gGK5ScaUedP6MS
// "0201060ed986a0dda4caa2ed26a3d6c26f604811895a6ed3459fe3b5e3cec99b23"
static const CScript SIDECHAIN_FEESCRIPT = CScript() << OP_DUP << OP_HASH160 << ParseHex("0201060ed986a0dda4caa2ed26a3d6c26f604811895a6ed3459fe3b5e3cec99b23") << OP_EQUALVERIFY << OP_CHECKSIG;

// The number of blocks between WT^ creations (during this period wt's are submitted)
static const uint32_t SIDECHAIN_TAU = 400;

/**
 * Drivechain object for database
 */
struct drivechainObj {
    char drivechainop;
    uint32_t nHeight;
    uint256 txid;

    drivechainObj(void): nHeight(INT_MAX) { }
    virtual ~drivechainObj(void) { }

    uint256 GetHash(void) const;
    CScript GetScript(void) const;
    virtual string ToString(void) const;
};

/**
 * Create drivechain object
 */
drivechainObj *drivechainObjCtr(const CScript &script);

/**
 * Drivechain deposit from mainchain
 */
struct drivechainDeposit : public drivechainObj {
    uint256 sidechainid;
    CKeyID keyID;
    CTransaction deposit;

    drivechainDeposit(void) : drivechainObj() { drivechainop = 'D'; }
    virtual ~drivechainDeposit(void) { }

    ADD_SERIALIZE_METHODS

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(drivechainop);
        READWRITE(sidechainid);
        READWRITE(keyID);
        READWRITE(deposit);
    }

    string ToString(void) const;
};

/**
 * An individual drivechain withdraw to mainchain (wt)
 */
struct drivechainWithdraw : public drivechainObj {
    uint256 sidechainid;
    CKeyID keyID;

    drivechainWithdraw(void) : drivechainObj() { drivechainop = 'W'; }
    virtual ~drivechainWithdraw(void) { }

    ADD_SERIALIZE_METHODS

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(drivechainop);
        READWRITE(sidechainid);
        READWRITE(keyID);
    }

    string ToString(void) const;
};

/**
 * Joined drivechain withdraw(s) to mainchain (WT^)
 */
struct drivechainJoinedWT: public drivechainObj {
    uint256 sidechainid;
    CTransaction wtJoined;

    drivechainJoinedWT(void) : drivechainObj() { drivechainop = 'J'; }
    virtual ~drivechainJoinedWT(void) { }

    ADD_SERIALIZE_METHODS

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(drivechainop);
        READWRITE(sidechainid);
        READWRITE(wtJoined);
    }

    string ToString(void) const;
};

#endif // DRIVECHAIN_H
