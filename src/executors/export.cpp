#include "global.h"

/**
 * @brief 
 * SYNTAX: EXPORT <relation_name> 
 */

bool syntacticParseEXPORT()
{
    logger.log("syntacticParseEXPORT");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = EXPORT;
    parsedQuery.exportRelationName = tokenizedQuery[1];
    return true;
}

// bool syntacticParseMatrixEXPORT()
// {
//     logger.log("syntacticParseMatrixEXPORT");
//     if (tokenizedQuery.size() != 3)
//     {
//         cout << "SYNTAX ERROR" << endl;
//         return false;
//     }
//     parsedQuery.queryType = EXPORT_MATRIX;
//     parsedQuery.exportRelationName = tokenizedQuery[2];
//     return true;
// }

bool syntacticParseMatrixEXPORT()
{
    logger.log("syntacticParseMatrixEXPORT");
    // Debug prints
    cout << "Tokens: ";
    for(int i = 0; i < tokenizedQuery.size(); i++) {
        cout << tokenizedQuery[i] << " ";
    }
    cout << endl;
    
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = EXPORT_MATRIX;
    // parsedQuery.exportRelationName = tokenizedQuery[2];
    parsedQuery.exportRelationMatrixName = tokenizedQuery[2];
    
    // Debug print
    cout << "Export relation name set to: " << parsedQuery.exportRelationMatrixName << endl;
    
    return true;
}

bool semanticParseEXPORT()
{
    logger.log("semanticParseEXPORT");
    //Table should exist
    if (tableCatalogue.isTable(parsedQuery.exportRelationName))
        return true;
    cout << "SEMANTIC ERROR: No such relation exists" << endl;
    return false;
}


bool semanticParseMatrixEXPORT()
{
    logger.log("semanticParseMatrixEXPORT");
    //Table should exist
    if (matrixCatalogue.isMatrix(parsedQuery.exportRelationMatrixName))
        return true;
    cout << "SEMANTIC ERROR: No such Matrix exists" << endl;
    return false;
}

void executeEXPORT()
{
    logger.log("executeEXPORT");
    Table* table = tableCatalogue.getTable(parsedQuery.exportRelationName);
    table->makePermanent();
    return;
}


void executeMatrixEXPORT()
{
    blocksRead=0;
    blocksWritten=0;
    logger.log("executeMatrixEXPORT");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.exportRelationMatrixName);
    cout << "Matrix Name: " << matrix->matrixName << "\n";
    matrix->makePermanent();
    cout << "Number of blocks read: " << blocksRead << "\n";
    cout << "Number of blocks written: " << blocksWritten << "\n";
    cout << "Number of blocks accessed: " << blocksRead + blocksWritten << "\n";
    return;
}