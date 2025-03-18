#include "SQLite.h"

#include "../Config/MoeConfig.h"

#include "sqlite3.h"

namespace
{
    sqlite3* db = nullptr;
}


void SQLite::initialize(MoeConfig* config)
{
    sqlite3_open(config->getString("dataDir").append("/History.db").toStdString().c_str(), &db);
}

sqlite3* SQLite::getInstance()
{
    return db;
}

void SQLite::dispose()
{
    sqlite3_close(db);
}
