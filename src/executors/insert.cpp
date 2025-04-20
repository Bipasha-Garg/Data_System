#include "global.h"
/**
 * @brief 
 * SYNTAX: INDEX ON column_name FROM relation_name USING indexing_strategy [BUCKET <bucketsize> | FANOUT <num_pointers>]
 * indexing_strategy: ASC | DESC | NOTHING
 */



bool syntacticParseINSERT() {
    logger.log("syntacticParseINSERT");
    cout << "SYNTAX: INSERT INTO relation_name col1 = val1 , col2 = val2 , ..." << endl;

    if (tokenizedQuery.size() < 4 || tokenizedQuery[1] != "INTO") {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    parsedQuery.queryType = INSERT;
    parsedQuery.insertRelationName = tokenizedQuery[2];

    parsedQuery.insertValues.clear();

    // Collect the rest of the query into a single string
    string valuesStr = "";
    for (int i = 3; i < tokenizedQuery.size(); i++) {
        valuesStr += tokenizedQuery[i] + " ";
    }

    // Trim leading/trailing spaces
    valuesStr = regex_replace(valuesStr, regex("^\\s+|\\s+$"), "");

    stringstream ss(valuesStr);
    string token;
    vector<string> tokens;

    // Split by spaces
    while (getline(ss, token, ' ')) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }

    // Expect format: col = val , col = val , ...
    // for (size_t i = 0; i < tokens.size(); i += 4) {
    //     if (i + 2 >= tokens.size()) {
    //         cout << "SYNTAX ERROR: Incomplete col = val pair" << endl;
    //         return false;
    //     }

    //     string col = tokens[i];
    //     string eq = tokens[i + 1];
    //     string val = tokens[i + 2];

    //     if (eq != "=") {
    //         cout << "SYNTAX ERROR: Expected '=' between column and value" << endl;
    //         return false;
    //     }

    //     try {
    //         parsedQuery.insertValues[col] = stoi(val);
    //     } catch (...) {
    //         cout << "SYNTAX ERROR: Non-integer value for column " << col << endl;
    //         return false;
    //     }

    //     // After each pair, expect either end or a comma
    //     if (i + 3 < tokens.size()) {
    //         if (tokens[i + 3] != ",") {
    //             cout << "SYNTAX ERROR: Expected ',' between pairs" << endl;
    //             return false;
    //         }
    //     }
    // }

    for (size_t i = 0; i < tokens.size();) {
        if (i + 2 >= tokens.size()) {
            cout << "SYNTAX ERROR: Incomplete col = val pair" << endl;
            return false;
        }

        string col = tokens[i];
        string eq = tokens[i + 1];
        string val = tokens[i + 2];

        if (eq != "=") {
            cout << "SYNTAX ERROR: Expected '=' between column and value" << endl;
            return false;
        }

        try {
            parsedQuery.insertValues[col] = stoi(val);
        } catch (...) {
            cout << "SYNTAX ERROR: Non-integer value for column " << col << endl;
            return false;
        }

        i += 3;  // Move to the next col = val group
    }

    // for (size_t i = 0; i < tokens.size();) {
    //     if (i + 2 >= tokens.size()) {
    //         cout << "SYNTAX ERROR: Incomplete col = val pair" << endl;
    //         return false;
    //     }

    //     // print all tokens
    //     for (int j = 0; j < tokens.size(); j++) {
    //         cout << tokens[j] << " ";
    //     }
    //     cout << endl;
        
    //     string col = tokens[i];
    //     string eq = tokens[i + 1];
    //     string val = tokens[i + 2];
        
    //     if (eq != "=") {
    //         cout << "SYNTAX ERROR: Expected '=' between column and value" << endl;
    //         return false;
    //     }
        
    //     try {
    //         parsedQuery.insertValues[col] = stoi(val);
    //     } catch (...) {
    //         cout << "SYNTAX ERROR: Non-integer value for column " << col << endl;
    //         return false;
    //     }
        
    //     // After each pair, expect either end or a comma
    //     if (i + 3 < tokens.size()) {
    //         if (tokens[i + 3] != ",") {
    //             cout << "SYNTAX ERROR: Expected ',' between pairs" << endl;
    //             return false;
    //         }
    //         i += 4; // Skip over the comma for the next iteration
    //     } else {
    //         i += 3; // No comma (end of input), just move to next position
    //     }
    // }

    if (parsedQuery.insertValues.empty()) {
        cout << "SYNTAX ERROR: No values provided" << endl;
        return false;
    }

    return true;
}


