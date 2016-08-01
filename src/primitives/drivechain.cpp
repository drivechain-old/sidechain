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
    if (drivechainop == 'I')
        ret = SerializeHash(*(drivechainIncoming *) this);
    else
    if (drivechainop == 'O')
        ret = SerializeHash(*(drivechainOutgoing *) this);

    return ret;
}

CScript drivechainObj::GetScript(void) const
{
    CDataStream ds(SER_DISK, CLIENT_VERSION);

    if (drivechainop == 'I')
        ((drivechainIncoming *) this)->Serialize(ds, nType, nVersion);
    else
    if (drivechainop == 'O')
        ((drivechainOutgoing *) this)->Serialize(ds, nType, nVersion);

    CScript script;
    script << vector<unsigned char>(ds.begin(), ds.end()) << OP_RETURN; // TODO OP_DRIVECHAIN
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

    if (*vchOP == 'I') {
        drivechainIncoming *obj = new drivechainIncoming;
        obj->Unserialize(ds, nType, nVersion);
        return obj;
    }
    else
    if (*vchOP == 'O') {
        drivechainOutgoing *obj = new drivechainOutgoing;
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

string drivechainIncoming::ToString() const
{
    stringstream str;
    str << "drivechainop=" << drivechainop << endl;
    str << "nHeight=" << nHeight << endl;
    str << "txid=" << txid.GetHex() << endl;
    return str.str();
}

string drivechainOutgoing::ToString() const
{
    stringstream str;
    str << "drivechainop=" << drivechainop << endl;
    str << "nHeight=" << nHeight << endl;
    str << "txid=" << txid.GetHex() << endl;
    return str.str();
}
