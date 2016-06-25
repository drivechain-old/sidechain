Copy of Bitcoin Core -- Sidechain ("Child" Half)
===================================================
See [the 'mainchain' repository in  this group](https://github.com/drivechain-project/mainchain) for the parent chain modifications.

The parent chain is where Coins would typically reside. Their origin is, as always, a Bitcoin coinbase transaction. From there, they are transferred (from the parent "mainchain") to the child "sidechain", where they are used in some way. Ultimately, they return to the parent mainchain.

The child chain is where developers and users can experiment with new features on an "opt in" basis. Each child chain is mostly the same as the mainchain, with the following exceptions:

1. The Sidechain is [merged mined](https://en.bitcoin.it/wiki/Merged_mining_specification) (with the Mainchain as PoW-Parent).
2. In order to transfer BTC main-to-side, the Sidechain must perform two operations:
  * Watch the Mainchain for special transactions (using "listreceivedbyaddress" call upon the finding of each block, or similar).
  * Credit those balances to kepairs ("users") on the sidechain.
3. In order to transfer BTC side-to-main, the Sidechain must perform several operations:
  * Must maintain a database of withdrawals (the "WT"s, on the sidechain).
  * Must maintain a database of the mainchains's origin-transactions.
  * Must aggregate the withdrawals in a predefined way, such that a valid mainchain withdrawal transaction (the "WT^") is created. Although this tx is "constructed" on the sidechain, it is a mainchain tx.
  * Must include this WT^ transaction in a block header (eg, if block cycles were of length="10 blocks": block #21 would contain WT-txns for blocks 9 through 19, block #31 would contain WT-txns for blocks 19 through 29, ...).
  * Must watch the mainchain to see if the withdrawal actually succeeded (this can be done by observing changes to the database of origin-transactions). If it does not succeed, the those failed withdrawals must be "rolled" into the next WT^.


This project aims to demo a key feature of sidechains: the "test a new feature". We do this by intentionally introducing a new feature, OP_BlockDivTen, which is explicitly pointless. It prevents txns from being valid unless they are included in a block number which is a multiple of 10.

In the future, we will probably try to implement a second 'new feature', OP_RockPaperScissors, so that we can test features which cause fund-ownership to change. This would open the door for more complex Hash-Locked-Contracts / Lightning Network, etc.


License
-------

Everything here is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.
