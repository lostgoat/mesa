/*
 * Copyright © 2016 Red Hat.
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
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "macros.h"
#include "mtypes.h"
#include "externalobjects.h"

/**
 * Allocate and initialize a new memory object.  But don't put it into the
 * memory object hash table.
 *
 * Called via ctx->Driver.NewMemoryObject, unless overridden by a device
 * driver.
 *
 * \return pointer to new memory object.
 */
static struct gl_memory_object *
_mesa_new_memory_object(struct gl_context *ctx, GLuint name)
{
   struct gl_memory_object *obj;
   (void) ctx;
   obj = MALLOC_STRUCT(gl_memory_object);

   _mesa_initialize_memory_object(ctx, obj, name);
   return obj;
}

/**
 * Delete a memory object.  Called via ctx->Driver.DeleteMemory().
 * Not removed from hash table here.
 */
void
_mesa_delete_memory_object(struct gl_context *ctx, struct gl_memory_object *mo)
{
   free(mo);
}

void
_mesa_init_memory_object_functions(struct dd_function_table *driver)
{
   driver->NewMemoryObject = _mesa_new_memory_object;
   driver->DeleteMemoryObject = _mesa_delete_memory_object;
}

/**
 * Initialize a buffer object to default values.
 */
void
_mesa_initialize_memory_object(struct gl_context *ctx,
                               struct gl_memory_object *obj,
                               GLuint name)
{
   memset(obj, 0, sizeof(struct gl_memory_object));
   obj->Name = name;
}

void GLAPIENTRY
_mesa_DeleteMemoryObjectsEXT(GLsizei n, const GLuint *memoryObjects)
{
   GET_CURRENT_CONTEXT(ctx);
   GLint i;

   if (MESA_VERBOSE & (VERBOSE_API))
      _mesa_debug(ctx, "glDeleteMemoryObjectsEXT(%d, %p)\n", n, memoryObjects);

   if (n < 0) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glDeleteMemoryObjects(n < 0)");
      return;
   }

   if (!memoryObjects)
      return;

   _mesa_HashLockMutex(ctx->Shared->MemoryObjects);
   for (i = 0; i < n; i++) {
      if (memoryObjects[i] > 0) {
         struct gl_memory_object *delObj
            = _mesa_lookup_memory_object_locked(ctx, memoryObjects[i]);

         if (delObj) {
            _mesa_HashRemoveLocked(ctx->Shared->MemoryObjects, memoryObjects[i]);
            ctx->Driver.DeleteMemoryObject(ctx, delObj);
         }
      }
   }
   _mesa_HashUnlockMutex(ctx->Shared->MemoryObjects);
}

GLboolean GLAPIENTRY
_mesa_IsMemoryObjectEXT(GLuint memoryObject)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_memory_object *obj;

   if (memoryObject == 0)
      return GL_FALSE;

   obj = _mesa_lookup_memory_object(ctx, memoryObject);

   return obj ? GL_TRUE : GL_FALSE;
}

void GLAPIENTRY
_mesa_CreateMemoryObjectsEXT(GLsizei n, GLuint *memoryObjects)
{
   GET_CURRENT_CONTEXT(ctx);
   GLuint first;

   if (MESA_VERBOSE & (VERBOSE_API))
      _mesa_debug(ctx, "glCreateMemoryObjectsEXT(%d, %p)", n, memoryObjects);

   if (n < 0) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glCreateMemoryObjects(n < 0)");
      return;
   }

   if (!memoryObjects)
      return;

   _mesa_HashLockMutex(ctx->Shared->MemoryObjects);
   first = _mesa_HashFindFreeKeyBlock(ctx->Shared->MemoryObjects, n);
   if (first) {
      GLsizei i;
      for (i = 0; i < n; i++) {
         struct gl_memory_object *memObj;

         memoryObjects[i] = first + i;

         /* allocate memory object */
         memObj = ctx->Driver.NewMemoryObject(ctx, memoryObjects[i]);
         if (!memObj)
            goto out_unlock;

         /* insert into hash table */
         _mesa_HashInsertLocked(ctx->Shared->MemoryObjects,
                                memoryObjects[i],
                                memObj);
      }
   }

out_unlock:
   _mesa_HashUnlockMutex(ctx->Shared->MemoryObjects);
}

void GLAPIENTRY
_mesa_MemoryObjectParameterivEXT(GLuint memoryObject,
                                 GLenum pname,
                                 const GLint *params)
{

}

void GLAPIENTRY
_mesa_GetMemoryObjectParameterivEXT(GLuint memoryObject,
                                    GLenum pname,
                                    GLint *params)
{

}

void GLAPIENTRY
_mesa_TexStorageMem2DEXT(GLenum target,
                         GLsizei levels,
                         GLenum internalFormat,
                         GLsizei width,
                         GLsizei height,
                         GLuint memory,
                         GLuint64 offset)
{

}

void GLAPIENTRY
_mesa_TexStorageMem2DMultisampleEXT(GLenum target,
                                    GLsizei samples,
                                    GLenum internalFormat,
                                    GLsizei width,
                                    GLsizei height,
                                    GLboolean fixedSampleLocations,
                                    GLuint memory,
                                    GLuint64 offset)
{

}

