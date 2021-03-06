/****************************************************************************
 * Common functions (both used by resources and installer: symlinked)
 *
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is Ndless code.
 *
 * The Initial Developer of the Original Code is Olivier ARMAND
 * <olivier.calc@gmail.com>.
 * Portions created by the Initial Developer are Copyright (C) 2010-2013
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): 
 *                 Geoffrey ANNEHEIM <geoffrey.anneheim@gmail.com>
 ****************************************************************************/

#include "ndless.h"
#ifndef STAGE1
#include <syscall-addrs.h>

struct next_descriptor ut_next_descriptor = {
	.next_version = 0x00010000,
	.ext_name = "NDLS",
	.ext_version = 0x00010007 // will be incremented only if new functionalities exposed to third-party tools
};
#endif

unsigned int ut_os_version_index;

/* Writes to ut_os_version_index a zero-based index identifying the OS version and HW model.
 * Also sets up the syscalls table.
 * Should be called only once.
 * May be used for OS-specific arrays of constants (marked with "// OS-specific"). */
void ut_read_os_version_index(void) {
	switch (*(unsigned*)(OS_BASE_ADDRESS + 0x20)) {
		// OS-specific
		case 0x10375BB0:  // 3.6.0 non-CAS
			ut_os_version_index = 0;
			break;
		case 0x103765F0:  // 3.6.0 CAS
			ut_os_version_index = 1;
			break;
		case 0x10375620:  // 3.6.0 non-CAS CX
			ut_os_version_index = 2;
			break;
		case 0x10376090:  // 3.6.0 CAS CX
			ut_os_version_index = 3;
			break;
		default:
			ut_calc_reboot();
	}

#ifndef STAGE1
	sc_addrs_ptr = syscall_addrs[ut_os_version_index];
#endif
}
void __attribute__ ((noreturn)) ut_calc_reboot(void) {
	*(unsigned*)0x900A0008 = 2; //CPU reset
	__builtin_unreachable();
}

void ut_disable_watchdog(void) {
	// Disable the watchdog on CX that may trigger a reset
	*(volatile unsigned*)0x90060C00 = 0x1ACCE551; // enable write access to all other watchdog registers
	*(volatile unsigned*)0x90060008 = 0; // disable reset, counter and interrupt
	*(volatile unsigned*)0x90060C00 = 0; // disable write access to all other watchdog registers
}
