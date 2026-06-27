#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <kernel.h>

#define SERVER_PORT 9025

#define CMD_NOTIFY     100
#define CMD_SHUTDOWN   101
#define CMD_REBOOT     102
#define CMD_REST_MODE  103

struct CommandPacket {
    int cmd_type;
    char message[256];
};

void show_notification(const char* text) {
    #ifdef sceKernelSendNotificationRequest
        sceKernelSendNotificationRequest(0, text, strlen(text), 0);
    #endif
}

int main(int argc, char *argv[]) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    struct CommandPacket packet;

    show_notification("Power Control PS5: Payload Loaded!");

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(SERVER_PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 5) < 0) {
        close(server_fd);
        return -1;
    }

    while(1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket >= 0) {
            memset(&packet, 0, sizeof(packet));
            int bytes_read = read(new_socket, &packet, sizeof(packet));
            
            if (bytes_read > 0) {
                switch(packet.cmd_type) {
                    case CMD_NOTIFY:
                        show_notification(packet.message);
                        break;
                    case CMD_SHUTDOWN:
                        show_notification("Vox Manager: Shutting down...");
                        usleep(1000000);
                        sceKernelShutdownSystem(); 
                        break;
                    case CMD_REBOOT:
                        show_notification("Vox Manager: Rebooting...");
                        usleep(1000000);
                        sceKernelReboot();
                        break;
                    case CMD_REST_MODE:
                        show_notification("Vox Manager: Rest Mode...");
                        usleep(1000000);
                        sceKernelSuspendSystem();
                        break;
                    default:
                        break;
                }
            }
            close(new_socket);
        }
        usleep(50000); 
    }

    close(server_fd);
    return 0;
}