void GLAPIENTRY
_mesa_TexStorageMem3DEXT(GLenum target,
                         GLsizei levels,
                         GLenum internalFormat,
                         GLsizei width,
                         GLsizei height,
                         GLsizei depth,
                         GLuint memory,
                         GLuint64 offset)
{

}

void GLAPIENTRY
_mesa_TexStorageMem3DMultisampleEXT(GLenum target,
                                    GLsizei samples,
                                    GLenum internalFormat,
                                    GLsizei width,
                                    GLsizei height,
                                    GLsizei depth,
                                    GLboolean fixedSampleLocations,
                                    GLuint memory,
                                    GLuint64 offset)
{

}

void GLAPIENTRY
_mesa_BufferStorageMemEXT(GLenum target,
                          GLsizeiptr size,
                          GLuint memory,
                          GLuint64 offset)
{

}

void GLAPIENTRY
_mesa_TextureStorageMem2DEXT(GLuint texture,
                             GLsizei levels,
                             GLenum internalFormat,
                             GLsizei width,
                             GLsizei height,
                             GLuint memory,
                             GLuint64 offset)
{

}

void GLAPIENTRY
_mesa_TextureStorageMem2DMultisampleEXT(GLuint texture,
                                        GLsizei samples,
                                        GLenum internalFormat,
                                        GLsizei width,
                                        GLsizei height,
                                        GLboolean fixedSampleLocations,
                                        GLuint memory,
                                        GLuint64 offset)
{

}

void GLAPIENTRY
_mesa_TextureStorageMem3DEXT(GLuint texture,
                             GLsizei levels,
                             GLenum internalFormat,
                             GLsizei width,
                             GLsizei height,
                             GLsizei depth,
                             GLuint memory,
                             GLuint64 offset)
{

}

void GLAPIENTRY
_mesa_TextureStorageMem3DMultisampleEXT(GLuint texture,
                                        GLsizei samples,
                                        GLenum internalFormat,
                                        GLsizei width,
                                        GLsizei height,
                                        GLsizei depth,
                                        GLboolean fixedSampleLocations,
                                        GLuint memory,
                                        GLuint64 offset)
{

}

void GLAPIENTRY
_mesa_NamedBufferStorageMemEXT(GLuint buffer,
                               GLsizeiptr size,
                               GLuint memory,
                               GLuint64 offset)
{

}

void GLAPIENTRY
_mesa_TexStorageMem1DEXT(GLenum target,
                         GLsizei levels,
                         GLenum internalFormat,
                         GLsizei width,
                         GLuint memory,
                         GLuint64 offset)
{

}

void GLAPIENTRY
_mesa_TextureStorageMem1DEXT(GLuint texture,
                             GLsizei levels,
                             GLenum internalFormat,
                             GLsizei width,
                             GLuint memory,
                             GLuint64 offset)
{

}

void GLAPIENTRY
_mesa_GenSemaphoresEXT(GLsizei n, GLuint *semaphores)
{

}

void GLAPIENTRY
_mesa_DeleteSemaphoresEXT(GLsizei n, const GLuint *semaphores)
{

}

GLboolean GLAPIENTRY
_mesa_IsSemaphoreEXT(GLuint semaphore)
{
   return GL_FALSE;
}

void GLAPIENTRY
_mesa_SemaphoreParameterui64vEXT(GLuint semaphore,
                                 GLenum pname,
                                 const GLuint64 *params)
{

}

void GLAPIENTRY
_mesa_GetSemaphoreParameterui64vEXT(GLuint semaphore,
                                    GLenum pname,
                                    GLuint64 *params)
{

}

void GLAPIENTRY
_mesa_WaitSemaphoreEXT(GLuint semaphore,
                       GLuint numBufferBarriers,
                       const GLuint *buffers,
                       GLuint numTextureBarriers,
                       const GLuint *textures,
                       const GLenum *srcLayouts)
{

}

void GLAPIENTRY
_mesa_SignalSemaphoreEXT(GLuint semaphore,
                         GLuint numBufferBarriers,
                         const GLuint *buffers,
                         GLuint numTextureBarriers,
                         const GLuint *textures,
                         const GLenum *dstLayouts)
{

}

void GLAPIENTRY
_mesa_ImportMemoryFdEXT(GLuint memory,
                        GLuint64 size,
                        GLenum handleType,
                        GLint fd)
{
   GET_CURRENT_CONTEXT(ctx);
   struct gl_memory_object *memObj;

   if (handleType != GL_HANDLE_TYPE_OPAQUE_FD_EXT) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glImportMemoryFdEXT(handleType=%u)", handleType);
      return;
   }

   if (memory == 0)
      return;

   memObj = _mesa_lookup_memory_object(ctx, memory);
   if (!memObj) {
      _mesa_error(ctx, GL_INVALID_OPERATION,
                  "glImportMemoryFdEXT(non-gen name)");
      return;
   }

   ctx->Driver.ImportMemoryObjectFd(ctx, memObj, size, fd);
}

void GLAPIENTRY
_mesa_ImportSemaphoreFdEXT(GLuint semaphore,
                           GLenum handleType,
                           GLint fd)
{

}
