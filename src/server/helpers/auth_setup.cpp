#include <libssh/libssh.h>
#include <libssh/server.h>
#include "auth_setup.hpp"

void auth_and_setup(bool &shell_started, ssh_channel &channel, ssh_session session) {
    ssh_message msg;
    while (!shell_started && (msg = ssh_message_get(session))) {
        int type = ssh_message_type(msg);
        int subtype = ssh_message_subtype(msg);

        if (type == SSH_REQUEST_AUTH && subtype == SSH_AUTH_METHOD_NONE) {
            ssh_message_auth_reply_success(msg, 0);
        } 
        else if (type == SSH_REQUEST_CHANNEL_OPEN && subtype == SSH_CHANNEL_SESSION) {
            channel = ssh_message_channel_request_open_reply_accept(msg);
        } 
        else if (type == SSH_REQUEST_CHANNEL && subtype == SSH_CHANNEL_REQUEST_PTY) {
            ssh_message_channel_request_reply_success(msg);
        } 
        else if (type == SSH_REQUEST_CHANNEL && subtype == SSH_CHANNEL_REQUEST_SHELL) {
            ssh_message_channel_request_reply_success(msg);
            shell_started = true; 
        } 
        else {
            ssh_message_reply_default(msg);
        }
        ssh_message_free(msg);
    }

}