#include <stdio.h>
#include <signal.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>

#include <iotp_device.h>
#include <syslog.h>

#include "ubus.h"

volatile sig_atomic_t deamonize = 1;
char *configFilePath = "/tmp/watsond.yaml";

void term_proc(int sigterm) 
{
	deamonize = 0;
}

void MQTTTraceCallback (int level, char * message)
{
    if ( level > 0 )
        fprintf(stdout, "%s\n", message? message:"NULL");
    fflush(stdout);
}

int main(int argc, char *argv[])
{
    int rc = 0;

    IoTPConfig *config = NULL;
    IoTPDevice *device = NULL;

	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = term_proc;
	sigaction(SIGTERM, &action, NULL);

    openlog("syslog_log", LOG_PID, LOG_USER);
    syslog(LOG_INFO, "The process watsond is starting...");

    /* Set IoTP Client log handler */
    rc = IoTPConfig_setLogHandler(IoTPLog_FileDescriptor, stdout);
    if ( rc != 0 ) {
        syslog(LOG_ERR, "Failed to set IoTP Client log handler: rc=%d\n", rc);
        exit(1);
    }

    /* Create IoTPConfig object using configuration options defined in the configuration file. */
    rc = IoTPConfig_create(&config, configFilePath);
    if ( rc != 0 ) {
        syslog(LOG_ERR, "Failed to initialize configuration: rc=%d\n", rc);
        exit(1);
    }

    IoTPConfig_readEnvironment(config);

    /* Create IoTPDevice object */
    rc = IoTPDevice_create(&device, config);
    if ( rc != 0 ) {
        syslog(LOG_ERR, "Failed to configure IoTP device: rc=%d\n", rc);
        exit(1);
    }

    /* Set MQTT Trace handler */
    rc = IoTPDevice_setMQTTLogHandler(device, &MQTTTraceCallback);
    if ( rc != 0 ) {
        syslog(LOG_WARNING, "Failed to set MQTT Trace handler: rc=%d\n", rc);
    }

    /* Invoke connection API IoTPDevice_connect() to connect to WIoTP. */
    rc = IoTPDevice_connect(device);
    if ( rc != 0 ) {
        syslog(LOG_ERR, "Failed to connect to Watson IoT Platform: rc=%d\n", rc);
        syslog(LOG_ERR, "Returned error reason: %s\n", IOTPRC_toString(rc));
        exit(1);
    }

    syslog(LOG_INFO, "Initialization phase for watsond finished, sending messages.\n");

    char* message = NULL;

    while (deamonize)
    {
        call_ubus_ports(&message);
        rc = IoTPDevice_sendEvent(device, "status", message, "json", QoS0, NULL);
        if (rc != 0)
            syslog(LOG_INFO, "RC from publishEvent(): %d\n", rc);
        
        sleep(10);
    }

    syslog(LOG_INFO, "Publish event cycle is complete, attempting to disconnect.\n");

    /* Disconnect device */
    rc = IoTPDevice_disconnect(device);
    if ( rc != IOTPRC_SUCCESS ) {
        syslog(LOG_ERR, "ERROR: Failed to disconnect from Watson IoT Platform: rc=%d\n", rc);
        exit(1);
    }
    else {
        syslog(LOG_INFO, "Disconnected from the Watson IoT Platform\n");
    }

    /* Destroy client and clear configuration */
    IoTPDevice_destroy(device);
    IoTPConfig_clear(config);

    free_ubus();

    closelog();
    return 0;
}