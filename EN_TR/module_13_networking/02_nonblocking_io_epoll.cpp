/**
 * @file module_13_networking/02_nonblocking_io_epoll.cpp
 * @brief Non-Blocking I/O, poll, epoll — Bloklamayan I/O, poll, epoll
 *
 * @details
 * =============================================================================
 * [THEORY: Blocking vs Non-Blocking I/O / TEORİ: Bloklayan vs Bloklamayan I/O]
 * =============================================================================
 *
 * EN: By default, socket operations (accept, recv, send) are BLOCKING:
 *     the calling thread sleeps until data arrives or a client connects.
 *     This is fine for simple servers, but doesn't scale:
 *
 *     ┌─────────────────────────────────────────────────────────────────────┐
 *     │ Problem: Thread-per-Connection                                      │
 *     │                                                                     │
 *     │ Client 1  ──→  Thread 1  (blocked on recv...)                       │
 *     │ Client 2  ──→  Thread 2  (blocked on recv...)                       │
 *     │ Client 3  ──→  Thread 3  (blocked on recv...)                       │
 *     │ ...                                                                 │
 *     │ Client 10000 ──→ Thread 10000  💥 OS limit / memory exhaustion      │
 *     └─────────────────────────────────────────────────────────────────────┘
 *
 *     Each thread costs ~8MB stack → 10K clients = ~80GB just for stacks!
 *
 *     Non-blocking I/O solves this:
 *     - Set socket to O_NONBLOCK via fcntl()
 *     - recv() returns immediately with EAGAIN/EWOULDBLOCK if no data
 *     - send() returns immediately with EAGAIN if buffer is full
 *     - One thread can handle THOUSANDS of connections
 *
 * TR: Varsayılan olarak soket işlemleri (accept, recv, send) BLOKLAYICIDIR:
 *     çağıran thread, veri gelene veya istemci bağlanana kadar uyur.
 *     Basit sunucular için sorunsuz ama ölçeklenmez:
 *
 *     ┌─────────────────────────────────────────────────────────────────────┐
 *     │ Sorun: Bağlantı-Başına-Thread                                       │
 *     │                                                                     │
 *     │ İstemci 1    ──→  Thread 1  (recv'de bloklandı...)                  │
 *     │ İstemci 2    ──→  Thread 2  (recv'de bloklandı...)                  │
 *     │ İstemci 3    ──→  Thread 3  (recv'de bloklandı...)                  │
 *     │ ...                                                                 │
 *     │ İstemci 10000 ──→ Thread 10000  💥 OS sınırı / bellek tükenmesi     │
 *     └─────────────────────────────────────────────────────────────────────┘
 *
 *     Her thread ~8MB yığın → 10K istemci = sadece yığınlar için ~80GB!
 *
 *     Bloklamayan I/O bunu çözer:
 *     - Soketi fcntl() ile O_NONBLOCK yap
 *     - Veri yoksa recv() hemen EAGAIN/EWOULDBLOCK ile döner
 *     - Tampon doluysa send() hemen EAGAIN ile döner
 *     - Tek thread BİNLERCE bağlantıyı yönetebilir
 *
 * =============================================================================
 * [THEORY: I / TEORİ: O Multiplexing: select → poll → epoll / I/O Çoklama]
 * =============================================================================
 *
 * EN: I/O multiplexing lets a single thread monitor MULTIPLE file descriptors
 *     and react when ANY of them is ready for I/O.
 *
 *     ┌────────────┬──────────────┬──────────────────┬──────────────────────┐
 *     │ Mechanism  │ Introduced   │ Scalability      │ Limitations          │
 *     ├────────────┼──────────────┼──────────────────┼──────────────────────┤
 *     │ select()   │ 1983 (4.2BSD)│ O(n), FD_SETSIZE │ Max 1024 FDs (hard!) │
 *     │            │              │ ≤ 1024 (default) │ Copies fd_set to/from│
 *     │            │              │                  │ kernel each call     │
 *     ├────────────┼──────────────┼──────────────────┼──────────────────────┤
 *     │ poll()     │ 1986 (SVR3)  │ O(n), no limit   │ Still linear scan of │
 *     │            │              │ on FD count      │ all FDs every call   │
 *     ├────────────┼──────────────┼──────────────────┼──────────────────────┤
 *     │ epoll      │ 2002 (Linux  │ O(1) for ready   │ Linux-only (BSD has  │
 *     │            │ 2.5.44)      │ FDs, kernel-     │ kqueue, Windows has  │
 *     │            │              │ managed          │ IOCP)                │
 *     └────────────┴──────────────┴──────────────────┴──────────────────────┘
 *
 * TR: I/O çoklama, tek bir thread'in BİRDEN FAZLA dosya tanımlayıcısını izlemesine
 *     ve herhangi biri I/O için hazır olduğunda tepki vermesine olanak tanır.
 *
 *     ┌────────────┬──────────────┬──────────────────┬──────────────────────┐
 *     │ Mekanizma  │ Tanıtım      │ Ölçeklenebilirlik│ Kısıtlamalar         │
 *     ├────────────┼──────────────┼──────────────────┼──────────────────────┤
 *     │ select()   │ 1983 (4.2BSD)│ O(n), FD_SETSIZE │ Maks 1024 FD (sabit!)│
 *     │ poll()     │ 1986 (SVR3)  │ O(n), sınır yok  │ Her çağrıda doğrusal │
 *     │ epoll      │ 2002 (Linux) │ Hazır FD'ler için│ Sadece Linux         │
 *     │            │              │ O(1), çekirdek   │                      │
 *     │            │              │ yönetimli        │                      │
 *     └────────────┴──────────────┴──────────────────┴──────────────────────┘
 *
 * =============================================================================
 * [THEORY: epoll Architecture / TEORİ: epoll Mimarisi]
 * =============================================================================
 *
 * EN: epoll uses a 3-step API:
 *
 *     1. epoll_create1(0)    → Create an epoll instance (returns an fd)
 *     2. epoll_ctl(epfd, op, fd, &event)  → Add/modify/remove monitored fds
 *        - EPOLL_CTL_ADD: start monitoring fd
 *        - EPOLL_CTL_MOD: change events of interest
 *        - EPOLL_CTL_DEL: stop monitoring fd
 *     3. epoll_wait(epfd, events[], maxevents, timeout) → Wait for ready fds
 *        - Returns ONLY the ready fds (not all of them like poll!)
 *        - This is why it's O(1) for ready events
 *
 *     Trigger modes:
 *     ┌──────────────────┬──────────────────────────────────────────────────┐
 *     │ Level-Triggered  │ Default. epoll_wait returns if fd STILL has data.│
 *     │ (LT)             │ Easy to use. Can re-read later.                  │
 *     ├──────────────────┼──────────────────────────────────────────────────┤
 *     │ Edge-Triggered   │ EPOLLET flag. Only notifies on NEW data arrival. │
 *     │ (ET)             │ MUST read ALL available data or you lose events! │
 *     │                  │ More efficient but harder to use correctly.      │
 *     └──────────────────┴──────────────────────────────────────────────────┘
 *
 * TR: epoll 3 adımlı bir API kullanır:
 *
 *     1. epoll_create1(0)    → epoll örneği oluştur (bir fd döndürür)
 *     2. epoll_ctl(epfd, op, fd, &event)  → İzlenen fd'leri ekle/değiştir/kaldır
 *        - EPOLL_CTL_ADD: fd izlemeyi başlat
 *        - EPOLL_CTL_MOD: ilgilenilen olayları değiştir
 *        - EPOLL_CTL_DEL: fd izlemeyi durdur
 *     3. epoll_wait(epfd, events[], maxevents, timeout) → Hazır fd'leri bekle
 *        - Sadece hazır fd'leri döndürür (poll gibi hepsini değil!)
 *        - Bu yüzden hazır olaylar için O(1)'dir
 *
 *     Tetikleme modları:
 *     ┌───────────────────┬─────────────────────────────────────────────────────┐
 *     │ Seviye Tetiklemeli│ Varsayılan. fd'de HÂLÂ veri varsa epoll_wait döner. │
 *     │ (LT)              │ Kullanımı kolay. Sonra tekrar okunabilir.           │
 *     ├───────────────────┼─────────────────────────────────────────────────────┤
 *     │ Kenar Tetiklemeli │ EPOLLET bayrağı. Sadece YENİ veri geldiğinde bildir.│
 *     │ (ET)              │ Tüm mevcut veriyi okuMALISINIZ yoksa olay kaçar!    │
 *     │                   │ Daha verimli ama doğru kullanması zor.              │
 *     └───────────────────┴─────────────────────────────────────────────────────┘
 *
 * =============================================================================
 * [THEORY: Reactor Pattern / TEORİ: Reaktör Kalıbı]
 * =============================================================================
 *
 * EN: The Reactor pattern is an event-driven architecture for handling I/O:
 *
 *     ┌──────────────────────────────────────────────────────────────────┐
 *     │                     Reactor Pattern                              │
 *     │                                                                  │
 *     │  ┌──────────┐  register   ┌──────────────┐  dispatch  ┌───────┐  │
 *     │  │ Handler1 │ ─────────→  │   Reactor    │ ────────→  │Handler│  │
 *     │  │ Handler2 │             │ (epoll_wait) │            │  (cb) │  │
 *     │  │ Handler3 │             │              │            └───────┘  │
 *     │  └──────────┘             └──────┬───────┘                       │
 *     │                                  │                               │
 *     │                             event loop                           │
 *     │                                  │                               │
 *     │                        ┌─────────▼──────────┐                    │
 *     │                        │  I/O Ready Event   │                    │
 *     │                        │  (read/write/error)│                    │
 *     │                        └────────────────────┘                    │
 *     └──────────────────────────────────────────────────────────────────┘
 *
 *     This solves the C10K problem (handling 10,000+ concurrent connections):
 *     - One thread, one epoll instance
 *     - Register interest in events (EPOLLIN, EPOLLOUT)
 *     - When epoll_wait returns, dispatch to the appropriate handler
 *     - No threads wasted on waiting
 *
 *     Used in: nginx, Node.js (libuv), Redis, HAProxy, Boost.Asio
 *
 * TR: Reaktör kalıbı, I/O yönetimi için olay güdümlü mimaridir:
 *
 *     ┌──────────────────────────────────────────────────────────────────┐
 *     │                     Reaktör Kalıbı                               │
 *     │                                                                  │
 *     │  ┌──────────┐  kaydet     ┌──────────────┐  dağıt     ┌───────┐  │
 *     │  │ Handler1 │ ─────────→  │   Reaktör    │ ────────→  │Handler│  │
 *     │  │ Handler2 │             │ (epoll_wait) │            │  (cb) │  │
 *     │  │ Handler3 │             │              │            └───────┘  │
 *     │  └──────────┘             └──────┬───────┘                       │
 *     │                                  │                               │
 *     │                             olay döngüsü                         │
 *     │                                  │                               │
 *     │                        ┌─────────▼──────────┐                    │
 *     │                        │  I/O Hazır Olayı   │                    │
 *     │                        │ (oku/yaz/hata)     │                    │
 *     │                        └────────────────────┘                    │
 *     └──────────────────────────────────────────────────────────────────┘
 *
 *     C10K problemini çözer (10.000+ eşzamanlı bağlantı):
 *     - Tek thread, tek epoll örneği
 *     - Olaylara ilgi kaydet (EPOLLIN, EPOLLOUT)
 *     - epoll_wait döndüğünde, uygun handler'a dağıt
 *     - Beklemede harcanan thread yok
 *
 *     Kullananlar: nginx, Node.js (libuv), Redis, HAProxy, Boost.Asio
 *
 * @see https://man7.org/linux/man-pages/man2/epoll_wait.2.html
 * @see https://man7.org/linux/man-pages/man2/poll.2.html
 * @see https://man7.org/linux/man-pages/man2/fcntl.2.html
 * @see https://en.wikipedia.org/wiki/C10k_problem
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wnull-dereference -Wformat=2 02_nonblocking_io_epoll.cpp -o 02_nonblocking_io_epoll
 * @note This file compiles and runs on LINUX only (uses epoll, which is Linux-specific).
 *       For cross-platform: use Boost.Asio or libuv, which abstract platform differences.
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <cstdint>
#include <cstring>
#include <string>
#include <array>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <algorithm>
#include <sstream>
#include <cerrno>
#include <cassert>
#include <iomanip>

// EN: POSIX / Linux headers for socket and epoll
// TR: Soket ve epoll için POSIX / Linux başlıkları
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/epoll.h>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: Non-Blocking Socket Setup (RAII Wrapper)
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: RAII socket wrapper with non-blocking support. Builds on the Socket class
 *     from module_13/01, adding:
 *     - set_nonblocking(): sets O_NONBLOCK via fcntl()
 *     - set_reuseaddr(): prevents "Address already in use" error
 *     - Proper error handling without exceptions (returns error codes)
 *
 * TR: Bloklamayan desteğe sahip RAII soket sarmalayıcı. module_13/01'deki Socket
 *     sınıfının üzerine şunları ekler:
 *     - set_nonblocking(): fcntl() ile O_NONBLOCK ayarlar
 *     - set_reuseaddr(): "Address already in use" hatasını önler
 *     - İstisnasız uygun hata yönetimi (hata kodları döndürür)
 */
