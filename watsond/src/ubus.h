#ifndef FILE_UBUS_H
#define FILE_UBUS_H

/* 
 * Creates an ubus context and makes a call to port_events.
 * char* res assigns a pointer to the message. Call via reference. This pointer should not be freed directly
 * This function can be called any number of times but
 * the allocated memory needs to be freed via free_ubus() function at the end of program termination
 */ 
int call_ubus_ports(char** res);

/* 
 * Free the memory used in the call_ubus_ports function.
 * Call only once at the end of the program.
 */ 
int free_ubus();

#endif