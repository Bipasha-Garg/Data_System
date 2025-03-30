// sortUtils.h
#ifndef SORT_UTILS_H
#define SORT_UTILS_H

#include "global.h"

// Common utility functions for sorting operations
bool rowComp(std::vector<int> &row1, std::vector<int> &row2);

// Structure for priority queue in k-way merge
struct RowComparator
{
    bool operator()(const std::pair<std::vector<int>, std::pair<int, int>> &a,
                    const std::pair<std::vector<int>, std::pair<int, int>> &b)
    {
        return !rowComp(const_cast<std::vector<int> &>(a.first), const_cast<std::vector<int> &>(b.first));
    }
};

void createTempTable(std::string tempTableName, Table *table);
void copySortedTable(std::string sortedTableName, std::string tableName, bool isNewTable);
void sortTable(bool isNewTable, std::string newTableName);

#endif // SORT_UTILS_H