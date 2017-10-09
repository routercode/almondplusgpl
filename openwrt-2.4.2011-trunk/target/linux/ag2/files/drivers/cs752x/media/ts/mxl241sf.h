/*-
 * Copyright (C) 2011 Encore Interactive Inc.
 * All rights reserved.
 *
 * Jared D. McNeill <jared.mcneill@encoreinteractive.ca>
 */

#ifndef _MXL241SF_H
#define _MXL241SF_H

#include <linux/dvb/frontend.h>

struct mxl241sf_config {
	uint8_t	demod_address;

	uint32_t xtal_freq;
};

extern struct dvb_frontend *	mxl241sf_attach(const struct mxl241sf_config *,
						struct i2c_adapter *);

#endif /* !_MXL241SF_H */
