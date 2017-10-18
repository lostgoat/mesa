
#include "main/imports.h"
#include "main/mtypes.h"
#include "main/context.h"

#include "main/externalobjects.h"

#include "st_context.h"
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

static void
st_server_wait_semaphore(struct gl_context *ctx,
                         struct gl_semaphore_object *semObj)
{
   struct st_semaphore_object *st_obj = st_semaphore_object(semObj);
   struct st_context *st = st_context(ctx);
   struct pipe_context *pipe = st->pipe;

   _mesa_flush(ctx);
   pipe->semobj_wait(pipe, st_obj->semaphore);
}

static void
st_server_signal_semaphore(struct gl_context *ctx,
                           struct gl_semaphore_object *semObj)
{
   struct st_semaphore_object *st_obj = st_semaphore_object(semObj);
   struct st_context *st = st_context(ctx);
   struct pipe_context *pipe = st->pipe;

   pipe->semobj_signal(pipe, st_obj->semaphore);
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
