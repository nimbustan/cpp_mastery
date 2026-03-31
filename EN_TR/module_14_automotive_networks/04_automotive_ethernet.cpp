/**
 * @file module_14_automotive_networks/04_automotive_ethernet.cpp
 * @brief Automotive Ethernet — 100BASE-T1, SOME/IP, DoIP, TSN
 *
 * @details
 * =============================================================================
 * [THEORY: Automotive Ethernet — High-Speed Vehicle Networking / TEORİ: Otomotiv Ethernet — Yüksek Hızlı Araç Ağı]
 * =============================================================================
 *
 * EN: Automotive Ethernet is the backbone of next-generation E/E architectures.
 *     Unlike consumer Ethernet (Cat5/6, 8 wires), automotive uses BroadR-Reach
 *     (100BASE-T1): single unshielded twisted pair, 100 Mbps, full-duplex.
 *
 *     Physical Layers:
 *     ┌──────────────────┬──────────┬───────┬───────────────────────┐
 *     │ Standard         │ Speed    │ Pairs │ Use Case              │
 *     ├──────────────────┼──────────┼───────┼───────────────────────┤
 *     │ 100BASE-T1       │ 100 Mbps │ 1 UTP │ General (ADAS, body)  │
 *     │ 1000BASE-T1      │ 1 Gbps   │ 1 UTP │ Camera, infotainment  │
 *     │ 10BASE-T1S       │ 10 Mbps  │ 1 UTP │ 10SPE multidrop bus   │
 *     │ MGBASE-T1 (2.5/5)│ 2.5-5G   │ 1 UTP │ High-res sensors      │
 *     │ 10GBASE-T1       │ 10 Gbps  │ 1 UTP │ Central computer      │
 *     └──────────────────┴──────────┴───────┴───────────────────────┘
 *
 *     Protocol Stack:
 *     ┌──────────────────────────────────────┐
 *     │ Application Layer                    │
 *     │   SOME/IP, DoIP, DDS, HTTP/REST      │
 *     ├──────────────────────────────────────┤
 *     │ Transport Layer                      │
 *     │   TCP, UDP                           │
 *     ├──────────────────────────────────────┤
 *     │ Network Layer                        │
 *     │   IPv4, IPv6                         │
 *     ├──────────────────────────────────────┤
 *     │ Data Link Layer                      │
 *     │   Ethernet II, VLAN (802.1Q), TSN    │
 *     ├──────────────────────────────────────┤
 *     │ Physical Layer                       │
 *     │   100BASE-T1, 1000BASE-T1            │
 *     └──────────────────────────────────────┘
 *
 * TR: Otomotiv Ethernet, yeni nesil E/E mimarilerinin omurgasıdır. Tüketici
 *     Ethernet'inden (Cat5/6, 8 kablo) farklı olarak otomotiv BroadR-Reach
 *     (100BASE-T1) kullanır: tek korunmasız bükümlü çift, 100 Mbps, tam çift yönlü.
 *
 *     Fiziksel Katmanlar:
 *     ┌──────────────────┬──────────┬───────┬───────────────────────┐
 *     │ Standart         │ Hız      │ Çift  │ Kullanım Alanı        │
 *     ├──────────────────┼──────────┼───────┼───────────────────────┤
 *     │ 100BASE-T1       │ 100 Mbps │ 1 UTP │ Genel (ADAS, gövde)   │
 *     │ 1000BASE-T1      │ 1 Gbps   │ 1 UTP │ Kamera, infotainment  │
 *     │ 10BASE-T1S       │ 10 Mbps  │ 1 UTP │ 10SPE multidrop bus   │
 *     │ MGBASE-T1 (2.5/5)│ 2.5-5G   │ 1 UTP │ Yüksek çöz. sensör    │
 *     │ 10GBASE-T1       │ 10 Gbps  │ 1 UTP │ Merkezi bilgisayar    │
 *     └──────────────────┴──────────┴───────┴───────────────────────┘
 *
 *     Protokol Yığını:
 *     ┌──────────────────────────────────────┐
 *     │ Uygulama Katmanı                     │
 *     │   SOME/IP, DoIP, DDS, HTTP/REST      │
 *     ├──────────────────────────────────────┤
 *     │ Taşıma Katmanı                       │
 *     │   TCP, UDP                           │
 *     ├──────────────────────────────────────┤
 *     │ Ağ Katmanı                           │
 *     │   IPv4, IPv6                         │
 *     ├──────────────────────────────────────┤
 *     │ Veri Bağlantı Katmanı                │
 *     │   Ethernet II, VLAN (802.1Q), TSN    │
 *     ├──────────────────────────────────────┤
 *     │ Fiziksel Katman                      │
 *     │   100BASE-T1, 1000BASE-T1            │
 *     └──────────────────────────────────────┘
 *
 * =============================================================================
 * [THEORY: SOME/IP — Scalable service-Oriented MiddlewarE over IP / TEORİ: SOME/IP — IP Üzerinden Ölçeklenebilir Servis Odaklı Katman]
 * =============================================================================
 *
 * EN: SOME/IP is the de facto service middleware for AUTOSAR Adaptive.
 *     - Request/Response (RPC): Client calls service method
 *     - Fire & Forget: Notification without reply
 *     - Events / Publish-Subscribe: Periodic or on-change notifications
 *     - Service Discovery (SOME/IP-SD): Dynamic find/offer services
 *
 *     SOME/IP Header (16 bytes):
 *     ┌─────────────┬──────────┬──────────┬───────────┐
 *     │ Service ID  │ Method ID│ Length   │ Client ID │
 *     │ (2 bytes)   │ (2 bytes)│ (4 bytes)│ (2 bytes) │
 *     ├─────────────┼──────────┼──────────┼───────────┤
 *     │ Session ID  │ Proto Ver│ Iface Ver│ Msg Type  │
 *     │ (2 bytes)   │ (1 byte) │ (1 byte) │ (1 byte)  │
 *     ├─────────────┼──────────┴──────────┴───────────┤
 *     │ Return Code │ Payload...                      │
 *     │ (1 byte)    │                                 │
 *     └─────────────┴─────────────────────────────────┘
 *
 * TR: SOME/IP, AUTOSAR Adaptive için fiili servis ara katıdır.
 *     - İstek/Yanıt (RPC): İstemci servis metodunu çağırır
 *     - Fire & Forget: Yanıt beklenmeden bildirim
 *     - Olaylar / Pub-Sub: Periyodik veya değişiklik bazlı bildirimler
 *     - Servis Keşfi (SOME/IP-SD): Dinamik servis bulma/sunma
 *
 *     SOME/IP Başlık (16 bayt):
 *     ┌─────────────┬──────────┬──────────┬───────────┐
 *     │ Service ID  │ Method ID│ Length   │ Client ID │
 *     │ (2 bayt)    │ (2 bayt) │ (4 bayt) │ (2 bayt)  │
 *     ├─────────────┼──────────┼──────────┼───────────┤
 *     │ Session ID  │ Proto Ver│ Iface Ver│ Msg Type  │
 *     │ (2 bayt)    │ (1 bayt) │ (1 bayt) │ (1 bayt)  │
 *     ├─────────────┼──────────┴──────────┴───────────┤
 *     │ Return Code │ Payload...                      │
 *     │ (1 bayt)    │                                 │
 *     └─────────────┴─────────────────────────────────┘
 *
 * =============================================================================
 * [THEORY: DoIP — Diagnostics over IP / TEORİ: DoIP — IP Üzerinden Teşhis]
 * =============================================================================
 *
 * EN: DoIP (ISO 13400) enables UDS diagnostic messages over Ethernet/TCP.
 *     Used by diagnostic testers to flash ECUs and read DTCs over Ethernet.
 *     Protocol: UDP port 13400 (discovery), TCP port 13400 (data).
 *
 * TR: DoIP (ISO 13400), UDS teşhis mesajlarını Ethernet/TCP üzerinden taşır.
 *     Teşhis test cihazları tarafından ECU flash'ı ve DTC okuma için kullanılır.
 *     Protokol: UDP port 13400 (keşif), TCP port 13400 (veri).
 *
 * =============================================================================
 * [THEORY: TSN — Time-Sensitive Networking / TEORİ: TSN — Zamana Duyarlı Ağ İletişimi]
 * =============================================================================
 *
 * EN: TSN is a set of IEEE 802.1 standards that add determinism to Ethernet:
 *     - 802.1AS (gPTP): Generalized Precision Time Protocol (~100 ns sync)
 *     - 802.1Qbv (TAS): Time-Aware Shaper — TDMA-like gate scheduling
 *     - 802.1Qci (PSFP): Per-Stream Filtering and Policing
 *     - 802.1CB (FRER): Frame Replication and Elimination for Redundancy
 *     - 802.1Qav (CBS): Credit-Based Shaper for AVB streams
 *
 * TR: TSN, Ethernet'e FlexRay benzeri determinizm ekleyen IEEE 802.1
 *     standartları kümesidir:
 *     - 802.1AS (gPTP): Genelleştirilmiş Hassas Zaman Protokolü (~100 ns senkron)
 *     - 802.1Qbv (TAS): Zamana Duyarlı Şekillendirici — TDMA benzeri kapı zamanlama
 *     - 802.1Qci (PSFP): Akış Başına Filtreleme ve Denetleme
 *     - 802.1CB (FRER): Yedeklilik için Çerçeve Çoğaltma ve Eleme
 *     - 802.1Qav (CBS): AVB akışları için Kredi Tabanlı Şekillendirici
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 04_automotive_ethernet.cpp -o 04_automotive_ethernet
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <iomanip>
#include <map>
#include <algorithm>
#include <cassert>
#include <array>
#include <sstream>
#include <functional>
#include <chrono>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: Ethernet Frame & VLAN Tagging
// ═════════════════════════════════════════════════════════════════════════════

using MacAddress = std::array<uint8_t, 6>;

std::string mac_to_string(const MacAddress& mac) {
    std::ostringstream oss;
    for (size_t i = 0; i < 6; ++i) {
        if (i > 0) oss << ":";
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(mac[i]);
    }
    return oss.str();
}

struct EthernetFrame {
    MacAddress dst_mac {};
    MacAddress src_mac {};
    uint16_t ethertype = 0x0800;  // IPv4 varsayılan (0x0800=IPv4, 0x86DD=IPv6, 0x88A8=VLAN)
    bool     has_vlan = false;
    uint16_t vlan_id = 0;         // 12-bit VLAN ID (trafik izolasyonu için)
    uint8_t  vlan_pcp = 0;        // 3-bit Öncelik Kod Noktası (0-7, 7=en yüksek)
    std::vector<uint8_t> payload;

    size_t frame_size() const {
        return 14 + (has_vlan ? 4 : 0) + payload.size() + 4; // başlık(14) + VLAN(4) + veri + FCS(4)
    }

    void print(const std::string& label) const {
        std::cout << "  [ETH] " << label << "\n";
        std::cout << "    DST: " << mac_to_string(dst_mac)
                  << " | SRC: " << mac_to_string(src_mac) << "\n";
        std::cout << "    EtherType: 0x" << std::hex << std::setw(4) << std::setfill('0')
                  << ethertype << std::dec;
        if (has_vlan) {
            std::cout << " | VLAN: " << vlan_id << " (PCP=" << static_cast<int>(vlan_pcp) << ")";
        }
        std::cout << " | Size: " << frame_size() << " bytes\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: SOME/IP Message Structure
// ═════════════════════════════════════════════════════════════════════════════

enum class SomeIpMessageType : uint8_t {
    Request         = 0x00,
    RequestNoReturn = 0x01,  // Fire & Forget
    Notification    = 0x02,
    Response        = 0x80,
    Error           = 0x81,
};

enum class SomeIpReturnCode : uint8_t {
    E_OK                = 0x00,
    E_NOT_OK            = 0x01,
    E_UNKNOWN_SERVICE   = 0x02,
    E_UNKNOWN_METHOD    = 0x03,
    E_NOT_READY         = 0x04,
    E_NOT_REACHABLE     = 0x05,
    E_TIMEOUT           = 0x06,
    E_WRONG_PROTOCOL    = 0x07,
    E_WRONG_INTERFACE   = 0x08,
    E_MALFORMED_MESSAGE = 0x09,
};

std::string to_string(SomeIpMessageType t) {
    switch (t) {
        case SomeIpMessageType::Request:         return "REQUEST";
        case SomeIpMessageType::RequestNoReturn: return "FIRE_AND_FORGET";
        case SomeIpMessageType::Notification:    return "NOTIFICATION";
        case SomeIpMessageType::Response:        return "RESPONSE";
        case SomeIpMessageType::Error:           return "ERROR";
    }
    return "UNKNOWN";
}

std::string to_string(SomeIpReturnCode c) {
    switch (c) {
        case SomeIpReturnCode::E_OK:                return "E_OK";
        case SomeIpReturnCode::E_NOT_OK:            return "E_NOT_OK";
        case SomeIpReturnCode::E_UNKNOWN_SERVICE:   return "E_UNKNOWN_SERVICE";
        case SomeIpReturnCode::E_UNKNOWN_METHOD:    return "E_UNKNOWN_METHOD";
        case SomeIpReturnCode::E_NOT_READY:         return "E_NOT_READY";
        case SomeIpReturnCode::E_NOT_REACHABLE:     return "E_NOT_REACHABLE";
        case SomeIpReturnCode::E_TIMEOUT:           return "E_TIMEOUT";
        case SomeIpReturnCode::E_WRONG_PROTOCOL:    return "E_WRONG_PROTOCOL";
        case SomeIpReturnCode::E_WRONG_INTERFACE:   return "E_WRONG_INTERFACE";
        case SomeIpReturnCode::E_MALFORMED_MESSAGE: return "E_MALFORMED_MESSAGE";
    }
    return "UNKNOWN";
}

struct SomeIpMessage {
    uint16_t service_id  = 0;
    uint16_t method_id   = 0;
    uint16_t client_id   = 0;
    uint16_t session_id  = 0;
    uint8_t  protocol_version = 1;
    uint8_t  interface_version = 1;
    SomeIpMessageType message_type = SomeIpMessageType::Request;
    SomeIpReturnCode  return_code  = SomeIpReturnCode::E_OK;
    std::vector<uint8_t> payload;

    uint32_t length() const {
        return static_cast<uint32_t>(8 + payload.size()); // header(8) + payload
    }

    void print(const std::string& label) const {
        std::cout << "  [SOME/IP] " << label << "\n";
        std::cout << "    Service: 0x" << std::hex << std::setw(4) << std::setfill('0') << service_id
                  << " Method: 0x" << std::setw(4) << method_id << std::dec
                  << " | Client: " << client_id << " Session: " << session_id << "\n";
        std::cout << "    Type: " << to_string(message_type)
                  << " | Return: " << to_string(return_code)
                  << " | PayLen: " << payload.size() << " bytes\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: SOME/IP Service Discovery (SOME/IP-SD)
// ═════════════════════════════════════════════════════════════════════════════

// EN: Service Discovery allows runtime find/offer of services.
//     - OfferService: Server announces availability
//     - FindService: Client searches for a service
//     - Subscribe: Client subscribes to event group
//     - SubscribeAck: Server confirms subscription
// TR: Service Discovery, servislerin çalışma zamanında bulunmasını sağlar.

enum class SDEntryType {
    FindService,
    OfferService,
    StopOfferService,
    SubscribeEventgroup,
    SubscribeEventgroupAck,
};

std::string to_string(SDEntryType t) {
    switch (t) {
        case SDEntryType::FindService:              return "FIND_SERVICE";
        case SDEntryType::OfferService:             return "OFFER_SERVICE";
        case SDEntryType::StopOfferService:         return "STOP_OFFER";
        case SDEntryType::SubscribeEventgroup:      return "SUBSCRIBE_EG";
        case SDEntryType::SubscribeEventgroupAck:   return "SUBSCRIBE_ACK";
    }
    return "UNKNOWN";
}

struct SDEntry {
    SDEntryType type;
    uint16_t service_id;
    uint16_t instance_id;
    uint8_t  major_version;
    uint32_t ttl;  // Time-to-live in seconds (0 = stop)
    std::string endpoint_ip;
    uint16_t endpoint_port;
};

class SomeIpServiceDirectory {
    std::map<uint32_t, SDEntry> services_;  // key = (service_id << 16) | instance_id

    static uint32_t make_key(uint16_t sid, uint16_t iid) {
        return (static_cast<uint32_t>(sid) << 16) | iid;
    }

public:
    void offer(const SDEntry& entry) {
        services_[make_key(entry.service_id, entry.instance_id)] = entry;
    }

    void stop_offer(uint16_t service_id, uint16_t instance_id) {
        services_.erase(make_key(service_id, instance_id));
    }

    const SDEntry* find(uint16_t service_id, uint16_t instance_id = 0xFFFF) const {
        if (instance_id != 0xFFFF) {
            auto it = services_.find(make_key(service_id, instance_id));
            return (it != services_.end()) ? &it->second : nullptr;
        }
        // EN: Any instance
        // TR: Herhangi bir örnek — ilk eşleşen servisi döndür
        for (auto& [k, v] : services_) {
            if ((k >> 16) == service_id) return &v;
        }
        return nullptr;
    }

    void print_all() const {
        std::cout << "  ┌──────────┬──────────┬──────┬────────────────────┬───────┐\n";
        std::cout << "  │ Service  │ Instance │ Ver  │ Endpoint           │ TTL   │\n";
        std::cout << "  ├──────────┼──────────┼──────┼────────────────────┼───────┤\n";
        for (auto& [k, e] : services_) {
            std::cout << "  │ 0x" << std::hex << std::setw(4) << std::setfill('0') << e.service_id
                      << "   │ 0x" << std::setw(4) << e.instance_id << std::dec
                      << "   │ " << std::setw(4) << static_cast<int>(e.major_version)
                      << " │ " << std::left << std::setw(13) << (e.endpoint_ip + ":" + std::to_string(e.endpoint_port))
                      << std::right << " │ " << std::setw(5) << e.ttl << " │\n";
        }
        std::cout << "  └──────────┴──────────┴──────┴────────────────────┴───────┘\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: DoIP — Diagnostics over IP
// ═════════════════════════════════════════════════════════════════════════════

// EN: DoIP is used by diagnostic tools (e.g., OBD tester) to communicate
//     with ECUs over TCP/UDP. ISO 13400-2.
// TR: DoIP, teşhis araçlarının Ethernet üzerinden ECU'larla iletişim kurmasını sağlar.

enum class DoIPPayloadType : uint16_t {
    VehicleIdentRequest            = 0x0001,
    VehicleIdentResponse           = 0x0004,
    RoutingActivationRequest       = 0x0005,
    RoutingActivationResponse      = 0x0006,
    DiagnosticMessage              = 0x8001,
    DiagnosticMessageAck           = 0x8002,
    DiagnosticMessageNack          = 0x8003,
};

std::string to_string(DoIPPayloadType t) {
    switch (t) {
        case DoIPPayloadType::VehicleIdentRequest:       return "VEHICLE_IDENT_REQ";
        case DoIPPayloadType::VehicleIdentResponse:      return "VEHICLE_IDENT_RESP";
        case DoIPPayloadType::RoutingActivationRequest:  return "ROUTING_ACT_REQ";
        case DoIPPayloadType::RoutingActivationResponse: return "ROUTING_ACT_RESP";
        case DoIPPayloadType::DiagnosticMessage:         return "DIAG_MESSAGE";
        case DoIPPayloadType::DiagnosticMessageAck:      return "DIAG_MSG_ACK";
        case DoIPPayloadType::DiagnosticMessageNack:     return "DIAG_MSG_NACK";
    }
    return "UNKNOWN";
}

struct DoIPHeader {
    uint8_t protocol_version = 0x03;  // DoIP ISO 13400-2:2019
    uint8_t inverse_version  = 0xFC;  // ~protocol_version
    DoIPPayloadType payload_type;
    uint32_t payload_length = 0;
    std::vector<uint8_t> payload;

    void print(const std::string& label) const {
        std::cout << "  [DoIP] " << label << "\n";
        std::cout << "    Version: " << static_cast<int>(protocol_version)
                  << " | Type: " << to_string(payload_type)
                  << " | PayLen: " << payload_length << " bytes\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: TSN — Time-Aware Shaper (802.1Qbv)
// ═════════════════════════════════════════════════════════════════════════════

// EN: TAS uses gate control lists (GCL) to open/close traffic class queues
//     at precise time intervals. This creates TDMA-like determinism.
// TR: TAS, trafik sınıfı kuyruklarını hassas zaman aralıklarında açıp
//     kapatarak TDMA benzeri determinizm oluşturur.

struct GateControlEntry {
    uint8_t gate_states;      // 8 bits → 8 traffic classes (TC0-TC7), 1=open, 0=closed
    uint32_t time_interval_ns; // Duration in nanoseconds

    void print() const {
        std::cout << "      Gates: ";
        for (int i = 7; i >= 0; --i) {
            std::cout << "TC" << i << "=" << ((gate_states >> i) & 1 ? "O" : "C") << " ";
        }
        std::cout << "| Duration: " << time_interval_ns / 1000 << " μs\n";
    }
};

struct GateControlList {
    std::vector<GateControlEntry> entries;
    uint32_t cycle_time_ns = 0;  // Total cycle = sum of all intervals

    void add(uint8_t gates, uint32_t time_ns) {
        entries.push_back({gates, time_ns});
        cycle_time_ns += time_ns;
    }

    void print() const {
        std::cout << "  TAS Gate Control List (cycle=" << cycle_time_ns / 1000 << " μs):\n";
        for (size_t i = 0; i < entries.size(); ++i) {
            std::cout << "    Slot " << i << ": ";
            entries[i].print();
        }
    }
};

// EN: Traffic class mapping for automotive
// TR: Otomotiv için trafik sınıfı eşlemesi — PCP önceliğine göre sınıflandırma
struct TrafficClassConfig {
    std::string name;
    uint8_t tc_index;       // 0-7
    uint8_t vlan_pcp;       // VLAN Priority Code Point
    std::string description;
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 6: Ethernet Switch Simulation
// ═════════════════════════════════════════════════════════════════════════════

class EthernetSwitch {
    std::string name_;
    std::map<MacAddress, int> mac_table_;  // MAC → port number
    int num_ports_;
    std::vector<std::string> port_names_;

public:
    EthernetSwitch(std::string name, int ports)
        : name_(std::move(name)), num_ports_(ports), port_names_(static_cast<size_t>(ports)) {}

    void set_port_name(int port, const std::string& name) {
        if (port >= 0 && port < num_ports_)
            port_names_[static_cast<size_t>(port)] = name;
    }

    void learn_mac(const MacAddress& mac, int port) {
        mac_table_[mac] = port;
    }

    int lookup(const MacAddress& mac) const {
        auto it = mac_table_.find(mac);
        return (it != mac_table_.end()) ? it->second : -1;  // -1 = flood
    }

    void forward(const EthernetFrame& frame) const {
        int dst_port = lookup(frame.dst_mac);
        std::cout << "    " << name_ << ": "
                  << mac_to_string(frame.src_mac) << " → " << mac_to_string(frame.dst_mac);
        if (dst_port >= 0) {
            std::cout << " → Port " << dst_port;
            if (!port_names_[static_cast<size_t>(dst_port)].empty())
                std::cout << " (" << port_names_[static_cast<size_t>(dst_port)] << ")";
            std::cout << " [UNICAST]\n";
        } else {
            std::cout << " → ALL ports [FLOOD]\n";
        }
    }

    void print_mac_table() const {
        std::cout << "  " << name_ << " MAC Table:\n";
        std::cout << "  ┌───────────────────┬──────┬─────────────────┐\n";
        std::cout << "  │ MAC Address       │ Port │ Device          │\n";
        std::cout << "  ├───────────────────┼──────┼─────────────────┤\n";
        for (auto& [mac, port] : mac_table_) {
            std::cout << "  │ " << mac_to_string(mac)
                      << " │ " << std::setw(4) << port
                      << " │ " << std::left << std::setw(15)
                      << (port >= 0 && port < num_ports_ ? port_names_[static_cast<size_t>(port)] : "")
                      << std::right << " │\n";
        }
        std::cout << "  └───────────────────┴──────┴─────────────────┘\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 14 - Automotive Ethernet\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: Ethernet Frames & VLAN ─────────────────────────────────
    {
        std::cout << "--- Demo 1: Automotive Ethernet Frames ---\n";

        // EN: ADAS camera frame (high priority, VLAN tagged)
        // TR: ADAS kamera çerçevesi (yüksek öncelik, VLAN etiketli)
        EthernetFrame camera_frame;
        camera_frame.dst_mac = {0x01, 0x00, 0x5E, 0x00, 0x00, 0x01};  // multicast
        camera_frame.src_mac = {0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x01};
        camera_frame.ethertype = 0x0800;
        camera_frame.has_vlan = true;
        camera_frame.vlan_id = 100;
        camera_frame.vlan_pcp = 6;  // high priority
        camera_frame.payload.resize(1400);  // camera image chunk
        camera_frame.print("ADAS Camera (VLAN 100, PCP=6 Critical)");

        // EN: CAN-over-Ethernet gateway frame
        // TR: CAN-üzerinden-Ethernet ağ geçidi çerçevesi
        EthernetFrame gw_frame;
        gw_frame.dst_mac = {0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x10};
        gw_frame.src_mac = {0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x02};
        gw_frame.ethertype = 0x0800;
        gw_frame.has_vlan = true;
        gw_frame.vlan_id = 200;
        gw_frame.vlan_pcp = 4;  // medium priority
        gw_frame.payload.resize(64);  // tunneled CAN frame
        gw_frame.print("CAN-to-Ethernet Gateway (VLAN 200, PCP=4)");

        // EN: OTA update frame (best effort)
        // TR: OTA güncelleme çerçevesi (en iyi çaba, düşük öncelik)
        EthernetFrame ota_frame;
        ota_frame.dst_mac = {0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x20};
        ota_frame.src_mac = {0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x03};
        ota_frame.ethertype = 0x0800;
        ota_frame.has_vlan = true;
        ota_frame.vlan_id = 300;
        ota_frame.vlan_pcp = 0;  // best effort
        ota_frame.payload.resize(1500);
        ota_frame.print("OTA Update (VLAN 300, PCP=0 Best Effort)");

        std::cout << "\n  VLAN PCP Priority Mapping (automotive):\n";
        std::cout << "    PCP 7: Network Control (switch management)\n";
        std::cout << "    PCP 6: Safety-Critical (ADAS, braking)\n";
        std::cout << "    PCP 5: Control Data (steering, powertrain)\n";
        std::cout << "    PCP 4: Audio/Video streaming\n";
        std::cout << "    PCP 3: Infotainment\n";
        std::cout << "    PCP 2: Medium priority\n";
        std::cout << "    PCP 1: Background\n";
        std::cout << "    PCP 0: Best Effort (OTA, logging)\n\n";
    }

    // ─── Demo 2: SOME/IP Request-Response ───────────────────────────────
    {
        std::cout << "--- Demo 2: SOME/IP Request-Response ---\n";
        std::cout << "  Scenario: ADAS requests camera image from Camera ECU\n\n";

        // EN: Request: ADAS → Camera ECU
        // TR: İstek: ADAS → Kamera ECU — GetImage metodu çağrısı
        SomeIpMessage request;
        request.service_id = 0x1001;   // Camera Service
        request.method_id  = 0x0001;   // GetImage method
        request.client_id  = 0x0100;   // ADAS ECU
        request.session_id = 0x0001;
        request.message_type = SomeIpMessageType::Request;
        request.payload = {0x01, 0x00};  // camera_id=1, format=RAW
        request.print("Request: ADAS → Camera (GetImage)");

        // EN: Response: Camera ECU → ADAS
        // TR: Yanıt: Kamera ECU → ADAS — görüntü verisi döndürülür
        SomeIpMessage response;
        response.service_id = 0x1001;
        response.method_id  = 0x0001;
        response.client_id  = 0x0100;
        response.session_id = 0x0001;
        response.message_type = SomeIpMessageType::Response;
        response.return_code  = SomeIpReturnCode::E_OK;
        response.payload.resize(256);  // image data chunk
        response.print("Response: Camera → ADAS (ImageData)");

        // EN: Error case
        // TR: Hata durumu — bilinmeyen metod çağrısı
        SomeIpMessage error_msg;
        error_msg.service_id = 0x1001;
        error_msg.method_id  = 0x0002;  // non-existent method
        error_msg.client_id  = 0x0100;
        error_msg.session_id = 0x0002;
        error_msg.message_type = SomeIpMessageType::Error;
        error_msg.return_code  = SomeIpReturnCode::E_UNKNOWN_METHOD;
        error_msg.print("Error: Unknown method");
        std::cout << "\n";
    }

    // ─── Demo 3: SOME/IP-SD Service Discovery ──────────────────────────
    {
        std::cout << "--- Demo 3: SOME/IP-SD Service Discovery ---\n";

        SomeIpServiceDirectory directory;

        // EN: ECUs offer their services
        // TR: ECU'lar servislerini sunar — SOME/IP-SD ile keşif
        directory.offer({SDEntryType::OfferService, 0x1001, 0x0001, 1, 3600,
                        "192.168.1.10", 30490});
        directory.offer({SDEntryType::OfferService, 0x2001, 0x0001, 1, 3600,
                        "192.168.1.20", 30491});
        directory.offer({SDEntryType::OfferService, 0x3001, 0x0001, 2, 3600,
                        "192.168.1.30", 30492});

        std::cout << "  Offered services:\n";
        directory.print_all();

        // EN: Client finds a service
        // TR: İstemci bir servis arar — service_id ile sorgulama
        auto* cam = directory.find(0x1001);
        std::cout << "\n  Find 0x1001 (Camera): "
                  << (cam ? "FOUND at " + cam->endpoint_ip + ":" + std::to_string(cam->endpoint_port)
                          : "NOT FOUND") << "\n";

        auto* unknown = directory.find(0x9999);
        std::cout << "  Find 0x9999 (Unknown): "
                  << (unknown ? "FOUND" : "NOT FOUND") << "\n";

        // EN: Stop offer
        // TR: Servis sunumunu durdur — TTL=0 ile StopOffer mesajı
        directory.stop_offer(0x2001, 0x0001);
        std::cout << "\n  After StopOffer(0x2001):\n";
        directory.print_all();
        std::cout << "\n";
    }

    // ─── Demo 4: DoIP — Diagnostic Session ─────────────────────────────
    {
        std::cout << "--- Demo 4: DoIP Diagnostic Session ---\n";
        std::cout << "  Scenario: Tester connects to ECU over Ethernet\n\n";

        // EN: Step 1: Vehicle identification (UDP broadcast)
        // TR: Adım 1: Araç tanımlama (UDP yayını, port 13400)
        DoIPHeader ident_req;
        ident_req.payload_type = DoIPPayloadType::VehicleIdentRequest;
        ident_req.payload_length = 0;
        ident_req.print("Step 1: Vehicle Identification Request (UDP:13400)");

        DoIPHeader ident_resp;
        ident_resp.payload_type = DoIPPayloadType::VehicleIdentResponse;
        // EN: VIN + logical address + EID + GID
        // TR: VIN + mantıksal adres + EID + GID — araç kimlik bilgileri
        ident_resp.payload = {
            'W', 'V', 'W', 'Z', 'Z', 'Z', '3', 'C', 'Z', 'W', 'E', '0', '0', '0', '0', '0', '1',  // VIN
            0x00, 0x10,  // logical address 0x0010
        };
        ident_resp.payload_length = static_cast<uint32_t>(ident_resp.payload.size());
        ident_resp.print("Step 2: Vehicle Identification Response");
        std::cout << "    VIN: WVWZZZ3CZWE000001, LogAddr: 0x0010\n";

        // EN: Step 3: Routing activation
        // TR: Adım 3: Yönlendirme aktivasyonu — TCP bağlantısı kurulur
        DoIPHeader routing_req;
        routing_req.payload_type = DoIPPayloadType::RoutingActivationRequest;
        routing_req.payload = {0x00, 0x01, 0x00};  // source_address, activation_type
        routing_req.payload_length = static_cast<uint32_t>(routing_req.payload.size());
        routing_req.print("Step 3: Routing Activation Request (TCP:13400)");

        DoIPHeader routing_resp;
        routing_resp.payload_type = DoIPPayloadType::RoutingActivationResponse;
        routing_resp.payload = {0x00, 0x01, 0x00, 0x10, 0x10};
        routing_resp.payload_length = static_cast<uint32_t>(routing_resp.payload.size());
        routing_resp.print("Step 4: Routing Activation Response (success)");

        // EN: Step 5: UDS DiagnosticMessage over DoIP
        // TR: Adım 5: DoIP üzerinden UDS teşhis mesajı (ReadDID, VIN okuma)
        DoIPHeader diag_msg;
        diag_msg.payload_type = DoIPPayloadType::DiagnosticMessage;
        diag_msg.payload = {
            0x00, 0x01,  // source address (tester)
            0x00, 0x10,  // target address (ECU)
            0x22, 0xF1, 0x90  // UDS: ReadDataByIdentifier, DID=0xF190 (VIN)
        };
        diag_msg.payload_length = static_cast<uint32_t>(diag_msg.payload.size());
        diag_msg.print("Step 5: UDS ReadDID(VIN) over DoIP");

        DoIPHeader diag_ack;
        diag_ack.payload_type = DoIPPayloadType::DiagnosticMessageAck;
        diag_ack.payload = {0x00, 0x10, 0x00};
        diag_ack.payload_length = static_cast<uint32_t>(diag_ack.payload.size());
        diag_ack.print("Step 6: Diagnostic Message ACK");
        std::cout << "\n";
    }

    // ─── Demo 5: TSN Time-Aware Shaper ─────────────────────────────────
    {
        std::cout << "--- Demo 5: TSN Time-Aware Shaper (802.1Qbv) ---\n";

        // EN: Define traffic classes for automotive
        // TR: Otomotiv için trafik sınıflarını tanımla — öncelik bazlı kuyruklar
        std::vector<TrafficClassConfig> tc_config = {
            {"Safety-Critical", 7, 7, "ADAS control, braking (< 1ms)"},
            {"Control",         6, 6, "Steering, powertrain (< 5ms)"},
            {"Audio/Video",     5, 5, "Camera streams, surround view"},
            {"Diagnostics",     4, 4, "UDS/DoIP diagnostic messages"},
            {"Infotainment",    3, 3, "Navigation, media streaming"},
            {"OTA/Logging",     1, 1, "Software updates, data logging"},
            {"Best-Effort",     0, 0, "Non-critical background traffic"},
        };

        std::cout << "  Traffic Class Configuration:\n";
        std::cout << "  ┌──────┬───────────────────┬─────┬─────────────────────────────────┐\n";
        std::cout << "  │ TC   │ Name              │ PCP │ Description                     │\n";
        std::cout << "  ├──────┼───────────────────┼─────┼─────────────────────────────────┤\n";
        for (auto& tc : tc_config) {
            std::cout << "  │ TC" << static_cast<int>(tc.tc_index)
                      << "  │ " << std::left << std::setw(17) << tc.name
                      << " │ " << std::right << std::setw(3) << static_cast<int>(tc.vlan_pcp)
                      << " │ " << std::left << std::setw(31) << tc.description
                      << std::right << " │\n";
        }
        std::cout << "  └──────┴───────────────────┴─────┴─────────────────────────────────┘\n\n";

        // EN: Gate Control List — deterministic schedule
        // TR: Kapı Kontrol Listesi — belirlenimci zamanlama, her slot farklı TC açar
        GateControlList gcl;
        // Slot 0: Safety only (TC7)
        gcl.add(0b10000000, 200'000);   // 200 μs — only TC7 open
        // Slot 1: Control (TC6+TC7)
        gcl.add(0b11000000, 300'000);   // 300 μs — TC6+TC7 open
        // Slot 2: AV Streams (TC5+TC6+TC7)
        gcl.add(0b11100000, 200'000);   // 200 μs
        // Slot 3: Everything (all TCs)
        gcl.add(0b11111111, 300'000);   // 300 μs — all open (best effort window)

        gcl.print();

        std::cout << "\n  TAS Timeline (1 cycle = " << gcl.cycle_time_ns / 1000 << " μs):\n";
        std::cout << "  |─ Safety ─|── Control ──|── AV ──|─── Best Effort ──|\n";
        std::cout << "  |  200μs   |   300μs     | 200μs  |     300μs        |\n";
        std::cout << "  Guaranteed: Safety data ALWAYS gets its 200μs window\n\n";
    }

    // ─── Demo 6: Ethernet Switch Forwarding ─────────────────────────────
    {
        std::cout << "--- Demo 6: Automotive Ethernet Switch ---\n";

        EthernetSwitch sw("ZonalSwitch_Front", 5);
        sw.set_port_name(0, "ADAS_ECU");
        sw.set_port_name(1, "Camera_FL");
        sw.set_port_name(2, "Camera_FR");
        sw.set_port_name(3, "Radar_Front");
        sw.set_port_name(4, "Uplink_Central");

        // EN: Learn MAC addresses
        // TR: MAC adreslerini öğren — anahtarlama tablosu oluştur
        MacAddress mac_adas   = {0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x01};
        MacAddress mac_cam_fl = {0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x02};
        MacAddress mac_cam_fr = {0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x03};
        MacAddress mac_radar  = {0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x04};
        MacAddress mac_central = {0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x10};
        MacAddress mac_unknown = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

        sw.learn_mac(mac_adas, 0);
        sw.learn_mac(mac_cam_fl, 1);
        sw.learn_mac(mac_cam_fr, 2);
        sw.learn_mac(mac_radar, 3);
        sw.learn_mac(mac_central, 4);

        sw.print_mac_table();

        std::cout << "\n  Frame forwarding:\n";
        // Camera→ADAS (known unicast)
        EthernetFrame f1;
        f1.src_mac = mac_cam_fl;
        f1.dst_mac = mac_adas;
        sw.forward(f1);

        // ADAS→Central (uplink)
        EthernetFrame f2;
        f2.src_mac = mac_adas;
        f2.dst_mac = mac_central;
        sw.forward(f2);

        // Broadcast (flood)
        EthernetFrame f3;
        f3.src_mac = mac_radar;
        f3.dst_mac = mac_unknown;
        sw.forward(f3);
        std::cout << "\n";
    }

    // ─── Demo 7: Network Topology — Zonal Architecture ─────────────────
    {
        std::cout << "--- Demo 7: Automotive Ethernet Topology ---\n\n";
        std::cout << "  Modern Zonal Architecture (Ethernet backbone):\n\n";
        std::cout << "  ┌──────────────────────────────────────────────────────┐\n";
        std::cout << "  │                  Central Compute (HPC)               │\n";
        std::cout << "  │              ┌──────────────────────┐                │\n";
        std::cout << "  │              │ Vehicle Computer     │                │\n";
        std::cout << "  │              │ (AUTOSAR Adaptive    │                │\n";
        std::cout << "  │              │  + Linux + SOME/IP)  │                │\n";
        std::cout << "  │              └──────┬───────────────┘                │\n";
        std::cout << "  │                     │ 1Gbps Ethernet Backbone        │\n";
        std::cout << "  │     ┌───────────────┼─────────────┬───────────┐      │\n";
        std::cout << "  │     │               │             │           │      │\n";
        std::cout << "  │  ┌──┴───┐      ┌────┴────┐    ┌───┴───┐   ┌───┴───┐  │\n";
        std::cout << "  │  │Zone  │      │Zone     │    │Zone   │   │Zone   │  │\n";
        std::cout << "  │  │Front │      │Rear     │    │Left   │   │Right  │  │\n";
        std::cout << "  │  │Switch│      │Switch   │    │Switch │   │Switch │  │\n";
        std::cout << "  │  └──┬───┘      └────┬────┘    └───┬───┘   └───┬───┘  │\n";
        std::cout << "  │     │               │             │           │      │\n";
        std::cout << "  │  100BASE-T1      100BASE-T1    CAN/LIN     CAN/LIN   │\n";
        std::cout << "  │  Camera×2        Camera×2     Door ECU    Door ECU   │\n";
        std::cout << "  │  Radar           Ultrasonic    Window      Mirror    │\n";
        std::cout << "  │  Lidar           Parking       Seat        Light     │\n";
        std::cout << "  └──────────────────────────────────────────────────────┘\n\n";

        std::cout << "  Key features:\n";
        std::cout << "    - Central HPC replaces 100+ traditional ECUs\n";
        std::cout << "    - Zonal switches reduce wiring harness weight by 30-40%%\n";
        std::cout << "    - 1Gbps backbone handles ADAS + infotainment + body\n";
        std::cout << "    - CAN/LIN still used for simple sensors/actuators at edges\n";
        std::cout << "    - TSN ensures real-time performance for safety-critical data\n\n";
    }

    // ─── Demo 8: Protocol Comparison ────────────────────────────────────
    {
        std::cout << "--- Demo 8: Automotive Protocol Comparison ---\n\n";
        std::cout << "  ┌─────────────────┬──────────┬──────────┬──────────┬──────────┬──────────┐\n";
        std::cout << "  │ Property        │ CAN      │ CAN FD   │ LIN      │ FlexRay  │ Ethernet │\n";
        std::cout << "  ├─────────────────┼──────────┼──────────┼──────────┼──────────┼──────────┤\n";
        std::cout << "  │ Speed           │ 1 Mbps   │ 8 Mbps   │ 20 kbps  │ 10 Mbps  │ 100M-10G │\n";
        std::cout << "  │ Payload         │ 8 B      │ 64 B     │ 8 B      │ 254 B    │ 1500+ B  │\n";
        std::cout << "  │ Topology        │ Bus      │ Bus      │ Bus      │ Bus/Star │ Star     │\n";
        std::cout << "  │ Determinism     │ Priority │ Priority │ Master   │ TDMA     │ TSN      │\n";
        std::cout << "  │ Redundancy      │ No       │ No       │ No       │ Dual-ch  │ FRER     │\n";
        std::cout << "  │ Wires           │ 2        │ 2        │ 1        │ 2/4      │ 1 pair   │\n";
        std::cout << "  │ Cost/node       │ $1-3     │ $3-5     │ $0.5-1   │ $10-20   │ $5-15    │\n";
        std::cout << "  │ ASIL level      │ D        │ D        │ B        │ D        │ D (TSN)  │\n";
        std::cout << "  │ Use case        │ Body,    │ Power-   │ Comfort  │ X-by-wire│ ADAS,    │\n";
        std::cout << "  │                 │ chassis  │ train    │ sensors  │ safety   │ central  │\n";
        std::cout << "  └─────────────────┴──────────┴──────────┴──────────┴──────────┴──────────┘\n";
    }

    std::cout << "\n============================================\n";
    std::cout << "   End of Automotive Ethernet\n";
    std::cout << "============================================\n";

    return 0;
}