// void executeINSERT() {
//     logger.log("executeINSERT");

//     Table* table = tableCatalogue.getTable(parsedQuery.insertRelationName);
//     vector<int> newRow(table->columns.size(), 0);

//     // Populate new row
//     for (auto& [col, val] : parsedQuery.insertValues) {
//         int idx = table->getColumnIndex(col);
//         newRow[idx] = val;
//     }

//     // If table is empty
//     if (table->blockCount == 0) {
//         table->blockCount = 1;
//         table->rowsPerBlockCount.push_back(1);
//         table->rowCount++;

//         vector<vector<int>> newBlock;
//         newBlock.push_back(newRow);

//         bufferManager.writePage(table->tableName, 0, newBlock, 1);
//     } else {
//         int lastPageIdx = table->blockCount - 1;
//         Page pg = bufferManager.getPage(table->tableName, lastPageIdx);
//         vector<vector<int>> rows = pg.getRows();

//         if (rows.size() < table->maxRowsPerBlock) {
//             rows.push_back(newRow);
//             table->rowsPerBlockCount[lastPageIdx]++;
//             table->rowCount++;
//             bufferManager.writePage(table->tableName, lastPageIdx, rows, rows.size());
//         } else {
//             // New page
//             vector<vector<int>> newBlock;
//             newBlock.push_back(newRow);
//             table->blockCount++;
//             table->rowsPerBlockCount.push_back(1);
//             table->rowCount++;
//             bufferManager.writePage(table->tableName, table->blockCount - 1, newBlock, 1);
//         }
//     }

//     // Optional: Update index
//     if (table->indexed) {
//         int key = newRow[table->indexedColumn];
//         int pageNum = table->blockCount - 1;

//         if (table->indexingStrategy == HASH)
//             table->hashtable->insert({key, pageNum});
//         else if (table->indexingStrategy == BTREE)
//             table->bptree->insert({key, pageNum});
//     }

//     cout << "Inserted row into " << parsedQuery.insertRelationName << endl;
// }

void executeINSERT() {
    logger.log("executeINSERT");

    Table* table = tableCatalogue.getTable(parsedQuery.insertRelationName);
    vector<int> newRow(table->columns.size(), 0);

    // Populate new row
    for (auto& [col, val] : parsedQuery.insertValues) {
        int idx = table->getColumnIndex(col);
        newRow[idx] = val;
    }

    // If table is empty
    if (table->blockCount == 0) {
        table->blockCount = 1;
        table->rowsPerBlockCount.push_back(1);
        table->rowCount++;

        vector<vector<int>> newBlock;
        newBlock.push_back(newRow);

        bufferManager.writePage(table->tableName, 0, newBlock, 1);
    } else {
        int lastPageIdx = table->blockCount - 1;
        Page pg = bufferManager.getPage(table->tableName, lastPageIdx);
        vector<vector<int>> rows = pg.getRows();

        if (rows.size() < table->maxRowsPerBlock) {
            rows.push_back(newRow);
            table->rowsPerBlockCount[lastPageIdx]++;
            table->rowCount++;

            // Remove old page from buffer so new write is clean
            bufferManager.deleteFromPool(pg.pageName);

            bufferManager.writePage(table->tableName, lastPageIdx, rows, rows.size());
        } else {
            // New page
            vector<vector<int>> newBlock;
            newBlock.push_back(newRow);
            table->blockCount++;
            table->rowsPerBlockCount.push_back(1);
            table->rowCount++;
            bufferManager.writePage(table->tableName, table->blockCount - 1, newBlock, 1);
        }
    }

    // Optional: Update index
    if (table->indexed) {
        int key = newRow[table->indexedColumn];
        int pageNum = table->blockCount - 1;

        if (table->indexingStrategy == HASH)
            table->hashtable->insert({key, pageNum});
        else if (table->indexingStrategy == BTREE)
            table->bptree->insert({key, pageNum});
    }

    cout << "Inserted row into " << parsedQuery.insertRelationName << endl;
}


