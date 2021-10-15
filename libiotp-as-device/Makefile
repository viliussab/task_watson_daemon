include $(TOPDIR)/rules.mk

PKG_NAME:=libiotp-as-device
PKG_RELEASE:=1
PKG_VERSION:=1.0
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/ibm-watson-iot/iot-c.git
PKG_SOURCE_VERSION:=f921dce616af815da2236954c608a2f377c928c5

include $(INCLUDE_DIR)/package.mk

define Package/libiotp-as-device
	CATEGORY:=Libraries
	TITLE:=libiotp-as-device
	DEPENDS:=+libopenssl
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/libiotp-as-device.so.1.0 $(1)/usr/lib
	$(LN) libiotp-as-device.so.1.0 $(1)/usr/lib/libiotp-as-device.so.1
	$(LN) libiotp-as-device.so.1.0 $(1)/usr/lib/libiotp-as-device.so

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/paho.mqtt.c/build/output/libpaho-mqtt3as.so.1.3 $(1)/usr/lib
	$(LN) libpaho-mqtt3as.so.1.3 $(1)/usr/lib/libpaho-mqtt3as.so.1
	$(LN) libpaho-mqtt3as.so.1.3 $(1)/usr/lib/libpaho-mqtt3as.so

	$(INSTALL_DATA) $(PKG_BUILD_DIR)/src/wiotp/sdk/*.h $(1)/usr/include
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/paho.mqtt.c/src/*.h $(1)/usr/include
endef

define Package/libiotp-as-device/install
	$(INSTALL_DIR) $(1)/usr/lib

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/libiotp-as-device.so.1.0 $(1)/usr/lib
	$(LN) libiotp-as-device.so.1.0 $(1)/usr/lib/libiotp-as-device.so.1
	$(LN) libiotp-as-device.so.1.0 $(1)/usr/lib/libiotp-as-device.so

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/paho.mqtt.c/build/output/libpaho-mqtt3as.so.1.3 $(1)/usr/lib
	$(LN) libpaho-mqtt3as.so.1.3 $(1)/usr/lib/libpaho-mqtt3as.so.1
	$(LN) libpaho-mqtt3as.so.1.3 $(1)/usr/lib/libpaho-mqtt3as.so
endef

$(eval $(call BuildPackage,libiotp-as-device))