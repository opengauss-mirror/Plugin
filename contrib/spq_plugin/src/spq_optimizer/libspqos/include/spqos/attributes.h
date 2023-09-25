#ifndef SPQOS_attributes_H
#define SPQOS_attributes_H

#ifdef __GNUC__
#define SPQOS_UNUSED __attribute__((unused))
#else
#define SPQOS_UNUSED
#endif

#ifndef SPQOS_DEBUG
#define SPQOS_ASSERTS_ONLY SPQOS_UNUSED
#else
#define SPQOS_ASSERTS_ONLY
#endif

#endif	// !SPQOS_attributes_H
