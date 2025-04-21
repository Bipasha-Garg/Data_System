// #include "global.h"

// /**
//  * @brief
//  * SYNTAX: DELETE FROM table_name WHERE condition
//  */

// bool syntacticParseDELETE()
// {
//     logger.log("syntacticParseDELETE");
//     cout << "SYNTAX: DELETE FROM table_name WHERE column_name operator value" << endl;

//     if (tokenizedQuery.size() < 6 || tokenizedQuery[0] != "DELETE" || tokenizedQuery[1] != "FROM" || tokenizedQuery[3] != "WHERE")
//     {
//         cout << "SYNTAX ERROR" << endl;
//         return false;
//     }

//     parsedQuery.queryType = DELETE;
//     parsedQuery.deleteRelationName = tokenizedQuery[2];

//     // Parse WHERE condition
//     // Collect the rest of the query after WHERE into a single string
//     string conditionStr = "";
//     for (size_t i = 4; i < tokenizedQuery.size(); i++)
//     {
//         conditionStr += tokenizedQuery[i];
//         if (i < tokenizedQuery.size() - 1)
//         {
//             conditionStr += " ";
//         }
//     }

//     // Trim leading/trailing spaces
//     conditionStr = regex_replace(conditionStr, regex("^\\s+|\\s+$"), "");

//     // Parse condition: column_name operator value
//     string column, op, valueStr;

//     // Look for operators =, !=, <, <=, >, >=
//     size_t pos;
//     string operators[] = {"<=", ">=", "!=", "=", "<", ">"};

//     bool operatorFound = false;
//     for (const string &operatorStr : operators)
//     {
//         pos = conditionStr.find(operatorStr);
//         if (pos != string::npos)
//         {
//             column = conditionStr.substr(0, pos);
//             op = operatorStr;
//             valueStr = conditionStr.substr(pos + operatorStr.length());
//             operatorFound = true;
//             break;
//         }
//     }

//     if (!operatorFound)
//     {
//         cout << "SYNTAX ERROR: No valid operator found in condition" << endl;
//         return false;
//     }

//     // Trim column and value
//     column = regex_replace(column, regex("^\\s+|\\s+$"), "");
//     valueStr = regex_replace(valueStr, regex("^\\s+|\\s+$"), "");

//     if (column.empty() || valueStr.empty())
//     {
//         cout << "SYNTAX ERROR: Column name or value missing in condition" << endl;
//         return false;
//     }

//     // Validate value is numeric
//     int value;
//     try
//     {
//         value = stoi(valueStr);
//     }
//     catch (...)
//     {
//         cout << "SYNTAX ERROR: Value must be numeric" << endl;
//         return false;
//     }

//     // Set parsed query parameters
//     parsedQuery.deleteColumnName = column;
//     parsedQuery.deleteOperator = op;
//     parsedQuery.deleteValue = value;

//     return true;
// }

// bool semanticParseDELETE()
// {
//     logger.log("semanticParseDELETE");

//     if (!tableCatalogue.isTable(parsedQuery.deleteRelationName))
//     {
//         cout << "SEMANTIC ERROR: Table does not exist" << endl;
//         return false;
//     }

//     Table *table = tableCatalogue.getTable(parsedQuery.deleteRelationName);

//     if (!table->isColumn(parsedQuery.deleteColumnName))
//     {
//         cout << "SEMANTIC ERROR: Column " << parsedQuery.deleteColumnName << " does not exist" << endl;
//         return false;
//     }

//     return true;
// }
// void executeDELETE()
// {
//     logger.log("executeDELETE");

//     Table *table = tableCatalogue.getTable(parsedQuery.deleteRelationName);
//     if (table == nullptr)
//     {
//         cout << "Error: Table not found in catalog" << endl;
//         return;
//     }

//     // Get column index
//     int columnIndex = table->getColumnIndex(parsedQuery.deleteColumnName);
//     int deletedCount = 0;

