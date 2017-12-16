//
// Created by Iliasse Wahbi on 14/11/2017.
//
#include "interface.h"

const TSentences sentences[] =
        {
                {"Que voulez-vous faire ?"},
                {"\t1: Ajouter une base de données"},
                {"\t2: Utiliser une base de données"},
                {"\t3: Supprimer une base de données"},
                {"\t4: Créer une table"},
                {"\t5: Supprimer une table"},
                {"Entrez le nom de la base de données"},
                {"Entrez le nom de la table"},
                {"Entrez le nom du champ à créer"},
                {"Entrez le type de champ\n(1: INT - 2: FLOAT - 3: CHAR - 4: VARCHAR)"},
                {"Ajouter un autre champ ? (N / n pour quitter)"},
                {"Entrez du texte"},
                {"\033[31mCe champ existe déjà.\033[00m"},
                {"\033[32mAction réalisée avec succès.\033[00m"},
                {"\033[31mErreur lors de la réalisation de l'action.\033[00m"},
                {NULL}
        };

const TDatabaseFunc databaseFunc[] =
        {
                {createDatabase},
                {useDatabase},
                {dropDatabase},
                {NULL}
        };

const TTableFunc tableFunc[] =
        {
                {preCreateTable},
                {dropTable},
                {NULL}
        };

/**
 * Print sentences which depends of result parameter
 * @param result
 */
void isSuccess(int result)
{
    if (!result)
        printInstruction(13);
    else
        printInstruction(14);
}

/**
 * Get length of database array
 * @return length
 */
int getDatabaseFuncLength()
{
    int i;

    for (i = 0; databaseFunc[i].function != NULL; i++);
    return i;
}

/**
 * Get length of table array
 * @return length
 */
int getTableFuncLength()
{
    int i;

    for (i = 0; tableFunc[i].function != NULL; i++);
    return i;
}

/**
 * Tells if there is only digit in string
 * @param str
 * @return 1 if success, 0 for error
 */
int isAllDigit(const char* str)
{
    for (int i = 0; i < strlen(str); i++)
        if (!isnumber(str[i]))
            return 0;

    return 1;
}

/**
 * Read user input
 * @param size
 * @return char* if success, NULL for error
 */
char *getUserInput(int size)
{
    char *text;
    int c;
    int i = 0;

    if ((text = xmalloc(sizeof(char) * size + 1, __FUNCTION__)) == NULL)
        return NULL;

    printf("> ");
    while (!i) {
        while ((c = getchar()) != '\n' && c != EOF) {
            if (i < size) {
                text[i] = (char) c;
                i++;
            }
        }
        if (!i)
            printInstruction(11);
    }
    text[i] = '\0';
    printf("\n");

    return text;
}

/**
 * Prints instruction
 * @param sentence, size
 */
void printInstruction(int choice)
{
    switch (choice)
    {
        case -1:
            for (int i = 0; i < 6; i++)
                printf("%s\n", sentences[i].sentence);
            break;
        default:
            printf("%s\n", sentences[choice].sentence);
            break;
    }
}

/**
 * Check if it's necessary to ask the database's name
 * @param database, table, name
 * @return Database*
 */
Database *preInitDatabase(Database *database, int userChoice)
{
    char *databaseName;
    if (database == NULL || (userChoice >= 1 && userChoice <= 3) ) {
        printInstruction(6);
        databaseName = getUserInput(50);
        database = initDatabase(databaseName);
        free(databaseName);
    }
    return database;
}

/**
 * Calls function that depends of the parameter userChoice
 * @param userChoice
 * @return Database*
 */
Database *choice(long userChoice, Database *database)
{
    if (userChoice < 0 || userChoice > (getTableFuncLength() + getDatabaseFuncLength()))
        return database;
    database = preInitDatabase(database, userChoice);
    if (userChoice <= getDatabaseFuncLength()) {
        isSuccess(databaseFunc[userChoice - 1].function(database));
        if (userChoice == 3)
        {
            Database *newDatabase = NULL;
            database = newDatabase;
        }
    }
    else
    {
        printInstruction(7);
        char *tableName = getUserInput(50);

        Table *table1 = xmalloc(sizeof(Table), __func__);
        table1->name = tableName;
        table1->pk = 0;
        table1->fieldHead = NULL;
        table1->next = NULL;
        isSuccess(tableFunc[userChoice - getDatabaseFuncLength() - 1].function(database, table1));
        free(tableName);
    }

    return database;
}

/**
 * Prepare needed data for the creation of the table
 * @param database
 * @param table
 * @return 0 if success, 1 for error
 */
int preCreateTable(Database* database, Table* table)
{
    if (table == NULL)
        return 1;
    while (1) {
        char *fieldName = getFieldName(table);
        database = createField(database, table, fieldName);
        printInstruction(10);
        char *again = getUserInput(2);
        if (again[0] == 'N' || again[0] == 'n')
        {
            free(again);
            return createTable(database, table);
        }
        free(again);
    }
}

/**
 * Create a field
 * @param database, table, name
 * @return Database*
 */
Database *createField(Database *database, Table *table, char *name)
{
    Field *field = xmalloc(sizeof(Field), __func__);

    field->name = name;
    field->type = getEnum();
    field->next = NULL;
    if (table->fieldHead == NULL)
        table->fieldHead = field;
    else
    {
        Field *tmp = table->fieldHead;
        while (tmp->next != NULL)
            tmp = tmp->next;
        tmp->next = field;
    }
    return database;
}

/**
 * Check if same field name already exists
 * @param table
 * @return char*
 */
char *getFieldName(Table *table)
{
    int exist;
    char *fieldName;

    exist = 1;
    fieldName = NULL;
    while (exist || strlen(fieldName) == 0)
    {
        printInstruction(8);
        exist = 0;
        fieldName = getUserInput(50);
        printf("\033c");
        Field *tmp = table->fieldHead;
        if (tmp)
            while (tmp != NULL) {
                if (!strcmp(tmp->name, fieldName)) {
                    printInstruction(12);
                    exist = 1;
                    free(fieldName);
                }
                tmp = tmp->next;
            }
    }
    return fieldName;
}

/**
 * Gets user input and begins processes
 * @return FieldType
 */
FieldType getEnum()
{
    long type = 0;
    while (type <= 0 || type >= 5) {
        printInstruction(9);
        char *numberChar = getUserInput(2);
        type = isAllDigit(numberChar) ? strtol(numberChar, NULL, 10) : 0;
        free(numberChar);
    }
    switch (type)
    {
        case 1:
            return INT;
        case 2:
            return FLOAT;
        case 3:
            return CHAR;
        case 4:
            return VARCHAR;
    }

    return VARCHAR;
}

/**
 * Gets user input and begins processes
 * @param userChoice
 * @return 0 if success, 1 for error
 */
int userInterface()
{
    Database *database = NULL;

    printf("\033c");
    while (1) {
        printInstruction(-1);
        char *text = getUserInput(5);
        printf("\033c");
        if (!strcmp(text, "quit"))
        {
            free(text);
            break;
        }
        else if (text[0] != '\0' && isAllDigit(text))
            database = choice(strtol(text, NULL, 10), database);
        free(text);
    }
    freeDatabase(database);
    return 0;
}