#include <stdio.h>
#include <signal.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>

#include <iotp_device.h>
#include <syslog.h>
#include <uci.h>

#include "ubus.h"

volatile sig_atomic_t deamonize = 1;

#define IOTP_NOCLEANUP -351
#define UCI_VALUE_UNSET -401
#define UCI_CONFIG_PREFIX "watsond.general."

void term_proc(int sigterm) 
{
	deamonize = 0;
}

void mqqt_trace_cb(int level, char * message)
{
    if (level > 0)
        syslog(LOG_ERR, "%s\n", message? message: "MQTT error with no traceback message");
}

bool uci_get_enable(struct uci_context* ctx)
{
    #define BUFFER_SIZE 256
    char buffer[BUFFER_SIZE];
    buffer[0] = '\0';
    char uci_path[BUFFER_SIZE];
    uci_path[0] = '\0';

    strncpy(uci_path, UCI_CONFIG_PREFIX "enable", BUFFER_SIZE - 1 - strlen(uci_path));

    struct uci_ptr ptr;
    if (uci_lookup_ptr(ctx, &ptr, uci_path, true) != UCI_OK) {
        return false;
    }

    strncpy(buffer, ptr.o->v.string, BUFFER_SIZE - 1);
    if (strcmp(buffer, "1") == 0) {
        return true;
    }
    else {
        return false;
    }
}

int set_iotp_entry(IoTPConfig** conf, struct uci_context* ctx, const char* entry, const char* IoTPConfig_identity_name)
{
    int rc = 0;

    #define BUFFER_SIZE 256
    char buffer[BUFFER_SIZE];
    buffer[0] = '\0';
    char uci_path[BUFFER_SIZE];
    uci_path[0] = '\0';

    strncat(uci_path, UCI_CONFIG_PREFIX, BUFFER_SIZE - 1 - strlen(uci_path));
    strncat(uci_path, entry, BUFFER_SIZE - 1 - strlen(uci_path));

    struct uci_ptr ptr;

    if ((rc = uci_lookup_ptr(ctx, &ptr, uci_path, true)) != UCI_OK)
    {
        syslog(LOG_ERR, "Can not find watsond.general.%s", entry);
        return rc;
    }

    strncpy(buffer, ptr.o->v.string, BUFFER_SIZE - 1);
    if (strcmp(buffer, "") == 0) {
        syslog(LOG_ERR, "%s is not set!\n", entry);
        return UCI_VALUE_UNSET;
    }
    IoTPConfig_setProperty(*conf, IoTPConfig_identity_name, buffer);

    return rc;
}

int read_uci_config(IoTPConfig** conf)
{
    int rc = 0;
    struct uci_context *ctx;

    ctx = uci_alloc_context();
    if (uci_get_enable(ctx) == false) {
        syslog(LOG_INFO, "enable is unset, aborting the program");
        return UCI_VALUE_UNSET;
    }

    rc = set_iotp_entry(conf, ctx, "orgId", IoTPConfig_identity_orgId);
    if (rc != 0)
        return rc;

    rc = set_iotp_entry(conf, ctx, "typeId", IoTPConfig_identity_typeId);
    if (rc != 0)
        return rc; 

    rc = set_iotp_entry(conf, ctx, "deviceId", IoTPConfig_identity_deviceId);
    if (rc != 0)
        return rc;

    rc = set_iotp_entry(conf, ctx, "authtoken", IoTPConfig_auth_token);
    if (rc != 0)
        return rc; 

    uci_free_context(ctx);
    return 0;
}

/* 
 * Creates a client connection with IBM Watson platform
 * Upon sucessful setup, return value is 0. Memory has to be cleaned with supplementary cleanup function
 * Upon unsucessful setup, return value is not 0. In case of failure memory is cleaned and no supplementary cleanup function needs to be called
 */ 
