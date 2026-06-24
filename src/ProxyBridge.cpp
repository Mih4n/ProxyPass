// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, version 3 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.
//
// SPDX-License-Identifier: AGPL-3.0-or-later

#include "ProxyBridge.hpp"

namespace sculk {

ProxyBridge::ProxyBridge(
    const RakNet::RakNetGUID&     guid,
    const RakNet::SystemAddress&  address,
    protocol::Session&            realClientSession,
    protocol::thread::ThreadPool& threadPool
) noexcept
: mRealGuid(guid),
  mRealAddress(address),
  mProxyClient(threadPool),
  mRealClientSession(realClientSession),
  mClientReady(false) {}

ProxyBridge::~ProxyBridge() {}

bool ProxyBridge::init() { return mProxyClient.start() == protocol::NetworkStartResult::Success; }

bool ProxyBridge::sendPacketToClient(const protocol::IPacket& packet, bool immediate) {
    protocol::Session::Buffer buffer{};
    protocol::BinaryStream    stream{buffer};
    packet.writeWithHeader(stream);
    if (immediate) {
        return mRealClientSession.sendPacketImmediately(std::move(buffer));
    }
    return mRealClientSession.sendPacket(std::move(buffer));
}

bool ProxyBridge::sendPacketToServer(const protocol::IPacket& packet, bool immediate) {
    auto session = mProxyClient.getSession().lock();
    if (!session) {
        return false;
    }
    protocol::Session::Buffer buffer{};
    protocol::BinaryStream    stream{buffer};
    packet.writeWithHeader(stream);
    if (immediate) {
        return session->sendPacketImmediately(std::move(buffer));
    }
    return session->sendPacket(std::move(buffer));
}

} // namespace sculk
