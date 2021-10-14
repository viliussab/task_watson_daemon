#ifndef FILE_UBUS_H
#define FILE_UBUS_H

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

#define MY_UBUS_CREATION_FAIL -251
#define MY_UBUS_REPEATED_CREATION -252
#define MY_UBUS_MSG_RECEIVE_FAIL -253
#define MY_UBUS_MEMFREE_FAIL -254
#define MY_UBUS_MEMALLOC_FAIL -255

#define STATE_MSG_SIZE 64

struct port_state {
    char state[STATE_MSG_SIZE] ;
    int speed;
};

struct message {
    int rc;
    struct port_state ports [__PORT_MAX];
};

int start_ubus();

/* 
 * Creates an ubus context and makes a call to port_events.
 * char* res assigns a pointer to the message. Call via reference. This pointer should not be freed directly
 * This function can be called any number of times but
 * the allocated memory needs to be freed via free_ubus() function at the end of program termination
 */ 
int call_ubus_ports(struct message** out_ptr);

/* 
 * Free the memory used in the call_ubus_ports function.
 * Call only once at the end of the program.
 */ 
void free_ubus(struct message *msg_ptr, char *string_ptr);

int execute_ubus(struct message** out_ptr);

int message_to_json(struct message *in_ptr, char ** out_ptr);

#endif