class Socket {
    int fd_ = -1;

public:
    Socket() = default;

    /// EN: Construct from an existing file descriptor (e.g., from accept())
    /// TR: Mevcut bir dosya tanımlayıcıdan oluştur (ör. accept()'ten)
    explicit Socket(int fd) : fd_(fd) {}

    /// EN: Create a new socket with given domain, type, protocol
    /// TR: Verilen domain, tip, protokol ile yeni soket oluştur
    static Socket create(int domain = AF_INET, int type = SOCK_STREAM, int protocol = 0) {
        int fd = ::socket(domain, type, protocol);
        if (fd < 0) {
            std::cerr << "  [ERROR] socket() failed: " << strerror(errno) << "\n";
        }
        return Socket(fd);
    }

    // EN: RAII — close on destruction / TR: RAII — yıkımda kapat
    ~Socket() { close(); }

    // EN: Move-only (transfer ownership like unique_ptr)
    // TR: Sadece taşınabilir (unique_ptr gibi sahiplik transferi)
    Socket(Socket&& other) noexcept : fd_(other.fd_) { other.fd_ = -1; }
    Socket& operator=(Socket&& other) noexcept {
        if (this != &other) {
            close();
            fd_ = other.fd_;
            other.fd_ = -1;
        }
        return *this;
    }
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    void close() {
        if (fd_ >= 0) {
            ::close(fd_);
            fd_ = -1;
        }
    }

