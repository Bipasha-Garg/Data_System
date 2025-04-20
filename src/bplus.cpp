#include "bplus.h"

Node::Node() 
{
    records.clear();
    childNodes.clear();
    foSize = 0;
}

BPlusTree::BPlusTree(int fan_out) 
{
    root = NULL;
    foSize = fan_out;
}

Node* BPlusTree::getParent(Node *cur_par, Node *node) 
{
    if(cur_par->isLeaf ||  cur_par->childNodes[0]->isLeaf) {
        return NULL;
    }
    for(int i = 0; i < cur_par->childNodes.size(); i++) {
        if(cur_par->childNodes[i] == node) {
            return cur_par;
        } else {
            Node *par = getParent(cur_par->childNodes[i], node);
            if(par) {
                return par;
            }
        }
    }
    return NULL;
}

void BPlusTree::splitNode(pair<int, int> record, Node *cur, Node *child)
{
    int br = -1; 
    for(int i = 0; i < cur->records.size(); i++) {
        if(record.first < cur->records[i].first) {
            br = i;
            break;
        }
        if(i == cur->records.size() - 1) {
            br = i+1;
        }
    }
    vector<pair<int, int>> tmprecords;
    vector<Node *> tmpchilds;
    for(int i = 0; i < cur->records.size(); i++) {
        if(i == br) {
            tmprecords.push_back(record);
        }
        tmprecords.push_back(cur->records[i]);
    }
    if(br == cur->records.size()) {
        tmprecords.push_back(record);
    }
    br++;
    for(int i = 0; i < cur->childNodes.size(); i++) {
        if(i == br) {
            tmpchilds.push_back(child);
        }
        tmpchilds.push_back(cur->childNodes[i]);
    }
    if(br == cur->childNodes.size()) {
        tmpchilds.push_back(child);
    }

    if(cur->records.size() < foSize) {
        cur->records = tmprecords;
        cur->childNodes = tmpchilds;
        return;
    }

    Node *newIntvl = new Node();
    newIntvl->isLeaf = false;

    int ltrecsize = (foSize + 1)/2;
    int rtrecsize = foSize - ltrecsize;

    for(int i = ltrecsize + 1; i < foSize + 1; i++) {
        newIntvl->records.push_back(tmprecords[i]);
    }
    for(int i = ltrecsize + 1; i < foSize + 2; i++) {
        newIntvl->childNodes.push_back(tmpchilds[i]);
    }

    vector<pair<int, int>> tmpCurRecords;
    vector<Node *> tmpCurNodes;

    for(int i = 0; i <= ltrecsize; i++) {
        tmpCurRecords.push_back(tmprecords[i]);
        tmpCurNodes.push_back(tmpchilds[i]);
    }
    tmpCurNodes.push_back(tmpchilds[ltrecsize+1]);

    cur->records = tmpCurRecords;
    cur->childNodes = tmpCurNodes;

    if(cur == root) {
        Node *newRoot = new Node();
        newRoot->records.push_back(newIntvl->records[0]);
        newRoot->childNodes.push_back(cur);
        newRoot->childNodes.push_back(newIntvl);
        newRoot->isLeaf = false;
        root = newRoot;
        return;
    }

    splitNode(newIntvl->records[0], getParent(root, cur), newIntvl);
}

