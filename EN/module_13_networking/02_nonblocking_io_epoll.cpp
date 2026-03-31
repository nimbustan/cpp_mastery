/**
 * @file module_13_networking/02_nonblocking_io_epoll.cpp
 * @brief Non-Blocking I/O, poll, epoll
 *
 * @details
 * =============================================================================
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
 * =============================================================================
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
 * =============================================================================
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
 * =============================================================================
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
 */
class Socket {
    int fd_ = -1;

public:
    Socket() = default;

    /// EN: Construct from an existing file descriptor (e.g., from accept())
    explicit Socket(int fd) : fd_(fd) {}

    /// EN: Create a new socket with given domain, type, protocol
    static Socket create(int domain = AF_INET, int type = SOCK_STREAM, int protocol = 0) {
        int fd = ::socket(domain, type, protocol);
        if (fd < 0) {
            std::cerr << "  [ERROR] socket() failed: " << strerror(errno) << "\n";
        }
        return Socket(fd);
    }

    // EN: RAII — close on destruction
    ~Socket() { close(); }

    // EN: Move-only (transfer ownership like unique_ptr)
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
    bool set_nonblocking() {
        int flags = fcntl(fd_, F_GETFL, 0);
        if (flags < 0) return false;
        return fcntl(fd_, F_SETFL, flags | O_NONBLOCK) >= 0;
    }

    /// EN: Allow address reuse — prevents "Address already in use" after server restart
    bool set_reuseaddr() {
        int opt = 1;
        return setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) >= 0;
    }

    /// EN: Bind to an address and port
    bool bind(uint16_t port, const char* addr = "0.0.0.0") {
        struct sockaddr_in sa{};
        sa.sin_family = AF_INET;
        sa.sin_port = htons(port);
        inet_pton(AF_INET, addr, &sa.sin_addr);
        return ::bind(fd_, reinterpret_cast<struct sockaddr*>(&sa), sizeof(sa)) >= 0;
    }

    /// EN: Start listening for connections
    bool listen(int backlog = SOMAXCONN) {
        return ::listen(fd_, backlog) >= 0;
    }

    /// EN: Accept a new connection (returns a new Socket with the client fd)
    Socket accept() {
        struct sockaddr_in client_addr{};
        socklen_t len = sizeof(client_addr);
        int client_fd = ::accept(fd_, reinterpret_cast<struct sockaddr*>(&client_addr), &len);
        return Socket(client_fd);
    }

    /// EN: Release ownership of the fd (caller takes responsibility)
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
 */
