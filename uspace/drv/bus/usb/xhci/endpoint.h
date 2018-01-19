/*
 * Copyright (c) 2017 Petr Manek
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** @addtogroup drvusbxhci
 * @{
 */
/** @file
 * @brief The host controller endpoint management.
 */

#ifndef XHCI_ENDPOINT_H
#define XHCI_ENDPOINT_H

#include <assert.h>

#include <usb/debug.h>
#include <usb/host/dma_buffer.h>
#include <usb/host/endpoint.h>
#include <usb/host/hcd.h>
#include <ddf/driver.h>

#include "isoch.h"
#include "transfers.h"
#include "trb_ring.h"

typedef struct xhci_device xhci_device_t;
typedef struct xhci_endpoint xhci_endpoint_t;
typedef struct xhci_stream_data xhci_stream_data_t;
typedef struct xhci_bus xhci_bus_t;

enum {
	EP_TYPE_INVALID = 0,
	EP_TYPE_ISOCH_OUT = 1,
	EP_TYPE_BULK_OUT = 2,
	EP_TYPE_INTERRUPT_OUT = 3,
	EP_TYPE_CONTROL = 4,
	EP_TYPE_ISOCH_IN = 5,
	EP_TYPE_BULK_IN = 6,
	EP_TYPE_INTERRUPT_IN = 7
};

/** Connector structure linking endpoint context to the endpoint. */
typedef struct xhci_endpoint {
	endpoint_t base;	/**< Inheritance. Keep this first. */

	/** Main transfer ring (unused if streams are enabled) */
	xhci_trb_ring_t ring;

	/** Primary stream context data array (or NULL if endpoint doesn't use streams). */
	xhci_stream_data_t *primary_stream_data_array;

	/** Primary stream context array - allocated for xHC hardware. */
	xhci_stream_ctx_t *primary_stream_ctx_array;
	dma_buffer_t primary_stream_ctx_dma;

	/** Size of the allocated primary stream data array (and context array). */
	uint16_t primary_stream_data_size;

	/* Maximum number of primary streams (0 - 2^16). */
	uint32_t max_streams;

	/** Maximum number of consecutive USB transactions (0-15) that should be executed per scheduling opportunity */
	uint8_t max_burst;

	/** Maximum number of bursts within an interval that this endpoint supports */
	uint8_t mult;

	/** Scheduling interval for periodic endpoints, as a number of 125us units. (0 - 2^16) */
	uint32_t interval;

	/** This field is a valid pointer for (and only for) isochronous transfers. */
	xhci_isoch_t isoch [0];
} xhci_endpoint_t;

#define XHCI_EP_FMT  "(%d:%d %s)"
/* FIXME: "Device -1" messes up log messages, figure out a better way. */
#define XHCI_EP_ARGS(ep)		\
	((ep).base.device ? (ep).base.device->address : -1),	\
	((ep).base.endpoint),		\
	(usb_str_transfer_type((ep).base.transfer_type))

typedef struct xhci_device {
	device_t base;		/**< Inheritance. Keep this first. */

	/** Slot ID assigned to the device by xHC. */
	uint32_t slot_id;

	/** Corresponding port on RH */
	uint8_t rh_port;

	/** USB Tier of the device */
	uint8_t tier;

	/** Route string */
	uint32_t route_str;

	/** Place to store the allocated context */
	dma_buffer_t dev_ctx;

	/** Hub specific information. Valid only if the device is_hub. */
	bool is_hub;
	uint8_t num_ports;
	uint8_t tt_think_time;
} xhci_device_t;

#define XHCI_DEV_FMT  "(%s, slot %d)"
#define XHCI_DEV_ARGS(dev)		 ddf_fun_get_name((dev).base.fun), (dev).slot_id

int xhci_endpoint_type(xhci_endpoint_t *ep);

int xhci_endpoint_init(xhci_endpoint_t *, device_t *, const usb_endpoint_descriptors_t *);
void xhci_endpoint_fini(xhci_endpoint_t *);

void xhci_endpoint_free_transfer_ds(xhci_endpoint_t *xhci_ep);

uint8_t xhci_endpoint_dci(xhci_endpoint_t *);
uint8_t xhci_endpoint_index(xhci_endpoint_t *);

void xhci_setup_endpoint_context(xhci_endpoint_t *, xhci_ep_ctx_t *);
int xhci_endpoint_clear_halt(xhci_endpoint_t *, unsigned);

static inline xhci_device_t * xhci_device_get(device_t *dev)
{
	assert(dev);
	return (xhci_device_t *) dev;
}

static inline xhci_endpoint_t * xhci_endpoint_get(endpoint_t *ep)
{
	assert(ep);
	return (xhci_endpoint_t *) ep;
}

static inline xhci_device_t * xhci_ep_to_dev(xhci_endpoint_t *ep)
{
	assert(ep);
	return xhci_device_get(ep->base.device);
}

uint8_t xhci_endpoint_get_state(xhci_endpoint_t *ep);

#endif

/**
 * @}
 */
