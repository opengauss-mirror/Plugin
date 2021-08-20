#ifndef TIDRANGESCAN_H
#define TIDRANGESCAN_H

extern "C" void _PG_init(void);
extern "C" void _PG_fini(void);
extern "C" void init_session_vars(void);
extern "C" void set_extension_index(uint32 index);
extern "C" void tidrangescan_invoke(void);

typedef struct tidrangescan_context {
    bool enable_tidrangescan;
} tidrangescan_context;

#endif