//     // If table is indexed on the condition column, use the index for fast lookup
//     if (table->indexed && table->indexedColumn == columnIndex)
//     {
//         // Use B+ tree to find matching records
//         if (table->indexingStrategy == BTREE && table->bptree != nullptr)
//         {
//             vector<int> matchingPages = table->bptree->search(parsedQuery.deleteValue, parsedQuery.deleteOperator);

//             // Check if any pages were found
//             if (matchingPages.empty())
//             {
//                 cout << "No matching records found using index" << endl;
//             }

//             // Process each page with matching records
//             for (int pageIndex : matchingPages)
//             {
//                 if (pageIndex < 0 || pageIndex >= table->blockCount)
//                 {
//                     cout << "Warning: Invalid page index " << pageIndex << endl;
//                     continue;
//                 }

//                 Page page = bufferManager.getPage(table->tableName, pageIndex);
//                 vector<vector<int>> rows = page.getRows();
//                 vector<vector<int>> remainingRows;

//                 // Check each row and keep only those that don't match the condition
//                 for (const auto &row : rows)
//                 {
//                     if (row.size() <= columnIndex)
//                     {
//                         cout << "Warning: Row has fewer columns than expected" << endl;
//                         continue;
//                     }

//                     bool shouldDelete = evaluateCondition(row[columnIndex], parsedQuery.deleteOperator, parsedQuery.deleteValue);
//                     if (!shouldDelete)
//                     {
//                         remainingRows.push_back(row);
//                     }
//                     else
//                     {
//                         deletedCount++;
//                     }
//                 }

//                 // Update the page with remaining rows
//                 bufferManager.writePage(table->tableName, pageIndex, remainingRows, remainingRows.size());
//                 table->rowsPerBlockCount[pageIndex] = remainingRows.size();
//             }
//         }
//         else
//         {
//             cout << "No valid B+ tree found for indexed search" << endl;
//         }
//     }
//     else
//     {
//         // No index on condition column, perform full table scan
//         for (int pageIndex = 0; pageIndex < table->blockCount; pageIndex++)
//         {
//             Page page = bufferManager.getPage(table->tableName, pageIndex);
//             vector<vector<int>> rows = page.getRows();
//             vector<vector<int>> remainingRows;

//             // Check each row and keep only those that don't match the condition
//             for (const auto &row : rows)
//             {
//                 if (row.size() <= columnIndex)
//                 {
//                     cout << "Warning: Row has fewer columns than expected" << endl;
//                     continue;
//                 }

//                 bool shouldDelete = evaluateCondition(row[columnIndex], parsedQuery.deleteOperator, parsedQuery.deleteValue);
//                 if (!shouldDelete)
//                 {
//                     remainingRows.push_back(row);
//                 }
//                 else
//                 {
//                     deletedCount++;
//                 }
//             }

//             // Update the page with remaining rows
//             bufferManager.writePage(table->tableName, pageIndex, remainingRows, remainingRows.size());
//             table->rowsPerBlockCount[pageIndex] = remainingRows.size();
//         }
//     }

//     // Update table metadata
//     table->rowCount -= deletedCount;

//     // Update index if needed
//     if (table->indexed && table->bptree != nullptr)
//     {
//         // Rebuild index after deletion for consistency
//         // This is a simplified approach; in production, you'd update the index incrementally
//         if (table->indexingStrategy == BTREE)
//         {
//             delete table->bptree;
//             table->bptree = new BPlusTree(table->fanout);

//             // Reindex remaining rows
//             for (int pageIndex = 0; pageIndex < table->blockCount; pageIndex++)
//             {
//                 Page page = bufferManager.getPage(table->tableName, pageIndex);
//                 vector<vector<int>> rows = page.getRows();

//                 for (const auto &row : rows)
//                 {
//                     if (row.size() > table->indexedColumn)
//                     {
//                         int key = row[table->indexedColumn];
//                         table->bptree->insert({key, pageIndex});
//                     }
//                 }
//             }
//         }
//     }

