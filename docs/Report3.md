# Project Phase 3 Report

## Implementation Details

### Indexing Structure
### Initalise the indexing with `INDEX ON a FROM A USING BTREE FANOUT 5`

For this phase of the project, we implemented a B+ tree as our indexing structure. Our implementation efficiently handles lookups, insertions, and deletions while maintaining the memory constraints of the project (maximum of 10 blocks worth of memory at any given time).

The core components of our B+ tree implementation include:
- `Node` class for tree nodes with vectors to store records and child nodes
- `BPlusTree` class that manages tree operations with a configurable fan-out size
- Specialized search methods for different comparison operators (`<`, `<=`, `>`, `>=`, `==`, `!=`)
- Methods for tree balancing, node splitting, and maintaining logarithmic height

Our implementation uses a pair of integers `<key, value>` to represent each record in the B+ tree, where the key is the indexed column value and the value is the pointer to the actual record in the table.


To initialize an index on a table, we use the following command:
```INDEX ON <column_name> FROM <Table_name> USING BTREE FANOUT <num>```
This command creates a B+ tree index on column <column_name> of table <Table_name> with a fan-out size of <num>, which controls the number of keys each node can hold and affects the overall tree structure and performance.

### Why B+ Tree?
We selected the B+ tree indexing structure for several reasons:

1. **Balanced Height**: Our implementation guarantees logarithmic search time regardless of data distribution. This is achieved through our node splitting algorithm that maintains balance during insertions.

2. **Range Queries**: The implementation excels at range queries through specialized search methods like `searchLessThan()` and `searchGreaterThan()` which are essential for operators like `<`, `<=`, `>`, and `>=`.

3. **Memory Efficiency**: The configurable fan-out size (`foSize`) allows us to optimize node capacity based on the block size, ensuring efficient use of the 10-block memory constraint.

4. **Stable Performance**: The code includes methods for handling node splitting, merging, and rebalancing, which maintain consistent performance regardless of insertion or deletion patterns.

5. **Versatility**: Our implementation handles all required binary operations through specialized search methods that navigate the tree structure efficiently.

### Command Implementations

#### SEARCH Command
Our SEARCH implementation leverages the specialized B+ tree search methods:

1. The query condition is parsed to extract the column name, operator, and value
2. Based on the operator, we call the appropriate search method:
   - `searchEqual()` for `==` operations
   - `searchLessThan()` for `<` and `<=` operations 
   - `searchGreaterThan()` for `>` and `>=` operations
   - Both `searchLessThan()` and `searchGreaterThan()` for `!=` operations

3. Search results are collected in a set (`resultSet`) to prevent duplicate entries, which is particularly important when handling range queries

4. The search methods traverse the tree efficiently:
   - For equality searches, we follow a single path down the tree
   - For range queries, we may need to examine multiple branches but avoid unnecessary traversals

5. After collecting the record pointers from the index, we retrieve the full records from disk to construct the result table

The implementation handles both point queries and range queries efficiently, avoiding full table scans for large datasets.

#### INSERT Command
Our INSERT operation maintains both the table and the index structure:

1. For each new record, we create a key-value pair where the key is the indexed column value and the value is the record's location

2. The `insert()` method finds the appropriate leaf node by traversing the tree from root to leaf, comparing the key at each step

3. If the leaf node has space (checked against `foSize`), the record is inserted directly

4. If the leaf node is full, our implementation:
   - Creates a new leaf node
   - Splits the records between the existing and new nodes
   - Promotes the smallest key from the new node to the parent
   - Recursively handles splits up the tree if needed
   - Creates a new root node if necessary

5. The `splitNode()` method handles the complex logic of reorganizing the tree during insertions, ensuring the tree remains balanced

The implementation maintains logarithmic insertion time even for large datasets by efficiently managing node splits and tree balancing.

#### UPDATE Command
The UPDATE operation modifies records while maintaining index consistency:

1. We use the `search()` function with the appropriate operator to find all records matching the condition

2. For each matching record ID in the result set:
   - The tree is traversed to locate the record
   - The record's value is updated in-place

3. Our `update()` method handles various conditions by leveraging the appropriate search methods:
   - Equality conditions use `searchEqual()`
   - Less than conditions use `searchLessThan()`
   - Greater than conditions use `searchGreaterThan()`
   - Not equal conditions use `searchNotEqual()` or a combination of other searches

4. If the update modifies an indexed column, the index is maintained by removing the old key and inserting the new key

By using the index for the initial search, the UPDATE operation avoids scanning the entire table, making it particularly efficient for large datasets.

#### DELETE Command
Our DELETE operation efficiently removes records using the index structure:

1. The `deleteRecords()` method first searches for records matching the condition using the appropriate search function based on the operator

2. For each matching record, the `deleteFromLeaf()` method:
   - Locates the record in the leaf node
   - Removes the record from the node
   - Handles potential underflow by calling `fixAfterDelete()`

3. The balancing operations maintain tree structure after deletion:
   - `borrowFromPrev()` and `borrowFromNext()` handle redistributing keys from sibling nodes
   - `mergeNodes()` combines nodes when redistribution isn't possible
   - `fixAfterDelete()` recursively ensures the tree remains balanced

4. For internal nodes, the `deleteFromInternal()` method handles key replacement and node merging as needed

5. Special handling is implemented for root node deletions to ensure the tree remains properly formed

The implementation ensures that deletion operations maintain logarithmic time complexity while preserving the B+ tree's properties.

## Assumptions
1. All column names and values provided in commands are valid.
2. Numeric data in indexed columns is handled as integers in our implementation.
3. The fan-out size (`foSize`) is optimized based on the block size to fit within the 10-block memory constraint.
4. For initial simplicity, our implementation currently indexes a single column per table.
5. The system maintains a mapping between table columns and their respective B+ tree indexes.
6. Search operations prioritize index usage over sequential scans whenever possible.
7. In node splitting operations, we use a balanced distribution of keys between the new nodes.
8. Delete operations maintain index consistency by properly handling underflow cases.
9. The B+ tree implementation handles equality and range queries differentially for optimal performance.
10. The 10-block memory constraint is managed by limiting the number of nodes loaded in memory during tree operations.

## Team Contribution

1. Tanish Gupta: Bplus Insert, Search, Update
2. Bipasha Garg: Bplus Delete
3. Shivam Singh: Hash Indexing Search, Update, Delete
