# Spring 2025 | Data Systems
## Project Phase 1
## Team 38

## Implementation

### Common Implementation Patterns

Steps Involved:-
- Creating new macros for each new command to be executed.
- Each command has their own respective syntactic, semantic and executor parser in their cpp file of executors.
- In semantic and syntactic parser, we relate the macros and functions and also the allocation of variables and deallocation of them.
- Matrix class was created with loading, unloading, get, constructing and similar functions as table.

### 1. SOURCE Command

```cpp
bool syntacticParseSOURCE()
bool semanticParseSOURCE()
void executeSOURCE()
```

The SOURCE command is implemented using this approach:

1. **Syntactic Parsing**:
   - Checks if query has exactly 2 tokens (SOURCE + filename)
   - Sets query type to SOURCE
   - Stores filename in parsedQuery.sourceFileName

2. **Semantic Parsing**:
   - Validates if the query file exists using isQueryFile()
   - Returns false if file doesn't exist with appropriate error message

3. **Execution**:
   - Opens file from "../data/" directory with ".ra" extension
   - Uses regex to tokenize each command
   - Processes commands line by line
   - Handles QUIT command as termination condition
   - Executes each command through syntacticParse(), semanticParse(), and executeCommand()
   - Proper file handle cleanup with close()

### 2. LOAD MATRIX Command

```cpp
void executeMatrixLOAD()
void MatrixCatalogue::insertMatrix()
```

The implementation includes:

1. **Matrix Loading**:
   - Creates new Matrix object with given name
   - Loads data using matrix->load()
   - Reads the file line by line and blockifies it with the func to write it to diff blocks.
   - Tracks block read/write operations
   - Outputs statistics about loaded matrix

2. **Catalogue Management**:
   - Inserts loaded matrix into MatrixCatalogue
   - Maintains mapping of matrix names to Matrix objects
   - Handles memory management

### 3. PRINT MATRIX Command

```cpp
void executeMatrixPRINT()
void Matrix::print()
```

The implementation handles:

1. **Print Logic**:
   - Retrieves matrix from catalogue
   - Uses cursor-based row access
   - Implements row count limitation (min of PRINT_COUNT and rowCount)
   - Handles column limitation (max 20 columns)

2. **Block Access**:
   - Tracks blocks read/written
   - Uses cursor for efficient block navigation
   - Maintains count of accessed blocks

### 4. EXPORT MATRIX Command

```cpp
void executeMatrixEXPORT()
void Matrix::makePermanent()
```

The implementation covers:

1. **Export Process**:
   - Creates/updates CSV file in ../data/ directory
   - Uses map to assemble complete rows from blocks
   - Processes blocks row by row
   - Maintains original file handling

2. **Block Processing**:
   - Handles blocks in row-major order
   - Uses cursor for block access
   - Assembles complete rows before writing

The export operation reconstructs the matrix from blocks and writes it to a CSV file:

- **File Setup**
    - Creates output file path: "../data/<matrixName>.csv"
    - Opens file stream in write mode (ios::out)

- **Data Assembly Process**
    - Uses map<int, vector<int>> pagesInLine to reconstruct complete rows
    - Keys: row numbers
    - Values: vectors containing complete row data

- **Block Processing**
    - Triple nested loop structure:
        - Outer loop: processes each block row (i)
        - Middle loop: processes each block column (j)
        - Inner loop: processes rows within each block (k)
    - Uses cursor for block navigation
    - Assembles partial rows into complete rows using map

- **Write Operation**
    - Writes complete rows to file after each block row is processed
    - Clears map after writing to manage memory
    - Updates sourceFileName if not already set

### 5. ROTATE Operation

```cpp
void executeROTATE()
void Matrix::rotate()
```

The rotation implementation involves:

1. **Validation**
   - Checks if matrix is square (pagesCountInRow == pageCountInColumn)
   - Returns with error if not square

2. **Block Processing**
   - Cleans buffer manager before operation
   - Processes each block individually
   - Double loop structure for row and column blocks

3. **Rotation Logic**
   - Reads original block
   - Rotates block using rotateSubmatrix helper
   - Writes to new position using transformation:
     - New row = original column
     - New column = (n-1) - original row

4. **Block Management**
   - Tracks blocks read and written
   - Uses buffer manager for efficient page handling

### 6. CROSSTRANSPOSE Operation

```cpp
void executeCROSSTRANSPOSE()
void Matrix::crossTranspose()
void Matrix::transpose()
```

### crossTranspose Implementation

```cpp
    // Perform the three-way rename to swap matrices
    this->renameMatrix(originalMatrix1Name, tempName);
    matrix2->renameMatrix(originalMatrix2Name, originalMatrix1Name);
    this->renameMatrix(tempName, originalMatrix2Name);
    
    // Restore the original matrix names but keep the swapped dimensions
    this->matrixName = originalMatrix1Name;
    matrix2->matrixName = originalMatrix2Name;
```
1. **State Preservation**
   - Stores original matrix names
   - Stores original dimensions for both matrices

