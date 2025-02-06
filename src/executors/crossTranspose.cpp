#include "global.h"

bool syntacticParseCROSSTRANSPOSE() {
    logger.log("syntacticParseCROSSTRANSPOSE");
    if (tokenizedQuery.size() != 3) {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = CROSS_TRANSPOSE;
    parsedQuery.crossTransposeMatrix1Name = tokenizedQuery[1];
    parsedQuery.crossTransposeMatrix2Name = tokenizedQuery[2];
    return true;
}

/**
 * @brief Semantic checking for CROSSTRANSPOSE command
 */
bool semanticParseCROSSTRANSPOSE() {
    logger.log("semanticParseCROSSTRANSPOSE");
    if (!matrixCatalogue.isMatrix(parsedQuery.crossTransposeMatrix1Name) || 
        !matrixCatalogue.isMatrix(parsedQuery.crossTransposeMatrix2Name)) {
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }
    return true;
}

/**
 * @brief Execution logic for CROSSTRANSPOSE command
 */
void executeCROSSTRANSPOSE() {
    logger.log("executeCROSSTRANSPOSE");
    Matrix* matrix1 = matrixCatalogue.getMatrix(parsedQuery.crossTransposeMatrix1Name);
    Matrix* matrix2 = matrixCatalogue.getMatrix(parsedQuery.crossTransposeMatrix2Name);
    matrix1->crossTranspose(matrix2);
    return;
}