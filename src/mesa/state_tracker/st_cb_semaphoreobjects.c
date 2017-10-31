
#include "main/imports.h"
#include "main/mtypes.h"
#include "main/context.h"

#include "main/externalobjects.h"

#include "st_context.h"
#include "st_texture.h"
#include "st_cb_bufferobjects.h"
#include "st_cb_semaphoreobjects.h"

#include "state_tracker/drm_driver.h"
#include "pipe/p_context.h"
#include "pipe/p_screen.h"

static struct gl_semaphore_object *
st_semaphoreobj_alloc(struct gl_context *ctx, GLuint name)
{
   struct st_semaphore_object *st_obj = ST_CALLOC_STRUCT(st_semaphore_object);
   if (!st_obj)
      return NULL;

   _mesa_initialize_semaphore_object(ctx, &st_obj->Base, name);
   return &st_obj->Base;
}

static void
st_semaphoreobj_free(struct gl_context *ctx,
                     struct gl_semaphore_object *semObj)
{
   _mesa_delete_semaphore_object(ctx, semObj);
}


static void
st_import_semaphoreobj_fd(struct gl_context *ctx,
                       struct gl_semaphore_object *semObj,
                       int fd)
{
   struct st_semaphore_object *st_obj = st_semaphore_object(semObj);
   struct st_context *st = st_context(ctx);
   struct pipe_context *pipe = st->pipe;
   struct pipe_screen *screen = pipe->screen;

   st_obj->semaphore = screen->semobj_create_from_fd(screen, fd);

#if !defined(_WIN32)
   /* We own fd, but we no longer need it. So get rid of it */
   close(fd);
#endif
}

static enum pipe_layout
pipe_layout(GLenum gl_layout)
{
   switch (gl_layout) {
   case GL_LAYOUT_GENERAL_EXT:
      return PIPE_LAYOUT_GENERAL_EXT;
   case PIPE_LAYOUT_COLOR_ATTACHMENT:
      return PIPE_LAYOUT_COLOR_ATTACHMENT;
   case PIPE_LAYOUT_DEPTH_STENCIL_ATTACHMENT:
      return PIPE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
   case PIPE_LAYOUT_DEPTH_STENCIL_READ_ONLY:
      return PIPE_LAYOUT_DEPTH_STENCIL_READ_ONLY;
   case PIPE_LAYOUT_SHADER_READ_ONLY:
      return PIPE_LAYOUT_SHADER_READ_ONLY;
   case PIPE_LAYOUT_TRANSFER_SRC:
      return PIPE_LAYOUT_TRANSFER_SRC;
   case PIPE_LAYOUT_TRANSFER_DST:
      return PIPE_LAYOUT_TRANSFER_DST;
   default:
      unreachable("unexpected layout");
   }
}

static void
st_server_wait_semaphore(struct gl_context *ctx,
                         struct gl_semaphore_object *semObj,
                         GLuint numBufferBarriers,
                         struct gl_buffer_object **bufObjs,
                         GLuint numTextureBarriers,
                         struct gl_texture_object **texObjs,
                         const GLenum *srcLayouts)
{
   struct st_semaphore_object *st_obj = st_semaphore_object(semObj);
   struct st_context *st = st_context(ctx);
   struct pipe_context *pipe = st->pipe;
   struct st_buffer_object *bufObj;
   struct st_texture_object *texObj;
   enum pipe_layout srcLayout;

   for (unsigned i = 0; i < numBufferBarriers; i++) {
      if (!bufObjs[i])
         continue;

      bufObj = st_buffer_object(bufObjs[i]);
      pipe->flush_resource(pipe, bufObj->buffer);
   }

   for (unsigned i = 0; i < numTextureBarriers; i++) {
      if (!texObjs[i])
         continue;

      texObj = st_texture_object(texObjs[i]);
      srcLayout = pipe_layout(srcLayouts[i]);

      pipe->flush_resource(pipe, texObj->pt);
      pipe->transition_resource(pipe, texObj->pt, srcLayout);
   }

   _mesa_flush(ctx);
   pipe->semobj_wait(pipe, st_obj->semaphore);
}

static void
st_server_signal_semaphore(struct gl_context *ctx,
                           struct gl_semaphore_object *semObj,
                           GLuint numBufferBarriers,
                           struct gl_buffer_object **bufObjs,
                           GLuint numTextureBarriers,
                           struct gl_texture_object **texObjs,
                           const GLenum *dstLayouts)
{
   struct st_semaphore_object *st_obj = st_semaphore_object(semObj);
   struct st_context *st = st_context(ctx);
   struct pipe_context *pipe = st->pipe;
   struct st_buffer_object *bufObj;
   struct st_texture_object *texObj;
   enum pipe_layout dstLayout;

   pipe->semobj_signal(pipe, st_obj->semaphore);

   for (unsigned i = 0; i < numBufferBarriers; i++) {
      if (!bufObjs[i])
         continue;

      bufObj = st_buffer_object(bufObjs[i]);
      pipe->flush_resource(pipe, bufObj->buffer);
   }

   for (unsigned i = 0; i < numTextureBarriers; i++) {
      if (!texObjs[i])
         continue;

      texObj = st_texture_object(texObjs[i]);
      dstLayout = pipe_layout(dstLayouts[i]);

      pipe->flush_resource(pipe, texObj->pt);
      pipe->transition_resource(pipe, texObj->pt, dstLayout);
   }

   _mesa_flush(ctx);
}

void
st_init_semaphoreobject_functions(struct dd_function_table *functions)
{
   functions->NewSemaphoreObject = st_semaphoreobj_alloc;
   functions->DeleteSemaphoreObject = st_semaphoreobj_free;
   functions->ImportSemaphoreFd = st_import_semaphoreobj_fd;
   functions->ServerWaitSemaphoreObject = st_server_wait_semaphore;
   functions->ServerSignalSemaphoreObject = st_server_signal_semaphore;
}
