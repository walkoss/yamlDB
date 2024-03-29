/*
**  Filename: database.c
**
**  Made by: Walkoss on 10/11/2017.
**
**  Description: 
*/

#include "database.h"
#include "../print_color/print_color.h"

/**
 * Initialize a Database structure
 * @param databaseName
 * @return a Database
 */
Database *initDatabase(const char *databaseName) {
    Database *database;

    database = xmalloc(sizeof(Table), __func__);

    if (database) {
        database->isUsed = 0;
        database->name = strdup(databaseName);
        database->tableHead = NULL;
    }

    return database;
}

/**
 * Free a Database with tables and fields
 * @param database
 * @return 0 if success, 1 for error
 */
int freeDatabase(Database *database) {
    if (!database)
        return 1;

    database->isUsed = 0;
    freeTables(database);
    free(database->name);
    free(database);

    return 0;
}

/**
 * Initialize a Database with tables and fields
 * @param database
 * @return 0 if success 1 otherwise
 */
int useDatabase(Database *database) {
    char *path;

    if (!database)
        return 1;

    path = getDatabasePath(database->name);
    if (!path)
        return 1;

    if (access(path, 0) == 0) { // If directory exist
        if (database->isUsed == 0) {
            database->isUsed = 1;
            if (initTables(database) != 0) {
                return 1;
            }
        } else {
            print_error_color("Error : you are already using this database\n");
        }
    } else {
        sprintf(error, "%sAn error has occured when opening database '%s': "
                "%s%s\n", COLOR_RED, database->name, strerror(errno), COLOR_RESET);
        return 1;
    }

    return 0;
}

/**
 * Create directory 'database->name'
 * @param database
 * @return 0 if succes, 1 for error
 */
int createDatabase(Database *database) {
    char *path;

    if (!database)
        return 1;

    path = getDatabasePath(database->name);
    if (!path)
        return 1;

    if (mkdir(path, 0777) == -1) {
        sprintf(error, "%sAn error has occured when creating database '%s': "
                "%s\n%s", COLOR_RED, database->name, strerror(errno), COLOR_RESET);
        free(path);
        return 1;
    }

    free(path);
    return 0;
}

/**
 * Remove file 'fpath'
 * Function pointer passed as parameter to ntfw function, see removeDatabase function below
 * @param fpath
 * @param sb
 * @param tflag
 * @param ftwbuf
 * @return 0 if success, 1 for error
 */
int removeFile(const char *fpath,
               const struct stat *sb,
               int tflag,
               struct FTW *ftwbuf) {
    if (remove(fpath) == -1) {
        sprintf(error,
                "An error has occured when removing directory/file '%s': "
                        "%s\n",
                fpath,
                strerror(errno));
        return 1;
    }
    return 0;
}

/**
 * Remove directory 'databaseName' and its content
 * Free the Database with tables and fields
 * @param databaseName
 * @return 0 if success, 1 for error
 */
int dropDatabase(Database *database) {
    char *path;

    if (!database)
        return 1;

    path = getDatabasePath(database->name);
    if (!path)
        return 1;

    if (nftw(path, removeFile, NOPENFD, FTW_DEPTH) == -1) {
        sprintf(error, "%sAn error has occured when removing database '%s': "
                "%s\n%s", COLOR_RED, database->name, strerror(errno), COLOR_RESET);
        free(path);
        return 1;
    }

    freeDatabase(database);
    free(path);
    return 0;
}

/**
 * Based on RESOURCES_DIR, this function returns an absolute path to resources
 * Example: 'test' -> 'resources/test'
 * @param databaseName
 * @return path is success, NULL otherwise
 */
char *getDatabasePath(const char *databaseName) {
    char *path;

    path = xmalloc(strlen(RESOURCES_DIR) + strlen(databaseName) + 1, __func__);
    if (!path)
        return NULL;
    strcpy(path, RESOURCES_DIR);
    strcat(path, databaseName);

    return path;
}

/**
 * display the database, tables and fields to debug
 * @param database
 * @return 0 if success, 1 for error
 */
int debugDatabase(Database *database) {
    Table *currentTable;

    if (!database->name)
        return 1;

    currentTable = database->tableHead;

    if (strcmp(database->name, "") != 0)
        printf("%d %s:\n", database->isUsed, database->name);

    while (currentTable != NULL) {
        printf("\t%s:\n ", currentTable->name);

        Field *currentField = currentTable->fieldHead;
        while (currentField != NULL) {
            printf("\t\t%s: %d\n", currentField->name, currentField->type);
            currentField = currentField->next;
        }

        printf("\n");
        currentTable = currentTable->next;
    }

    return 0;
}