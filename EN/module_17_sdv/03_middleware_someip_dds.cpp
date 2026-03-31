/**
 * @file module_17_sdv/03_middleware_someip_dds.cpp
 * @brief Middleware: SOME/IP, DDS, iceoryx
 *
 * @details
 * =============================================================================
 * [THEORY: Why Middleware?]
 * =============================================================================
 *
 * EN: Middleware abstracts low-level communication (sockets, shared memory)
 *     and provides service-oriented communication for distributed systems.
 *
 *     Without Middleware          With Middleware
 *     ┌──────────┐               ┌──────────┐
 *     │  App A   │               │  App A   │
 *     │ socket() │               │ proxy->  │
 *     │ connect()│               │  call()  │
 *     │ send()   │               └────┬─────┘
 *     │ recv()   │                    │ (transparent)
 *     └────┬─────┘               ┌────┴─────┐
 *          │ raw TCP/UDP         │Middleware│
 *     ┌────┴─────┐               │ SOME/IP  │
 *     │  App B   │               │  or DDS  │
 *     └──────────┘               └────┬─────┘
 *                                ┌────┴─────┐
 *                                │  App B   │
 *                                │ skeleton │
 *                                └──────────┘
 *
 *
 *     ┌──────────┐               ┌──────────┐
 *     │  Uygulama│               │  Uygulama│
 *     │ socket() │               │ proxy->  │
 *     │ connect()│               │  call()  │
 *     │ send()   │               └────┬─────┘
 *     │ recv()   │                    │ (saydam)
 *     └────┬─────┘               ┌────┴─────┐
 *          │ ham TCP/UDP         │Middleware│
 *     ┌────┴─────┐               │ SOME/IP  │
 *     │  Uygulama│               │  veya DDS│
 *     └──────────┘               └────┬─────┘
 *                                ┌────┴─────┐
 *                                │  Uygulama│
 *                                │ skeleton │
 *                                └──────────┘
 *
 *
 * =============================================================================
 * [THEORY: SOME/IP Header Format]
 * =============================================================================
 *
 * EN: Byte: 0       4       8       12      16
 *        ┌───────────────┬───────────────┐
 *        │ Message ID    │ Length        │
 *        │ (Service+Meth)│ (payload+8)   │
 *        ├───────────────┼───────┬───────┤
 *        │Request ID     │Proto  │ Iface │
 *        │(Client+Sess)  │Version│Version│
 *        ├───────┬───────┼───────┴───────┤
 *        │MsgType│RetCode│   Payload...  │
 *        └───────┴───────┴───────────────┘
 *
 *  Message Types: REQUEST(0x00), REQUEST_NO_RETURN(0x01),
 *                 NOTIFICATION(0x02), RESPONSE(0x80), ERROR(0x81)
 *
 *
 * =============================================================================
 * [THEORY: DDS — Data Distribution Service]
 * =============================================================================
 *
 * EN:
 *     ┌─────────────────────────────────────────────┐
 *     │                DDS Domain                   │
 *     │  ┌────────────┐         ┌────────────┐      │
 *     │  │ DataWriter │──Topic──│ DataReader │      │
 *     │  │ (Publisher)│  name   │(Subscriber)│      │
 *     │  └────────────┘         └────────────┘      │
 *     │         ↓                     ↑             │
 *     │    [QoS Policies: Reliability, Durability,  │
 *     │     Deadline, History, Lifespan]            │
 *     └─────────────────────────────────────────────┘
 *
 *
 * =============================================================================
 * [THEORY: Middleware Comparison]
 * =============================================================================
 *
 * EN:
 *     ┌──────────┬───────────────┬───────────────┬──────────────┬──────────┐
 *     │ Feature  │ SOME/IP       │ DDS           │ iceoryx      │ gRPC     │
 *     ├──────────┼───────────────┼───────────────┼──────────────┼──────────┤
 *     │ Pattern  │ Req/Resp+Event│ Pub/Sub       │ Pub/Sub      │ Req/Resp │
 *     │ Transport│ TCP/UDP       │ UDP multicast │ Shared memory│ HTTP/2   │
 *     │ Discovery│ SOME/IP-SD    │ Built-in (DDP)│ RouDi daemon │ Manual   │
 *     │ Latency  │ ~100µs        │ ~50µs         │ <1µs (0-copy)│ ~500µs   │
 *     │ Standard │ AUTOSAR       │ OMG           │ Eclipse      │ Google   │
 *     │ C++ API  │ vsomeip       │ Fast-DDS      │ iceoryx2     │ grpc++   │
 *     │ Use case │ Classic auto  │ ADAS, robotics│ Intra-SoC IPC│ Cloud    │
 *     └──────────┴───────────────┴───────────────┴──────────────┴──────────┘
 *
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 03_middleware_someip_dds.cpp -o 03_middleware
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <string_view>
#include <map>
#include <unordered_map>
#include <functional>
#include <memory>
#include <optional>
#include <iomanip>
#include <algorithm>
#include <array>
#include <queue>
#include <cassert>
#include <cstring>
#include <variant>
#include <numeric>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: SOME/IP Protocol Simulation
// ═════════════════════════════════════════════════════════════════════════════

namespace someip {

// EN: SOME/IP message types (AUTOSAR PRS_SOMEIP_00055)
enum class MessageType : uint8_t {
    REQUEST           = 0x00,
    REQUEST_NO_RETURN = 0x01,
    NOTIFICATION      = 0x02,
    RESPONSE          = 0x80,
    ERROR             = 0x81
};

enum class ReturnCode : uint8_t {
    E_OK               = 0x00,
    E_NOT_OK           = 0x01,
    E_UNKNOWN_SERVICE  = 0x02,
    E_UNKNOWN_METHOD   = 0x03,
    E_NOT_READY        = 0x04,
    E_NOT_REACHABLE    = 0x05
};

inline std::string_view MsgTypeStr(MessageType t) {
    switch (t) {
        case MessageType::REQUEST:           return "REQUEST";
        case MessageType::REQUEST_NO_RETURN: return "REQUEST_NO_RETURN";
        case MessageType::NOTIFICATION:      return "NOTIFICATION";
        case MessageType::RESPONSE:          return "RESPONSE";
        case MessageType::ERROR:             return "ERROR";
    }
    return "UNKNOWN";
}

// EN: SOME/IP header (16 bytes)
struct SomeIpHeader {
    uint16_t    service_id;
    uint16_t    method_id;
    uint32_t    length;         // payload + 8 bytes
    uint16_t    client_id;
    uint16_t    session_id;
    uint8_t     protocol_version;
    uint8_t     interface_version;
    MessageType message_type;
    ReturnCode  return_code;

    void Print() const {
        std::cout << "    SOME/IP Header:\n"
                  << "      Service: 0x" << std::hex << service_id
                  << "  Method: 0x" << method_id << std::dec << "\n"
                  << "      Client: " << client_id << "  Session: " << session_id << "\n"
                  << "      Type: " << MsgTypeStr(message_type)
                  << "  Return: " << static_cast<int>(return_code) << "\n";
    }
};

struct SomeIpMessage {
    SomeIpHeader         header;
    std::vector<uint8_t> payload;

    static SomeIpMessage CreateRequest(uint16_t svc, uint16_t mth,
                                        uint16_t client, uint16_t session,
                                        const std::vector<uint8_t>& data) {
        SomeIpMessage msg;
        msg.header.service_id = svc;
        msg.header.method_id = mth;
        msg.header.length = static_cast<uint32_t>(data.size() + 8);
        msg.header.client_id = client;
        msg.header.session_id = session;
        msg.header.protocol_version = 1;
        msg.header.interface_version = 1;
        msg.header.message_type = MessageType::REQUEST;
        msg.header.return_code = ReturnCode::E_OK;
        msg.payload = data;
        return msg;
    }

    static SomeIpMessage CreateResponse(const SomeIpMessage& request,
                                         const std::vector<uint8_t>& data) {
        SomeIpMessage msg;
        msg.header = request.header;
        msg.header.message_type = MessageType::RESPONSE;
        msg.header.return_code = ReturnCode::E_OK;
        msg.header.length = static_cast<uint32_t>(data.size() + 8);
        msg.payload = data;
        return msg;
    }

    static SomeIpMessage CreateNotification(uint16_t svc, uint16_t event_id,
                                             const std::vector<uint8_t>& data) {
        SomeIpMessage msg;
        msg.header.service_id = svc;
        msg.header.method_id = event_id;
        msg.header.length = static_cast<uint32_t>(data.size() + 8);
        msg.header.client_id = 0;
        msg.header.session_id = 0;
        msg.header.protocol_version = 1;
        msg.header.interface_version = 1;
        msg.header.message_type = MessageType::NOTIFICATION;
        msg.header.return_code = ReturnCode::E_OK;
        msg.payload = data;
        return msg;
    }
};

// EN: SOME/IP Service Discovery (simplified)
struct ServiceEntry {
    uint16_t service_id;
    uint16_t instance_id;
    std::string provider_endpoint;
};

class ServiceDiscovery {
    std::vector<ServiceEntry> offered_;

public:
    void OfferService(uint16_t svc, uint16_t inst, const std::string& endpoint) {
        offered_.push_back({svc, inst, endpoint});
        std::cout << "    [SD] OfferService: 0x" << std::hex << svc
                  << " instance=" << inst << std::dec
                  << " at " << endpoint << "\n";
    }

    std::optional<ServiceEntry> FindService(uint16_t svc) const {
        for (const auto& e : offered_) {
            if (e.service_id == svc) return e;
        }
        return std::nullopt;
    }

    [[nodiscard]] size_t Count() const { return offered_.size(); }
};

}  // namespace someip

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: DDS Simulation — Topic-Based Pub/Sub
// ═════════════════════════════════════════════════════════════════════════════

namespace dds {

// EN: QoS (Quality of Service) policies
enum class Reliability : uint8_t { BEST_EFFORT, RELIABLE };
enum class Durability : uint8_t { VOLATILE, TRANSIENT_LOCAL, TRANSIENT, PERSISTENT };

struct QoSPolicy {
    Reliability reliability = Reliability::RELIABLE;
    Durability  durability  = Durability::VOLATILE;
    uint32_t    deadline_ms = 0;   // 0 = no deadline
    uint32_t    history_depth = 1;

    void Print() const {
        std::cout << "    QoS: reliability="
                  << (reliability == Reliability::RELIABLE ? "RELIABLE" : "BEST_EFFORT")
                  << " durability="
                  << static_cast<int>(durability)
                  << " deadline=" << deadline_ms << "ms"
                  << " history=" << history_depth << "\n";
    }
};

// EN: DDS Sample (data + metadata)
struct Sample {
    std::string topic;
    std::vector<uint8_t> data;
    uint64_t    sequence_number;
    uint64_t    timestamp;
};

// EN: DataWriter — publishes samples on a topic
class DataWriter {
    std::string                               topic_;
    QoSPolicy                                qos_;
    uint64_t                                 seq_ = 0;
    std::function<void(const Sample&)>       on_write_;

public:
    DataWriter(std::string topic, QoSPolicy qos)
        : topic_(std::move(topic)), qos_(std::move(qos)) {}

    void SetCallback(std::function<void(const Sample&)> cb) {
        on_write_ = std::move(cb);
    }

    void Write(const std::vector<uint8_t>& data) {
        Sample s;
        s.topic = topic_;
        s.data = data;
        s.sequence_number = ++seq_;
        s.timestamp = 1717200000 + seq_;
        if (on_write_) on_write_(s);
    }

    [[nodiscard]] const std::string& TopicName() const { return topic_; }
    [[nodiscard]] const QoSPolicy& GetQoS() const { return qos_; }
};

// EN: DataReader — subscribes to a topic
class DataReader {
    std::string                               topic_;
    QoSPolicy                                qos_;
    std::vector<Sample>                      history_;

public:
    DataReader(std::string topic, QoSPolicy qos)
        : topic_(std::move(topic)), qos_(std::move(qos)) {}

    void OnData(const Sample& s) {
        if (s.topic == topic_) {
            history_.push_back(s);
            if (history_.size() > qos_.history_depth)
                history_.erase(history_.begin());
        }
    }

    [[nodiscard]] std::optional<Sample> TakeLast() const {
        if (history_.empty()) return std::nullopt;
        return history_.back();
    }

    [[nodiscard]] size_t SampleCount() const { return history_.size(); }
    [[nodiscard]] const std::string& TopicName() const { return topic_; }
};

// EN: DDS Participant — manages writers and readers
class DomainParticipant {
    std::string name_;
    std::vector<DataWriter*>                writers_;
    std::vector<DataReader*>                readers_;
    // EN: Topic → readers mapping for routing
    std::unordered_map<std::string, std::vector<DataReader*>> topic_readers_;

public:
    explicit DomainParticipant(std::string name) : name_(std::move(name)) {}

    void RegisterWriter(DataWriter* w) {
        writers_.push_back(w);
        w->SetCallback([this](const Sample& s) { Route(s); });
        std::cout << "  [DDS:" << name_ << "] Writer registered: " << w->TopicName() << "\n";
    }

    void RegisterReader(DataReader* r) {
        readers_.push_back(r);
        topic_readers_[r->TopicName()].push_back(r);
        std::cout << "  [DDS:" << name_ << "] Reader registered: " << r->TopicName() << "\n";
    }

    void Route(const Sample& s) {
        auto it = topic_readers_.find(s.topic);
        if (it != topic_readers_.end()) {
            for (auto* reader : it->second) {
                reader->OnData(s);
            }
        }
    }
};

}  // namespace dds

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: iceoryx — Zero-Copy Shared Memory IPC
// ═════════════════════════════════════════════════════════════════════════════

namespace iceoryx_sim {

// EN: Simulated shared memory segment (no real shm, just a buffer)
class SharedMemorySegment {
    static constexpr size_t SEGMENT_SIZE = 4096;
    std::array<uint8_t, SEGMENT_SIZE> memory_{};
    size_t write_pos_ = 0;

public:
    // EN: "Loan" a chunk — publisher gets pointer, writes, then publishes
    struct Chunk {
        uint8_t* data;
        size_t   size;
        size_t   offset;
    };

    std::optional<Chunk> Loan(size_t size) {
        if (write_pos_ + size > SEGMENT_SIZE) return std::nullopt;
        Chunk c;
        c.data = &memory_[write_pos_];
        c.size = size;
        c.offset = write_pos_;
        write_pos_ += size;
        return c;
    }

    const uint8_t* Read(size_t offset) const {
        if (offset >= SEGMENT_SIZE) return nullptr;
        return &memory_[offset];
    }
};

// EN: iceoryx Publisher (zero-copy)
class Publisher {
    std::string         topic_;
    SharedMemorySegment& shm_;
    std::function<void(const std::string&, size_t, size_t)> notify_;

public:
    Publisher(std::string topic, SharedMemorySegment& shm)
        : topic_(std::move(topic)), shm_(shm) {}

    void SetNotifyCallback(std::function<void(const std::string&, size_t, size_t)> cb) {
        notify_ = std::move(cb);
    }

    bool Publish(const void* data, size_t size) {
        auto chunk = shm_.Loan(size);
        if (!chunk) return false;
        std::memcpy(chunk->data, data, size);
        std::cout << "    [iceoryx] Published " << size << " bytes on '" << topic_
                  << "' (zero-copy @ offset " << chunk->offset << ")\n";
        if (notify_) notify_(topic_, chunk->offset, chunk->size);
        return true;
    }
};

// EN: iceoryx Subscriber (zero-copy read)
class Subscriber {
    std::string          topic_;
    SharedMemorySegment& shm_;
    size_t               last_offset_ = 0;
    size_t               last_size_ = 0;
    bool                 has_data_ = false;

public:
    Subscriber(std::string topic, SharedMemorySegment& shm)
        : topic_(std::move(topic)), shm_(shm) {}

    void OnNotify(const std::string& topic, size_t offset, size_t size) {
        if (topic == topic_) {
            last_offset_ = offset;
            last_size_ = size;
            has_data_ = true;
        }
    }

    const uint8_t* Take(size_t& out_size) {
        if (!has_data_) { out_size = 0; return nullptr; }
        has_data_ = false;
        out_size = last_size_;
        return shm_.Read(last_offset_);
    }
};

// EN: RouDi (Routing and Discovery daemon) — simplified
class RouDi {
    SharedMemorySegment shm_;
    std::vector<Publisher*>  publishers_;
    std::vector<Subscriber*> subscribers_;

public:
    SharedMemorySegment& GetShm() { return shm_; }

    void RegisterPublisher(Publisher* pub) {
        pub->SetNotifyCallback([this](const std::string& topic, size_t off, size_t sz) {
            for (auto* sub : subscribers_) {
                sub->OnNotify(topic, off, sz);
            }
        });
        publishers_.push_back(pub);
    }

    void RegisterSubscriber(Subscriber* sub) {
        subscribers_.push_back(sub);
    }
};

}  // namespace iceoryx_sim

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Mini Pub/Sub Middleware with Topic Routing + QoS
// ═════════════════════════════════════════════════════════════════════════════

namespace mini_middleware {

enum class QoSReliability : uint8_t { BEST_EFFORT, RELIABLE };

struct TopicMessage {
    std::string              topic;
    std::vector<uint8_t>     data;
    uint64_t                 seq = 0;
};

class MiddlewareBroker {
    struct SubscriberEntry {
        std::string topic;
        QoSReliability qos;
        std::function<void(const TopicMessage&)> callback;
    };

    std::vector<SubscriberEntry>  subscribers_;
    uint64_t global_seq_ = 0;

public:
    void Subscribe(const std::string& topic, QoSReliability qos,
                   std::function<void(const TopicMessage&)> cb) {
        subscribers_.push_back({topic, qos, std::move(cb)});
        std::cout << "  [Broker] Subscribed to '" << topic << "'\n";
    }

    void Publish(const std::string& topic, const std::vector<uint8_t>& data) {
        ++global_seq_;
        TopicMessage msg{topic, data, global_seq_};
        for (auto& sub : subscribers_) {
            if (sub.topic == topic) {
                // EN: Simulate BEST_EFFORT having 20% drop chance
                if (sub.qos == QoSReliability::BEST_EFFORT && (global_seq_ % 5 == 0)) {
                    std::cout << "    [Broker] DROPPED (best-effort) seq=" << global_seq_ << "\n";
                    continue;
                }
                sub.callback(msg);
            }
        }
    }
};

}  // namespace mini_middleware

// ═════════════════════════════════════════════════════════════════════════════
// MAIN — Demos
// ═════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n"
              << "║ Module 17: Middleware — SOME/IP, DDS, iceoryx                ║\n"
              << "╚══════════════════════════════════════════════════════════════╝\n\n";

    // --- Demo 1: SOME/IP Message Exchange ---
    std::cout << "--- Demo 1: SOME/IP — Request/Response + Notification ---\n";
    {
        // EN: Create a SOME/IP request for GetVehicleSpeed (service=0x1234, method=0x0001)
        std::vector<uint8_t> req_payload = {0x00};  // empty request
        auto request = someip::SomeIpMessage::CreateRequest(
            0x1234, 0x0001, 100, 1, req_payload);
        std::cout << "  Request:\n";
        request.header.Print();

        // EN: Server processes and responds with speed=120 km/h
        std::vector<uint8_t> resp_payload = {0x00, 0x78};  // 120 in big-endian 16-bit
        auto response = someip::SomeIpMessage::CreateResponse(request, resp_payload);
        std::cout << "  Response:\n";
        response.header.Print();
        uint16_t speed = static_cast<uint16_t>((resp_payload[0] << 8) | resp_payload[1]);
        std::cout << "    Payload: speed = " << speed << " km/h\n";

        // EN: Event notification (speed changed)
        std::vector<uint8_t> event_data = {0x00, 0x82};  // 130 km/h
        auto notif = someip::SomeIpMessage::CreateNotification(0x1234, 0x8001, event_data);
        std::cout << "  Notification:\n";
        notif.header.Print();
    }

    // --- Demo 2: SOME/IP Service Discovery ---
    std::cout << "\n--- Demo 2: SOME/IP Service Discovery ---\n";
    {
        someip::ServiceDiscovery sd;
        sd.OfferService(0x1234, 1, "192.168.1.10:30490");
        sd.OfferService(0x5678, 1, "192.168.1.20:30491");
        sd.OfferService(0xABCD, 2, "192.168.1.30:30492");

        auto found = sd.FindService(0x5678);
        if (found) {
            std::cout << "    Found: service=0x" << std::hex << found->service_id
                      << " at " << found->provider_endpoint << std::dec << "\n";
        }

        auto not_found = sd.FindService(0x9999);
        std::cout << "    FindService(0x9999): "
                  << (not_found ? "found" : "NOT_FOUND") << "\n";
    }

    // --- Demo 3: DDS Topic-Based Pub/Sub ---
    std::cout << "\n--- Demo 3: DDS — Topic Pub/Sub with QoS ---\n";
    {
        dds::DomainParticipant participant("Vehicle");

        dds::QoSPolicy writer_qos{dds::Reliability::RELIABLE, dds::Durability::VOLATILE, 100, 1};
        dds::DataWriter writer("VehicleSpeed", writer_qos);

        dds::QoSPolicy reader_qos{dds::Reliability::RELIABLE, dds::Durability::VOLATILE, 100, 5};
        dds::DataReader reader1("VehicleSpeed", reader_qos);
        dds::DataReader reader2("VehicleSpeed", reader_qos);

        participant.RegisterWriter(&writer);
        participant.RegisterReader(&reader1);
        participant.RegisterReader(&reader2);

        writer.GetQoS().Print();

        // EN: Publish 3 speed values
        for (uint32_t spd : {60u, 80u, 120u}) {
            std::vector<uint8_t> data(4);
            std::memcpy(data.data(), &spd, 4);
            writer.Write(data);
        }

        // EN: Check reader state
        auto sample = reader1.TakeLast();
        if (sample) {
            uint32_t received_speed = 0;
            std::memcpy(&received_speed, sample->data.data(), 4);
            std::cout << "  Reader1 last sample: speed=" << received_speed
                      << " km/h, seq=" << sample->sequence_number << "\n";
        }
        std::cout << "  Reader1 samples buffered: " << reader1.SampleCount() << "\n";
        std::cout << "  Reader2 samples buffered: " << reader2.SampleCount() << "\n";
    }

    // --- Demo 4: iceoryx Zero-Copy ---
    std::cout << "\n--- Demo 4: iceoryx — Zero-Copy Shared Memory IPC ---\n";
    {
        iceoryx_sim::RouDi roudi;

        iceoryx_sim::Publisher  pub("LidarPointCloud", roudi.GetShm());
        iceoryx_sim::Subscriber sub("LidarPointCloud", roudi.GetShm());

        roudi.RegisterPublisher(&pub);
        roudi.RegisterSubscriber(&sub);

        // EN: Simulate publishing a pointcloud (1KB)
        struct PointCloud {
            uint32_t num_points;
            float    min_range;
            float    max_range;
        };
        PointCloud cloud{1024, 0.5f, 100.0f};
        pub.Publish(&cloud, sizeof(cloud));

        // EN: Subscriber reads — zero copy (pointer into shared memory)
        size_t sz = 0;
        const auto* ptr = sub.Take(sz);
        if (ptr != nullptr) {
            PointCloud received{};
            std::memcpy(&received, ptr, sizeof(received));
            std::cout << "    [Sub] Received: points=" << received.num_points
                      << " range=[" << received.min_range << ", " << received.max_range << "]\n";
            std::cout << "    Zero-copy: data read directly from shared memory "
                      << "(no memcpy between processes in real iceoryx)\n";
        }
    }

    // --- Demo 5: Mini Middleware Broker with QoS ---
    std::cout << "\n--- Demo 5: Mini Middleware Broker (Topic + QoS) ---\n";
    {
        mini_middleware::MiddlewareBroker broker;

        int reliable_count = 0;
        int best_effort_count = 0;

        broker.Subscribe("radar/objects", mini_middleware::QoSReliability::RELIABLE,
            [&](const mini_middleware::TopicMessage& m) {
                ++reliable_count;
                std::cout << "    [RELIABLE] Received seq=" << m.seq << "\n";
            });

        broker.Subscribe("radar/objects", mini_middleware::QoSReliability::BEST_EFFORT,
            [&](const mini_middleware::TopicMessage& m) {
                ++best_effort_count;
                std::cout << "    [BEST_EFFORT] Received seq=" << m.seq << "\n";
            });

        std::cout << "  Publishing 10 messages:\n";
        for (int i = 0; i < 10; ++i) {
            broker.Publish("radar/objects", {static_cast<uint8_t>(i)});
        }

        std::cout << "  Reliable received: " << reliable_count << "/10\n";
        std::cout << "  Best-effort received: " << best_effort_count << "/10\n";
    }

    // --- Demo 6: Middleware Comparison Matrix ---
    std::cout << "\n--- Demo 6: Middleware Selection Guide ---\n";
    {
        struct MWEntry { std::string_view name; std::string_view best_for; std::string_view latency; };
        MWEntry entries[] = {
            {"SOME/IP (vsomeip)",   "AUTOSAR Classic/Adaptive, standard automotive", "~100us"},
            {"DDS (Fast-DDS)",      "ADAS, robotics, pub/sub heavy workloads",       "~50us"},
            {"iceoryx",             "Intra-SoC zero-copy (lidar/camera data)",       "<1us"},
            {"gRPC",                "Cloud backend, REST-like services",              "~500us"},
            {"ROS 2 (rmw_dds)",     "ADAS prototyping, ROS ecosystem",               "~50us"},
        };
        std::cout << "  ┌────────────────────┬────────────────────────────────────────────────┬─────────┐\n";
        std::cout << "  │ Middleware         │ Best For                                       │ Latency │\n";
        std::cout << "  ├────────────────────┼────────────────────────────────────────────────┼─────────┤\n";
        for (const auto& e : entries) {
            std::cout << "  │ " << std::setw(18) << std::left << e.name
                      << " │ " << std::setw(46) << e.best_for
                      << " │ " << std::setw(7) << e.latency << " │\n";
        }
        std::cout << std::right;
        std::cout << "  └────────────────────┴────────────────────────────────────────────────┴─────────┘\n";
    }

    std::cout << "\n[All demos completed successfully.]\n";
    return 0;
}

