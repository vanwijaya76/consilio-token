import json
from web3 import Web3

w3 = Web3(Web3.HTTPProvider(ETH_RPC_URL))
tx_receipt = w3.eth.get_transaction_receipt(tx_hash)

audit_record = {
    "json_reference_id": "05526eec-721d-4b9e-b6e5-caec81cfaf7e",
    "swift_log": swift_data,
    "blockchain_tx": tx_receipt,
    "signed_json": sign_json(original_json, private_key),
    "timestamp_utc": "2026-01-08T13:05:27Z"
}

with open("audit_trail.json", "w") as f:
    json.dump(audit_record, f, indent=2)