int iotp_device_setup(IoTPConfig** out_conf, IoTPDevice** out_dev)
{
    int rc = 0;

    /* Create IoTPConfig object using configuration options defined in the configuration file. */
    rc = IoTPConfig_create(out_conf, NULL);
    if (rc != 0) {
        syslog(LOG_ERR, "Failed to initialize configuration: rc=%d\n", rc);
        goto setup_fail;
    }
    rc = read_uci_config(out_conf);
    if (rc != 0) {
        syslog(LOG_ERR, "Failed to set configuration values: rc=%d\n", rc);
        goto setup_fail;
    }
     syslog(LOG_INFO, "enable is set, csdfdasf the programsfdfdsfsdsfd");

    /* Create IoTPDevice object */
    rc = IoTPDevice_create(out_dev, *out_conf);
    if (rc != 0) {
        syslog(LOG_ERR, "Failed to configure IoTP device: rc=%d\n", rc);
        goto setup_fail;
    }

     /* Set MQTT Trace handler */
    rc = IoTPDevice_setMQTTLogHandler(*out_dev, &mqqt_trace_cb);
    if (rc != 0) {
        syslog(LOG_WARNING, "Failed to set MQTT Trace handler: rc=%d\n", rc);
    }

    /* Invoke connection API IoTPDevice_connect() to connect to WIoTP. */
    rc = IoTPDevice_connect(*out_dev);
    if (rc != 0) {
        syslog(LOG_ERR, "Failed to connect to Watson IoT Platform, reason: %s\n", IOTPRC_toString(rc));
        goto setup_fail;
    }

setup_fail:
    return rc;
}

/* 
 * Cleans poinif ers. preventing memory leak
 * Only call when setup has been performed previously
 */ 
int iotp_device_cleanup(IoTPConfig* out_conf, IoTPDevice* out_dev)
{
    int rc = 0;

    if (out_dev == NULL) {
        syslog(LOG_INFO, "Failed to cleanup device because it was not found");
        goto conf_cleanup;
    }
    /* Disconnect device */
    rc = IoTPDevice_disconnect(out_dev);
    if (rc != IOTPRC_SUCCESS) {
        syslog(LOG_ERR, "Failed to disconnect from Watson IoT Platform, reason: %s\n", IOTPRC_toString(rc));
    }
    else {
        syslog(LOG_INFO, "Disconnected from the Watson IoT Platform, reason: %s\n", IOTPRC_toString(rc));
    }

    /* Destroy client and clear configuration */
    rc = IoTPDevice_destroy(out_dev);
    if (rc != 0) {
        (LOG_ERR, "Failed to free the device rc=%d\n", rc);
    }

conf_cleanup: 
    if (out_conf == NULL) {
        syslog(LOG_INFO, "Failed to cleanup device because it was not found");
        return IOTP_NOCLEANUP;
    }

    rc = IoTPConfig_clear(out_conf);
    if (rc != 0) {
        (LOG_ERR, "Failed to free the config rc=%d\n", rc);
    }

    return rc;
}

int message_cycle(IoTPDevice** device_ref)
{
    struct message *ubus_msg;
    char *msg_json = NULL;
    int rc = 0;
    rc = start_ubus();
    if (rc != 0)
        return rc;

    while (deamonize)
    {
        rc = call_ubus_ports(&ubus_msg);
        if (rc != 0) {
            syslog(LOG_ERR, "Failure to retrieve the message to send\n");
            break;
        }

        rc = message_to_json(ubus_msg, &msg_json);
        if (rc != 0) {
            syslog(LOG_ERR, "Failure while parsing message to json\n");
            break;
        }
        rc = IoTPDevice_sendEvent(*device_ref, "status", msg_json, "json", QoS0, NULL);
        if (rc != 0) {
            syslog(LOG_ERR, "Failure to send event. RC from publishEvent(): %d\n", rc);
            break;
        }
        free(ubus_msg);
        free(msg_json);
        sleep(10);
    }

    free_ubus(ubus_msg, msg_json);
    return rc;
}

int main(int argc, char *argv[])
{
    int rc = 0;
    
    IoTPConfig* config = NULL;
    IoTPDevice* device = NULL;

    struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = term_proc;
	sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);

    openlog("syslog_log", LOG_PID, LOG_USER);
    syslog(LOG_INFO, "Process watsond is starting...");

    rc = iotp_device_setup((IoTPConfig**) &config, (IoTPDevice**) &device);
    if (rc == 0) {
        syslog(LOG_INFO, "Initialization phase for watsond finished, sending messages.\n");
    }
    else {
        goto clean_iotp;
    }

    rc = message_cycle(&device);
    if (rc == 0) {
        syslog(LOG_INFO, "Publish event cycle is sucessfully complete, attempting to disconnect.\n");
    }
    else {
        syslog(LOG_WARNING, "Publish event cycle was interrupted, attempting to disconnect, rc = %d.\n", rc);
    }

clean_iotp:
    rc = iotp_device_cleanup(config, device);
clean_general:
    syslog(LOG_INFO, "Program exited with error code %d.\n", rc);
    closelog();
    return rc;
}