class PollServer {
    Socket listen_socket_;
    std::vector<struct pollfd> poll_fds_;
    static constexpr int BUFFER_SIZE = 1024;

public:
    /// EN: Initialize the server socket (bind + listen)
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
        poll_fds_.push_back({listen_socket_.fd(), POLLIN, 0});
        return true;
    }

    /// EN: Run one iteration of the poll loop (for demonstration).
    ///     In production, this would be an infinite loop.
    void poll_once(int timeout_ms = 100) {
        int ready = poll(poll_fds_.data(), static_cast<nfds_t>(poll_fds_.size()), timeout_ms);

        if (ready < 0) {
            if (errno != EINTR) {
                std::cerr << "  [PollServer] poll() error: " << strerror(errno) << "\n";
            }
            return;
        }

        if (ready == 0) return;  // EN: Timeout, no events

        // EN: Check the listening socket for new connections
        if (poll_fds_[0].revents & POLLIN) {
            accept_new_client();
        }

        // EN: Check all client sockets for data (iterate backwards for safe removal)
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
        client.set_nonblocking();
        int fd = client.release();  // EN: Transfer ownership to poll_fds_

        poll_fds_.push_back({fd, POLLIN, 0});
        std::cout << "  [PollServer] New client connected (fd=" << fd
                  << "), total clients: " << client_count() << "\n";
    }

    void handle_client(std::size_t idx) {
        char buffer[BUFFER_SIZE];
        ssize_t n = recv(poll_fds_[idx].fd, buffer, sizeof(buffer) - 1, 0);

        if (n <= 0) {
            // EN: Client disconnected or error → remove from poll set
            std::cout << "  [PollServer] Client disconnected (fd=" << poll_fds_[idx].fd << ")\n";
            ::close(poll_fds_[idx].fd);
            poll_fds_.erase(poll_fds_.begin() + static_cast<std::ptrdiff_t>(idx));
            return;
        }

        buffer[n] = '\0';
        std::cout << "  [PollServer] Received from fd=" << poll_fds_[idx].fd
                  << ": " << buffer;

        // EN: Echo back to client
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
 */
class EpollServer {
    Socket listen_socket_;
    int epoll_fd_ = -1;
    std::unordered_map<int, std::string> clients_;  // EN: fd → client info
    static constexpr int MAX_EVENTS = 64;
    static constexpr int BUFFER_SIZE = 4096;

public:
    EpollServer() = default;

    ~EpollServer() {
        // EN: Close all client connections
        for (auto& [fd, info] : clients_) {
            ::close(fd);
        }
        // EN: Close epoll fd
        if (epoll_fd_ >= 0) ::close(epoll_fd_);
    }

    EpollServer(const EpollServer&) = delete;
    EpollServer& operator=(const EpollServer&) = delete;

    /// EN: Initialize: create socket, bind, listen, create epoll instance
    bool init(uint16_t port) {
        // EN: Step 1: Create and configure listening socket
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
        epoll_fd_ = epoll_create1(0);
        if (epoll_fd_ < 0) {
            std::cerr << "  [EpollServer] epoll_create1 failed: " << strerror(errno) << "\n";
            return false;
        }

        // EN: Step 3: Register listening socket with epoll
        struct epoll_event ev{};
        ev.events = EPOLLIN;               // EN: Watch for incoming connections
        ev.data.fd = listen_socket_.fd();
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, listen_socket_.fd(), &ev) < 0) {
            std::cerr << "  [EpollServer] epoll_ctl ADD failed: " << strerror(errno) << "\n";
            return false;
        }

        std::cout << "  [EpollServer] Listening on port " << port << "...\n";
        return true;
    }

    /// EN: Run one iteration of the epoll event loop
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
        while (true) {
            auto client = listen_socket_.accept();
            if (!client.valid()) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) break;  // EN: No more pending
                break;
            }

            client.set_nonblocking();
            int fd = client.release();

            // EN: Register the new client with epoll (watch for read events)
            struct epoll_event ev{};
            ev.events = EPOLLIN | EPOLLET;  // EN: Edge-triggered for efficiency
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
            char buffer[BUFFER_SIZE];
            while (true) {
                ssize_t n = recv(fd, buffer, sizeof(buffer) - 1, 0);

                if (n < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) break;  // EN: All data read
                    remove_client(fd);
                    return;
                }

                if (n == 0) {
                    // EN: Client gracefully disconnected
                    remove_client(fd);
                    return;
                }

                buffer[n] = '\0';
                std::cout << "  [EpollServer] From " << clients_[fd] << ": " << buffer;

                // EN: Echo back
                send(fd, buffer, static_cast<size_t>(n), MSG_NOSIGNAL);
            }
        }
    }

    void remove_client(int fd) {
        // EN: epoll automatically removes closed fds, but we call CTL_DEL for clarity
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

    // EN: Move-only
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
    bool add(int target_fd, uint32_t events) {
        struct epoll_event ev{};
        ev.events = events;
        ev.data.fd = target_fd;
        return epoll_ctl(fd_, EPOLL_CTL_ADD, target_fd, &ev) >= 0;
    }

    /// EN: Modify events for an already-registered fd
    bool modify(int target_fd, uint32_t events) {
        struct epoll_event ev{};
        ev.events = events;
        ev.data.fd = target_fd;
        return epoll_ctl(fd_, EPOLL_CTL_MOD, target_fd, &ev) >= 0;
    }

    /// EN: Remove a file descriptor from epoll
    bool remove(int target_fd) {
        return epoll_ctl(fd_, EPOLL_CTL_DEL, target_fd, nullptr) >= 0;
    }

    /// EN: Wait for events with timeout
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
 */

/// EN: UDS (Unified Diagnostic Services) service identifiers — ISO 14229
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
struct DiagRequest {
    UdsService service;
    uint16_t target_ecu_id;            // EN: Logical ECU address
    std::vector<uint8_t> payload;
};

/// EN: Simulated diagnostic response
struct DiagResponse {
    uint8_t service_id;                // EN: service + 0x40 = positive response
    uint16_t source_ecu_id;
    std::vector<uint8_t> data;
    bool positive;
};

DiagResponse simulate_ecu_response(const DiagRequest& req) {
    DiagResponse resp;
    resp.service_id = static_cast<uint8_t>(req.service) + 0x40;  // EN: Positive response
    resp.source_ecu_id = req.target_ecu_id;
    resp.positive = true;

    switch (req.service) {
        case UdsService::ReadDataByIdentifier:
            resp.data = {0xF1, 0x90, 'V', 'I', 'N', '1', '2', '3'};  // EN: VIN example
            break;
        case UdsService::TesterPresent:
            resp.data = {};  // EN: Empty positive response
            break;
        default:
            resp.data = {0x00};  // EN: Generic positive
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
        Socket sock = Socket::create(AF_INET, SOCK_STREAM, 0);
        if (sock.valid()) {
            std::cout << "  Socket created: fd=" << sock.fd() << "\n";

            // EN: Set non-blocking — recv will return EAGAIN instead of sleeping
            if (sock.set_nonblocking()) {
                std::cout << "  Set to non-blocking mode (O_NONBLOCK)\n";

                // EN: Verify with fcntl
                int flags = fcntl(sock.fd(), F_GETFL, 0);
                std::cout << "  Socket flags: 0x" << std::hex << flags << std::dec;
                if (flags & O_NONBLOCK) {
                    std::cout << " [O_NONBLOCK is SET]\n";
                } else {
                    std::cout << " [O_NONBLOCK is NOT set]\n";
                }
            }

            // EN: Set address reuse — essential for server sockets
            if (sock.set_reuseaddr()) {
                std::cout << "  SO_REUSEADDR enabled\n";
            }
        }

        std::cout << "  Socket will be closed automatically (RAII)\n\n";
        // EN: Socket closes here — RAII destructor
    }

    // ─── Demo 2: poll()-Based Server Concept ─────────────────────────────
    {
        std::cout << "--- Demo 2: poll() Server Architecture ---\n";
        std::cout << "  poll() provides I/O multiplexing without the FD limit of select().\n\n";

        // EN: Demonstrate poll concepts without starting a server
        //     (starting a server would require a separate client process)
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
        int pipefd[2];
        if (pipe(pipefd) == 0) {
            // EN: Write some data to the pipe
            const char* msg = "Hello from pipe!";
            write(pipefd[1], msg, strlen(msg));

            // EN: Poll the read end — should be immediately ready
            struct pollfd pfd{};
            pfd.fd = pipefd[0];
            pfd.events = POLLIN;

            int ready = poll(&pfd, 1, 0);  // EN: timeout=0 → non-blocking check
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
        EpollHandle epoller;
        if (epoller.valid()) {
            std::cout << "  epoll instance created: fd=" << epoller.fd() << "\n";

            // EN: Demonstrate with a pipe (like demo 2, but using epoll)
            int pipefd[2];
            if (pipe(pipefd) == 0) {
                // EN: Set read end to non-blocking
                int flags = fcntl(pipefd[0], F_GETFL, 0);
                fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);

                // EN: Register read end with epoll
                if (epoller.add(pipefd[0], EPOLLIN | EPOLLET)) {
                    std::cout << "  Registered pipe read-end with epoll (EPOLLIN | EPOLLET)\n";
                }

                // EN: Write data to trigger the event
                const char* msg = "epoll event triggered!";
                write(pipefd[1], msg, strlen(msg));

                // EN: Wait for events (should be immediately ready)
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
        int pipefd[2];
        if (pipe(pipefd) == 0) {
            int flags = fcntl(pipefd[0], F_GETFL, 0);
            fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);

            // EN: Test Level-Triggered (LT) behavior
            {
                EpollHandle lt_epoller;
                lt_epoller.add(pipefd[0], EPOLLIN);  // EN: No EPOLLET = Level-Triggered

                // EN: Write 10 bytes
                write(pipefd[1], "1234567890", 10);

                // EN: First epoll_wait — should be ready (10 bytes available)
                struct epoll_event events[4];
                int n = lt_epoller.wait(events, 4, 50);
                std::cout << "  [LT] After writing 10 bytes: epoll_wait returned " << n << " event(s)\n";

                // EN: Read only 5 bytes (partial read)
                char buf[16];
                read(pipefd[0], buf, 5);
                std::cout << "  [LT] Read 5 bytes. 5 bytes remaining in pipe.\n";

                // EN: Second epoll_wait — LT will STILL report ready (5 bytes remain!)
                n = lt_epoller.wait(events, 4, 50);
                std::cout << "  [LT] Second epoll_wait: returned " << n << " event(s) → "
                          << (n > 0 ? "STILL READY (correct for LT)" : "NOT ready") << "\n";

                // EN: Drain remaining
                read(pipefd[0], buf, sizeof(buf));
                lt_epoller.remove(pipefd[0]);
            }

            // EN: Test Edge-Triggered (ET) behavior
            {
                EpollHandle et_epoller;
                et_epoller.add(pipefd[0], EPOLLIN | EPOLLET);  // EN: EPOLLET = Edge-Triggered

                // EN: Write 10 bytes
                write(pipefd[1], "ABCDEFGHIJ", 10);

                struct epoll_event events[4];
                int n = et_epoller.wait(events, 4, 50);
                std::cout << "\n  [ET] After writing 10 bytes: epoll_wait returned " << n << " event(s)\n";

                // EN: Read only 5 bytes (partial read)
                char buf[16];
                read(pipefd[0], buf, 5);
                std::cout << "  [ET] Read 5 bytes. 5 bytes remaining.\n";

                // EN: Second epoll_wait — ET will NOT report ready (no NEW data arrived!)
                n = et_epoller.wait(events, 4, 50);
                std::cout << "  [ET] Second epoll_wait: returned " << n << " event(s) → "
                          << (n == 0 ? "NOT READY (correct for ET — no new data!)" : "READY") << "\n";
                std::cout << "  [ET] ⚠  This is why ET mode MUST drain all data in one read!\n";

                // EN: Drain remaining
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
        std::vector<DiagRequest> requests = {
            {UdsService::DiagnosticSessionControl, 0x0010, {0x01}},        // EN: Default session
            {UdsService::TesterPresent,            0x0010, {0x00}},        // EN: Keep-alive
            {UdsService::ReadDataByIdentifier,     0x0020, {0xF1, 0x90}},  // EN: Read VIN
            {UdsService::RoutineControl,           0x0030, {0x01, 0xFF, 0x00}}, // EN: Start routine
            {UdsService::ECUReset,                 0x0010, {0x01}},        // EN: Hard reset
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
