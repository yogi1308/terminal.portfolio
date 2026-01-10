#include <iostream>

#include <libssh/libssh.h>
#include <libssh/server.h>

#include "./helpers/auth_setup.hpp"
#include "./helpers/routes.hpp"

#include "session_handler.hpp"

void handle_client_session(ssh_session session) {
    std::cout << "[Thread] New session thread started." << std::endl;

    if (ssh_handle_key_exchange(session) != SSH_OK) {
        std::cerr << "Handshake failed: " << ssh_get_error(session) << std::endl;
        ssh_free(session);
        return;
    }

    ssh_channel channel = auth_and_setup(session); // changes channel and shell 

    routes(channel);

    // 4. Cleanup when done
    ssh_disconnect(session);
    ssh_free(session);
    std::cout << "[Thread] Session finished and closed." << std::endl;
}