    [[nodiscard]] int fd() const { return fd_; }
    [[nodiscard]] bool valid() const { return fd_ >= 0; }

    /// EN: Set socket to non-blocking mode using fcntl O_NONBLOCK.
    ///     After this, recv/send return EAGAIN instead of blocking.
    /// TR: Soketi fcntl O_NONBLOCK ile bloklamayan moda ayarla.
    ///     Bundan sonra recv/send bloklama yerine EAGAIN döndürür.
    bool set_nonblocking() {
        int flags = fcntl(fd_, F_GETFL, 0);
        if (flags < 0) return false;
        return fcntl(fd_, F_SETFL, flags | O_NONBLOCK) >= 0;
    }

    /// EN: Allow address reuse — prevents "Address already in use" after server restart
    /// TR: Adres yeniden kullanımına izin ver — sunucu yeniden başlatıldıktan sonra
    ///     "Address already in use" hatasını önler
    bool set_reuseaddr() {
        int opt = 1;
        return setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) >= 0;
    }

    /// EN: Bind to an address and port / TR: Adres ve porta bağla
    bool bind(uint16_t port, const char* addr = "0.0.0.0") {
        struct sockaddr_in sa{};
        sa.sin_family = AF_INET;
        sa.sin_port = htons(port);
        inet_pton(AF_INET, addr, &sa.sin_addr);
        return ::bind(fd_, reinterpret_cast<struct sockaddr*>(&sa), sizeof(sa)) >= 0;
    }

    /// EN: Start listening for connections / TR: Bağlantıları dinlemeye başla
    bool listen(int backlog = SOMAXCONN) {
        return ::listen(fd_, backlog) >= 0;
    }

    /// EN: Accept a new connection (returns a new Socket with the client fd)
    /// TR: Yeni bağlantı kabul et (istemci fd'si ile yeni Socket döndürür)
    Socket accept() {
        struct sockaddr_in client_addr{};
        socklen_t len = sizeof(client_addr);
        int client_fd = ::accept(fd_, reinterpret_cast<struct sockaddr*>(&client_addr), &len);
        return Socket(client_fd);
    }

    /// EN: Release ownership of the fd (caller takes responsibility)
    /// TR: fd sahipliğini bırak (çağıran sorumluluk alır)
    int release() {
        int fd = fd_;
        fd_ = -1;
        return fd;
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: poll()-Based Multi-Client Server
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: A multi-client TCP echo server using poll(). poll() is more portable than
 *     epoll (works on macOS, BSDs, etc.) and has no FD limit like select().
 *
 *     How poll() works:
 *     - You provide an array of pollfd structs: {fd, events_to_watch, returned_events}
 *     - poll() blocks until at least one fd is ready
 *     - You iterate ALL fds to check which ones have events → O(n)
 *
 *     This is fine for ~100-1000 clients. For 10K+, use epoll.
 *
 * TR: poll() kullanan çoklu istemci TCP echo sunucusu. poll(), epoll'dan daha
 *     taşınabilirdir (macOS, BSD'lerde çalışır) ve select() gibi FD sınırı yoktur.
 *
 *     poll() nasıl çalışır:
 *     - pollfd yapıları dizisi verirsiniz: {fd, izlenecek_olaylar, dönen_olaylar}
 *     - En az bir fd hazır olana kadar poll() bloklar
 *     - Hangi olayların olduğunu kontrol etmek için TÜM fd'leri iterasyonla gezersiniz → O(n)
 *
 *     ~100-1000 istemci için uygundur. 10K+ için epoll kullanın.
 */
class PollServer {
    Socket listen_socket_;
    std::vector<struct pollfd> poll_fds_;
    static constexpr int BUFFER_SIZE = 1024;

public:
    /// EN: Initialize the server socket (bind + listen)
    /// TR: Sunucu soketini başlat (bağla + dinle)
    bool init(uint16_t port) {
        listen_socket_ = Socket::create();
        if (!listen_socket_.valid()) return false;

        listen_socket_.set_reuseaddr();
        listen_socket_.set_nonblocking();

        if (!listen_socket_.bind(port)) {
            std::cerr << "  [PollServer] Bind failed: " << strerror(errno) << "\n";
            return false;
        }
        if (!listen_socket_.listen()) {
            std::cerr << "  [PollServer] Listen failed: " << strerror(errno) << "\n";
            return false;
        }

        // EN: Add the listening socket as the first pollfd entry
        // TR: Dinleme soketini ilk pollfd girdisi olarak ekle
        poll_fds_.push_back({listen_socket_.fd(), POLLIN, 0});
        return true;
    }

    /// EN: Run one iteration of the poll loop (for demonstration).
    ///     In production, this would be an infinite loop.
    /// TR: Poll döngüsünün bir iterasyonunu çalıştır (gösterim için).
    ///     Üretimde bu sonsuz döngü olurdu.
    void poll_once(int timeout_ms = 100) {
        int ready = poll(poll_fds_.data(), static_cast<nfds_t>(poll_fds_.size()), timeout_ms);

        if (ready < 0) {
            if (errno != EINTR) {
                std::cerr << "  [PollServer] poll() error: " << strerror(errno) << "\n";
            }
            return;
        }

        if (ready == 0) return;  // EN: Timeout, no events / TR: Zaman aşımı, olay yok

        // EN: Check the listening socket for new connections
        // TR: Dinleme soketini yeni bağlantılar için kontrol et
        if (poll_fds_[0].revents & POLLIN) {
            accept_new_client();
        }

        // EN: Check all client sockets for data (iterate backwards for safe removal)
        // TR: Tüm istemci soketlerini veri için kontrol et (güvenli kaldırma için geriye iterasyon)
        for (std::size_t i = poll_fds_.size(); i > 1; --i) {
            std::size_t idx = i - 1;
            if (poll_fds_[idx].revents & (POLLIN | POLLERR | POLLHUP)) {
                handle_client(idx);
            }
        }
    }

    [[nodiscard]] std::size_t client_count() const {
        return poll_fds_.size() > 0 ? poll_fds_.size() - 1 : 0;
    }

private:
    void accept_new_client() {
        auto client = listen_socket_.accept();
        if (!client.valid()) return;

        // EN: Set new client to non-blocking
        // TR: Yeni istemciyi bloklamayan yap
        client.set_nonblocking();
        int fd = client.release();  // EN: Transfer ownership to poll_fds_ / TR: Sahipliği poll_fds_'e aktar

        poll_fds_.push_back({fd, POLLIN, 0});
        std::cout << "  [PollServer] New client connected (fd=" << fd
                  << "), total clients: " << client_count() << "\n";
    }

    void handle_client(std::size_t idx) {
        char buffer[BUFFER_SIZE];
        ssize_t n = recv(poll_fds_[idx].fd, buffer, sizeof(buffer) - 1, 0);

        if (n <= 0) {
            // EN: Client disconnected or error → remove from poll set
            // TR: İstemci bağlantıyı kesti veya hata → poll kümesinden kaldır
            std::cout << "  [PollServer] Client disconnected (fd=" << poll_fds_[idx].fd << ")\n";
            ::close(poll_fds_[idx].fd);
            poll_fds_.erase(poll_fds_.begin() + static_cast<std::ptrdiff_t>(idx));
            return;
        }

        buffer[n] = '\0';
        std::cout << "  [PollServer] Received from fd=" << poll_fds_[idx].fd
                  << ": " << buffer;

        // EN: Echo back to client / TR: İstemciye geri yankıla
        send(poll_fds_[idx].fd, buffer, static_cast<size_t>(n), 0);
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: epoll Event Loop — Multi-Client TCP Server
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: High-performance multi-client TCP server using epoll (Linux).
 *     epoll is the gold standard for I/O multiplexing on Linux:
 *     - O(1) event notification (only returns READY fds)
 *     - Kernel maintains the interest list → no copying fd sets per call
 *     - Scales to millions of connections (used by nginx, Redis)
 *
 *     RAII design: epoll fd is managed automatically, client fds tracked in a map.
 *
 * TR: epoll (Linux) kullanan yüksek performanslı çoklu istemci TCP sunucusu.
 *     epoll, Linux'ta I/O çoklamanın altın standardıdır:
 *     - O(1) olay bildirimi (sadece HAZIR fd'leri döndürür)
 *     - Çekirdek ilgi listesini yönetir → çağrı başına fd seti kopyalama yok
 *     - Milyonlarca bağlantıya ölçeklenir (nginx, Redis tarafından kullanılır)
 *
 *     RAII tasarım: epoll fd otomatik yönetilir, istemci fd'leri haritada izlenir.
 */
class EpollServer {
    Socket listen_socket_;
    int epoll_fd_ = -1;
    std::unordered_map<int, std::string> clients_;  // EN: fd → client info / TR: fd → istemci bilgisi
    static constexpr int MAX_EVENTS = 64;
    static constexpr int BUFFER_SIZE = 4096;

public:
    EpollServer() = default;

    ~EpollServer() {
        // EN: Close all client connections / TR: Tüm istemci bağlantılarını kapat
        for (auto& [fd, info] : clients_) {
            ::close(fd);
        }
        // EN: Close epoll fd / TR: epoll fd'yi kapat
        if (epoll_fd_ >= 0) ::close(epoll_fd_);
    }

    EpollServer(const EpollServer&) = delete;
    EpollServer& operator=(const EpollServer&) = delete;

    /// EN: Initialize: create socket, bind, listen, create epoll instance
    /// TR: Başlat: soket oluştur, bağla, dinle, epoll örneği oluştur
    bool init(uint16_t port) {
        // EN: Step 1: Create and configure listening socket
        // TR: Adım 1: Dinleme soketi oluştur ve yapılandır
        listen_socket_ = Socket::create();
        if (!listen_socket_.valid()) return false;

        listen_socket_.set_reuseaddr();
        listen_socket_.set_nonblocking();

        if (!listen_socket_.bind(port)) {
            std::cerr << "  [EpollServer] Bind failed: " << strerror(errno) << "\n";
            return false;
        }
        if (!listen_socket_.listen()) {
            std::cerr << "  [EpollServer] Listen failed: " << strerror(errno) << "\n";
            return false;
        }

        // EN: Step 2: Create epoll instance
        // TR: Adım 2: epoll örneği oluştur
        epoll_fd_ = epoll_create1(0);
        if (epoll_fd_ < 0) {
            std::cerr << "  [EpollServer] epoll_create1 failed: " << strerror(errno) << "\n";
            return false;
        }

        // EN: Step 3: Register listening socket with epoll
        // TR: Adım 3: Dinleme soketini epoll'e kaydet
        struct epoll_event ev{};
        ev.events = EPOLLIN;               // EN: Watch for incoming connections / TR: Gelen bağlantıları izle
        ev.data.fd = listen_socket_.fd();
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, listen_socket_.fd(), &ev) < 0) {
            std::cerr << "  [EpollServer] epoll_ctl ADD failed: " << strerror(errno) << "\n";
            return false;
        }

        std::cout << "  [EpollServer] Listening on port " << port << "...\n";
        return true;
    }

    /// EN: Run one iteration of the epoll event loop
    /// TR: epoll olay döngüsünün bir iterasyonunu çalıştır
    void poll_once(int timeout_ms = 100) {
        std::array<struct epoll_event, MAX_EVENTS> events{};
        int n = epoll_wait(epoll_fd_, events.data(), MAX_EVENTS, timeout_ms);

        if (n < 0) {
            if (errno != EINTR) {
                std::cerr << "  [EpollServer] epoll_wait error: " << strerror(errno) << "\n";
            }
            return;
        }

        // EN: Process ONLY the ready events (this is why epoll is O(1) per ready fd)
        // TR: SADECE hazır olayları işle (epoll'un hazır fd başına O(1) olmasının nedeni)
        for (int i = 0; i < n; ++i) {
            if (events[static_cast<std::size_t>(i)].data.fd == listen_socket_.fd()) {
                accept_new_client();
            } else {
                handle_client_event(events[static_cast<std::size_t>(i)]);
            }
        }
    }

    [[nodiscard]] std::size_t client_count() const { return clients_.size(); }

private:
    void accept_new_client() {
        // EN: Accept in a loop — epoll may batch multiple accepts
        // TR: Döngüde kabul et — epoll birden fazla accept'i gruplayabilir
        while (true) {
            auto client = listen_socket_.accept();
            if (!client.valid()) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) break;  // EN: No more pending / TR: Bekleyen kalmadı
                break;
            }

            client.set_nonblocking();
            int fd = client.release();

            // EN: Register the new client with epoll (watch for read events)
            // TR: Yeni istemciyi epoll'e kaydet (okuma olaylarını izle)
            struct epoll_event ev{};
            ev.events = EPOLLIN | EPOLLET;  // EN: Edge-triggered for efficiency / TR: Verimlilik için kenar tetiklemeli
            ev.data.fd = fd;
            if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) < 0) {
                ::close(fd);
                continue;
            }

            struct sockaddr_in peer{};
            socklen_t peer_len = sizeof(peer);
            getpeername(fd, reinterpret_cast<struct sockaddr*>(&peer), &peer_len);

            char addr_str[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &peer.sin_addr, addr_str, sizeof(addr_str));

            clients_[fd] = std::string(addr_str) + ":" + std::to_string(ntohs(peer.sin_port));
            std::cout << "  [EpollServer] Client connected: " << clients_[fd]
                      << " (fd=" << fd << "), total: " << clients_.size() << "\n";
        }
    }

    void handle_client_event(const struct epoll_event& event) {
        int fd = event.data.fd;

        if (event.events & (EPOLLERR | EPOLLHUP)) {
            remove_client(fd);
            return;
        }

        if (event.events & EPOLLIN) {
            // EN: Edge-triggered: MUST read ALL available data
            // TR: Kenar tetiklemeli: TÜM mevcut veriyi okuMALISIN
            char buffer[BUFFER_SIZE];
            while (true) {
                ssize_t n = recv(fd, buffer, sizeof(buffer) - 1, 0);

                if (n < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) break;  // EN: All data read / TR: Tüm veri okundu
                    remove_client(fd);
                    return;
                }

                if (n == 0) {
                    // EN: Client gracefully disconnected
                    // TR: İstemci düzgünce bağlantıyı kesti
                    remove_client(fd);
                    return;
                }

                buffer[n] = '\0';
                std::cout << "  [EpollServer] From " << clients_[fd] << ": " << buffer;

                // EN: Echo back / TR: Geri yankıla
                send(fd, buffer, static_cast<size_t>(n), MSG_NOSIGNAL);
            }
        }
    }

    void remove_client(int fd) {
        // EN: epoll automatically removes closed fds, but we call CTL_DEL for clarity
        // TR: epoll kapatılan fd'leri otomatik kaldırır ama açıklık için CTL_DEL çağırırız
        epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr);
        std::cout << "  [EpollServer] Client disconnected: " << clients_[fd] << "\n";
        clients_.erase(fd);
        ::close(fd);
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Edge-Triggered epoll with RAII (EpollHandle)
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: RAII wrapper for an epoll file descriptor. Ensures the epoll instance is
 *     always properly closed, even on exceptions. Follows the Rule of Five.
 *
 *     This is a reusable building block for any epoll-based server or event loop.
 *     Combined with the Socket RAII wrapper, you get fully deterministic cleanup.
 *
 * TR: epoll dosya tanımlayıcısı için RAII sarmalayıcı. İstisna durumunda bile
 *     epoll örneğinin her zaman düzgün kapatılmasını sağlar. Beşli Kuralını takip eder.
 *
 *     Herhangi bir epoll tabanlı sunucu veya olay döngüsü için yeniden kullanılabilir
 *     yapı taşıdır. Socket RAII sarmalayıcısıyla birlikte tam belirleyici temizlik sağlar.
 */
