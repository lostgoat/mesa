/*
 * Copyright 2017 Valve Software
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef VK_EXT_QUEUE_GLOBAL_PRIORITY_H_
#define VK_EXT_QUEUE_GLOBAL_PRIORITY_H_

#include "vulkan.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Draft spec lives at <docs/specs/VK_EXT_queue_global_prioruity.txt>.
 *
 * TODO: Discuss with everyone who cares.
 * TODO: Get a proper VkStructureType
 * TODO: Finish spec
 */
#define VK_EXT_queue_global_priority
#define VK_EXT_queue_global_priotiry_SPEC_VERSION 0 /* experimental */
#define VK_EXT_queue_global_priority_EXTENSION_NAME "VK_EXT_queue_global_priority"

/** System-wide queue priority
 *
 *  High priority queues may starve queues in other processes
 */
typedef enum VkQueueGlobalPriority {
    VK_QUEUE_GLOBAL_PRIORITY_HIGH = 1,
    VK_QUEUE_GLOBAL_PRIORITY_NORMAL = 2,
    VK_QUEUE_GLOBAL_PRIORITY_BEGIN_RANGE = VK_QUEUE_GLOBAL_PRIORITY_HIGH,
    VK_QUEUE_GLOBAL_PRIORITY_END_RANGE = VK_QUEUE_GLOBAL_PRIORITY_NORMAL,
    VK_QUEUE_GLOBAL_PRIORITY_RANGE_SIZE = (VK_QUEUE_GLOBAL_PRIORITY_HIGH - VK_QUEUE_GLOBAL_PRIORITY_NORMAL + 1),
    VK_QUEUE_GLOBAL_PRIORITY_MAX_ENUM = 0x7FFFFFFF
} VkQueueGlobalPriority;

/* Extends VkDeviceQueueCreateInfo.
 *
 * Used to set the global priority for the allocated queue
 */
typedef struct VkQueueGlobalPriorityEXT {
    VkStructureType                         sType;
    void*                                   pNext;
    VkQueueGlobalPriority                   priority;
} VkQueueGlobalPriorityEXT;

#define VK_STRUCTURE_TYPE_QUEUE_GLOBAL_PRIORITY_EXT (VkStructureType)1000120001

#ifdef __cplusplus
}
#endif

#endif