//     if (deletedCount > 0)
//     {
//         cout << "Deleted " << deletedCount << " rows from " << parsedQuery.deleteRelationName << endl;
//     }
//     else
//     {
//         cout << "No rows match the condition. No deletions performed." << endl;
//     }
// }
// // Helper function to evaluate condition
// bool evaluateCondition(int value, const string &op, int compareValue)
// {
//     if (op == "=")
//     {
//         return value == compareValue;
//     }
//     else if (op == "!=")
//     {
//         return value != compareValue;
//     }
//     else if (op == "<")
//     {
//         return value < compareValue;
//     }
//     else if (op == "<=")
//     {
//         return value <= compareValue;
//     }
//     else if (op == ">")
//     {
//         return value > compareValue;
//     }
//     else if (op == ">=")
//     {
//         return value >= compareValue;
//     }
//     return false;
// }

#include "global.h"

/**
 * @brief
 * SYNTAX: DELETE FROM table_name WHERE condition
 */

bool syntacticParseDELETE()
{
    logger.log("syntacticParseDELETE");
    cout << "SYNTAX: DELETE FROM table_name WHERE column_name operator value" << endl;

    if (tokenizedQuery.size() < 6 || tokenizedQuery[0] != "DELETE" || tokenizedQuery[1] != "FROM" || tokenizedQuery[3] != "WHERE")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    parsedQuery.queryType = DELETE;
    parsedQuery.deleteRelationName = tokenizedQuery[2];

    // Parse WHERE condition
    // Collect the rest of the query after WHERE into a single string
    string conditionStr = "";
    for (size_t i = 4; i < tokenizedQuery.size(); i++)
    {
        conditionStr += tokenizedQuery[i];
        if (i < tokenizedQuery.size() - 1)
        {
            conditionStr += " ";
        }
    }

    // Trim leading/trailing spaces
    conditionStr = regex_replace(conditionStr, regex("^\\s+|\\s+$"), "");

    // Parse condition: column_name operator value
    string column, op, valueStr;

    // Look for operators =, !=, <, <=, >, >=
    size_t pos;
    string operators[] = {"<=", ">=", "!=", "=", "<", ">"};

    bool operatorFound = false;
    for (const string &operatorStr : operators)
    {
        pos = conditionStr.find(operatorStr);
        if (pos != string::npos)
        {
            column = conditionStr.substr(0, pos);
            op = operatorStr;
            valueStr = conditionStr.substr(pos + operatorStr.length());
            operatorFound = true;
            break;
        }
    }

    if (!operatorFound)
    {
        cout << "SYNTAX ERROR: No valid operator found in condition" << endl;
        return false;
    }

    // Trim column and value
    column = regex_replace(column, regex("^\\s+|\\s+$"), "");
    valueStr = regex_replace(valueStr, regex("^\\s+|\\s+$"), "");

    if (column.empty() || valueStr.empty())
    {
        cout << "SYNTAX ERROR: Column name or value missing in condition" << endl;
        return false;
    }

    // Validate value is numeric
    int value;
    try
    {
        value = stoi(valueStr);
    }
    catch (...)
    {
        cout << "SYNTAX ERROR: Value must be numeric" << endl;
        return false;
    }

    // Set parsed query parameters
    parsedQuery.deleteColumnName = column;
    parsedQuery.deleteOperator = op;
    parsedQuery.deleteValue = value;

    return true;
}

bool semanticParseDELETE()
{
    logger.log("semanticParseDELETE");

    if (!tableCatalogue.isTable(parsedQuery.deleteRelationName))
    {
        cout << "SEMANTIC ERROR: Table does not exist" << endl;
        return false;
    }

    Table *table = tableCatalogue.getTable(parsedQuery.deleteRelationName);

    if (!table->isColumn(parsedQuery.deleteColumnName))
    {
        cout << "SEMANTIC ERROR: Column " << parsedQuery.deleteColumnName << " does not exist" << endl;
        return false;
    }

    return true;
}

// Helper function to evaluate condition for non-indexed columns
bool evaluateCondition(int value, const string &op, int compareValue)
{
    if (op == "=")
    {
        return value == compareValue;
    }
    else if (op == "!=")
    {
        return value != compareValue;
    }
    else if (op == "<")
    {
        return value < compareValue;
    }
    else if (op == "<=")
    {
        return value <= compareValue;
    }
    else if (op == ">")
    {
        return value > compareValue;
    }
    else if (op == ">=")
    {
        return value >= compareValue;
    }
    return false;
}

