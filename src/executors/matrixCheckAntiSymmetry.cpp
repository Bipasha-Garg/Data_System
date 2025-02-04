#include "global.h"

/**
 * @brief 
 * SYNTAX: CHECKANTISYM < matrix_name >
 */

bool syntacticParseMatrixCHECKANTISYMMETRY() {
    logger.log("syntacticParseMatrixCHECKANTISYMMETRY");
    if(tokenizedQuery.size() != 2) {
        cout << "SYNTAX ERROR\n";
        return false;
    }
    parsedQuery.queryType = CHECKANTISYM;
    parsedQuery.antisymmetryMatrixName = tokenizedQuery[1];
    return true;
}

bool semanticParseMatrixCHECKANTISYMMETRY() {
    logger.log("semanticParseMatrixCHECKANTISYMMETRY");
    if (!matrixCatalogue.isMatrix(parsedQuery.antisymmetryMatrixName))
    {
        cout << "SEMANTIC ERROR: Matrix doesn't exists\n";
        return false;
    }
    
    return true;
}

void executeMatrixCHECKANTISYMMETRY() {
    logger.log("executeMatrixCHECKANTISYMMETRY");
    Matrix *matrix = matrixCatalogue.getMatrix(parsedQuery.antisymmetryMatrixName);
    blocksRead = 0;
    blocksWritten = 0;
    if(matrix->checkAntiSymmetry()) {
        cout << "TRUE\n";
    } else {
        cout << "FALSE\n";
    }
    cout << "\nNumber of blocks read: " << blocksRead << "\n";
    cout << "Number of blocks written: " << blocksWritten << "\n";
    cout << "Number of blocks accessed: " << blocksRead + blocksWritten << "\n";
}

