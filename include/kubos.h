/*
 * Copyright (C) 2017 Kubos Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
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
 * update_kubos
 *
 * U-Boot has a DFU utility which currently allows a board to download a new firmware package and
 * distribute the package components to the appropriate end-point locations.
 *
 * We want to leverage the second half of this utility and distribute a new firmware package that's
 * been copied into the upgrade partition, rather than via a USB/TFTP connection.
 *
 * Returns:
 *    0 - An upgrade package was successfully installed
 *   -1 - No upgrade package could be installed and the system should continue normal boot
 *   -2 - No upgrade package could be installed, bu the system should be rebooted so the update can
 *        be attempted again.
 */

#ifndef __KUBOS_H__
#define __KUBOS_H__

#ifdef CONFIG_UPDATE_KUBOS

#define KUBOS_OK_REBOOT      0
#define KUBOS_ERR_NO_REBOOT -1
#define KUBOS_ERR_REBOOT    -2

#define KUBOS_RECOVER 0
#define KUBOS_UPGRADE 1

int update_kubos(void);

#endif

#endif /* __KUBOS_H__ */