class EpollHandle {
    int fd_ = -1;

public:
    EpollHandle() : fd_(epoll_create1(0)) {
        if (fd_ < 0) {
            std::cerr << "  [EpollHandle] epoll_create1 failed: " << strerror(errno) << "\n";
        }
    }

    ~EpollHandle() { if (fd_ >= 0) ::close(fd_); }

    // EN: Move-only / TR: Sadece taşınabilir
    EpollHandle(EpollHandle&& o) noexcept : fd_(o.fd_) { o.fd_ = -1; }
    EpollHandle& operator=(EpollHandle&& o) noexcept {
        if (this != &o) { if (fd_ >= 0) ::close(fd_); fd_ = o.fd_; o.fd_ = -1; }
        return *this;
    }
    EpollHandle(const EpollHandle&) = delete;
    EpollHandle& operator=(const EpollHandle&) = delete;

    [[nodiscard]] int fd() const { return fd_; }
    [[nodiscard]] bool valid() const { return fd_ >= 0; }

    /// EN: Add a file descriptor to the epoll interest list
    /// TR: Dosya tanımlayıcısını epoll ilgi listesine ekle
    bool add(int target_fd, uint32_t events) {
        struct epoll_event ev{};
        ev.events = events;
        ev.data.fd = target_fd;
        return epoll_ctl(fd_, EPOLL_CTL_ADD, target_fd, &ev) >= 0;
    }

