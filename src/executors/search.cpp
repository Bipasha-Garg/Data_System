#include "global.h"
bool syntacticParseSEARCH() {
    logger.log("syntacticParseSEARCH");

    cout << "SYNTAX: SEARCH result_relation_name <- SEARCH FROM relation_name WHERE condition" << endl;

    if (tokenizedQuery.size() != 7 || tokenizedQuery[1] != "<-" || tokenizedQuery[2] != "SEARCH" ||
        tokenizedQuery[3] != "FROM" || tokenizedQuery[5] != "WHERE") {
        cout << "SYNTAXX ERROR" << endl;
        return false;
    }

    parsedQuery.queryType = SEARCH;
    parsedQuery.searchResultRelationName = tokenizedQuery[0];
    parsedQuery.searchRelationName = tokenizedQuery[4];
    
    cout << "hi";

    string condition = tokenizedQuery[6];
    cout << "Condition: " << condition << endl;
    regex conditionRegex(R"(([a-zA-Z_][a-zA-Z0-9_]*)\s*(<=|>=|!=|=|<|>)\s*(-?\d+))");
    smatch match;
    if (!regex_match(condition, match, conditionRegex)) {
        cout << "SYNTAX ERROR in condition" << endl;
        return false;
    }

    parsedQuery.searchColumnName = match[1];
    parsedQuery.searchOperator = match[2];
    parsedQuery.searchValue = stoi(match[3]);

    return true;
}
bool semanticParseSEARCH() {
    logger.log("semanticParseSEARCH");

    if (!tableCatalogue.isTable(parsedQuery.searchRelationName)) {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.searchColumnName, parsedQuery.searchRelationName)) {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }

    Table* table = tableCatalogue.getTable(parsedQuery.searchRelationName);
    printf("Table Name: %s\n", table->tableName.c_str());
    cout << "Search Column: " << parsedQuery.searchColumnName << endl;
    cout << "Indexed Column: " << table->columns[table->indexedColumn] << endl;
    cout << "Indexed: " << table->indexed << endl;
    if (!table->indexed || table->getColumnIndex(parsedQuery.searchColumnName) != table->indexedColumn) {
        cout << "SEMANTIC ERROR: Table not indexed on this column" << endl;
        return false;
    }

    return true;
}

void executeSEARCH() {
    logger.log("executeSEARCH");
    Table* table = tableCatalogue.getTable(parsedQuery.searchRelationName);
    int colIndex = table->getColumnIndex(parsedQuery.searchColumnName);
    string op = parsedQuery.searchOperator;
    int value = parsedQuery.searchValue;

    vector<vector<int>> resultRows;

    // Use B+Tree index to get matching blocks
    vector<int> matchingPages = table->bptree->search(value, op);

    for (int pageIndex : matchingPages) {
        Page pg = bufferManager.getPage(table->tableName, pageIndex);
        vector<vector<int>> rows = pg.getRows();

        for (auto row : rows) {
            int cellVal = row[colIndex];
            bool match = false;
            if (op == "=") match = (cellVal == value);
            else if (op == "<") match = (cellVal < value);
            else if (op == "<=") match = (cellVal <= value);
            else if (op == ">") match = (cellVal > value);
            else if (op == ">=") match = (cellVal >= value);
            else if (op == "!=") match = (cellVal != value);

            if (match)
                resultRows.push_back(row);
        }
    }

    // Create result table
// If a table with the same name exists, delete it first
if (tableCatalogue.isTable(parsedQuery.searchResultRelationName)) {
    tableCatalogue.deleteTable(parsedQuery.searchResultRelationName);
}

// Create and populate the new result table
Table* result = new Table(parsedQuery.searchResultRelationName, table->columns);
for (const auto& row : resultRows) {
    result->writeRow<int>(row);
}
result->blockify();

// Insert into catalogue
tableCatalogue.insertTable(result);

}
