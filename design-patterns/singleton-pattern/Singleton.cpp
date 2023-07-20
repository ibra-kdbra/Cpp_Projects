#include "Singleton.h"

Singleton& Singleton::instance() {
    static Singleton instance;
    return instance;
}