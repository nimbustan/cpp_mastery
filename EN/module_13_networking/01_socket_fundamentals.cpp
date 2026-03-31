/**
 * @file module_13_networking/01_socket_fundamentals.cpp
 * @brief Network Programming: BSD Sockets, TCP/UDP
 *
 * @details
 * =============================================================================
 * =============================================================================
 *
 * EN: A socket is an endpoint for sending/receiving data across a network.
 *     Think of it like a phone: you need one on each end to have a conversation.
 *
 *     The socket API (BSD sockets) is a C interface from 1983 that still powers
 *     virtually ALL network communication today — from web browsers to game servers.
 *
 *     Key concepts:
 *     ┌──────────────────┬───────────────────────────────────────────────┐
 *     │ Concept          │ Description                                   │
 *     ├──────────────────┼───────────────────────────────────────────────┤
 *     │ IP Address       │ Machine address on network (IPv4: x.x.x.x)    │
 *     │ Port             │ Service identifier (0-65535)                  │
 *     │ Socket           │ IP + Port + Protocol = unique endpoint        │
 *     │ Protocol         │ TCP (reliable stream) or UDP (fast datagram)  │
 *     │ File Descriptor  │ Integer handle returned by socket()           │
 *     └──────────────────┴───────────────────────────────────────────────┘
 *
 * =============================================================================
 * =============================================================================
 *
 * EN:
 * ┌─────────────────┬──────────────────────┬──────────────────────────┐
 * │ Feature         │ TCP                  │ UDP                      │
 * ├─────────────────┼──────────────────────┼──────────────────────────┤
 * │ Connection      │ Connection-oriented  │ Connectionless           │
 * │ Reliability     │ Guaranteed delivery  │ Best-effort (may lose)   │
 * │ Ordering        │ Ordered              │ No ordering guarantee    │
 * │ Speed           │ Slower (handshake)   │ Faster (no handshake)    │
 * │ Use case        │ HTTP, file transfer  │ DNS, video stream, games │
 * │ Analogy         │ Phone call           │ Sending postcards        │
 * │ Overhead        │ Higher (ACK, retx)   │ Minimal (just send)      │
 * │ C++ socket type │ SOCK_STREAM          │ SOCK_DGRAM               │
 * └─────────────────┴──────────────────────┴──────────────────────────┘
 *
 * =============================================================================
 * =============================================================================
 *
 * EN: The classic TCP server lifecycle (a.k.a. "the socket dance"):
 *
 *     SERVER                          CLIENT
 *     ──────                          ──────
 *     1. socket()    → create socket
 *     2. bind()      → assign address/port
 *     3. listen()    → mark as passive (accepting connections)
 *     4. accept()    → BLOCK until client connects     ←── connect()
 *     5. recv/send   → data exchange                   ←→  recv/send
 *     6. close()     → shutdown                        ←── close()
 *
 *     IMPORTANT RAII note: In C, forgetting close() LEAKS file descriptors.
 *     In this file, we wrap the fd in a RAII class for safety.
 *
 * =============================================================================
 * =============================================================================
 *
 * EN: Network byte order is BIG-ENDIAN (most significant byte first).
 *     x86 CPUs use LITTLE-ENDIAN (least significant byte first).
 *     Conversion functions:
 *       htons() / htonl() → Host TO Network (short/long)
 *       ntohs() / ntohl() → Network TO Host (short/long)
 *     ALWAYS use these for port numbers and IP addresses!
 *     Forgetting byte order conversion = silent data corruption.
 *
 */

#include <iostream>
#include <string>
#include <cstring>       // EN: memset, strerror
#include <array>
#include <vector>
#include <thread>
#include <chrono>

// ── POSIX headers for socket programming ──
// EN: These are C headers from the POSIX standard — available on Linux/macOS.
//     On Windows, you'd use <winsock2.h> and <ws2tcpip.h> instead.
#include <sys/socket.h>  // EN: socket(), bind(), listen(), accept(), connect()
#include <netinet/in.h>  // EN: sockaddr_in, INADDR_LOOPBACK, htons()
#include <arpa/inet.h>   // EN: inet_ntop(), inet_pton() — IP address conversion
#include <unistd.h>      // EN: close() — close file descriptors

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: RAII Socket Wrapper
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: RAII wrapper for a socket file descriptor.
 *     In C, socket() returns an int (file descriptor). If you forget to close()
 *     it, you leak the fd. This wrapper ensures close() is called automatically
 *     when the object goes out of scope — exactly like unique_ptr for memory.
 *
 *     Rule: NEVER use raw `int fd` for sockets in C++ code. Always wrap in RAII.
 *
 */
