#include "global.h"

bool syntacticParseUPDATE() {
    logger.log("syntacticParseUPDATE");

    if (tokenizedQuery.size() != 8 || tokenizedQuery[2] != "WHERE" || tokenizedQuery[4] != "SET" || 
        tokenizedQuery[6] != "=") {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    parsedQuery.queryType = UPDATE;
    parsedQuery.updateRelationName = tokenizedQuery[1];
    


    // Parse condition
    string condition = tokenizedQuery[3];
    regex conditionRegex(R"(([a-zA-Z_][a-zA-Z0-9_]*)\s*(<=|>=|!=|=|<|>)\s*(-?\d+))");
    smatch match;
    if (!regex_match(condition, match, conditionRegex)) {
        cout << "SYNTAX ERROR in condition" << endl;
        return false;
    }
    parsedQuery.updateConditionColumn = match[1];
    parsedQuery.updateConditionOperator = match[2];
    parsedQuery.updateConditionValue = stoi(match[3]);

    // Parse set clause
    parsedQuery.updateColumnName = tokenizedQuery[5];
    parsedQuery.updateValue = stoi(tokenizedQuery[7]);


    return true;
}

bool semanticParseUPDATE() {
    logger.log("semanticParseUPDATE");

    if (!tableCatalogue.isTable(parsedQuery.updateRelationName)) {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    Table* table = tableCatalogue.getTable(parsedQuery.updateRelationName);

    // Check condition column exists
    if (!tableCatalogue.isColumnFromTable(parsedQuery.updateConditionColumn, parsedQuery.updateRelationName)) {
        cout << "SEMANTIC ERROR: Condition column doesn't exist in relation" << endl;
        return false;
    }

    // Check update column exists
    if (!tableCatalogue.isColumnFromTable(parsedQuery.updateColumnName, parsedQuery.updateRelationName)) {
        cout << "SEMANTIC ERROR: Update column doesn't exist in relation" << endl;
        return false;
    }

    // Check if table is indexed on condition column
    if (!table->indexed || table->getColumnIndex(parsedQuery.updateConditionColumn) != table->indexedColumn) {
        cout << "SEMANTIC ERROR: Table not indexed on condition column" << endl;
        return false;
    }

    return true;
}

void executeUPDATE() {
    logger.log("executeUPDATE");

    cout << "Executing UPDATE on table: " << parsedQuery.updateRelationName << endl;
    Table* table = tableCatalogue.getTable(parsedQuery.updateRelationName);
    int conditionColIndex = table->getColumnIndex(parsedQuery.updateConditionColumn);
    int updateColIndex = table->getColumnIndex(parsedQuery.updateColumnName);
    string op = parsedQuery.updateConditionOperator;
    int conditionValue = parsedQuery.updateConditionValue;
    int newValue = parsedQuery.updateValue;

    // Use B+Tree index to get matching blocks
    vector<int> matchingPages = table->bptree->search(conditionValue, op);

    int updatedRows = 0;

    for (int pageIndex : matchingPages) {
        Page pg = bufferManager.getPage(table->tableName, pageIndex);
        vector<vector<int>> rows = pg.getRows();
        bool pageModified = false;

        for (auto& row : rows) {
            int cellVal = row[conditionColIndex];
            bool match = false;
            if (op == "=") match = (cellVal == conditionValue);
            else if (op == "<") match = (cellVal < conditionValue);
            else if (op == "<=") match = (cellVal <= conditionValue);
            else if (op == ">") match = (cellVal > conditionValue);
            else if (op == ">=") match = (cellVal >= conditionValue);
            else if (op == "!=") match = (cellVal != conditionValue);

            if (match) {
                row[updateColIndex] = newValue;
                updatedRows++;
                pageModified = true;
            }
        }

        if (pageModified) {
            // Remove old page from buffer so new write is clean
            bufferManager.deleteFromPool(pg.pageName);
            bufferManager.writePage(table->tableName, pageIndex, rows, rows.size());
        }
    }

    cout << "Updated " << updatedRows << " row(s)." << endl;
    
    // Optional: Update index if necessary
    if (table->indexed) {
        int key = newValue; // Assuming the new value is the key for the index
        int pageNum = table->blockCount - 1;

        if (table->indexingStrategy == HASH)
            table->hashtable->insert({key, pageNum});
        else if (table->indexingStrategy == BTREE)
            table->bptree->insert({key, pageNum});
    }

}