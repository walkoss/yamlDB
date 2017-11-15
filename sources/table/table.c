/*
**  Filename: table.c
**
**  Made by: Alexis PETRILLO on 12/11/2017.
**
**  Description: Functions for manage tables
*/

#include "../database/database.h"
#include "table.h"

/**
 * Init tables structure
 * @param databaseName
 * @return
 */
Table *initTable(char *databaseName) {
    Database *database = xmalloc(sizeof(Table), __func__);

    if (database) {
        database->name = databaseName;
        // TODO: initTables, initFields
        database->tableNext = NULL;
    }

    return database;
}

void freeDatabase(Database *database) {
    free(database->name);
    // TODO: freeTables, freeFields
    free(database);
}


/**
 *
 * @param table
 * @param name
 * @param type
 * @return 0 if success, 1 for error
 */
int newField(Table *table, char *name, char *type) {
    Field *new = malloc(sizeof(*new));

    if (!table || !new)
        return 1;

    new->name = name;
    new->type = type;
    new->next = NULL;

    Field *current = table->first;

    if (current == NULL) {
        table->first = new;
    } else
        while (current != NULL) {
            if (current->next == NULL) {
                current->next = new;
                current = NULL;
            } else
                current = current->next;
        }

    return 0;
}

/**
 *
 * @param table
 * @param file
 * @return 0 if success
 */
int addFields(Table *table, FILE *file) {
    Field *current = table->first;

    fprintf(file, "fields:\n");

    while (current != NULL) {
        fprintf(file, "\t%s: %d\n", current->name, 1);
        current = current->next;
    }

    return 0;
}

/**
 *
 * @param databaseName
 * @param tableName
 * @return 0 if success, 1 for error
 */
int addTable(const char *databaseName, const char *tableName, Table *table) {
    char *path;
    FILE *file;

    path = xmalloc(sizeof(char) * strlen(RESOURCES_DIR) + strlen
            (tableName), __func__);
    if (!path)
        return 1;

    strcpy(path, RESOURCES_DIR);
    strcat(path, databaseName);
    strcat(path, "/");
    strcat(path, tableName);
    strcat(path, ".yml");
    file = fopen(path, "w");

    if (!file) {
        fprintf(stderr, "An error has occured when creating table '%s': "
                "%s\n", tableName, strerror(errno));
        free(path);
        return 1;
    }

//    addFields(table, file);
    fclose(file);
    free(path);

    return 0;
}

/**
 *
 * @param databaseName
 * @param tableName
 * @return 0 if success, 1 for error
 */
int removeTable(const char *databaseName, const char *tableName) {
    char *path;

    path = xmalloc(sizeof(char) * (strlen(RESOURCES_DIR) + strlen
            (databaseName) + strlen(tableName) + 6), __func__);
    if (!path)
        return 1;

    strcpy(path, RESOURCES_DIR);
    strcat(path, databaseName);
    strcat(path, "/");
    strcat(path, tableName);
    strcat(path, ".yml");

    if (remove(path) == -1) {
        fprintf(stderr, "An error has occured when removing table '%s': "
                "%s\n", tableName, strerror(errno));

        free(path);
        return 1;
    }

    free(path);
    return 0;
}
