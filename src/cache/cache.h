#ifndef CACHE_H
#define CACHE_H

#include <QtSql>
#include <QString>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include "src/object/fileobj.h"

using namespace std;
class cache
{
public:
    string db_file, locate_file, cache_dir;
    QSqlDatabase db;

    cache();
    virtual ~cache();

    void openDB();
    void closeDB();

    void init();

    void addTables();
    bool initDB();
    void setInitDB();
    void createCache();

    void writeMe(string qry);
    void writeDB(fileObj *file, string type);

    void readDB(fileObj &file, string type);
    void readAll(fileObj &artist, fileObj &song, fileObj &viddirs, fileObj &videos);

    void removeFrom(int key, string table);

private:
};

#endif // CACHE_H
