#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

const std::string WALLET = "0xcd787c5f15d7aE2061C8F61a60e7573e20B14466";

const std::vector<std::string> RPC_URLS = {
    "https://eth.llamarpc.com",
    "https://rpc.ankr.com/eth",
    "https://eth-mainnet.public.blastapi.io"
};

// Callback function to handle the data returning from the HTTP request
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to send JSON-RPC HTTP POST payloads via libcurl
bool sendJsonRpc(const std::string& url, const json& payload, json& responseOut) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    std::string responseString;
    std::string postFields = payload.dump();

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L); // 5 seconds timeout

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) return false;

    try {
        responseOut = json::parse(responseString);
        return true;
    } catch (...) {
        return false;
    }
}

// Converts a hex string from Wei (hex) to a fractional double of Ether
double weiHexToEther(const std::string& hexWei) {
    // Basic conversion strategy for typical wallet limits: Parse via unsigned long long
    // note: For production nodes handling massive smart contract integer numbers,
    // consider a multi-precision library like Boost.Multiprecision (cpp_int)
    unsigned long long wei = std::stoull(hexWei, nullptr, 16);
    return static_cast<double>(wei) / 1e18;
}

int main() {
    // Initialize libcurl context
    curl_global_init(CURL_GLOBAL_DEFAULT);

    std::cout << std::string(60, '=') << std::endl;
    std::cout << "WALLET BALANCE CHECKER" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "Wallet: " << WALLET << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    for (const auto& rpc_url : RPC_URLS) {
        std::cout << "\n📡 Trying: " << rpc_url.substr(0, 50) << "..." << std::endl;

        // 1. Prepare JSON-RPC standard payload requests
        json balancePayload = {
            {"jsonrpc", "2.0"},
            {"method", "eth_getBalance"},
            {"params", {WALLET, "latest"}},
            {"id", 1}
        };

        json blockPayload = {
            {"jsonrpc", "2.0"},
            {"method", "eth_blockNumber"},
            {"params", json::array()},
            {"id", 2}
        };

        json balanceResponse, blockResponse;

        // 2. Dispatch queries to nodes
        if (sendJsonRpc(rpc_url, balancePayload, balanceResponse) && 
            sendJsonRpc(rpc_url, blockPayload, blockResponse)) {
            
            if (balanceResponse.contains("result") && blockResponse.contains("result")) {
                std::string hexBalance = balanceResponse["result"].get<std::string>();
                std::string hexBlock = blockResponse["result"].get<std::string>();

                double ethBalance = weiHexToEther(hexBalance);
                unsigned long long blockNum = std::stoull(hexBlock, nullptr, 16);

                std::cout << "✅ Connected!" << std::endl;
                std::cout << "💰 ETH Balance: " << std::fixed << std::setprecision(6) << ethBalance << " ETH" << std::endl;
                std::cout << " Bars: Block: " << blockNum << std::endl;
                break; // Exit connection loop on successful balance gathering
            } else {
                std::cout << "❌ Invalid JSON-RPC response format received." << std::endl;
            }
        } else {
            std::cout << "❌ Connection failed / Timeout" << std::endl;
        }
    }

    std::cout << "\n" << std::string(60, '=') << std::endl;
    
    curl_global_cleanup();
    return 0;
}
