
#include"semanticParser.h"

void executeCommand();
bool isMatrix(string filename);

void executeCLEAR();
void executeCROSS();
void executeDISTINCT();
void executeEXPORT();
void executeINDEX();
void executeJOIN();
void executeLIST();
void executeLOAD();
void executePRINT();
void executePROJECTION();
void executeRENAME();
void executeSELECTION();
void executeSORT();
void executeSOURCE();
void executeMatrixLOAD();
void executeMatrixPRINT();
void executeMatrixEXPORT();
// void executeMatrixTRANSPOSE();
void executeMatrixCHECKSYMMETRY();
void executeMatrixCHECKANTISYMMETRY();
void executeROTATE();
void executeCROSSTRANSPOSE();
void executeInplaceSORT();

bool evaluateBinOp(int value1, int value2, BinaryOperator binaryOperator);
void printRowCount(int rowCount);
void sortTable(bool isNewTable, string newTableName);
bool rowComp(vector<int> &row1, vector<int> &row2);
void createTempTable(string tempTableName, Table* table);
void copySortedTable(string sortedTableName, string tableName, bool isNewTable);