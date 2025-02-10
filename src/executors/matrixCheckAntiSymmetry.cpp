#include "global.h"

/**
 * @brief 
 * SYNTAX: CHECKANTISYM <matrix name1> <matrix name2>
 */

bool syntacticParseMatrixCHECKANTISYMMETRY() {
    logger.log("syntacticParseMatrixCHECKANTISYMMETRY");
    if (tokenizedQuery.size() != 3) {
        cout << "SYNTAX ERROR\n";
        return false;
    }
    parsedQuery.queryType = CHECKANTISYM;
    parsedQuery.antisymmetryMatrixName1 = tokenizedQuery[1];
    parsedQuery.antisymmetryMatrixName2 = tokenizedQuery[2];
    return true;
}

bool semanticParseMatrixCHECKANTISYMMETRY() {
    logger.log("semanticParseMatrixCHECKANTISYMMETRY");
    if (!matrixCatalogue.isMatrix(parsedQuery.antisymmetryMatrixName1) || 
        !matrixCatalogue.isMatrix(parsedQuery.antisymmetryMatrixName2)) {
        cout << "SEMANTIC ERROR: One or both matrices don't exist\n";
        return false;
    }
    return true;
}

void executeMatrixCHECKANTISYMMETRY() {
    logger.log("executeMatrixCHECKANTISYMMETRY");
    Matrix *matrix1 = matrixCatalogue.getMatrix(parsedQuery.antisymmetryMatrixName1);
    Matrix *matrix2 = matrixCatalogue.getMatrix(parsedQuery.antisymmetryMatrixName2);
    blocksRead = 0;
    blocksWritten = 0;
    
    if (matrix1->checkAntiSymmetryWith(matrix2)) {
        cout << "TRUE\n";
    } else {
        cout << "FALSE\n";
    }
    
    cout << "\nNumber of blocks read: " << blocksRead << "\n";
    cout << "Number of blocks written: " << blocksWritten << "\n";
    cout << "Number of blocks accessed: " << blocksRead + blocksWritten << "\n";
}