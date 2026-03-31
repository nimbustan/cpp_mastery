/**
 * @file module_13_networking/01_socket_fundamentals.cpp
 * @brief Network Programming: BSD Sockets, TCP/UDP — Ağ Programlama: BSD Soketleri, TCP/UDP
 *
 * @details
 * =============================================================================
 * [THEORY: What Is a Socket? / TEORİ: Soket Nedir?]
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
 * TR: Soket, ağ üzerinden veri göndermek/almak için bir uç noktadır.
 *     Telefon gibi düşün: konuşmak için her iki uçta da birer tane gerekli.
 *
 *     Soket API'si (BSD soketleri) 1983'ten kalma bir C arayüzüdür ve
 *     web tarayıcılarından oyun sunucularına kadar bugün hâlâ TÜM ağ
 *     iletişimini güçlendirir. *
 *     Temel kavramlar:
 *     ┌──────────────────┬────────────────────────────────────────────────┐
 *     │ Kavram           │ Açıklama                                       │
 *     ├──────────────────┼────────────────────────────────────────────────┤
 *     │ IP Adresi        │ Ağdaki makine adresi (IPv4: x.x.x.x)           │
 *     │ Port             │ Servis tanımlayıcısı (0-65535)                 │
 *     │ Soket            │ IP + Port + Protokol = benzersiz uç nokta      │
 *     │ Protokol         │ TCP (güvenilir akış) veya UDP (hızlı datagram) │
 *     │ Dosya Tanımlayıcı│ socket() tarafından döndürülen tam sayı tutamak│
 *     └──────────────────┴────────────────────────────────────────────────┘ *
 * =============================================================================
 * [THEORY: TCP vs UDP / TEORİ: TCP ve UDP KARŞILAŞTIRMASI]
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
 * TR:
 * ┌─────────────────┬──────────────────────┬───────────────────────────┐
 * │ Özellik         │ TCP                  │ UDP                       │
 * ├─────────────────┼──────────────────────┼───────────────────────────┤
 * │ Bağlantı        │ Bağlantı yönelimli   │ Bağlantısız               │
 * │ Güvenilirlik    │ Teslim garantili     │ En iyi çaba (kaybolabilir)│
 * │ Sıralama        │ Sıralı               │ Sıralama garantisi yok    │
 * │ Hız             │ Yavaş (el sıkışma)   │ Hızlı (el sıkışma yok)    │
 * │ Kullanım        │ HTTP, dosya transfer │ DNS, video, oyunlar       │
 * │ Benzetme        │ Telefon görüşmesi    │ Kartpostal göndermek      │
 * └─────────────────┴──────────────────────┴───────────────────────────┘
 *
 * =============================================================================
 * [THEORY: TCP Server Flow / TEORİ: TCP Sunucu Akışı]
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
 * TR: Klasik TCP sunucu yaşam döngüsü ("soket dansı"):
 *     1. socket()    → soket oluştur
 *     2. bind()      → adres/port ata
 *     3. listen()    → pasif olarak işaretle (bağlantı kabul edecek)
 *     4. accept()    → istemci bağlanana kadar BEKLE    ←── connect()
 *     5. recv/send   → veri alışverişi                  ←→  recv/send
 *     6. close()     → kapat                            ←── close()
 *
 *     ÖNEMLİ RAII notu: C'de close() unutmak dosya tanımlayıcılarını SIZDIRIR.
 *     Bu dosyada fd'yi güvenlik için RAII sınıfında sarıyoruz.
 *
 * =============================================================================
 * [THEORY: Byte Order / TEORİ: Bayt Sırası]
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
 * TR: Ağ bayt sırası BIG-ENDIAN (en anlamlı bayt önce).
 *     x86 CPU'lar LITTLE-ENDIAN kullanır (en az anlamlı bayt önce).
 *     Dönüşüm fonksiyonları:
 *       htons() / htonl() → Host'TAN Ağ'A (kısa/uzun)
 *       ntohs() / ntohl() → Ağ'DAN Host'A (kısa/uzun)
 *     Port numaraları ve IP adresleri için HER ZAMAN bunları kullan!
 *
 * @see https://man7.org/linux/man-pages/man2/socket.2.html
 * @see https://man7.org/linux/man-pages/man7/tcp.7.html
 * @see https://beej.org/guide/bgnet/ — "Beej's Guide to Network Programming"
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 01_socket_fundamentals.cpp -o 01_socket_fundamentals
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>
#include <cstring>       // EN: memset, strerror / TR: memset, strerror
#include <array>
#include <vector>
#include <thread>
#include <chrono>

// ── POSIX headers for socket programming ──
// EN: These are C headers from the POSIX standard — available on Linux/macOS.
//     On Windows, you'd use <winsock2.h> and <ws2tcpip.h> instead.
// TR: Bunlar POSIX standardından C başlıkları — Linux/macOS'ta mevcuttur.
//     Windows'ta <winsock2.h> ve <ws2tcpip.h> kullanılır.
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
 * TR: Soket dosya tanımlayıcısı için RAII sarmalayıcı.
 *     C'de socket() bir int (dosya tanımlayıcısı) döndürür. close() unutulursa
 *     fd sızar. Bu sarmalayıcı nesne scope'dan çıkınca close()'un otomatik
 *     çağrılmasını garanti eder — bellek için unique_ptr gibi.
 */
