#ifndef VPROM_INCLUDED
#define VPROM_INCLUDED

#include <stdint.h>

/* Virtual EEPROM support. */

#ifdef CORE_TEENSY
template<class T>
void vpromPut(uint32_t addr, const T& t) { EEPROM.put(addr, t); )}
template <class T>
void vpromGet(uint32_t addr, T& t) { EEPROM.get(addr, t); }

#else
void vpromPutRaw(uint32_t addr, uint32_t size, const void* data);
void vpromGetRaw(uint32_t addr, uint32_t size, void* data);

template<class T>
void vpromPut(uint32_t addr, const T& t) { vpromPutRaw(addr, sizeof(t), &t; )}
template <class T>
void vpromGet(uint32_t addr, T& t) { vpromGetRaw(addr, sizeof(t), &t); }
#endif

#endif // VPROM_INCLUDED