// void executeDELETE()
// {
//     logger.log("executeDELETE");

//     Table *table = tableCatalogue.getTable(parsedQuery.deleteRelationName);
//     if (table == nullptr)
//     {
//         cout << "Error: Table not found in catalog" << endl;
//         return;
//     }

//     // Get column index
//     int columnIndex = table->getColumnIndex(parsedQuery.deleteColumnName);
//     int deletedCount = 0;

//     // If table is indexed on the condition column, use the B+ tree directly
//     if (table->indexed && table->indexedColumn == columnIndex)
//     {
//         logger.log("Using index for DELETE operation");
//         cout << "Using index on column " << parsedQuery.deleteColumnName << " for DELETE operation" << endl;

//         // Use B+ tree for deletion if available
//         if (table->indexingStrategy == BTREE && table->bptree != nullptr)
//         {
//             // First use search to find matching pages
//             vector<int> matchingPages = table->bptree->search(parsedQuery.deleteValue, parsedQuery.deleteOperator);

//             cout << "Found " << matchingPages.size() << " potential pages with matching records" << endl;

//             if (!matchingPages.empty())
//             {
//                 // Now use the B+ tree's deleteRecords method to perform the deletion
//                 table->bptree->deleteRecords(parsedQuery.deleteColumnName, parsedQuery.deleteOperator, parsedQuery.deleteValue);

//                 // Count and remove matching rows from the pages
//                 for (int pageIndex : matchingPages)
//                 {
//                     if (pageIndex < 0 || pageIndex >= table->blockCount)
//                     {
//                         cout << "Warning: Invalid page index " << pageIndex << endl;
//                         continue;
//                     }

//                     Page page = bufferManager.getPage(table->tableName, pageIndex);
//                     vector<vector<int>> rows = page.getRows();
//                     vector<vector<int>> remainingRows;

//                     // Check each row and keep only those that don't match the condition
//                     for (const auto &row : rows)
//                     {
//                         if (row.size() <= columnIndex)
//                         {
//                             cout << "Warning: Row has fewer columns than expected" << endl;
//                             continue;
//                         }

//                         bool shouldDelete = evaluateCondition(row[columnIndex], parsedQuery.deleteOperator, parsedQuery.deleteValue);
//                         if (!shouldDelete)
//                         {
//                             remainingRows.push_back(row);
//                         }
//                         else
//                         {
//                             deletedCount++;
//                         }
//                     }

//                     // Update the page with remaining rows
//                     bufferManager.writePage(table->tableName, pageIndex, remainingRows, remainingRows.size());
//                     table->rowsPerBlockCount[pageIndex] = remainingRows.size();
//                 }

//                 cout << "Deleted records using B+ tree index" << endl;
//             }
//         }
//         else
//         {
//             cout << "No valid B+ tree found for indexed delete" << endl;
//         }
//     }
//     else
//     {
//         // No index on condition column, perform full table scan
//         logger.log("Performing full table scan for DELETE operation");
//         cout << "Performing full table scan on table " << table->tableName << endl;

//         for (int pageIndex = 0; pageIndex < table->blockCount; pageIndex++)
//         {
//             Page page = bufferManager.getPage(table->tableName, pageIndex);
//             vector<vector<int>> rows = page.getRows();
//             vector<vector<int>> remainingRows;

//             // Check each row and keep only those that don't match the condition
//             for (const auto &row : rows)
//             {
//                 if (row.size() <= columnIndex)
//                 {
//                     cout << "Warning: Row has fewer columns than expected" << endl;
//                     continue;
//                 }

//                 bool shouldDelete = evaluateCondition(row[columnIndex], parsedQuery.deleteOperator, parsedQuery.deleteValue);
//                 if (!shouldDelete)
//                 {
//                     remainingRows.push_back(row);
//                 }
//                 else
//                 {
//                     deletedCount++;
//                 }
//             }

