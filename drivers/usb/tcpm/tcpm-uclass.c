// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2024 Collabora Ltd.
 *
 * USB Power Delivery protocol stack.
 */

#include <dm/device.h>
#include <dm/device_compat.h>
#include <dm/uclass.h>
#include <linux/err.h>
#include <usb/tcpm.h>
#include "tcpm-internal.h"

int tcpm_get_voltage(struct udevice *dev)
{
	struct tcpm_port *port = dev_get_uclass_plat(dev);

	return port->supply_voltage;
}

int tcpm_get_current(struct udevice *dev)
{
	struct tcpm_port *port = dev_get_uclass_plat(dev);

	return port->current_limit;
}

enum typec_orientation tcpm_get_orientation(struct udevice *dev)
{
	struct tcpm_port *port = dev_get_uclass_plat(dev);

	switch (port->polarity) {
	case TYPEC_POLARITY_CC1:
		return TYPEC_ORIENTATION_NORMAL;
	case TYPEC_POLARITY_CC2:
		return TYPEC_ORIENTATION_REVERSE;
	default:
		return TYPEC_ORIENTATION_NONE;
	}
}

const char *tcpm_get_state(struct udevice *dev)
{
	struct tcpm_port *port = dev_get_uclass_plat(dev);

	return tcpm_states[port->state];
}

int tcpm_get_pd_rev(struct udevice *dev)
{
	struct tcpm_port *port = dev_get_uclass_plat(dev);

	return port->negotiated_rev;
}

enum typec_role tcpm_get_pwr_role(struct udevice *dev)
{
	struct tcpm_port *port = dev_get_uclass_plat(dev);

	return port->pwr_role;
}

enum typec_data_role tcpm_get_data_role(struct udevice *dev)
{
	struct tcpm_port *port = dev_get_uclass_plat(dev);

	return port->data_role;
}

bool tcpm_is_connected(struct udevice *dev)
{
	struct tcpm_port *port = dev_get_uclass_plat(dev);

	return port->connected;
}

int tcpm_get(const char *name, struct udevice **devp)
{
	return uclass_get_device_by_name(UCLASS_TCPM, name, devp);
}

static int tcpm_post_probe(struct udevice *dev)
{
	int ret = tcpm_port_init(dev);

	if (ret < 0) {
		dev_err(dev, "failed to tcpm port init\n");
		return ret;
	}

	tcpm_poll_event(dev);

	return 0;
}

UCLASS_DRIVER(tcpm) = {
	.id		= UCLASS_TCPM,
	.name		= "tcpm",
	.per_device_plat_auto	= sizeof(struct tcpm_port),
	.post_probe	= tcpm_post_probe,
};
