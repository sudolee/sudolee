################################################################################
#
# matti
#
# NOTE:
#   - add "source package/Config.in" into "../Config.in" sorted alphabetically
#   - make matti-rebuild / matti-rebuild all
#   - make matti-dirclean
#
# Details in $(TOPDIR)/docs/manual:
#   - adding-packages-directory.txt
#   - adding-packages-generic.txt
#   - rebuilding-packages.txt
#   - adding-packages-hooks.txt
#
################################################################################

MATTI_VERSION = 0.1
MATTI_SOURCE = matti-$(MATTI_VERSION).tar.gz
# path to source code
MATTI_SITE = $(TOPDIR)/../matti/
MATTI_SITE_METHOD = local
LIBFOO_INSTALL_STAGING = NO
#LIBFOO_INSTALL_STAGING = YES


# The +$(@D)+ variable contains the directory where the source code of the
# package has been extracted, i.e. the build directory.
define MATTI_BUILD_CMDS
	$(MAKE) CC="gcc" LD="ld" -C $(@D) all
endef
#	$(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all


# This is often needed for libraries, since they must install header files
# and other development files in the staging space.
# This will ensure that the commands listed in the
# +LIBFOO_INSTALL_STAGING_CMDS+ variable will be executed.
define MATTI_INSTALL_STAGING_CMDS
	$(INSTALL) -D -m 0755 $(@D)/matti.a $(STAGING_DIR)/usr/lib/matti.a
	$(INSTALL) -D -m 0644 $(@D)/matti.h $(STAGING_DIR)/usr/include/matti.h
	$(INSTALL) -D -m 0755 $(@D)/matti.so $(STAGING_DIR)/usr/lib/matti.so
endef


define MATTI_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/matti $(TARGET_DIR)/usr/bin
endef

# hooks after install target, make sure this package rebuild anytime
define MATTI_POST_INSTALL_TARGET_HOOKS_FIXUP
	$(RM) $(@D)/.stamp_*
endef
#	$(RM) $(@D)/.stamp_rsynced
MATTI_POST_INSTALL_TARGET_HOOKS += MATTI_POST_INSTALL_TARGET_HOOKS_FIXUP

$(eval $(generic-package))
