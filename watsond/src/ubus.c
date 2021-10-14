#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <syslog.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "ubus.h"

#define JSON_SIZE 1024
#define MESSAGE_QUARTER 256

static const struct blobmsg_policy port_policy[__PORT_MAX] = {
	[LAN1_PORT] = { .name = "LAN 1", .type = BLOBMSG_TYPE_TABLE },
	[LAN2_PORT] = { .name = "LAN 2", .type = BLOBMSG_TYPE_TABLE },
	[LAN3_PORT] = { .name = "LAN 3", .type = BLOBMSG_TYPE_TABLE },
	[WAN_PORT] = { .name = "WAN", .type = BLOBMSG_TYPE_TABLE },
};

static const struct blobmsg_policy port_info_policy[__PORT_INFO_MAX] = {
	[STATE_VALUE] = { .name = "state", .type = BLOBMSG_TYPE_STRING },
	[SPEED_VALUE] = { .name = "speed", .type = BLOBMSG_TYPE_INT32},
};

static struct ubus_context *g_ctx;
static bool is_connected = false;

static void port_callback(struct ubus_request *req, int type, struct blob_attr *msg)
{
	struct message *buffer = (struct message *)req->priv;
	struct blob_attr *tables[__PORT_MAX];
	struct blob_attr *port_state[__PORT_INFO_MAX];

	blobmsg_parse(port_policy, __PORT_MAX, tables, blob_data(msg), blob_len(msg));

    buffer->rc = 0;
	for (int i = 0; i < __PORT_MAX; i++) {
		if (!tables[i]) {
			syslog(LOG_ERR, "failed parsing ports at index %d", i);
            buffer->rc = MY_UBUS_MSG_RECEIVE_FAIL;
			return;
		}
		blobmsg_parse(port_info_policy, __PORT_INFO_MAX, port_state,
			blobmsg_data(tables[i]), blobmsg_data_len(tables[i]));
        buffer->ports[i].state[0] = '\0';
        strncat(buffer->ports[i].state, blobmsg_get_string(port_state[STATE_VALUE]), STATE_MSG_SIZE);
        buffer->ports[i].speed = blobmsg_get_u32(port_state[SPEED_VALUE]);
	}    
}

int message_to_json(struct message *in_ptr, char ** out_ptr)
{
    *out_ptr = (char*)malloc(sizeof(char) * JSON_SIZE);
    if (*out_ptr == NULL) {
        return MY_UBUS_MEMALLOC_FAIL;
    }
    char port_message[__PORT_MAX][MESSAGE_QUARTER];
    for (int i = 0; i < __PORT_MAX; i++) {
        snprintf(
            port_message[i],
            MESSAGE_QUARTER,
            " \"%s\": { \"state\": \"%s\", \"speed\": %d }",
            port_policy[i].name,
			in_ptr->ports[i].state,
			in_ptr->ports[i].speed
        );
    }
    snprintf(
        *out_ptr,
        JSON_SIZE,
        "{ %s, %s, %s, %s }",
        port_message[0],
        port_message[1],
        port_message[2],
        port_message[3]
    );

    return 0;
}

int start_ubus()
{
    if (is_connected == false) {
        g_ctx = ubus_connect(NULL);
        is_connected = true;
    }

    if (!g_ctx) {
        syslog(LOG_ERR, "Failed to connect to ubus\n");
        return MY_UBUS_CREATION_FAIL;
    }
    return 0;
}

int call_ubus_ports(struct message** out_ptr)
{
    *out_ptr = (struct message*)malloc(sizeof(struct message));

    uint32_t id;
    if (ubus_lookup_id(g_ctx, "port_events", &id) ||
        ubus_invoke(g_ctx, id, "show", NULL, port_callback, *out_ptr, 3000)) {

        syslog(LOG_ERR, "Failed to invoke ubus port_events\n");
        return MY_UBUS_MSG_RECEIVE_FAIL;
    }
    
    if (*out_ptr == NULL || (*out_ptr)->rc != 0) {
        return MY_UBUS_MSG_RECEIVE_FAIL;
    }
    return 0;
}

void free_ubus(struct message *msg_ptr, char *string_ptr)
{
    if (msg_ptr != NULL) {
        free(msg_ptr);
    }
    if (string_ptr != NULL) {
        free(string_ptr);
    }

    ubus_free(g_ctx);
    is_connected = false;
}

int execute_ubus(struct message** out_ptr) {
    struct ubus_context *local_ctx;
    uint32_t id;

    local_ctx = ubus_connect(NULL);
    if (!local_ctx) {
        syslog(LOG_ERR, "Failed to connect to ubus\n");
        return MY_UBUS_CREATION_FAIL;
    }

    if (*out_ptr) {
        free(*out_ptr);
    }
    if (ubus_lookup_id(local_ctx, "port_events", &id) ||
        ubus_invoke(local_ctx, id, "show", NULL, port_callback, (void *)(*out_ptr), 3000)) {

        syslog(LOG_ERR, "Failed to invoke ubus port_events\n");
        return MY_UBUS_MSG_RECEIVE_FAIL;
    }
    if (*out_ptr || (*out_ptr)->rc != 0) {
        return MY_UBUS_MSG_RECEIVE_FAIL;
    }

    ubus_free(local_ctx);
    
    return 0;
}