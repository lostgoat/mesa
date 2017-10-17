#ifndef ST_CB_SEMAPHOREOBJECTS_H
#define ST_CB_SEMAPHOREOBJECTS_H

#include "main/compiler.h"
#include "main/mtypes.h"

struct dd_function_table;
struct pipe_screen;

struct st_semaphore_object
{
   struct gl_semaphore_object Base;
   struct pipe_semaphore_object *semaphore;
};

static inline struct st_semaphore_object *
st_semaphore_object(struct gl_semaphore_object *obj)
{
   return (struct st_semaphore_object *)obj;
}

extern void
st_init_semaphoreobject_functions(struct dd_function_table *functions);

#endif
