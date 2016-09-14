// Copyright (c) 2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "primitives/drivechain.h"

#include "clientversion.h"
#include "hash.h"
#include "streams.h"

#include <sstream>
#include <vector>

const uint32_t nType = 1;
const uint32_t nVersion = 1;

uint256 drivechainObj::GetHash(void) const
{
    uint256 ret;
    if (drivechainop == 'D')
        ret = SerializeHash(*(drivechainDeposit *) this);
    else
    if (drivechainop == 'W')
        ret = SerializeHash(*(drivechainWithdraw *) this);
    else
    if (drivechainop == 'J')
        ret = SerializeHash(*(drivechainJoinedWT *) this);

    return ret;
}

CScript drivechainObj::GetScript(void) const
{
    CDataStream ds(SER_DISK, CLIENT_VERSION);

    if (drivechainop == 'D')
        ((drivechainDeposit *) this)->Serialize(ds, nType, nVersion);
    else
    if (drivechainop == 'W')
        ((drivechainWithdraw *) this)->Serialize(ds, nType, nVersion);
    else
    if (drivechainop == 'J')
        ((drivechainJoinedWT *) this)->Serialize(ds, nType, nVersion);

    CScript script;
    script << vector<unsigned char>(ds.begin(), ds.end()) << OP_DRIVECHAIN;
    return script;
}

drivechainObj *drivechainObjCtr(const CScript &script)
{
    CScript::const_iterator pc = script.begin();
    vector<unsigned char> vch;
    opcodetype opcode;

    if (!script.GetOp(pc, opcode, vch))
        return NULL;
    if (vch.size() == 0)
        return NULL;
    const char *vchOP = (const char *) &vch.begin()[0];
    CDataStream ds(vchOP, vchOP+vch.size(), SER_DISK, CLIENT_VERSION);

    if (*vchOP == 'D') {
        drivechainDeposit *obj = new drivechainDeposit;
        obj->Unserialize(ds, nType, nVersion);
        return obj;
    }
    else
    if (*vchOP == 'W') {
        drivechainWithdraw *obj = new drivechainWithdraw;
        obj->Unserialize(ds, nType, nVersion);
        return obj;
    }
    else
    if (*vchOP == 'J') {
        drivechainJoinedWT *obj = new drivechainJoinedWT;
        obj->Unserialize(ds, nType, nVersion);
        return obj;
    }

    return NULL;
}

string drivechainObj::ToString(void) const
{
    stringstream str;
    str << "drivechainop=" << drivechainop << endl;
    str << "nHeight=" << nHeight << endl;
    str << "txid=" << txid.GetHex() << endl;
    return str.str();
}

string drivechainDeposit::ToString() const
{
    stringstream str;
    str << "drivechainop=" << drivechainop << endl;
    str << "nHeight=" << nHeight << endl;
    str << "txid=" << txid.GetHex() << endl;
    return str.str();
}

string drivechainWithdraw::ToString() const
{
    stringstream str;
    str << "drivechainop=" << drivechainop << endl;
    str << "nHeight=" << nHeight << endl;
    str << "txid=" << txid.GetHex() << endl;
    return str.str();
}

string drivechainJoinedWT::ToString() const
{
    stringstream str;
    str << "drivechainop=" << drivechainop << endl;
    str << "nHeight=" << nHeight << endl;
    str << "txid=" << txid.GetHex() << endl;
    return str.str();
}