class SocketRAII {
public:
    // EN: Default constructor — no socket yet
    SocketRAII() : fd_(-1) {}

    // EN: Construct from an existing file descriptor
    explicit SocketRAII(int fd) : fd_(fd) {}

    // EN: Destructor — close the socket if valid
    ~SocketRAII() {
        if (fd_ >= 0) {
            ::close(fd_);
            // EN: We use ::close (global scope) to avoid naming conflicts
        }
    }

    // EN: Move only — ownership semantics (like unique_ptr)
    SocketRAII(const SocketRAII&) = delete;
    SocketRAII& operator=(const SocketRAII&) = delete;

    SocketRAII(SocketRAII&& other) noexcept : fd_(other.fd_) {
        other.fd_ = -1;  // EN: Source loses ownership
    }
    SocketRAII& operator=(SocketRAII&& other) noexcept {
        if (this != &other) {
            if (fd_ >= 0) ::close(fd_);
            fd_ = other.fd_;
            other.fd_ = -1;
        }
        return *this;
    }

    // EN: Get the raw file descriptor (for use with C APIs)
    int get() const { return fd_; }

    // EN: Check if the socket is valid
    bool valid() const { return fd_ >= 0; }

    // EN: Release ownership without closing (rare, but sometimes needed)
    int release() {
        int temp = fd_;
        fd_ = -1;
        return temp;
    }

private:
    int fd_;
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: TCP Echo Server (single-client, educational)
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: A simple TCP echo server that:
 *     1. Listens on a given port
 *     2. Accepts ONE client
 *     3. Reads a message
 *     4. Sends it back (echo)
 *     5. Closes
 *
 *     This function demonstrates every step of the "socket dance":
 *     socket() → bind() → listen() → accept() → recv() → send() → close()
 *
 */
void run_tcp_echo_server(uint16_t port) {
    std::cout << "  [SERVER] Starting TCP echo server on port " << port << "...\n";

    // ── Step 1: socket() — Create the socket ──
    // EN: AF_INET     = IPv4 address family
    //     SOCK_STREAM = TCP (reliable, ordered, byte-stream)
    //     0           = Let OS choose protocol (TCP for SOCK_STREAM)
    SocketRAII server_socket(::socket(AF_INET, SOCK_STREAM, 0));
    if (!server_socket.valid()) {
        std::cerr << "  [SERVER] socket() failed: " << strerror(errno) << "\n";
        return;
    }

    // EN: SO_REUSEADDR allows reusing the port immediately after server stops.
    //     Without this, bind() fails with "Address already in use" for ~60 seconds
    //     after stopping the server (due to TCP TIME_WAIT state).
    int opt = 1;
    setsockopt(server_socket.get(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // ── Step 2: bind() — Assign address and port ──
    // EN: sockaddr_in is the IPv4 address structure:
    //     sin_family = AF_INET (IPv4)
    //     sin_port   = port in NETWORK byte order (htons converts host→network)
    //     sin_addr   = IP address (INADDR_LOOPBACK = 127.0.0.1 = localhost)
    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);  // EN: Host-to-network byte order!
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (::bind(server_socket.get(),
               reinterpret_cast<struct sockaddr*>(&server_addr),
               sizeof(server_addr)) < 0) {
        std::cerr << "  [SERVER] bind() failed: " << strerror(errno) << "\n";
        return;
    }

    // ── Step 3: listen() — Mark socket as passive (ready to accept) ──
    // EN: The backlog (5) is the maximum queue of pending connections.
    //     If 6th client tries to connect while 5 are waiting, it gets ECONNREFUSED.
    if (::listen(server_socket.get(), 5) < 0) {
        std::cerr << "  [SERVER] listen() failed: " << strerror(errno) << "\n";
        return;
    }
    std::cout << "  [SERVER] Listening on 127.0.0.1:" << port << " (waiting for client...)\n";

    // ── Step 4: accept() — Wait for a client to connect ──
    // EN: accept() BLOCKS until a client calls connect().
    //     It returns a NEW socket fd for this specific conversation.
    //     The original socket continues to listen for other clients.
    //     client_addr is filled with the client's IP/port info.
    struct sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    SocketRAII client_socket(
        ::accept(server_socket.get(),
                 reinterpret_cast<struct sockaddr*>(&client_addr),
                 &client_len)
    );

    if (!client_socket.valid()) {
        std::cerr << "  [SERVER] accept() failed: " << strerror(errno) << "\n";
        return;
    }

    // EN: Convert client IP from binary to human-readable string
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    std::cout << "  [SERVER] Client connected from " << client_ip
              << ":" << ntohs(client_addr.sin_port) << "\n";

    // ── Step 5: recv() — Read data from client ──
    // EN: recv() reads up to buffer_size bytes from the client socket.
    //     Returns the number of bytes actually received, or 0 if client closed, or -1 on error.
    //     IMPORTANT: recv() may return LESS data than sent — TCP is a BYTE STREAM,
    //     not a message protocol. In production, loop until you have the full message.
    std::array<char, 1024> buffer{};
    ssize_t bytes_received = ::recv(client_socket.get(), buffer.data(),
                                    buffer.size() - 1, 0);  // -1 for null-terminator space

    if (bytes_received <= 0) {
        std::cout << "  [SERVER] Client disconnected or error.\n";
        return;
    }

    buffer[static_cast<size_t>(bytes_received)] = '\0';
    std::cout << "  [SERVER] Received (" << bytes_received << " bytes): " << buffer.data() << "\n";

    // ── Step 6: send() — Send response back (echo) ──
    // EN: send() writes data to the socket. Returns bytes sent or -1 on error.
    //     Like recv(), may send LESS than requested in one call.
    //     For large messages, loop until all bytes are sent.
    std::string response = "ECHO: " + std::string(buffer.data());
    ssize_t bytes_sent = ::send(client_socket.get(), response.c_str(),
                                response.size(), 0);
    std::cout << "  [SERVER] Sent " << bytes_sent << " bytes back.\n";

    // EN: SocketRAII destructors close both sockets automatically here
    std::cout << "  [SERVER] Connection closed (RAII cleanup).\n";
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: TCP Client
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: TCP client that connects to the echo server, sends a message,
 *     and receives the echoed response.
 *
 *     Client flow is simpler than server:
 *     socket() → connect() → send() → recv() → close()
 *
 */
void run_tcp_client(uint16_t port, const std::string& message) {
    std::cout << "  [CLIENT] Connecting to 127.0.0.1:" << port << "...\n";

    // EN: Create a TCP socket (same as server)
    SocketRAII client_socket(::socket(AF_INET, SOCK_STREAM, 0));
    if (!client_socket.valid()) {
        std::cerr << "  [CLIENT] socket() failed: " << strerror(errno) << "\n";
        return;
    }

    // EN: Set up server address to connect to
    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // EN: connect() initiates the TCP 3-way handshake:
    //     Client → SYN → Server
    //     Client ← SYN-ACK ← Server
    //     Client → ACK → Server
    //     After this, the connection is established and data can flow.
    if (::connect(client_socket.get(),
                  reinterpret_cast<struct sockaddr*>(&server_addr),
                  sizeof(server_addr)) < 0) {
        std::cerr << "  [CLIENT] connect() failed: " << strerror(errno) << "\n";
        return;
    }
    std::cout << "  [CLIENT] Connected!\n";

    // EN: Send the message
    ::send(client_socket.get(), message.c_str(), message.size(), 0);
    std::cout << "  [CLIENT] Sent: " << message << "\n";

    // EN: Receive the echo response
    std::array<char, 1024> buffer{};
    ssize_t bytes_received = ::recv(client_socket.get(), buffer.data(),
                                    buffer.size() - 1, 0);
    if (bytes_received > 0) {
        buffer[static_cast<size_t>(bytes_received)] = '\0';
        std::cout << "  [CLIENT] Received: " << buffer.data() << "\n";
    }

    std::cout << "  [CLIENT] Done.\n";
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: UDP Sender / Receiver
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: UDP is connectionless — no connect(), no accept(), no handshake.
 *     Just sendto() and recvfrom() with the destination address each time.
 *     This is faster but unreliable: packets can be lost, duplicated, or reordered.
 *
 *     Use cases: DNS lookups, real-time video, game state updates, sensor telemetry
 *
 */
void run_udp_receiver(uint16_t port) {
    std::cout << "  [UDP-RX] Starting UDP receiver on port " << port << "...\n";

    // EN: SOCK_DGRAM = UDP (datagram-based, connectionless)
    SocketRAII udp_socket(::socket(AF_INET, SOCK_DGRAM, 0));
    if (!udp_socket.valid()) {
        std::cerr << "  [UDP-RX] socket() failed\n";
        return;
    }

    int opt = 1;
    setsockopt(udp_socket.get(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in recv_addr{};
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = htons(port);
    recv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (::bind(udp_socket.get(),
               reinterpret_cast<struct sockaddr*>(&recv_addr),
               sizeof(recv_addr)) < 0) {
        std::cerr << "  [UDP-RX] bind() failed: " << strerror(errno) << "\n";
        return;
    }

    // EN: recvfrom() blocks until a datagram arrives.
    //     Unlike TCP, each recvfrom() returns ONE complete datagram (message boundary preserved).
    //     sender_addr is filled with the sender's IP/port.
    struct sockaddr_in sender_addr{};
    socklen_t sender_len = sizeof(sender_addr);
    std::array<char, 1024> buffer{};

    std::cout << "  [UDP-RX] Waiting for datagram...\n";
    ssize_t bytes = ::recvfrom(udp_socket.get(), buffer.data(), buffer.size() - 1, 0,
                               reinterpret_cast<struct sockaddr*>(&sender_addr),
                               &sender_len);
    if (bytes > 0) {
        buffer[static_cast<size_t>(bytes)] = '\0';
        char sender_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &sender_addr.sin_addr, sender_ip, sizeof(sender_ip));
        std::cout << "  [UDP-RX] Received " << bytes << " bytes from "
                  << sender_ip << ":" << ntohs(sender_addr.sin_port)
                  << " → \"" << buffer.data() << "\"\n";
    }
}

/**
 * EN: UDP sender — sends a single datagram to the receiver.
 *     No connection setup needed. Just fire and forget.
 *
 */
void run_udp_sender(uint16_t port, const std::string& message) {
    std::cout << "  [UDP-TX] Sending UDP datagram to port " << port << "...\n";

    SocketRAII udp_socket(::socket(AF_INET, SOCK_DGRAM, 0));
    if (!udp_socket.valid()) {
        std::cerr << "  [UDP-TX] socket() failed\n";
        return;
    }

    struct sockaddr_in dest_addr{};
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    dest_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // EN: sendto() sends the datagram to dest_addr. No connect() needed!
    //     The entire message is sent as ONE datagram (up to 65507 bytes for IPv4).
    //     UDP either sends the WHOLE datagram or NONE — never partial.
    ssize_t bytes_sent = ::sendto(udp_socket.get(), message.c_str(), message.size(), 0,
                                   reinterpret_cast<const struct sockaddr*>(&dest_addr),
                                   sizeof(dest_addr));
    std::cout << "  [UDP-TX] Sent " << bytes_sent << " bytes: \"" << message << "\"\n";
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: Byte Order Demonstration
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Shows byte order conversion functions and why they matter.
 */
void demo_byte_order() {
    uint16_t host_port = 8080;
    uint16_t net_port = htons(host_port);

    std::cout << "  Host byte order (port 8080):\n";
    std::cout << "    host_port = 0x" << std::hex << host_port << std::dec
              << " (" << host_port << ")\n";
    std::cout << "    net_port  = 0x" << std::hex << net_port << std::dec
              << " (" << net_port << ")\n";

    // EN: Show the individual bytes to make byte order visible
    auto* host_bytes = reinterpret_cast<const uint8_t*>(&host_port);
    auto* net_bytes = reinterpret_cast<const uint8_t*>(&net_port);
    std::cout << "    Host bytes: [" << static_cast<int>(host_bytes[0]) << "]["
              << static_cast<int>(host_bytes[1]) << "]\n";
    std::cout << "    Net bytes:  [" << static_cast<int>(net_bytes[0]) << "]["
              << static_cast<int>(net_bytes[1]) << "]\n";

    // EN: On x86 (little-endian), host_port bytes are [0x90, 0x1F]
    //     After htons(), net_port bytes are [0x1F, 0x90] (big-endian)
    //     If you forget htons(): port 8080 looks like port 36895 on the network!

    uint32_t ip_addr = 0xC0A80001;  // EN: 192.168.0.1
    uint32_t net_ip = htonl(ip_addr);
    std::cout << "\n  IP address 192.168.0.1 (0xC0A80001):\n";
    std::cout << "    Host order: 0x" << std::hex << ip_addr << std::dec << "\n";
    std::cout << "    Net order:  0x" << std::hex << net_ip << std::dec << "\n";
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 6: Address Conversion (inet_pton / inet_ntop)
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Demonstrates safe IP address conversion functions:
 *     inet_pton() → "presentation to numeric" (string → binary)
 *     inet_ntop() → "numeric to presentation" (binary → string)
 *     NEVER use inet_addr() or inet_aton() — they can't handle errors properly.
 *
 */
void demo_address_conversion() {
    // EN: Convert human-readable IP string to binary
    struct in_addr addr{};
    const char* ip_str = "192.168.1.42";

    if (inet_pton(AF_INET, ip_str, &addr) == 1) {
        std::cout << "  inet_pton(\"" << ip_str << "\") → binary 0x"
                  << std::hex << ntohl(addr.s_addr) << std::dec << "\n";
    }

    // EN: Convert binary back to human-readable string
    char buffer[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &addr, buffer, sizeof(buffer))) {
        std::cout << "  inet_ntop(binary) → \"" << buffer << "\"\n";
    }

    // EN: Try an invalid IP — inet_pton returns 0 (not -1!)
    struct in_addr bad_addr{};
    int result = inet_pton(AF_INET, "999.888.777.666", &bad_addr);
    std::cout << "  inet_pton(\"999.888.777.666\") → " << result << " (0 = invalid)\n";
}

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 13 - Socket Fundamentals\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: Byte Order ──────────────────────────────────────────────
    {
        std::cout << "--- Demo 1: Byte Order (Network vs Host) ---\n";
        demo_byte_order();
        std::cout << "\n";
    }

    // ─── Demo 2: Address Conversion ──────────────────────────────────────
    {
        std::cout << "--- Demo 2: IP Address Conversion ---\n";
        demo_address_conversion();
        std::cout << "\n";
    }

    // ─── Demo 3: TCP Echo — Server + Client ──────────────────────────────
    {
        std::cout << "--- Demo 3: TCP Echo (Server + Client) ---\n";

        // EN: We run server in a thread, give it a moment to start listening,
        //     then run the client in the main thread.
        //     The server thread receives the message, echoes it, and exits.
        //
        //     Port selection: Using a high port (> 1024) that doesn't need root.
        //     We use a somewhat random port to avoid conflicts with other services.
        //
        constexpr uint16_t TCP_PORT = 19850;

        std::thread server_thread(run_tcp_echo_server, TCP_PORT);

        // EN: Small delay so server has time to bind() and listen()
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        run_tcp_client(TCP_PORT, "Hello from CAN controller module!");

        server_thread.join();
        std::cout << "\n";
    }

    // ─── Demo 4: UDP Datagram ────────────────────────────────────────────
    {
        std::cout << "--- Demo 4: UDP Datagram (Send + Receive) ---\n";

        constexpr uint16_t UDP_PORT = 19851;

        // EN: Start receiver in a thread, then send a datagram
        std::thread receiver_thread(run_udp_receiver, UDP_PORT);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        run_udp_sender(UDP_PORT, "ECU_TEMP:85.3:NORMAL");

        receiver_thread.join();

        // EN: Note: No connection was established! Pure fire-and-forget.
        //     In real systems, UDP is used for telemetry where occasional
        //     packet loss is acceptable (better than TCP's latency overhead).
        std::cout << "\n";
    }

    // ─── Demo 5: Socket Structure Sizes ──────────────────────────────────
    {
        std::cout << "--- Demo 5: Socket Structure Sizes ---\n";
        std::cout << "  sizeof(sockaddr_in):  " << sizeof(sockaddr_in) << " bytes (IPv4)\n";
        std::cout << "  sizeof(sockaddr_in6): " << sizeof(sockaddr_in6) << " bytes (IPv6)\n";
        std::cout << "  sizeof(SocketRAII):   " << sizeof(SocketRAII) << " bytes (our wrapper)\n";

        // EN: sockaddr_in is 16 bytes:
        //     sin_family (2) + sin_port (2) + sin_addr (4) + sin_zero (8) = 16
        //     It's padded to match the size of the generic sockaddr structure.
        std::cout << "\n";
    }

    std::cout << "============================================\n";
    std::cout << "   End of Module 13 - Socket Fundamentals\n";
    std::cout << "============================================\n";

    return 0;
}
