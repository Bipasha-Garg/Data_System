#include "global.h"

bool syntacticParseUPDATE() {
    logger.log("syntacticParseUPDATE");
    cout << "SYNTAX: UPDATE relation_name WHERE col<op>value SET col=new_value" << endl;

    if (tokenizedQuery.size() != 10 || tokenizedQuery[2] != "WHERE" || tokenizedQuery[6] != "SET" || tokenizedQuery[8] != "=") {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    cout << "Parsed Query: ";
    for (const auto& token : tokenizedQuery) {
        cout << token << " ";
    }
    cout << endl;

    parsedQuery.queryType = UPDATE;
    parsedQuery.updateRelationName = tokenizedQuery[1];

    // Parse the WHERE clause using smatch (no space between col, op, and value)
    string whereClause = tokenizedQuery[3] + tokenizedQuery[4] + tokenizedQuery[5];  // "col<op>value"

    regex whereRegex(R"((\w+)(<=|>=|!=|=|<|>)(-?\d+))");
    smatch whereMatch;

    if (!regex_match(whereClause, whereMatch, whereRegex)) {
        cout << "SYNTAX ERROR: Invalid WHERE clause format" << endl;
        return false;
    }

    try {

        parsedQuery.updateConditionColumn = whereMatch[1];   // Column in WHERE clause
        parsedQuery.updateConditionOperator = whereMatch[2]; // Operator in WHERE clause
        parsedQuery.updateConditionValue = stoi(whereMatch[3]); // Value in WHERE clause
    } catch (...) {
        cout << "SYNTAX ERROR: Invalid value in WHERE clause (must be integer)" << endl;
        return false;
    }

    // Parse the SET clause using smatch (no space between col and new_value)
    string setClause = tokenizedQuery[7] + tokenizedQuery[8] + tokenizedQuery[9]; // "col=new_value"
    regex setRegex(R"((\w+)=(\d+))");
    smatch setMatch;

    if (!regex_match(setClause, setMatch, setRegex)) {
        cout << "SYNTAX ERROR: Invalid SET clause format" << endl;
        return false;
    }

    try {
        parsedQuery.updateTargetColumn = setMatch[1];    // Column in SET clause
        parsedQuery.updateNewValue = stoi(setMatch[2]);  // New value in SET clause
    } catch (...) {
        cout << "SYNTAX ERROR: Invalid value in SET clause (must be integer)" << endl;
        return false;
    }

    return true;
}


bool semanticParseUPDATE() {
    logger.log("semanticParseUPDATE");

    if (!tableCatalogue.isTable(parsedQuery.updateRelationName)) {
        cout << "SEMANTIC ERROR: Table doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.updateConditionColumn, parsedQuery.updateRelationName) ||
        !tableCatalogue.isColumnFromTable(parsedQuery.updateTargetColumn, parsedQuery.updateRelationName)) {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }

    Table* table = tableCatalogue.getTable(parsedQuery.updateRelationName);
    if (!table->indexed || table->getColumnIndex(parsedQuery.updateConditionColumn) != table->indexedColumn) {
        cout << "SEMANTIC ERROR: Table not indexed on the condition column" << endl;
        return false;
    }

    return true;
}

void executeUPDATE() {
    logger.log("executeUPDATE");

    Table* table = tableCatalogue.getTable(parsedQuery.updateRelationName);
    int condColIndex = table->getColumnIndex(parsedQuery.updateConditionColumn);
    int targetColIndex = table->getColumnIndex(parsedQuery.updateTargetColumn);
    string op = parsedQuery.updateConditionOperator;
    int value = parsedQuery.updateConditionValue;
    int newVal = parsedQuery.updateNewValue;

    // print all the values
    cout << "Condition Column: " << parsedQuery.updateConditionColumn << endl;
    cout << "Condition Operator: " << parsedQuery.updateConditionOperator << endl;
    cout << "Condition Value: " << parsedQuery.updateConditionValue << endl;
    cout << "Target Column: " << parsedQuery.updateTargetColumn << endl;
    cout << "New Value: " << parsedQuery.updateNewValue << endl;
    

    vector<int> matchingPages = table->bptree->search(value, op);

    for (int pageIndex : matchingPages) {
        Page page = bufferManager.getPage(table->tableName, pageIndex);
        vector<vector<int>> rows = page.getRows();
        bool modified = false;

        for (auto& row : rows) {
            int cellVal = row[condColIndex];
            bool match = false;

            if (op == "=") match = (cellVal == value);
            else if (op == "<") match = (cellVal < value);
            else if (op == "<=") match = (cellVal <= value);
            else if (op == ">") match = (cellVal > value);
            else if (op == ">=") match = (cellVal >= value);
            else if (op == "!=") match = (cellVal != value);

            // if (match) {
            //     row[targetColIndex] = newVal;
            //     modified = true;
            // }

            if (match) {
                // Print the matching row's column values before modification
                cout << "Matching Row: ";
                for (int i = 0; i < row.size(); ++i) {
                    cout << table->columns[i] << "=" << row[i] << " ";
                }
                cout << endl;
    
                // Perform the update
                row[targetColIndex] = newVal;
                modified = true;
            }
            
        }

        if (modified) {
            // Write the updated rows back to disk
            bufferManager.deleteFromPool(page.pageName);
            bufferManager.writePage(table->tableName, pageIndex, rows, rows.size());

        }
    }

    cout << "UPDATE COMPLETE" << endl;
}
