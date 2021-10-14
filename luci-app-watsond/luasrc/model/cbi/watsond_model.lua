map = Map("watsond")

section = map:section(NamedSection, "general", "connectionsettings", "Connection settings")

enable = section:option(Flag, "enable", "Enable")

organisation_id = section:option(Value, "orgId", "Organisation ID", "Specify the Unique ID seen in ibmcloud.com web-service")

type_id = section:option(Value, "typeId", "Type ID", "Specify what kind of device it is (Computer, lightbulb, Raspberry PI, etc.)")

device_id = section:option(Value, "deviceId", "Device ID", "Specify how the device was named (PC15, lightbulb_1, Raspberry PI device 12, etc.)")

authtoken = section:option(Value, "authtoken", "Authentication token", "Specify the one-time password for the device generated in IBM Cloud IoT service")

authtoken.datatype = "string"

return map
