#include "bits/pthreadtypes.h"
#include "netinet/in.h"
#include <netinet/ip.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 128
#define MAX_CLIENTS 128

/* utility functions */

// returns true if a port is a valid port
bool is_valid_port(int port) {
	return port >= 1024 && port <= 49151;
}

// initializes a filedescriptor for the server, binds it to the provided port and starts listening
int init_server_fd(uint16_t port) {
	int res;
	int fd;
	struct sockaddr_in addr;

	// create socket
	fd = socket(PF_INET, SOCK_STREAM, 0);

	// create socket address
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	// bind socket
	res = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
	if (res == -1) {
		perror("bind");
		close(fd);
		return -1;
	}

	// start listening (128 clients max)
	res = listen(fd, MAX_CLIENTS);
	if (res == -1) {
		perror("bind");
		close(fd);
		return -2;
	}

	return fd;
}

/* data */

typedef struct config {
	int fd;
	int port;
	int connected_clients;
	char* admins[5];
} config_t;

config_t* config_init(int argc, char* argv[]) {
	// generate config
	config_t* config = malloc(sizeof(*config));

	// get port
	config->port = atoi(argv[1]);
	if (!is_valid_port(config->port)) {
		printf("invalod por");
		free(config);
		return NULL;
	}

	// populate admins
	for (int i = 2; i < argc; i++) {
		config->admins[i - 2] = argv[i];
	}

	// create file descriptor
	config->fd = init_server_fd(config->port);
	if (config->fd < 0) {
		printf("invalod fd");
		free(config);
		return NULL;
	}

	config->connected_clients = 0;

	return config;
}

void* config_free(config_t* config) {
	free(config);
}

typedef struct server {
	config_t* config;
	pthread_mutex_t conf_mutex;
	pthread_t listener_thread;
} server_t;

server_t* server_init(config_t* config) {
	server_t* server = malloc(sizeof(*server));

	server->config = config;
	pthread_mutex_init(&server->conf_mutex, NULL);

	return server;
}

server_t* server_free(server_t* server) {
	free(server->config);
	free(server);
}

bool server_user_is_admin(server_t* server, char* name) {
	pthread_mutex_lock(&server->conf_mutex);
	for (size_t i = 0; i < 5; i++) {
		if (strcmp(server->config->admins[i], name) != 0) {
			return true;
		}
	}
	pthread_mutex_unlock(&server->conf_mutex);

	return false;
}

typedef struct conversation {
	server_t* server;
	pthread_t thread;
	int fd;
	char* name;
} conversation_t;

conversation_t* conversation_init(int fd, server_t* server) {
	conversation_t* conv = malloc(sizeof(*conv));

	conv->fd = fd;
	conv->server = server;
	conv->name = NULL;

	pthread_mutex_lock(&server->conf_mutex);
	server->config->connected_clients++;
	pthread_mutex_unlock(&server->conf_mutex);

	return conv;
}

void* conversation_free(conversation_t* conv) {

	pthread_mutex_lock(&conv->server->conf_mutex);
	conv->server->config->connected_clients++;
	pthread_mutex_unlock(&conv->server->conf_mutex);

	free(conv->name);
	free(conv);
}

/* thread routines */

void* conversation(void* arg_conv) {
	if (!arg_conv) {
		pthread_exit(NULL);
	}

	conversation_t* conv = (conversation_t*)arg_conv;

	char msg_buffer[BUF_SIZE];
	int res, msg_count = 0;

	while (1) {
		res = recv(conv->fd, &msg_buffer, BUF_SIZE, 0);
		if (res == -1) {
			perror("recv");
			close(conv->fd);
			pthread_exit(NULL);
		} else if (res == 0) {
			printf("%s disconnected%s.\n", conv->name,
			       server_user_is_admin(conv->server, conv->name) ? " (admin)" : "");

			close(conv->fd);
			break;
		}

		// replace newlines
		for (size_t i = 0; i < BUF_SIZE; i++) {
			if (msg_buffer[i] == '\n') {
				msg_buffer[i] = '\0';
			}
		}

		msg_count++;
		if (msg_count > 1) {
			printf("%s: %s\n", conv->name, msg_buffer);
		} else if (msg_count == 1) {
			conv->name = malloc(sizeof(char) * strlen(msg_buffer) + 1);
			strcpy(conv->name, msg_buffer);

			printf("%s connected%s.\n", conv->name,
			       server_user_is_admin(conv->server, conv->name) ? " (admin)" : "");
		} else if (strcmp(msg_buffer, "/shutdown") && server_user_is_admin(conv->server, conv->name)) {
			pthread_cancel(conv->server->listener_thread);

			printf("Server is shutting down.\n");

			pthread_mutex_lock(&conv->server->conf_mutex);
			printf("Waiting for %d client(s) to disconnect.", conv->server->config->connected_clients);
			pthread_mutex_unlock(&conv->server->conf_mutex);
		}
	}
}

void* listener(void* arg_server) {
	if (!arg_server) {
		pthread_exit(NULL);
	}

	server_t* server = (server_t*)arg_server;

	while (1) {
		pthread_mutex_lock(&server->conf_mutex);
		int conversation_fd = accept(server->config->fd, (struct sockaddr*)NULL, NULL);
		pthread_mutex_unlock(&server->conf_mutex);
		if (conversation_fd == -1) {
			perror("accept");
			pthread_exit(NULL);
		}

		conversation_t* conv = conversation_init(conversation_fd, server);

		pthread_create(&conv->thread, NULL, conversation, conv);
	}
}

int main(int argc, char* argv[]) {
	// usage
	if (argc < 3) {
		printf("usage: %s <port> [admins... <=5]\n", argv[0]);
		exit(EXIT_SUCCESS);
	}

	config_t* config = config_init(argc, argv);
	server_t* server = server_init(config);

	pthread_create(&server->listener_thread, NULL, listener, server);
	pthread_join(server->listener_thread, NULL);
}
