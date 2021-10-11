#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <syslog.h>
#include <stdlib.h>

enum {
	LAN1_PORT,
	LAN2_PORT,
	LAN3_PORT,
	WAN_PORT,
	__PORT_MAX,
};

enum {
	STATE_VALUE,
	SPEED_VALUE,
	__PORT_INFO_MAX,
};

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

#define MESSAGE_SIZE 1024

static char message[MESSAGE_SIZE];
static struct ubus_context *ctx;
static uint32_t id;
static bool was_connected;

static void port_callback(struct ubus_request *req, int type, struct blob_attr *msg)
{
	struct blob_buf *buf = (struct blob_buf *)req->priv;
	struct blob_attr *tables[__PORT_MAX];
	struct blob_attr *port_state[__PORT_INFO_MAX];

	blobmsg_parse(port_policy, __PORT_MAX, tables, blob_data(msg), blob_len(msg));

    char port_message[__PORT_MAX][255];

	for (int i = 0; i < __PORT_MAX; i++) {
		if (!tables[i]) {
			fprintf(stderr, "failed parsing ports at index %d", i);
			return;
		}
		blobmsg_parse(port_info_policy, __PORT_INFO_MAX, port_state,
			blobmsg_data(tables[i]), blobmsg_data_len(tables[i]));
	
        snprintf(
            port_message[i],
            255,
            " \"%s\": { \"state\": \"%s\", \"speed\": %d }",
            port_policy[i].name,
			blobmsg_get_string(port_state[STATE_VALUE]),
			blobmsg_get_u32(port_state[SPEED_VALUE])
        );
	}
    snprintf(
        message,
        MESSAGE_SIZE,
        "{ %s, %s, %s, %s }",
        port_message[0],
        port_message[1],
        port_message[2],
        port_message[3]
    );
    
}

int call_ubus_ports(char** res)
{
    if (was_connected == false) {
        ctx = ubus_connect(NULL);
        was_connected = true;
    }

    if (!ctx) {
        syslog(LOG_ERR, "Failed to connect to ubus\n");
        return -1;
    }

    if (ubus_lookup_id(ctx, "port_events", &id) ||
        ubus_invoke(ctx, id, "show", NULL, port_callback, NULL, 3000)) {
        syslog(LOG_ERR, "Failed to invoke ubus port_events\n");
        return -1;
    }

    *res = message;
    return 0;
}

int free_ubus()
{
    ubus_free(ctx);
}