    /// EN: Modify events for an already-registered fd
    /// TR: Zaten kayıtlı bir fd için olayları değiştir
    bool modify(int target_fd, uint32_t events) {
        struct epoll_event ev{};
        ev.events = events;
        ev.data.fd = target_fd;
        return epoll_ctl(fd_, EPOLL_CTL_MOD, target_fd, &ev) >= 0;
    }

    /// EN: Remove a file descriptor from epoll
    /// TR: Dosya tanımlayıcısını epoll'den kaldır
    bool remove(int target_fd) {
        return epoll_ctl(fd_, EPOLL_CTL_DEL, target_fd, nullptr) >= 0;
    }

    /// EN: Wait for events with timeout
    /// TR: Zaman aşımı ile olayları bekle
    int wait(struct epoll_event* events, int max_events, int timeout_ms) {
        return epoll_wait(fd_, events, max_events, timeout_ms);
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: Real-World — Diagnostic Gateway (Automotive)
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Simulates an automotive diagnostic gateway scenario:
 *     A diagnostic tester (like ADOS, CANoe, or a workshop tool) connects to
 *     the ECU's diagnostic port. The gateway:
 *     1. Listens for incoming diagnostic clients (UDS over DoIP)
 *     2. Routes diagnostic requests to appropriate ECUs
 *     3. Handles multiple concurrent diagnostic sessions
 *
 *     In real automotive systems, this is built on top of epoll/IOCP for performance.
 *     The gateway must handle: diagnostic session timeouts, priority scheduling,
 *     and keep-alive messages — all while maintaining low latency.
 *
 * TR: Otomotiv teşhis ağ geçidi senaryosunu simüle eder:
 *     Teşhis test cihazı (ADOS, CANoe veya atölye aracı gibi) ECU'nun teşhis
 *     portuna bağlanır. Ağ geçidi:
 *     1. Gelen teşhis istemcilerini dinler (DoIP üzerinden UDS)
 *     2. Teşhis isteklerini uygun ECU'lara yönlendirir
 *     3. Birden fazla eşzamanlı teşhis oturumunu yönetir
 *
 *     Gerçek otomotiv sistemlerinde performans için epoll/IOCP üzerine inşa edilir.
 */

/// EN: UDS (Unified Diagnostic Services) service identifiers — ISO 14229
/// TR: UDS (Birleşik Teşhis Servisleri) servis tanımlayıcıları — ISO 14229
enum class UdsService : uint8_t {
    DiagnosticSessionControl = 0x10,
    ECUReset                 = 0x11,
    ReadDataByIdentifier     = 0x22,
    WriteDataByIdentifier    = 0x2E,
    RoutineControl           = 0x31,
    RequestDownload          = 0x34,
    TesterPresent            = 0x3E,
};

const char* uds_service_name(UdsService svc) {
    switch (svc) {
        case UdsService::DiagnosticSessionControl: return "DiagSessionCtrl(0x10)";
        case UdsService::ECUReset:                 return "ECUReset(0x11)";
        case UdsService::ReadDataByIdentifier:     return "ReadDID(0x22)";
        case UdsService::WriteDataByIdentifier:    return "WriteDID(0x2E)";
        case UdsService::RoutineControl:           return "RoutineCtrl(0x31)";
        case UdsService::RequestDownload:          return "ReqDownload(0x34)";
        case UdsService::TesterPresent:            return "TesterPresent(0x3E)";
        default: return "Unknown";
    }
}

/// EN: Simulated diagnostic request routing
/// TR: Simüle edilmiş teşhis istek yönlendirmesi
struct DiagRequest {
    UdsService service;
    uint16_t target_ecu_id;            // EN: Logical ECU address / TR: Mantıksal ECU adresi
    std::vector<uint8_t> payload;
};

/// EN: Simulated diagnostic response
/// TR: Simüle edilmiş teşhis yanıtı
struct DiagResponse {
    uint8_t service_id;                // EN: service + 0x40 = positive response / TR: servis + 0x40 = olumlu yanıt
    uint16_t source_ecu_id;
    std::vector<uint8_t> data;
    bool positive;
};

DiagResponse simulate_ecu_response(const DiagRequest& req) {
    DiagResponse resp;
    resp.service_id = static_cast<uint8_t>(req.service) + 0x40;  // EN: Positive response / TR: Olumlu yanıt
    resp.source_ecu_id = req.target_ecu_id;
    resp.positive = true;

    switch (req.service) {
        case UdsService::ReadDataByIdentifier:
            resp.data = {0xF1, 0x90, 'V', 'I', 'N', '1', '2', '3'};  // EN: VIN example / TR: VIN örneği
            break;
        case UdsService::TesterPresent:
            resp.data = {};  // EN: Empty positive response / TR: Boş olumlu yanıt
            break;
        default:
            resp.data = {0x00};  // EN: Generic positive / TR: Genel olumlu
            break;
    }
    return resp;
}

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 13 — Non-Blocking I/O & epoll\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: Non-Blocking Socket Setup ───────────────────────────────
    {
        std::cout << "--- Demo 1: Non-Blocking Socket Setup ---\n";

        // EN: Create a socket and set it to non-blocking mode
        // TR: Bir soket oluştur ve bloklamayan moda ayarla
        Socket sock = Socket::create(AF_INET, SOCK_STREAM, 0);
        if (sock.valid()) {
            std::cout << "  Socket created: fd=" << sock.fd() << "\n";

            // EN: Set non-blocking — recv will return EAGAIN instead of sleeping
            // TR: Bloklamayan ayarla — recv uyumak yerine EAGAIN döndürecek
            if (sock.set_nonblocking()) {
                std::cout << "  Set to non-blocking mode (O_NONBLOCK)\n";

                // EN: Verify with fcntl
                // TR: fcntl ile doğrula
                int flags = fcntl(sock.fd(), F_GETFL, 0);
                std::cout << "  Socket flags: 0x" << std::hex << flags << std::dec;
                if (flags & O_NONBLOCK) {
                    std::cout << " [O_NONBLOCK is SET]\n";
                } else {
                    std::cout << " [O_NONBLOCK is NOT set]\n";
                }
            }

            // EN: Set address reuse — essential for server sockets
            // TR: Adres yeniden kullanımını ayarla — sunucu soketleri için zorunlu
            if (sock.set_reuseaddr()) {
                std::cout << "  SO_REUSEADDR enabled\n";
            }
        }

        std::cout << "  Socket will be closed automatically (RAII)\n\n";
        // EN: Socket closes here — RAII destructor / TR: Soket burada kapanır — RAII destructor
    }

    // ─── Demo 2: poll()-Based Server Concept ─────────────────────────────
    {
        std::cout << "--- Demo 2: poll() Server Architecture ---\n";
        std::cout << "  poll() provides I/O multiplexing without the FD limit of select().\n\n";

        // EN: Demonstrate poll concepts without starting a server
        //     (starting a server would require a separate client process)
        // TR: Sunucu başlatmadan poll kavramlarını göster
        std::cout << "  pollfd structure:\n";
        std::cout << "    struct pollfd {\n";
        std::cout << "      int   fd;       // file descriptor to watch\n";
        std::cout << "      short events;   // events to watch (POLLIN, POLLOUT)\n";
        std::cout << "      short revents;  // events that occurred (filled by poll)\n";
        std::cout << "    };\n\n";

        std::cout << "  Event flags:\n";
        std::cout << "    POLLIN  = 0x" << std::hex << POLLIN  << " → data available to read\n";
        std::cout << "    POLLOUT = 0x" << POLLOUT << " → ready to write\n";
        std::cout << "    POLLERR = 0x" << POLLERR << " → error condition\n";
        std::cout << "    POLLHUP = 0x" << POLLHUP << " → hang up (peer closed)\n";
        std::cout << std::dec;

        // EN: Demonstrate with a self-connected pipe (non-network example of poll)
        // TR: Kendi kendine bağlı pipe ile göster (poll'un ağ dışı örneği)
        int pipefd[2];
        if (pipe(pipefd) == 0) {
            // EN: Write some data to the pipe
            // TR: Pipe'a bazı veriler yaz
            const char* msg = "Hello from pipe!";
            write(pipefd[1], msg, strlen(msg));

            // EN: Poll the read end — should be immediately ready
            // TR: Okuma ucunu poll et — hemen hazır olmalı
            struct pollfd pfd{};
            pfd.fd = pipefd[0];
            pfd.events = POLLIN;

            int ready = poll(&pfd, 1, 0);  // EN: timeout=0 → non-blocking check / TR: timeout=0 → bloklamayan kontrol
            std::cout << "\n  poll() on pipe read-end: ready=" << ready;
            if (pfd.revents & POLLIN) {
                char buf[64];
                ssize_t n = read(pipefd[0], buf, sizeof(buf) - 1);
                if (n > 0) {
                    buf[n] = '\0';
                    std::cout << ", data=\"" << buf << "\"";
                }
            }
            std::cout << "\n";

            close(pipefd[0]);
            close(pipefd[1]);
        }
        std::cout << "\n";
    }

    // ─── Demo 3: epoll Server Architecture ───────────────────────────────
    {
        std::cout << "--- Demo 3: epoll Server Architecture ---\n";
        std::cout << "  epoll is Linux's high-performance I/O multiplexing mechanism.\n\n";

        // EN: Demonstrate epoll concepts with the RAII wrapper
        // TR: RAII sarmalayıcı ile epoll kavramlarını göster
        EpollHandle epoller;
        if (epoller.valid()) {
            std::cout << "  epoll instance created: fd=" << epoller.fd() << "\n";

            // EN: Demonstrate with a pipe (like demo 2, but using epoll)
            // TR: Pipe ile göster (demo 2 gibi ama epoll kullanarak)
            int pipefd[2];
            if (pipe(pipefd) == 0) {
                // EN: Set read end to non-blocking
                // TR: Okuma ucunu bloklamayan yap
                int flags = fcntl(pipefd[0], F_GETFL, 0);
                fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);

                // EN: Register read end with epoll
                // TR: Okuma ucunu epoll'e kaydet
                if (epoller.add(pipefd[0], EPOLLIN | EPOLLET)) {
                    std::cout << "  Registered pipe read-end with epoll (EPOLLIN | EPOLLET)\n";
                }

                // EN: Write data to trigger the event
                // TR: Olayı tetiklemek için veri yaz
                const char* msg = "epoll event triggered!";
                write(pipefd[1], msg, strlen(msg));

                // EN: Wait for events (should be immediately ready)
                // TR: Olayları bekle (hemen hazır olmalı)
                struct epoll_event events[4];
                int n = epoller.wait(events, 4, 100);
                std::cout << "  epoll_wait returned: " << n << " event(s)\n";

                for (int i = 0; i < n; ++i) {
                    if (events[i].events & EPOLLIN) {
                        char buf[128];
                        ssize_t bytes = read(events[i].data.fd, buf, sizeof(buf) - 1);
                        if (bytes > 0) {
                            buf[bytes] = '\0';
                            std::cout << "  [EPOLLIN] fd=" << events[i].data.fd
                                      << " data=\"" << buf << "\"\n";
                        }
                    }
                }

                // EN: Edge-triggered: second read should return EAGAIN (no new data)
                // TR: Kenar tetiklemeli: ikinci okuma EAGAIN döndürmeli (yeni veri yok)
                char buf2[128];
                ssize_t n2 = read(pipefd[0], buf2, sizeof(buf2));
                if (n2 < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                    std::cout << "  [ET Mode] Second read → EAGAIN (no new data, as expected)\n";
                }

                epoller.remove(pipefd[0]);
                close(pipefd[0]);
                close(pipefd[1]);
            }
        }
        std::cout << "\n";
    }