class SocketRAII {
public:
    // EN: Default constructor — no socket yet
    // TR: Varsayılan constructor — henüz soket yok
    SocketRAII() : fd_(-1) {}

    // EN: Construct from an existing file descriptor
    // TR: Mevcut dosya tanımlayıcısından oluştur
    explicit SocketRAII(int fd) : fd_(fd) {}

    // EN: Destructor — close the socket if valid
    // TR: Destructor — geçerliyse soketi kapat
    ~SocketRAII() {
        if (fd_ >= 0) {
            ::close(fd_);
            // EN: We use ::close (global scope) to avoid naming conflicts
            // TR: İsimlendirme çakışmasını önlemek için ::close (global scope) kullanıyoruz
        }
    }

    // EN: Move only — ownership semantics (like unique_ptr)
    // TR: Sadece taşınabilir — sahiplik semantiği (unique_ptr gibi)
    SocketRAII(const SocketRAII&) = delete;
    SocketRAII& operator=(const SocketRAII&) = delete;

    SocketRAII(SocketRAII&& other) noexcept : fd_(other.fd_) {
        other.fd_ = -1;  // EN: Source loses ownership / TR: Kaynak sahipliği kaybeder
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
    // TR: Ham dosya tanımlayıcısını al (C API'leri ile kullanmak için)
    int get() const { return fd_; }

    // EN: Check if the socket is valid
    // TR: Soketin geçerli olup olmadığını kontrol et
    bool valid() const { return fd_ >= 0; }

    // EN: Release ownership without closing (rare, but sometimes needed)
    // TR: Kapatmadan sahipliği bırak (nadir ama bazen gerekli)
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
 * TR: Basit bir TCP echo (yankı) sunucusu:
 *     1. Verilen portta dinler
 *     2. BİR istemci kabul eder
 *     3. Mesaj okur
 *     4. Geri gönderir (yankı)
 *     5. Kapatır
 */
void run_tcp_echo_server(uint16_t port) {
    std::cout << "  [SERVER] Starting TCP echo server on port " << port << "...\n";

    // ── Step 1: socket() — Create the socket ──
    // EN: AF_INET     = IPv4 address family
    //     SOCK_STREAM = TCP (reliable, ordered, byte-stream)
    //     0           = Let OS choose protocol (TCP for SOCK_STREAM)
    // TR: AF_INET     = IPv4 adres ailesi
    //     SOCK_STREAM = TCP (güvenilir, sıralı, bayt akışı)
    //     0           = İşletim sistemi protokolü seçsin (SOCK_STREAM için TCP)
    SocketRAII server_socket(::socket(AF_INET, SOCK_STREAM, 0));
    if (!server_socket.valid()) {
        std::cerr << "  [SERVER] socket() failed: " << strerror(errno) << "\n";
        return;
    }

    // EN: SO_REUSEADDR allows reusing the port immediately after server stops.
    //     Without this, bind() fails with "Address already in use" for ~60 seconds
    //     after stopping the server (due to TCP TIME_WAIT state).
    // TR: SO_REUSEADDR sunucu durduktan hemen sonra portu tekrar kullanmaya izin verir.
    //     Bu olmadan bind() ~60 saniye "Address already in use" ile başarısız olur
    //     (TCP TIME_WAIT durumu yüzünden).
    int opt = 1;
    setsockopt(server_socket.get(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // ── Step 2: bind() — Assign address and port ──
    // EN: sockaddr_in is the IPv4 address structure:
    //     sin_family = AF_INET (IPv4)
    //     sin_port   = port in NETWORK byte order (htons converts host→network)
    //     sin_addr   = IP address (INADDR_LOOPBACK = 127.0.0.1 = localhost)
    // TR: sockaddr_in IPv4 adres yapısıdır:
    //     sin_family = AF_INET (IPv4)
    //     sin_port   = AĞ bayt sırasında port (htons host→ağ dönüştürür)
    //     sin_addr   = IP adresi (INADDR_LOOPBACK = 127.0.0.1 = yerel makine)
    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);  // EN: Host-to-network byte order! / TR: Host→ağ bayt sırası!
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
    // TR: Backlog (5) bekleyen bağlantıların maksimum kuyruğudur.
    //     5 beklerken 6. istemci bağlanmaya çalışırsa ECONNREFUSED alır.
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
    // TR: accept() bir istemci connect() çağırana kadar BLOKE olur.
    //     Bu belirli konuşma için YENİ bir soket fd döndürür.
    //     Orijinal soket diğer istemcileri dinlemeye devam eder.
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
    // TR: İstemci IP'sini ikili formattan okunabilir string'e dönüştür
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    std::cout << "  [SERVER] Client connected from " << client_ip
              << ":" << ntohs(client_addr.sin_port) << "\n";

    // ── Step 5: recv() — Read data from client ──
    // EN: recv() reads up to buffer_size bytes from the client socket.
    //     Returns the number of bytes actually received, or 0 if client closed, or -1 on error.
    //     IMPORTANT: recv() may return LESS data than sent — TCP is a BYTE STREAM,
    //     not a message protocol. In production, loop until you have the full message.
    // TR: recv() istemci soketinden buffer_size bayta kadar okur.
    //     Gerçekten alınan bayt sayısını döndürür, istemci kapattıysa 0, hatada -1.
    //     ÖNEMLİ: recv() gönderilenden AZ veri döndürebilir — TCP BAYT AKIŞIDIR,
    //     mesaj protokolü değil. Üretimde tam mesaj alana kadar döngü kullan.
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
    // TR: send() sokete veri yazar. Gönderilen baytları veya hatada -1 döndürür.
    //     recv() gibi, tek çağrıda istenenin AZINI gönderebilir.
    std::string response = "ECHO: " + std::string(buffer.data());
    ssize_t bytes_sent = ::send(client_socket.get(), response.c_str(),
                                response.size(), 0);
    std::cout << "  [SERVER] Sent " << bytes_sent << " bytes back.\n";

    // EN: SocketRAII destructors close both sockets automatically here
    // TR: SocketRAII destructor'ları her iki soketi de burada otomatik kapatır
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
 * TR: Echo sunucusuna bağlanan, mesaj gönderen ve yankılanan yanıtı alan TCP istemci.
 *     İstemci akışı sunucudan daha basit:
 *     socket() → connect() → send() → recv() → close()
 */
void run_tcp_client(uint16_t port, const std::string& message) {
    std::cout << "  [CLIENT] Connecting to 127.0.0.1:" << port << "...\n";

    // EN: Create a TCP socket (same as server)
    // TR: TCP soketi oluştur (sunucuyla aynı)
    SocketRAII client_socket(::socket(AF_INET, SOCK_STREAM, 0));
    if (!client_socket.valid()) {
        std::cerr << "  [CLIENT] socket() failed: " << strerror(errno) << "\n";
        return;
    }

    // EN: Set up server address to connect to
    // TR: Bağlanılacak sunucu adresini ayarla
    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // EN: connect() initiates the TCP 3-way handshake:
    //     Client → SYN → Server
    //     Client ← SYN-ACK ← Server
    //     Client → ACK → Server
    //     After this, the connection is established and data can flow.
    // TR: connect() TCP 3'lü el sıkışmasını başlatır:
    //     İstemci → SYN → Sunucu
    //     İstemci ← SYN-ACK ← Sunucu
    //     İstemci → ACK → Sunucu
    //     Bundan sonra bağlantı kurulur ve veri akabilir.
    if (::connect(client_socket.get(),
                  reinterpret_cast<struct sockaddr*>(&server_addr),
                  sizeof(server_addr)) < 0) {
        std::cerr << "  [CLIENT] connect() failed: " << strerror(errno) << "\n";
        return;
    }
    std::cout << "  [CLIENT] Connected!\n";

    // EN: Send the message
    // TR: Mesajı gönder
    ::send(client_socket.get(), message.c_str(), message.size(), 0);
    std::cout << "  [CLIENT] Sent: " << message << "\n";

    // EN: Receive the echo response
    // TR: Yankı yanıtını al
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
 * TR: UDP bağlantısızdır — connect() yok, accept() yok, el sıkışma yok.
 *     Her seferinde hedef adresle sendto() ve recvfrom().
 *     Daha hızlı ama güvenilmez: paketler kaybolabilir, çoğaltılabilir, yeniden sıralanabilir.
 *
 *     Kullanım: DNS sorguları, gerçek zamanlı video, oyun durum güncellemeleri, sensör telemetrisi
 */
void run_udp_receiver(uint16_t port) {
    std::cout << "  [UDP-RX] Starting UDP receiver on port " << port << "...\n";

    // EN: SOCK_DGRAM = UDP (datagram-based, connectionless)
    // TR: SOCK_DGRAM = UDP (datagram tabanlı, bağlantısız)
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
    // TR: recvfrom() bir datagram gelene kadar bloke olur.
    //     TCP'den farklı olarak, her recvfrom() TAM BİR datagram döndürür (mesaj sınırı korunur).
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
 * TR: UDP gönderici — alıcıya tek bir datagram gönderir.
 *     Bağlantı kurulumu gerekmez. Sadece gönder ve unut.
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
    // TR: sendto() datagramı dest_addr'a gönderir. connect() gerekmez!
    //     Tüm mesaj TEK datagram olarak gönderilir (IPv4 için 65507 bayta kadar).
    //     UDP ya TÜMÜNÜ gönderir ya da HİÇBİRİNİ — asla kısmi değil.
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
 * TR: Bayt sırası dönüşüm fonksiyonlarını ve neden önemli olduklarını gösterir.
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
    // TR: Bayt sırasını görünür kılmak için bireysel baytları göster
    auto* host_bytes = reinterpret_cast<const uint8_t*>(&host_port);
    auto* net_bytes = reinterpret_cast<const uint8_t*>(&net_port);
    std::cout << "    Host bytes: [" << static_cast<int>(host_bytes[0]) << "]["
              << static_cast<int>(host_bytes[1]) << "]\n";
    std::cout << "    Net bytes:  [" << static_cast<int>(net_bytes[0]) << "]["
              << static_cast<int>(net_bytes[1]) << "]\n";

    // EN: On x86 (little-endian), host_port bytes are [0x90, 0x1F]
    //     After htons(), net_port bytes are [0x1F, 0x90] (big-endian)
    //     If you forget htons(): port 8080 looks like port 36895 on the network!
    // TR: x86'da (little-endian), host_port baytları [0x90, 0x1F]
    //     htons() sonrası, net_port baytları [0x1F, 0x90] (big-endian)
    //     htons() unutulursa: port 8080 ağda port 36895 gibi görünür!

    uint32_t ip_addr = 0xC0A80001;  // EN: 192.168.0.1 / TR: 192.168.0.1
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
 * TR: Güvenli IP adresi dönüşüm fonksiyonlarını gösterir:
 *     inet_pton() → "gösterimden sayısala" (string → ikili)
 *     inet_ntop() → "sayısaldan gösterime" (ikili → string)
 *     ASLA inet_addr() veya inet_aton() kullanma — hataları düzgün yakalayamaz.
 */
void demo_address_conversion() {
    // EN: Convert human-readable IP string to binary
    // TR: Okunabilir IP string'ini ikiliye dönüştür
    struct in_addr addr{};
    const char* ip_str = "192.168.1.42";

    if (inet_pton(AF_INET, ip_str, &addr) == 1) {
        std::cout << "  inet_pton(\"" << ip_str << "\") → binary 0x"
                  << std::hex << ntohl(addr.s_addr) << std::dec << "\n";
    }

    // EN: Convert binary back to human-readable string
    // TR: İkiliyi tekrar okunabilir string'e dönüştür
    char buffer[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &addr, buffer, sizeof(buffer))) {
        std::cout << "  inet_ntop(binary) → \"" << buffer << "\"\n";
    }

    // EN: Try an invalid IP — inet_pton returns 0 (not -1!)
    // TR: Geçersiz IP dene — inet_pton 0 döndürür (-1 değil!)
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
        // TR: Sunucuyu bir thread'de çalıştırıp dinlemeye başlaması için bir an veriyoruz,
        //     sonra istemciyi ana thread'de çalıştırıyoruz.
        //
        //     Port seçimi: root gerektirmeyen yüksek port (> 1024) kullanıyoruz.
        constexpr uint16_t TCP_PORT = 19850;

        std::thread server_thread(run_tcp_echo_server, TCP_PORT);

        // EN: Small delay so server has time to bind() and listen()
        // TR: Sunucunun bind() ve listen() yapması için küçük gecikme
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
        // TR: Alıcıyı thread'de başlat, sonra datagram gönder
        std::thread receiver_thread(run_udp_receiver, UDP_PORT);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        run_udp_sender(UDP_PORT, "ECU_TEMP:85.3:NORMAL");

        receiver_thread.join();

        // EN: Note: No connection was established! Pure fire-and-forget.
        //     In real systems, UDP is used for telemetry where occasional
        //     packet loss is acceptable (better than TCP's latency overhead).
        // TR: Not: Bağlantı kurulmadı! Saf gönder-ve-unut.
        //     Gerçek sistemlerde UDP, arada paket kaybının kabul edilebilir
        //     olduğu telemetri için kullanılır (TCP'nin gecikme yükünden daha iyi).
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
        // TR: sockaddr_in 16 bayttır:
        //     sin_family (2) + sin_port (2) + sin_addr (4) + sin_zero (8) = 16
        //     Genel sockaddr yapısının boyutuyla eşleşmek için doldurulur.
        std::cout << "\n";
    }

    std::cout << "============================================\n";
    std::cout << "   End of Module 13 - Socket Fundamentals\n";
    std::cout << "============================================\n";

    return 0;
}
