include $(TOPDIR)/rules.mk

PKG_NAME:=libiotp-as-device
PKG_RELEASE:=1
PKG_VERSION:=1.0

PAHO_NAME:=libpaho-mqtt3as
PAHO_RELEASE:=1
PAHO_VERSION:=1.3

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

	$(CP) $(PKG_BUILD_DIR)/build/libiotp-as-device.so* $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/paho.mqtt.c/build/output/libpaho-mqtt3as.so* $(1)/usr/lib

	# uzkomentuota dalis neveikia :(
	# $(INSTALL_BIN) $(PKG_BUILD_DIR)/build/libiotp-as-device.so.$(PKG_VERSION) $(1)/usr/lib
	# ln -s $(1)/usr/lib/libiotp-as-device.so.$(PKG_VERSION) $(1)/usr/lib/libiotp-as-device.$(PKG_RELEASE)
	# ln -s $(1)/usr/lib/libiotp-as-device.so.$(PKG_VERSION) $(1)/usr/lib/libiotp-as-device.so

	# $(INSTALL_BIN) $(PKG_BUILD_DIR)/paho.mqtt.c/build/output/libpaho-mqtt3as.so.$(PAHO_VERSION) $(1)/usr/lib
	# ln -s $(1)/usr/lib/libpaho-mqtt3as.so.$(PAHO_VERSION) $(1)/usr/lib/libpaho-mqtt3as.so.$(PAHO_RELEASE)
	# ln -s $(1)/usr/lib/libpaho-mqtt3as.so.so.$(PAHO_VERSION) $(1)/usr/lib/libpaho-mqtt3as.so

	$(INSTALL_DATA) $(PKG_BUILD_DIR)/src/wiotp/sdk/*.h $(1)/usr/include
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/paho.mqtt.c/src/*.h $(1)/usr/include
endef

define Package/libiotp-as-device/install
	$(INSTALL_DIR) $(1)/usr/lib

	$(CP) $(PKG_BUILD_DIR)/build/libiotp-as-device.so* $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/paho.mqtt.c/build/output/libpaho-mqtt3as.so* $(1)/usr/lib

	# $(INSTALL_BIN) $(PKG_BUILD_DIR)/paho.mqtt.c/build/output/libpaho-mqtt3as.so.$(PAHO_VERSION) $(1)/usr/lib
	# ln -s $(1)/usr/lib/libpaho-mqtt3as.so.$(PAHO_VERSION) $(1)/usr/lib/libpaho-mqtt3as.so.$(PAHO_RELEASE)
	# ln -s $(1)/usr/lib/libpaho-mqtt3as.so.so.$(PAHO_VERSION) $(1)/usr/lib/libpaho-mqtt3as.so

	# $(INSTALL_BIN) $(PKG_BUILD_DIR)/build/libiotp-as-device.so.$(PKG_VERSION) $(1)/usr/lib
	# ln -s $(1)/usr/lib/libiotp-as-device.so.$(PKG_VERSION) $(1)/usr/lib/libiotp-as-device.$(PKG_RELEASE)
	# ln -s $(1)/usr/lib/libiotp-as-device.so.$(PKG_VERSION) $(1)/usr/lib/libiotp-as-device.so
endef

$(eval $(call BuildPackage,libiotp-as-device))