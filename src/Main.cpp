#include "ProxyPass.hpp"
#include <iostream>

#include <print>

int main() {
    sculk::protocol::thread::ThreadPool       sharedPool{4};
    sculk::protocol::io::ClientIoRuntime      sharedIoRuntime{4};
    sculk::protocol::AuthenticationKeyManager authKeyManager{};

    std::mutex              waitMutex{};
    std::condition_variable waitCv{};
    bool                    stopped{false};

    std::println("[server] Waiting for Microsoft Service...");
    if (auto status = authKeyManager.initMojangPublicKeyBlocking(); !status) {
        std::println(stderr, "[server] Failed to connect to Microsoft Service: {}", status.error().message());
        return 1;
    }

    auto proxyPass = sculk::ProxyPass(sharedPool, sharedIoRuntime, authKeyManager);
    if (!proxyPass.start(19132, 19133, 100, "127.0.0.1", 19134)) {
        std::println(stderr, "[server] Failed to start proxy server.");
        return 1;
    }
    std::println("[server] Proxy server started successfully.");

    std::unique_lock waitLock{waitMutex};
    waitCv.wait(waitLock, [&] { return stopped; });

    std::println("[server] Stopping proxy server...");

    return 0;
}