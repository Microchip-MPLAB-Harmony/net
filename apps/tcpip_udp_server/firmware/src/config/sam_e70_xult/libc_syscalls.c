/**
 * Copyright (c) 2016-2017 Microchip Technology Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <device.h> /* for ARM CMSIS __BKPT() */

#ifdef __cplusplus
extern "C" {
#endif

/* Harmony specific
 * We implement only the syscalls we want over the stubs provided by libpic32c
 */
extern void _exit(int status);

extern void _exit(int status)
{
    /* Software breakpoint */
#ifdef DEBUG
//    asm("bkpt #0");
    __BKPT(0);
#endif

    /* halt CPU */
    while (1)
    {
    }
}

#ifdef __cplusplus
}
#endif
