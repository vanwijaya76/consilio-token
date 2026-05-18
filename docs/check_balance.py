#!/usr/bin/env python3
# Cek balance wallet menggunakan Infura (tanpa API key)

from web3 import Web3

# Gunakan public RPC (gratis, tanpa API key)
RPC_URLS = [
    "https://eth.llamarpc.com",
    "https://rpc.ankr.com/eth",
    "https://eth-mainnet.public.blastapi.io",
]

WALLET = "0xcd787c5f15d7aE2061C8F61a60e7573e20B14466"

print("=" * 60)
print("CEK BALANCE WALLET")
print("=" * 60)
print(f"Wallet: {WALLET}")
print("=" * 60)

for rpc_url in RPC_URLS:
    try:
        print(f"\n📡 Mencoba: {rpc_url[:50]}...")
        w3 = Web3(Web3.HTTPProvider(rpc_url))
        
        if w3.is_connected():
            balance = w3.eth.get_balance(WALLET)
            balance_eth = w3.from_wei(balance, 'ether')
            print(f"✅ Connected!")
            print(f"💰 ETH Balance: {balance_eth:.6f} ETH")
            print(f"📊 Block: {w3.eth.block_number}")
            break
        else:
            print(f"❌ Tidak bisa konek")
    except Exception as e:
        print(f"❌ Error: {str(e)[:50]}")

print("\n" + "=" * 60)
