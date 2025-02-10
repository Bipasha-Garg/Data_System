// #include "cursor.h"

// /**
//  * @brief The Matrix class holds all information related to a loaded matrix. It
//  * also implements methods that interact with the parsers, executors, and the
//  * buffer manager. Matrices are created through the LOAD MATRIX command and
//  * can be used for various matrix operations.
//  *
//  */
// class Matrix 
// {
// public:
//     string sourceFileName = "";
//     string matrixName = "";
//     vector<vector<int>> elements;
//     uint dimension = 0;          // n for an n x n matrix
//     uint blockCount = 0;
//     uint maxElementsPerBlock = 0;
//     uint elementsPerBlockRow = 0;  // sqrt of maxElementsPerBlock for square blocking

//     Matrix();
//     Matrix(string matrixName);
    
//     /**
//      * @brief Loads matrix from CSV file and performs blocking
//      *
//      * @return true if matrix was successfully loaded
//      * @return false if an error occurred
//      */
//     bool load();

//     /**
//      * @brief Writes the matrix to disk as blocks
//      *
//      * @return true if matrix was successfully written
//      * @return false if an error occurred
//      */
//     bool blockify();

//     /**
//      * @brief Prints the first PRINT_COUNT x PRINT_COUNT elements
//      * of the matrix (or full matrix if smaller)
//      *
//      */
//     void print();

//     /**
//      * @brief If matrix is not already on disk, writes matrix
//      * to disk
//      *
//      */
//     void makePermanent();

//     /**
//      * @brief Checks if matrix file exists on disk
//      *
//      * @return true if matrix is on disk
//      * @return false otherwise
//      */
//     bool isPermanent();

//     /**
//      * @brief The unload function removes the matrix from memory
//      * along with deleting its temporary files
//      *
//      */
//     void unload();

//     /**
//      * @brief Static function that takes a vector of values and prints them out in a
//      * comma separated format.
//      *
//      * @tparam T current usages include int
//      * @param row 
//      */
//     template <typename T>
//     void writeRow(vector<T> row, ostream &fout)
//     {
//         logger.log("Matrix::writeRow");
//         for (int columnCounter = 0; columnCounter < row.size(); columnCounter++)
//         {
//             if (columnCounter != 0)
//                 fout << ",";
//             fout << row[columnCounter];
//         }
//         fout << endl;
//     }

//     /**
//      * @brief Static function that takes a vector of values and prints them out in a
//      * comma separated format to the source file.
//      *
//      * @tparam T current usages include int
//      * @param row
//      */
//     template <typename T>
//     void writeRow(vector<T> row)
//     {
//         logger.log("Matrix::writeRow");
//         ofstream fout(this->sourceFileName, ios::app);
//         this->writeRow(row, fout);
//         fout.close();
//     }
// };