#ifndef __CUSTOM_ANALYZER_DEMO__
#define __CUSTOM_ANALYZER_DEMO__

extern "C" void _PG_init(void);
extern "C" void _PG_fini(void);
extern "C" void init_session_vars(void);
extern "C" void set_extension_index(uint32 index);
extern "C" void custom_analyzer_demo_invoke(void);

typedef struct custom_analyzer_context {
    bool enable_custom_analyzer;
} custom_analyzer_context;

#endif