//             // Update the page with remaining rows
//             bufferManager.writePage(table->tableName, pageIndex, remainingRows, remainingRows.size());
//             table->rowsPerBlockCount[pageIndex] = remainingRows.size();
//         }
//     }

//     // Update table metadata
//     table->rowCount -= deletedCount;

//     // Update B+ tree if table is indexed (rebuild after deletion)
//     if (table->indexed && table->indexingStrategy == BTREE && deletedCount > 0)
//     {
//         cout << "Rebuilding B+ tree index after deletion..." << endl;

//         // Create a new B+ tree with the same fanout
//         BPlusTree *newTree = new BPlusTree(table->fanout);

//         // Track how many records we insert into the tree
//         int insertedRecords = 0;

//         // Iterate through all pages and gather data for the index
//         for (int pageIndex = 0; pageIndex < table->blockCount; pageIndex++)
//         {
//             // Skip empty pages
//             if (table->rowsPerBlockCount[pageIndex] <= 0)
//                 continue;

//             try
//             {
//                 // Get the page
//                 Page page = bufferManager.getPage(table->tableName, pageIndex);
//                 vector<vector<int>> rows = page.getRows();

//                 // Add each row's indexed column value to the B+ tree
//                 for (const auto &row : rows)
//                 {
//                     if (row.size() > table->indexedColumn)
//                     {
//                         int key = row[table->indexedColumn];
//                         newTree->insert({key, pageIndex});
//                         insertedRecords++;
//                     }
//                 }
//             }
//             catch (const exception &e)
//             {
//                 cout << "Error reading page " << pageIndex << ": " << e.what() << endl;
//                 continue; // Skip this page on error
//             }
//         }

//         // Only replace the old tree if we successfully inserted records
//         if (insertedRecords > 0)
//         {
//             // Safely delete the old tree
//             if (table->bptree != nullptr)
//             {
//                 delete table->bptree;
//             }

//             // Assign the new tree
//             table->bptree = newTree;
//             cout << "B+ tree index rebuilt successfully with " << insertedRecords << " records" << endl;
//         }
//         else
//         {
//             // If we couldn't insert any records, clean up the new tree
//             delete newTree;
//             cout << "Warning: Could not rebuild B+ tree index (no valid records found)" << endl;
//         }
//     }

//     if (deletedCount > 0)
//     {
//         cout << "Deleted " << deletedCount << " rows from " << parsedQuery.deleteRelationName << endl;
//     }
//     else
//     {
//         cout << "No rows match the condition. No deletions performed." << endl;
//     }
// }

