#pragma once
#include "sqlite3.h"

class MoeConfig;

class SQLite
{
public:
    static void initialize(MoeConfig* config);
    static sqlite3* getInstance();
    static void dispose();
};
