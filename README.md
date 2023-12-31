# BLST on Arbitrum Stylus

This repo defines a smart contract for [Arbitrum Stylus](https://arbitrum.io/stylus) which can verify Ethereum [BLS signatures](https://eth2book.info/capella/part2/building_blocks/signatures/) onchain. It uses the heavily optimized and robust [BLST](https://github.com/supranational/blst) project, written in C, compiles it to WebAssembly, and deploys it to the Stylus testnet for use.

## Building

### Requirements

- Python 3
- Clang 16
- Git
- Rust 1.72 (to use the Stylus CLI tool for deploying your program)

Next, initialize the submodules:

```
git submodule update --init
```

### Compiling to WASM

```
make wasm
```

This will output a `bls.wasm` file that is ready for an onchain deployment. It runs the file through a Python script that also strips it of some unnecessary information so that an onchain deployment will succeed.

A version of this contract is deployed to the Stylus testnet at address `0x832Dc14b36be855DAA49933E9CEFAd6e4C2Ebb3F`. You can interact with it using the [cast](https://github.com/foundry-rs/foundry) tool, which is part of the foundry Ethereum toolkit.

```
export ADDR=0x832Dc14b36be855DAA49933E9CEFAd6e4C2Ebb3F
cast call \
 --rpc-url 'https://stylus-testnet.arbitrum.io/rpc' \
 $ADDR \
 '0xa5acc7f57b7df6ade2b7630e09a925b2ef10fb8c977aa1656b526db0d02b3998055c74f74fc79034678c352ddf531591a388ba9227c6f4d08954d017956b3dd947e5a18a9df064d137417afa8e8809e848af1ad8e47d887820e86a6a50ea0ba001fa422935358c7e0eec86077613406e69953688490437408d08a6995ec57dfccbba0c0f2ce42e8d18359ac0148fc915666f6f626172'
```

This sends over a BLS public key, followed by a signature, followed by the message being signed as a single hex string. If no output is received, the call was successful.

### Compiling to Native

```
make host
```

This will produce a `main` executable in the current directory, which runs the same signature verification code in native mode for your host architecture.

```
./main a5acc7f57b7df6ade2b7630e09a925b2ef10fb8c977aa1656b526db0d02b3998055c74f74fc79034678c352ddf531591a388ba9227c6f4d08954d017956b3dd947e5a18a9df064d137417afa8e8809e848af1ad8e47d887820e86a6a50ea0ba001fa422935358c7e0eec86077613406e69953688490437408d08a6995ec57dfccbba0c0f2ce42e8d18359ac0148fc915666f6f626172
```

With the following output:
```
Public Key: a5acc7f57b7df6ade2b7630e09a925b2ef10fb8c977aa1656b526db0d02b3998055c74f74fc79034678c352ddf531591
Signature: a388ba9227c6f4d08954d017956b3dd947e5a18a9df064d137417afa8e8809e848af1ad8e47d887820e86a6a50ea0ba001fa422935358c7e0eec86077613406e69953688490437408d08a6995ec57dfccbba0c0f2ce42e8d18359ac0148fc915
Message: 666f6f626172
Verified sig
```

## Deploying

The easiest way to deploy your C or C++ program is to use the [Cargo Stylus CLI tool][cargo], which has a `--wasm-file-path` flag that accepts arbitrary WASMs.

```sh
cargo stylus deploy --wasm-file-path <wasm> --private-key <secret>
```

[cargo]: https://github.com/OffchainLabs/cargo-stylus

## Performance & Gas Costs

The gas cost for a single BLST signature verification on Stylus is approximately 17 million gas units. At a basefee of 0.1 gwei, which is what Arbitrum One uses, it amounts to a few dollars of value based on the current price of ETH.

See here for an [example tx](https://stylus-testnet-explorer.arbitrum.io/tx/0xa744164f5226a2e81385f68a742594911427e5c09ac3968f8d8545616d582901)

If this were priced as a precompile, we estimate it would be ~430k gas units, as the native compilation of BLST uses heavily optimized assembly and SIMD instructions compared to its WASM counterpart. Nonetheless, deploying a program as complex as this to Stylus is far more time efficient than expecting new precompiles to be added to Ethereum chains.

## License

The [SPDX](https://spdx.dev) license identifier for this project is `MIT OR Apache-2.0`.
