
#include "global.h"

bool syntacticParseROTATE() {
    logger.log("syntacticParseROTATE");
    if (tokenizedQuery.size() != 2) {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = ROTATE_MATRIX;
    parsedQuery.rotateMatrixName = tokenizedQuery[1];
    return true;
}

/**
 * @brief Semantic checking for ROTATE command
 */
bool semanticParseROTATE() {
    logger.log("semanticParseROTATE");
    if (!matrixCatalogue.isMatrix(parsedQuery.rotateMatrixName)) {
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }
    return true;
}

/**
 * @brief Execution logic for ROTATE command
 */
void executeROTATE() {
    logger.log("executeROTATE");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.rotateMatrixName);
    matrix->rotate();
    return;
}