/**
 * data sturctures of usb module.
 */


sturct usb_device {
	struct usb_host_endpoint ep0;	/* endpoint 0=contrl point */

	struct usb_device_descriptor descriptor;

	struct usb_host_config *config;	/* all of the configs */
	struct usb_host_config *actconfig {	/* current config */
		struct usb_config_descriptor desc;

		/* all of the association interfaces */
		struct usb_interface *interface[USB_MAXINTERFACES] {
			struct device dev;	/* interface specific device info */
			/* Driver: we can use this func to get the assoc device.
			 * struct usb_device *interface_to_usbdev(struct usb_interface *intf) {
			 *              return to_usb_device(intf->dev.parent);
			 * }
			 */

			unsigned num_altsetting;    /* number of alternate settings */
			struct usb_host_interface *altsetting;
			struct usb_host_interface *cur_altsetting {
				struct usb_interface_descriptor desc;
				/* array of desc.bNumEndpoint endpoints associated with this
				 * interface setting.  these will be in no particular order.
				 */
				struct usb_host_endpoint *endpoint {
					struct usb_endpoint_descriptor desc;
					struct usb_ss_ep_comp_descriptor  ss_ep_comp;
					struct list_head urb_list; /* list of USB request block */
				};
			};
		};
	};
};

struct usb_device_descriptor descriptor {
	__u8 bLength;
	__u8 bDescriptorType;
	__le16 bcdUSB;
	__u8 bDeviceClass;
	__u8 bDeviceSubClass;
	__u8 bDeviceProtocol;
	__u8 bMaxPacketSize0;
	__le16 idVendor;
	__le16 idProduct;
	__le16 bcdDevice;
	__u8 iManufacturer;
	__u8 iProduct;
	__u8 iSerialNumber;
	__u8 bNumConfigurations;
};

struct usb_config_descriptor desc {
	__u8 bLength;
	__u8 bDescriptorType;
	__le16 wTotalLength;
	__u8 bNumInterfaces;
	__u8 bConfigurationValue;
	__u8 iConfiguration;
	__u8 bmAttributes;
	__u8 bMaxPower;
};

struct usb_interface_descriptor desc {
	__u8 bLength;
	__u8 bDescriptorType;
	__u8 bInterfaceNumber;
	__u8 bAlternateSetting;
	__u8 bNumEndpoints;
	__u8 bInterfaceClass;
	__u8 bInterfaceSubClass;
	__u8 bInterfaceProtocol;
	__u8 iInterface;
};

struct usb_endpoint_descriptor desc {
	__u8 bLength;
	__u8 bDescriptorType;
	__u8 bEndpointAddress;
	__u8 bmAttributes;
	__le16 wMaxPacketSize;
	__u8 bInterval;
	__u8 bRefresh;
	__u8 bSynchAddress;
};
