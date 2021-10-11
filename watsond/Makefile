include $(TOPDIR)/rules.mk

PKG_NAME:=watsond
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/watsond
	CATEGORY:=Base system
	TITLE:=watsond
	DEPENDS:=+libiotp-as-device +libubus +libubox +libblobmsg-json
endef

define Package/watsond/description
	This is a test app to test out the libiotp-as-device lib
endef

define Package/watsond/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/watsond $(1)/usr/bin

	$(INSTALL_DIR) $(1)/etc/config $(1)/etc/init.d
	$(INSTALL_BIN) ./files/watsond.init $(1)/etc/init.d/watsond
	$(INSTALL_CONF) ./files/watsond.config $(1)/etc/config/watsond
endef

$(eval $(call BuildPackage,watsond))