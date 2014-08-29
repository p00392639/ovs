/*
 * Copyright (c) 2014 VMware, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * XXX: OVS_USE_NL_INTERFACE is being used to keep the legacy DPIF interface
 * alive while we transition over to the netlink based interface.
 * OVS_USE_NL_INTERFACE = 0 => legacy inteface to use with dpif-windows.c
 * OVS_USE_NL_INTERFACE = 1 => netlink inteface to use with ported dpif-linux.c
 */
#if defined OVS_USE_NL_INTERFACE && OVS_USE_NL_INTERFACE == 0
#include "Ioctl.h"

#else

#ifndef __DATAPATH_H_
#define __DATAPATH_H_ 1

typedef struct _OVS_DEVICE_EXTENSION {
    INT numberOpenInstance;
    INT pidCount;
} OVS_DEVICE_EXTENSION, *POVS_DEVICE_EXTENSION;


/*
 * Private context for each handle on the device.
 */
typedef struct _OVS_OPEN_INSTANCE {
    UINT32 cookie;
    PFILE_OBJECT fileObject;
    PVOID eventQueue;
    PVOID packetQueue;
    UINT32 pid;

    /*
     * On platforms that support netlink natively, there's generally some form of
     * serialization between concurrent calls to netlink sockets. However, OVS
     * userspace guarantees that a given netlink handle is not concurrently used.
     * Despite this, we do want to have some basic checks in the kernel to make
     * sure that things don't break if there are concurrent calls.
     *
     * This is generally not an issue since kernel data structure access should
     * be sychronized anyway. Only reason to have this safeguared is to protect
     * the state in "state-aware" read calls which rely on previous state. This
     * restriction might go away as the userspace code gets implemented.
     */
    INT inUse;
} OVS_OPEN_INSTANCE, *POVS_OPEN_INSTANCE;

NDIS_STATUS OvsCreateDeviceObject(NDIS_HANDLE ovsExtDriverHandle);
VOID OvsDeleteDeviceObject();

POVS_OPEN_INSTANCE OvsGetOpenInstance(PFILE_OBJECT fileObject,
                                      UINT32 dpNo);

NTSTATUS OvsCompleteIrpRequest(PIRP irp, ULONG_PTR infoPtr, NTSTATUS status);

/*
 * Structure of any message passed between userspace and kernel.
 */
typedef struct _OVS_MESSAGE {
    NL_MSG_HDR nlMsg;
    GENL_MSG_HDR genlMsg;
    struct ovs_header ovsHdr;
    /* Variable length nl_attrs follow. */
} OVS_MESSAGE, *POVS_MESSAGE;

#endif /* __DATAPATH_H_ */

#endif /* OVS_USE_NL_INTERFACE */
