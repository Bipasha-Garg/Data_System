#include "hash.h"

class Node
{
    public:
    vector< pair<int, int> > records;
    vector<Node *> childNodes;
    bool isLeaf;
    int foSize;
    Node();
};

class BPlusTree
{
    public:
    int foSize;
    Node *root;
    BPlusTree(int fan_out);
    Node *getParent(Node *cur_par, Node *node);
    void insert(pair<int, int> record);
    pair<int, int> getRecord(int key);
    void splitNode(pair<int, int> record, Node *cur, Node *child);
    void display(Node *cur);
    vector<int> search(int value, string op);
    void searchEqual(Node* node, int value, set<int>& resultSet);
    void searchLessThan(Node* node, int value, bool inclusive, set<int>& resultSet);
    void searchGreaterThan(Node* node, int value, bool inclusive, set<int>& resultSet);
    void searchNotEqual(Node* node, int value, set<int>& resultSet);
    void update(string col_name, int value, string condition, int condition_value);
    void deleteRecords(string condition, string op, int value);
    void deleteFromLeaf(Node *node, int key);
    void deleteFromInternal(Node *node, int key);
    void borrowFromPrev(Node *parent, int idx);
    void borrowFromNext(Node *parent, int idx);
    void mergeNodes(Node *parent, int idx);
    void fixAfterDelete(Node *node);
    int findKeyIndex(Node *node, int key);
    Node *findPredecessor(Node *node, int idx);
    Node *findSuccessor(Node *node, int idx);
};