// void executeINSERT() {
//     logger.log("executeINSERT");

//     Table* table = tableCatalogue.getTable(parsedQuery.insertRelationName);
//     vector<int> newRow(table->columns.size(), 0);

//     cout << "Inserting the row with values: ";
//     for (const auto& [col, val] : parsedQuery.insertValues) {
//         cout << col << "=" << val << " ";
//     }
//     cout << endl;

//     // Fill the new row with values from insertValues or 0
//     for (int i = 0; i < table->columns.size(); i++) {
//         string col = table->columns[i];
//         if (parsedQuery.insertValues.find(col) != parsedQuery.insertValues.end()) {
//             newRow[i] = parsedQuery.insertValues[col];
//         }
//     }

//     // Print the record being inserted with proper formatting
//     cout << "Record being inserted: ";
//     for (int i = 0; i < newRow.size(); i++) {
//         cout << newRow[i];
//         if (i < newRow.size() - 1) {
//             cout << ", ";
//         }
//     }
//     cout << endl;

//     // Append row to table (in memory)
//     table->rows.push_back(newRow);
//     table->rowCount++;

//     table->blockify();

//     // Calculate the correct page index for B+Tree
//     // if (table->indexed) {
//     //     int key = newRow[table->indexedColumn];
//     //     int pageNum = table->blockCount - 1;

//     //     if (table->indexingStrategy == HASH)
//     //         table->hashtable->insert({key, pageNum});
//     //     else if (table->indexingStrategy == BTREE)
//     //         table->bptree->insert({key, pageNum});
//     // }
// }

bool semanticParseINSERT() {
    logger.log("semanticParseINSERT");

    if (!tableCatalogue.isTable(parsedQuery.insertRelationName)) {
        cout << "SEMANTIC ERROR: Table does not exist" << endl;
        return false;
    }

    Table* table = tableCatalogue.getTable(parsedQuery.insertRelationName);

    for (const auto& [col, _] : parsedQuery.insertValues) {
        if (!table->isColumn(col)) {
            cout << "SEMANTIC ERROR: Column " << col << " does not exist" << endl;
            return false;
        }
    }

    return true;
}


// void executeINSERT() {
//     logger.log("executeINSERT");

//     Table* table = tableCatalogue.getTable(parsedQuery.insertRelationName);
//     vector<int> newRow(table->columns.size(), 0);

//     cout << "Inserting the row with values: ";
//     for (const auto& [col, val] : parsedQuery.insertValues) {
//         cout << col << "=" << val << " ";
//     }
//     cout << endl;

//     // Fill the new row with values from insertValues or 0
//     for (int i = 0; i < table->columns.size(); i++) {
//         string col = table->columns[i];
//         if (parsedQuery.insertValues.find(col) != parsedQuery.insertValues.end()) {
//             newRow[i] = parsedQuery.insertValues[col];
//         }
//     }


//     // print new row
   
//     // Append row to table (in memory)
//     table->rows.push_back(newRow);
//     table->rowCount++;

//     // // Get the proper page index for the B+Tree record
//     // int pageIndex = (table->rowCount - 1) / table->maxRowsPerBlock;
    
//     // // Write back in blocks
//     // table->blockify();

//     // cout << "Table indexed" << table->indexed << endl;

//     // // Also insert into B+ Tree if indexed
//     // if (table->indexed && table->indexingStrategy == BTREE) {
//     //     int key = newRow[table->indexedColumn];
//     //     // Use the page index calculated before blockify
//     //     cout << "yes"<<endl;
//     //     pair<int, int> record = {key, pageIndex};
//     //     table->bptree->insert(record);
//     // }
// }