void BPlusTree::insert(pair<int, int> record)
{
    if (root == NULL) { 
		root = new Node(); 
		root->records.push_back(record); 
		root->isLeaf = true;  
        return;
	} 
    Node *cur = root;
    Node *parent;

    while(cur->isLeaf == false) {
        parent = cur;
        for(int i = 0; i < cur->records.size(); i++) {
            if(record.first < cur->records[i].first) {
                cur = cur->childNodes[i];
                break;
            }
            if(i == cur->records.size() - 1) {
                cur = cur->childNodes[i+1];
                break;
            }
        }
    }

    int br = -1; 
    for(int i = 0; i < cur->records.size(); i++) {
        if(record.first < cur->records[i].first) {
            br = i;
            break;
        }
        if(i == cur->records.size() - 1) {
            br = i+1;
        }
    }
    vector<pair<int, int>> tmprecords;
    for(int i = 0; i < cur->records.size(); i++) {
        if(i == br) {
            tmprecords.push_back(record);
        }
        tmprecords.push_back(cur->records[i]);
    }
    if(br == cur->records.size()) {
        tmprecords.push_back(record);
    }
    br++;
    if(cur->records.size() < foSize) {
        cur->records = tmprecords;
        return;
    }

    Node *newIntvl = new Node();
    newIntvl->isLeaf = true;

    int ltrecsize = (foSize + 1)/2;
    int rtrecsize = foSize - ltrecsize;

    for(int i = ltrecsize + 1; i < foSize + 1; i++) {
        newIntvl->records.push_back(tmprecords[i]);
    }

    vector<pair<int, int>> tmpCurRecords;

    for(int i = 0; i <= ltrecsize; i++) {
        tmpCurRecords.push_back(tmprecords[i]);
    }
    cur->records = tmpCurRecords;

    if(cur == root) {
        Node *newRoot = new Node();
        newRoot->records.push_back(newIntvl->records[0]);
        newRoot->childNodes.push_back(cur);
        newRoot->childNodes.push_back(newIntvl);
        newRoot->isLeaf = false;
        root = newRoot;
        return;
    }
    splitNode(newIntvl->records[0], parent, newIntvl);
}

pair<int, int> BPlusTree::getRecord(int key) 
{
    if(root == NULL) {
        return {-1, -1};
    }
    Node *cur = root;
    while(cur->isLeaf == false) {
        for(int i = 0; i < cur->records.size(); i++) {
            if(key < cur->records[i].first) {
                cur = cur->childNodes[i];
                break;
            }
            if(i == cur->records.size() - 1) {
                cur = cur->childNodes[i+1];
                break;
            }
        }
    }

    for(int i = 0; i < cur->records.size(); i++) {
        if(cur->records[i].first == key) {
            return cur->records[i];
        }
    }
    return {-1, -1};
}

void BPlusTree::display(Node *cur)
{
    cout << "Node : ";
    for(int i = 0; i < cur->records.size(); i++) {
        cout << cur->records[i].first << "," << cur->records[i].second << " ";
    }
    cout << "\n";
    if(cur->isLeaf == false) {
        for(int i = 0; i < cur->childNodes.size(); i++) {
            display(cur->childNodes[i]);
        }
    }
}

// Modified search method for BPlusTree class
vector<int> BPlusTree::search(int value, string op) {
    // Use a set to prevent duplicate page indices
    set<int> resultSet;
    vector<int> result;

    cout << "Searching for value: " << value << " with operator: " << op << endl;
    
    if (op == "=") {
        // Exact match search
        searchEqual(root, value, resultSet);
    } else if (op == "<") {
        // Less than search
        searchLessThan(root, value, false, resultSet);
    } else if (op == "<=") {
        // Less than or equal search
        searchLessThan(root, value, true, resultSet);
    } else if (op == ">") {
        // Greater than search
        searchGreaterThan(root, value, false, resultSet);
    } else if (op == ">=") {
        // Greater than or equal search
        searchGreaterThan(root, value, true, resultSet);
    } else if (op == "!=") {
        // Not equal search (combines < and >)
        searchLessThan(root, value, false, resultSet);
        searchGreaterThan(root, value, false, resultSet);
    }
    
    // Convert set to vector
    result.assign(resultSet.begin(), resultSet.end());
    return result;
}

// Modified search helper methods to use sets instead of vectors
void BPlusTree::searchEqual(Node* node, int value, set<int>& resultSet) {
    if (node == nullptr) return;
    
    if (node->isLeaf) {
        // Search in leaf node
        for (auto& record : node->records) {
            if (record.first == value) {
                resultSet.insert(record.second); // Use set to prevent duplicates
            }
        }
    } else {
        // Find appropriate child node to search
        int i = 0;
        while (i < node->records.size() && value >= node->records[i].first) {
            i++;
        }
        searchEqual(node->childNodes[i], value, resultSet);
    }
}