    // ─── Demo 4: epoll Level-Triggered vs Edge-Triggered ─────────────────
    {
        std::cout << "--- Demo 4: Level-Triggered (LT) vs Edge-Triggered (ET) ---\n\n";

        std::cout << "  Level-Triggered (default):\n";
        std::cout << "    • epoll_wait returns AS LONG AS data is available\n";
        std::cout << "    • You can read partial data and get notified again\n";
        std::cout << "    • Simpler but slightly less efficient\n";
        std::cout << "    • Safe for beginners\n\n";

        std::cout << "  Edge-Triggered (EPOLLET):\n";
        std::cout << "    • epoll_wait returns ONLY when NEW data arrives\n";
        std::cout << "    • You MUST read ALL data in one go (loop until EAGAIN)\n";
        std::cout << "    • More efficient (fewer epoll_wait wakeups)\n";
        std::cout << "    • Used in production servers (nginx, etc.)\n\n";

        // EN: Practical demonstration with pipe
        // TR: Pipe ile pratik gösteri
        int pipefd[2];
        if (pipe(pipefd) == 0) {
            int flags = fcntl(pipefd[0], F_GETFL, 0);
            fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);

            // EN: Test Level-Triggered (LT) behavior
            // TR: Seviye Tetiklemeli (LT) davranışını test et
            {
                EpollHandle lt_epoller;
                lt_epoller.add(pipefd[0], EPOLLIN);  // EN: No EPOLLET = Level-Triggered / TR: EPOLLET yok = Seviye Tetiklemeli

                // EN: Write 10 bytes / TR: 10 bayt yaz
                write(pipefd[1], "1234567890", 10);

                // EN: First epoll_wait — should be ready (10 bytes available)
                // TR: İlk epoll_wait — hazır olmalı (10 bayt mevcut)
                struct epoll_event events[4];
                int n = lt_epoller.wait(events, 4, 50);
                std::cout << "  [LT] After writing 10 bytes: epoll_wait returned " << n << " event(s)\n";

                // EN: Read only 5 bytes (partial read)
                // TR: Sadece 5 bayt oku (kısmi okuma)
                char buf[16];
                read(pipefd[0], buf, 5);
                std::cout << "  [LT] Read 5 bytes. 5 bytes remaining in pipe.\n";

                // EN: Second epoll_wait — LT will STILL report ready (5 bytes remain!)
                // TR: İkinci epoll_wait — LT HALİYLE hazır bildirecek (5 bayt kaldı!)
                n = lt_epoller.wait(events, 4, 50);
                std::cout << "  [LT] Second epoll_wait: returned " << n << " event(s) → "
                          << (n > 0 ? "STILL READY (correct for LT)" : "NOT ready") << "\n";

                // EN: Drain remaining / TR: Kalanı boşalt
                read(pipefd[0], buf, sizeof(buf));
                lt_epoller.remove(pipefd[0]);
            }

            // EN: Test Edge-Triggered (ET) behavior
            // TR: Kenar Tetiklemeli (ET) davranışını test et
            {
                EpollHandle et_epoller;
                et_epoller.add(pipefd[0], EPOLLIN | EPOLLET);  // EN: EPOLLET = Edge-Triggered / TR: EPOLLET = Kenar Tetiklemeli

                // EN: Write 10 bytes / TR: 10 bayt yaz
                write(pipefd[1], "ABCDEFGHIJ", 10);

                struct epoll_event events[4];
                int n = et_epoller.wait(events, 4, 50);
                std::cout << "\n  [ET] After writing 10 bytes: epoll_wait returned " << n << " event(s)\n";

                // EN: Read only 5 bytes (partial read)
                // TR: Sadece 5 bayt oku (kısmi okuma)
                char buf[16];
                read(pipefd[0], buf, 5);
                std::cout << "  [ET] Read 5 bytes. 5 bytes remaining.\n";

                // EN: Second epoll_wait — ET will NOT report ready (no NEW data arrived!)
                // TR: İkinci epoll_wait — ET hazır BİLDİRMEYECEK (YENİ veri gelmedi!)
                n = et_epoller.wait(events, 4, 50);
                std::cout << "  [ET] Second epoll_wait: returned " << n << " event(s) → "
                          << (n == 0 ? "NOT READY (correct for ET — no new data!)" : "READY") << "\n";
                std::cout << "  [ET] ⚠  This is why ET mode MUST drain all data in one read!\n";

                // EN: Drain remaining / TR: Kalanı boşalt
                read(pipefd[0], buf, sizeof(buf));
                et_epoller.remove(pipefd[0]);
            }

            close(pipefd[0]);
            close(pipefd[1]);
        }
        std::cout << "\n";
    }

    // ─── Demo 5: Diagnostic Gateway Simulation ──────────────────────────
    {
        std::cout << "--- Demo 5: Automotive Diagnostic Gateway (UDS/DoIP) ---\n";
        std::cout << "  Simulating diagnostic request/response routing.\n\n";

        // EN: Simulate diagnostic requests from a tester tool
        // TR: Bir test cihazından teşhis isteklerini simüle et
        std::vector<DiagRequest> requests = {
            {UdsService::DiagnosticSessionControl, 0x0010, {0x01}},        // EN: Default session / TR: Varsayılan oturum
            {UdsService::TesterPresent,            0x0010, {0x00}},        // EN: Keep-alive / TR: Canlı tut
            {UdsService::ReadDataByIdentifier,     0x0020, {0xF1, 0x90}},  // EN: Read VIN / TR: VIN oku
            {UdsService::RoutineControl,           0x0030, {0x01, 0xFF, 0x00}}, // EN: Start routine / TR: Rutin başlat
            {UdsService::ECUReset,                 0x0010, {0x01}},        // EN: Hard reset / TR: Sert sıfırlama
        };

        std::cout << "  Tester → Gateway → ECU routing:\n";
        std::cout << "  ┌───────────────────────────┬──────────┬──────────────────────┐\n";
        std::cout << "  │ Service                   │ ECU Addr │ Response             │\n";
        std::cout << "  ├───────────────────────────┼──────────┼──────────────────────┤\n";

        for (const auto& req : requests) {
            auto resp = simulate_ecu_response(req);

            std::cout << "  │ " << std::left << std::setw(26) << uds_service_name(req.service)
                      << "│ 0x" << std::hex << std::setfill('0') << std::setw(4) << req.target_ecu_id
                      << std::dec << std::setfill(' ')
                      << "   │ " << std::setw(21)
                      << (resp.positive ? "Positive (+" + std::to_string(resp.service_id) + "h)" : "Negative")
                      << "│\n";
        }

        std::cout << "  └───────────────────────────┴──────────┴──────────────────────┘\n";

        // EN: Demonstrate timing of diagnostic sessions
        // TR: Teşhis oturumlarının zamanlamasını göster
        std::cout << "\n  Diagnostic session timing (S3 timeout = 5s):\n";
        std::cout << "    Tester → DiagSessionCtrl(0x10, Extended) → ECU: Positive\n";
        std::cout << "    ...3s... Tester → TesterPresent(0x3E) → ECU: Positive (session alive)\n";
        std::cout << "    ...3s... Tester → TesterPresent(0x3E) → ECU: Positive (session alive)\n";
        std::cout << "    ...6s... (no TesterPresent) → ECU: Session timeout → Default session\n";
    }

    std::cout << "\n============================================\n";
    std::cout << "   End of Module 13 — Non-Blocking I/O\n";
    std::cout << "============================================\n";

    return 0;
}
