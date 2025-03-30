// sortUtils.cpp
#include "sortUtils.h"

bool rowComp(std::vector<int> &row1, std::vector<int> &row2)
{
    for (int i = 0; i < parsedQuery.sortColumnNames.size(); i++)
    {
        int colIdx = parsedQuery.sortColumnIndices[i];
        SortingStrategy order = parsedQuery.sortingStrategies[i];

        if (order == ASC)
        {
            if (row1[colIdx] < row2[colIdx])
                return true;
            else if (row1[colIdx] > row2[colIdx])
                return false;
        }
        else
        {
            if (row1[colIdx] > row2[colIdx])
                return true;
            else if (row1[colIdx] < row2[colIdx])
                return false;
        }
    }

    return false;
}

void createTempTable(std::string tempTableName, Table *table)
{
    Table *tempTable = new Table();
    tempTable->tableName = tempTableName;
    tempTable->columns = table->columns;
    tempTable->distinctValuesPerColumnCount = table->distinctValuesPerColumnCount;
    tempTable->columnCount = table->columnCount;
    tempTable->rowCount = table->rowCount;
    tempTable->blockCount = table->blockCount;
    tempTable->maxRowsPerBlock = table->maxRowsPerBlock;
    tempTable->rowsPerBlockCount = table->rowsPerBlockCount;
    tempTable->indexed = table->indexed;
    tempTable->indexedColumn = table->indexedColumn;
    tempTable->indexingStrategy = table->indexingStrategy;
    tableCatalogue.insertTable(tempTable);
}

void copySortedTable(std::string sortedTableName, std::string tableName, bool isNewTable)
{
    logger.log("copySortedTable");
    Table *sortedTable = tableCatalogue.getTable(sortedTableName);
    if (isNewTable)
    {
        createTempTable(tableName, sortedTable);
    }

    for (int pageIdx = 0; pageIdx < sortedTable->blockCount; pageIdx++)
    {
        std::vector<std::vector<int>> rows;
        Cursor cursor(sortedTableName, pageIdx);

        for (int rowIdx = 0; rowIdx < sortedTable->rowsPerBlockCount[pageIdx]; rowIdx++)
        {
            rows.push_back(cursor.getNext());
        }

        bufferManager.writePage(tableName, pageIdx, rows, rows.size());
    }

    tableCatalogue.deleteTable(sortedTableName);
}

void sortTable(bool isNewTable, std::string newTableName)
{
    logger.log("sortTable");
    // Phase 1: sorting chunks of relation
    Table *table = tableCatalogue.getTable(parsedQuery.sortRelationName);

    // Maintain a vector containing the column indices in the order of their sorting priority
    for (int i = 0; i < parsedQuery.sortColumnNames.size(); i++)
    {
        int colIdx = table->getColumnIndex(parsedQuery.sortColumnNames[i]);
        parsedQuery.sortColumnIndices.push_back(colIdx);
    }

    std::string tempTableName = table->tableName + "_temp0";
    createTempTable(tempTableName, table);

    for (int pageIdx = 0; pageIdx < table->blockCount; pageIdx++)
    {
        std::vector<std::vector<int>> rows;
        Cursor cursor(table->tableName, pageIdx);

        for (int rowIdx = 0; rowIdx < table->rowsPerBlockCount[pageIdx]; rowIdx++)
        {
            rows.push_back(cursor.getNext());
        }

        sort(rows.begin(), rows.end(), rowComp);
        bufferManager.writePage(tempTableName, pageIdx, rows, rows.size());
    }

    // Phase 2: merging the sorted chunks of relation using 9-way merge
    const int K = 9; // K-way merge parameter
    int sortedPages = 1;
    int runNumber = 0;

    while (sortedPages < table->blockCount)
    {
        int filesCount = table->blockCount / sortedPages;
        if (table->blockCount % sortedPages)
            filesCount++;

        runNumber++;
        std::string tempTableNamePrev = table->tableName + "_temp" + to_string(runNumber - 1);
        std::string tempTableNameNext = table->tableName + "_temp" + to_string(runNumber);
        createTempTable(tempTableNameNext, table);
        int newPageIdx = 0;

        // Process files in groups of K
        for (int fileStart = 0; fileStart < filesCount; fileStart += K)
        {
            int filesToProcess = min(K, filesCount - fileStart);

            // Setup K-way merge
            std::vector<Cursor> cursors;
            std::vector<int> endPageIndices;
            std::vector<std::vector<int>> currentRows;
            std::vector<bool> isActive;

            // Initialize cursors and fetch first row from each file
            for (int i = 0; i < filesToProcess; i++)
            {
                int startPageIdx = (fileStart + i) * sortedPages;
                int endPageIdx = min(startPageIdx + sortedPages, (int)table->blockCount);

                cursors.push_back(Cursor(tempTableNamePrev, startPageIdx));
                endPageIndices.push_back(endPageIdx);

                std::vector<int> row = cursors[i].getNext();
                currentRows.push_back(row);
                isActive.push_back(!row.empty() && cursors[i].pageIndex < endPageIdx);
            }

            // Use priority queue for K-way merge
            std::priority_queue<std::pair<std::vector<int>, std::pair<int, int>>,
                                std::vector<std::pair<std::vector<int>, std::pair<int, int>>>,
                                RowComparator>
                pq;

            // Initialize priority queue with first row from each file
            for (int i = 0; i < filesToProcess; i++)
            {
                if (isActive[i])
                {
                    pq.push({currentRows[i], {i, cursors[i].pageIndex}});
                }
            }

            std::vector<std::vector<int>> mergedRows;

            // Perform K-way merge
            while (!pq.empty())
            {
                // Get the smallest row
                auto [minRow, fileInfo] = pq.top();
                pq.pop();

                int fileIdx = fileInfo.first;

                // Add the row to the merged results
                mergedRows.push_back(minRow);

                // Write to disk if buffer is full
                if (mergedRows.size() == table->maxRowsPerBlock)
                {
                    bufferManager.writePage(tempTableNameNext, newPageIdx, mergedRows, mergedRows.size());
                    newPageIdx++;
                    mergedRows.clear();
                }

                // Get the next row from the same file
                std::vector<int> nextRow = cursors[fileIdx].getNext();
                if (!nextRow.empty() && cursors[fileIdx].pageIndex < endPageIndices[fileIdx])
                {
                    pq.push({nextRow, {fileIdx, cursors[fileIdx].pageIndex}});
                }
            }

            // Write any remaining rows
            if (!mergedRows.empty())
            {
                bufferManager.writePage(tempTableNameNext, newPageIdx, mergedRows, mergedRows.size());
                newPageIdx++;
            }
        }

        tableCatalogue.deleteTable(tempTableNamePrev);
        sortedPages *= K;
    }

    // Copy the sorted temp table back to the original or new table
    tempTableName = table->tableName + "_temp" + to_string(runNumber);
    copySortedTable(tempTableName, newTableName, isNewTable);
}