void BPlusTree::searchLessThan(Node* node, int value, bool inclusive, set<int>& resultSet) {
    if (node == nullptr) return;
    
    if (node->isLeaf) {
        // Search in leaf node
        for (auto& record : node->records) {
            if ((inclusive && record.first <= value) || (!inclusive && record.first < value)) {
                resultSet.insert(record.second);
            }
        }
    } else {
        // Search all relevant child nodes
        for (int i = 0; i <= node->records.size(); i++) {
            // If we've gone beyond the child that could contain our value, no need to search further
            if (i < node->records.size() && node->records[i].first >= value && !inclusive) {
                break;
            }
            if (i < node->records.size() && node->records[i].first > value && inclusive) {
                break;
            }
            searchLessThan(node->childNodes[i], value, inclusive, resultSet);
        }
    }
}

void BPlusTree::searchGreaterThan(Node* node, int value, bool inclusive, set<int>& resultSet) {
    if (node == nullptr) return;
    
    if (node->isLeaf) {
        // Search in leaf node
        for (auto& record : node->records) {
            if ((inclusive && record.first >= value) || (!inclusive && record.first > value)) {
                resultSet.insert(record.second);
            }
        }
    } else {
        // We need to be more careful with our search path for greater than queries
        // Start by finding the leftmost path that could contain values > our target
        int i = 0;
        // Find child that may contain our value or greater
        while (i < node->records.size() && node->records[i].first <= value) {
            i++;
        }
        
        // Search this child and all children to its right
        for (; i <= node->records.size(); i++) {
            searchGreaterThan(node->childNodes[i], value, inclusive, resultSet);
        }
        
        // For the child that contains values <= our target, we still need to check
        // since it might also contain values > our target
        if (i > 0 && inclusive) {
            Node* leftChild = node->childNodes[i-1];
            // Only search if this is for >= (inclusive) or if this key equals our value
            if (i-1 < node->records.size() && node->records[i-1].first == value) {
                searchGreaterThan(leftChild, value, inclusive, resultSet);
            }
        }
    }
}

void BPlusTree::searchNotEqual(Node* node, int value, set<int>& resultSet) {
    if (node == nullptr) return;
    
    if (node->isLeaf) {
        // Search in leaf node
        for (auto& record : node->records) {
            if (record.first != value) {
                resultSet.insert(record.second); // Use set to prevent duplicates
            }
        }
    } else {
        // Traverse all child nodes
        for (int i = 0; i < node->records.size(); i++) {
            // Search all child nodes, even if the current key is equal to the value
            if (node->records[i].first != value) {
                searchNotEqual(node->childNodes[i], value, resultSet);
            }
        }
        // Don't forget to search the rightmost child node
        if (node->records.back().first != value) {
            searchNotEqual(node->childNodes.back(), value, resultSet);
        }
    }
}


void BPlusTree::update(string col_name, int value, string condition, int condition_value) {
    std::set<int> resultSet;
    
    // Traverse the tree to search for records that meet the condition
    if (condition == "equal") {
        searchEqual(root, condition_value, resultSet);
    }
    else if (condition == "lessThan") {
        searchLessThan(root, condition_value, true, resultSet);
    }
    else if (condition == "greaterThan") {
        searchGreaterThan(root, condition_value, true, resultSet);
    }
    else if (condition == "notEqual") {
        searchNotEqual(root, condition_value, resultSet);
    }
    
    // If no records match, we exit the function (no update performed)
    if (resultSet.empty()) {
        std::cout << "No records match the condition." << std::endl;
        return;
    }
    
    // Otherwise, update the matching records
    for (int record_id : resultSet) {
        // Iterate through the tree and find the matching record
        Node *curNode = root;
        while (!curNode->isLeaf) {
            // Traverse to the correct child node based on the record_id
            // In this example, we're just assuming the `record_id` maps directly to the index
            // Adjust based on actual tree structure and search logic
            curNode = curNode->childNodes[record_id % curNode->foSize]; // Modify as needed
        }
        
        // Find and update the record
        for (auto &record : curNode->records) {
            if (record.first == record_id) {
                record.second = value;  // Update the record's value
                std::cout << "Record updated: ID = " << record_id << " Value = " << value << std::endl;
                break;
            }
        }
    }
}