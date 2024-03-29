/*
**  Filename: table.c
**
**  Made by: Alexis PETRILLO on 12/11/2017.
**
**  Description: Functions for manage tables
*/

#include "../database/database.h"
#include "../field/field.h"
#include "../print_color/print_color.h"

/**
 * Initialize the tables in Database structure
 * @param database
 * @return 0 if success, 1 for error
 */
int initTables(Database *database) {
    struct dirent *file;
    DIR *dir;
    Table *table;

    dir = opendir(getDatabasePath(database->name));
    if (!dir) {
        sprintf(error, "%sAn error has occured when opening database '%s': "
                "%s\n%s", COLOR_RED, database->name, strerror(errno), COLOR_RESET);
        return 1;
    }

    while ((file = readdir(dir))) {
        if (strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0) {
            table = xmalloc(sizeof(Table), __func__);
            if (!table)
                return 1;

            table->name = xmalloc(sizeof(char) * strlen(file->d_name), __func__);
            if (!table->name)
                return 1;

            file->d_name[strlen(file->d_name) - 4] = '\0'; // To remove the ".yml"
            strcpy(table->name, file->d_name);
            table->fieldHead = NULL;
            table->next = database->tableHead;

            if (initFields(database, table) != 0) {
                return 1;
            }
            database->tableHead = table;
        }
    }

    closedir(dir);

    return 0;
}

/**
 * Free the tables in a Database structure
 * @param database
 * @return 0 if success, 1 for error
 */
int freeTables(Database *database) {
    Table *currentTable;
    Table *tableToFree;

    if (!database)
        return 1;

    currentTable = database->tableHead;
    while (currentTable != NULL) {
        tableToFree = currentTable->next;
        database->tableHead = currentTable;
        currentTable = currentTable->next;
        freeFields(tableToFree);
        free(tableToFree);
    }

    database->tableHead = NULL;
    free(currentTable);
    return 0;
}

/**
 * Find a table in a Database structure
 * @param database
 * @param tableName
 * @return table if success, NULL for error
 */
Table *findTable(Database *database, char *tableName) {
    Table *currentTable;

    if (!database || !database->isUsed) {
        sprintf(error, "%sYou need to use a database\n%s", COLOR_RED, COLOR_RESET);
        return NULL;
    }

    currentTable = database->tableHead;

    while (currentTable != NULL) {
        if (strcmp(currentTable->name, tableName) == 0)
            return currentTable;
        currentTable = currentTable->next;
    }

    sprintf(error, "%sTable %s doesn't exist\n%s", COLOR_RED, tableName, COLOR_RESET);
    return NULL;
}

/**
 * Free a table in a Database structure
 * @param database
 * @param table
 * @return  0 if success, 1 for error
 */
int freeTable(Database *database, Table *table) {
    Table *tableToFree;
    Table *currentTable;

    if (!database)
        return 1;

    currentTable = database->tableHead;

    while (currentTable != NULL) {
        if (currentTable->next == table) {
            tableToFree = currentTable->next;
            currentTable->next = currentTable->next->next;
            free(tableToFree);
            break;
        } else if (currentTable == table) {
            tableToFree = currentTable;
            database->tableHead = currentTable->next;
            free(tableToFree);
            break;
        }
        currentTable = currentTable->next;
    }

    return 0;
}

/**
 * Create a table file
 * @param database
 * @param table
 * @return 0 if success, 1 for error
 */
int createTable(Database *database, Table *table) {
    char *path;

    if (!database || !database->isUsed) {
        sprintf(error, "%sYou need to use a database\n%s", COLOR_RED, COLOR_RESET);
        return 1;
    }

    if (!table)
        return 1;

    path = getTablePath(database->name, table->name);
    if (!path)
        return 1;

    if (access(path, 0) != 0) { // If table doesn't exist
        table->next = database->tableHead;
        addFieldsInFile(database, table);
        database->tableHead = table;
    } else {
        sprintf(error, "%sThe table '%s' already exist\n%s", COLOR_RED, table->name, COLOR_RESET);
        free(path);
        return 1;
    }
    free(path);

    return 0;
}

/**
 * Remove file 'table->name'
 * Free the Table and fields
 * @param database
 * @param table
 * @return
 */
int dropTable(Database *database, Table *table) {
    char *path;

    if (!database || !database->isUsed) {
        sprintf(error, "%sYou need to use a database\n%s", COLOR_RED, COLOR_RESET);
        return 1;
    }

    if (!table)
        return 1;

    path = getTablePath(database->name, table->name);
    if (!path)
        return 1;
    if (remove(path) == -1) {
        sprintf(error, "%sAn error has occured when removing table '%s': "
                "%s\n%s", COLOR_RED, table->name, strerror(errno), COLOR_RESET);

        free(path);
        return 1;
    }

    freeTable(database, findTable(database, table->name));
    free(path);

    return 0;
}

/**
 * Based on RESOURCES_DIR and databaseName, this function returns an absolute
 * path to the file
 * @param databaseName
 * @param tableName
 * @return
 */
char *getTablePath(const char *databaseName, const char *tableName) {
    char *path;

    path = xmalloc(sizeof(char) * (strlen(RESOURCES_DIR) + strlen
            (databaseName) + strlen(tableName) + 6), __func__);
    if (!path)
        return NULL;

    strcpy(path, RESOURCES_DIR);
    strcat(path, databaseName);
    strcat(path, "/");
    strcat(path, tableName);
    strcat(path, ".yml");

    return path;
}