```cpp
    this->transpose();
    matrix2->transpose();
```
2. **Operation Flow**
   - Transposes both matrices individually
   - Uses three-way rename for matrix swap
   - Updates matrix names and dimensions

3. **Dimension Management**
   - Swaps page counts between matrices
   - Maintains correct dimensions after transpose

### transpose Implementation
1. **Block Processing**
   - Processes blocks in upper triangular order
   - Special handling for diagonal blocks
   - Paired block processing for non-diagonal elements

2. **Block Operations**
   - Reads blocks using buffer manager
   - Transposes individual submatrices
   - Writes transformed blocks back

3. **File Management**
   - Creates temporary files for block swapping
   - Uses rename operations for atomic swaps
   - Maintains block consistency throughout operation


### 7. CHECKANTISYM Operation

The anti-symmetry check determines if two matrices A and B satisfy the condition A = -1 * B^T. The implementation is divided into three main components: syntactic parsing, semantic parsing, and execution.

### Components
```cpp
bool syntacticParseMatrixCHECKANTISYMMETRY()
```

### Implementation
1. **Command Validation**
   - Verifies exactly 3 tokens in command (CHECKANTISYM matrix1 matrix2)
   - Sets query type to CHECKANTISYM
   - Stores matrix names in parsedQuery structure:
     - antisymmetryMatrixName1
     - antisymmetryMatrixName2

2. **Error Handling**
   - Returns false if token count is incorrect
   - Outputs "SYNTAX ERROR" message

```cpp
bool semanticParseMatrixCHECKANTISYMMETRY()
```
1. **Matrix Existence Check**
   - Verifies both matrices exist in matrixCatalogue
   - Uses matrixCatalogue.isMatrix() for validation

2. **Error Handling**
   - Returns false if either matrix doesn't exist
   - Outputs specific error message for missing matrices

```cpp
void executeMatrixCHECKANTISYMMETRY()
```
1. **Setup**
   - Retrieves both matrices from catalogue
   - Initializes block access counters
   - Calls checkAntiSymmetryWith() for comparison

2. **Result Handling**
   - Outputs "TRUE" or "FALSE" based on result
   - Reports block access statistics:
     - Blocks read
     - Blocks written
     - Total blocks accessed

```cpp
bool Matrix::checkAntiSymmetryWith(Matrix* other)
```

### Implementation Details
1. **Dimension Validation**
   - Checks compatibility:
     ```cpp
     if (this->rowCount != other->columnCount || 
         this->columnCount != other->rowCount)
     ```
   - Returns false if dimensions don't match

2. **Block Processing**
   - Iterates through all blocks using nested loops
   - Processes corresponding blocks from both matrices
   - Reads blocks using buffer manager:
     ```cpp
     submatrix1 = bufferManager.readPage(this->matrixName, pageRow, pageCol);
     submatrix2 = bufferManager.readPage(other->matrixName, pageCol, pageRow);
     ```

3. **Block Access**
   - Tracks block reads (increments by 2 for each comparison)
   - Uses buffer manager for efficient page handling

```cpp
bool Matrix::checkSubmatricesAntiSymmetry()
```

### Implementation Details
1. **Element-wise Comparison**
   - Compares corresponding elements:
     ```cpp
     if (submatrix1[row][col] != -submatrix2[col][row])
     ```
   - Returns false on first mismatch

2. **Optimization**
   - Early termination on finding mismatch
   - Processes blocks in memory efficiently



### Block Access Optimization
1. **Read Operations**
   - Minimizes block loads
   - Uses buffer manager efficiently
   - Tracks all block accesses

### Memory Management
1. **Buffer Usage**
   - Efficient block loading/unloading
   - Temporary storage for submatrices
   - Clean memory management

### Error Handling
1. **Validation Checks**
   - Matrix existence
   - Dimension compatibility
   - Element-wise comparison accuracy




### Memory Management
1. **Buffer Usage**
   - Clean buffer before operations
   - Efficient block loading/unloading
   - Temporary storage management

### Error Handling
1. **Input Validation**
   - Matrix dimension checks
   - File operation validation
   - Resource availability checks

### Performance Optimization
1. **Block Access**
   - Minimizes block loading
   - Efficient block traversal patterns
   - In-place operations where possible

2. **I/O Operations**
   - Tracks block reads/writes
   - Optimizes file operations
   - Uses buffer manager effectively

## Assumption Taken
- The first row doesnt consists of column name but the actual matrix values.

## Contribution
All of the teammates had equal weightage of work to do.
- Bipasha did the macros and relations along with constructors.
- Tanish made the matrix class and the corresponding functions, and the individual functions
- Shivam did the documentation and also handled the buffer management.
