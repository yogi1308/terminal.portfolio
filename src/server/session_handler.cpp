#include <iostream>
#include <atomic> // [Required] Thread-safe counter
#include <mutex>  // [Required] Prevents garbled console output

#include <libssh/libssh.h>
#include <libssh/server.h>

#include "./helpers/auth_setup.hpp"
#include "./helpers/routes.hpp"

#include "session_handler.hpp"

// 'atomic' ensures multiple threads can update this number simultaneously without errors.
static std::atomic<int> active_sessions{0}; 
// 'mutex' ensures two threads don't try to print to the console at the exact same time.
static std::mutex print_mutex;

void handle_client_session(ssh_session session) {
    int current_count = ++active_sessions;
    std::cout << "[Thread] New session thread started." << std::endl;

    {
        // Lock the console just for this print statement
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cout << "[Session] New connection. Active sessions: " << current_count << std::endl;
    }

    if (ssh_handle_key_exchange(session) != SSH_OK) {
        std::cerr << "Handshake failed: " << ssh_get_error(session) << std::endl;
        current_count = --active_sessions;
        {
            std::lock_guard<std::mutex> lock(print_mutex);
            std::cout << "[Session] Handshake failed. Active sessions: " << current_count << std::endl;
        }
        ssh_free(session);
        return;
    }

    ssh_channel channel = auth_and_setup(session); // changes channel and shell 

    if (channel != nullptr) {
        routes(channel);
    } else {
        std::cerr << "Client failed to setup shell." << std::endl;
    }

    // 4. Cleanup when done

    current_count = --active_sessions;
    {
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cout << "[Session] Disconnected. Active sessions: " << current_count << std::endl;
    }
    ssh_disconnect(session);
    ssh_free(session);
    std::cout << "[Thread] Session finished and closed." << std::endl;
}