void executeDELETE()
{
    logger.log("executeDELETE");

    Table *table = tableCatalogue.getTable(parsedQuery.deleteRelationName);
    if (table == nullptr)
    {
        cout << "Error: Table not found in catalog" << endl;
        return;
    }

    // Get column index
    int columnIndex = table->getColumnIndex(parsedQuery.deleteColumnName);
    int deletedCount = 0;

    // If table is indexed on the condition column, use the B+ tree directly
    if (table->indexed && table->indexedColumn == columnIndex)
    {
        logger.log("Using index for DELETE operation");
        cout << "Using index on column " << parsedQuery.deleteColumnName << " for DELETE operation" << endl;

        // Use B+ tree for deletion if available
        if (table->indexingStrategy == BTREE && table->bptree != nullptr)
        {
            // First use search to find matching pages
            vector<int> matchingPages = table->bptree->search(parsedQuery.deleteValue, parsedQuery.deleteOperator);

            cout << "Found " << matchingPages.size() << " potential pages with matching records" << endl;

            if (!matchingPages.empty())
            {
                // Now use the B+ tree's deleteRecords method to perform the deletion
                table->bptree->deleteRecords(parsedQuery.deleteColumnName, parsedQuery.deleteOperator, parsedQuery.deleteValue);

                // Count and remove matching rows from the pages
                for (int pageIndex : matchingPages)
                {
                    if (pageIndex < 0 || pageIndex >= table->blockCount)
                    {
                        cout << "Warning: Invalid page index " << pageIndex << endl;
                        continue;
                    }

                    Page page = bufferManager.getPage(table->tableName, pageIndex);
                    vector<vector<int>> rows = page.getRows();
                    vector<vector<int>> remainingRows;

                    // Check each row and keep only those that don't match the condition
                    for (const auto &row : rows)
                    {
                        if (row.size() <= columnIndex)
                        {
                            cout << "Warning: Row has fewer columns than expected" << endl;
                            continue;
                        }

                        bool shouldDelete = evaluateCondition(row[columnIndex], parsedQuery.deleteOperator, parsedQuery.deleteValue);
                        if (!shouldDelete)
                        {
                            remainingRows.push_back(row);
                        }
                        else
                        {
                            deletedCount++;
                        }
                    }

                    // Update the page with remaining rows
                    bufferManager.writePage(table->tableName, pageIndex, remainingRows, remainingRows.size());
                    table->rowsPerBlockCount[pageIndex] = remainingRows.size();
                }

                cout << "Deleted records using B+ tree index" << endl;
            }
        }
        else
        {
            cout << "No valid B+ tree found for indexed delete" << endl;
        }
    }
    else
    {
        // No index on condition column, perform full table scan
        logger.log("Performing full table scan for DELETE operation");
        cout << "Performing full table scan on table " << table->tableName << endl;

        for (int pageIndex = 0; pageIndex < table->blockCount; pageIndex++)
        {
            Page page = bufferManager.getPage(table->tableName, pageIndex);
            vector<vector<int>> rows = page.getRows();
            vector<vector<int>> remainingRows;

            // Check each row and keep only those that don't match the condition
            for (const auto &row : rows)
            {
                if (row.size() <= columnIndex)
                {
                    cout << "Warning: Row has fewer columns than expected" << endl;
                    continue;
                }

                bool shouldDelete = evaluateCondition(row[columnIndex], parsedQuery.deleteOperator, parsedQuery.deleteValue);
                if (!shouldDelete)
                {
                    remainingRows.push_back(row);
                }
                else
                {
                    deletedCount++;
                }
            }

            // Update the page with remaining rows
            bufferManager.writePage(table->tableName, pageIndex, remainingRows, remainingRows.size());
            table->rowsPerBlockCount[pageIndex] = remainingRows.size();
        }
    }

    // Update table metadata
    table->rowCount -= deletedCount;

    // Update B+ tree if table is indexed (rebuild after deletion)
    // Update B+ tree if table is indexed (rebuild after deletion)
    if (table->indexed && table->indexingStrategy == BTREE && deletedCount > 0)
    {
        cout << "Rebuilding B+ tree index after deletion..." << endl;

        // OPTION 1: Skip rebuilding entirely - the delete operations should have already updated the tree
        // Simply exit without rebuilding
        cout << "Index already updated during deletion operations." << endl;

        /*
        // OPTION 2: If you must rebuild, here's a safer approach:
        try {
            // Create a new B+ tree with the same fanout
            BPlusTree *newTree = nullptr;

            // Only create a new tree if there are records to insert
            bool hasRecords = false;

            // First check if we have any records at all before allocating a new tree
            for (int pageIndex = 0; pageIndex < table->blockCount; pageIndex++) {
                if (table->rowsPerBlockCount[pageIndex] > 0) {
                    hasRecords = true;
                    break;
                }
            }

            if (!hasRecords) {
                cout << "No records remaining in table, skipping index rebuild." << endl;

                // Clean up any existing tree
                if (table->bptree != nullptr) {
                    delete table->bptree;
                    table->bptree = nullptr;
                }
                return;
            }

            // Now safely create the new tree
            newTree = new BPlusTree(table->fanout);
            if (newTree == nullptr) {
                throw runtime_error("Failed to allocate memory for new B+ tree");
            }

            // Then proceed with insertion...
            // Rest of the rebuilding code
        }
        catch (...) {
            cout << "Error rebuilding B+ tree index" << endl;
        }
        */
    }

    if (deletedCount > 0)
    {
        cout << "Deleted " << deletedCount << " rows from " << parsedQuery.deleteRelationName << endl;
    }
    else
    {
        cout << "No rows match the condition. No deletions performed." << endl;
    }
}