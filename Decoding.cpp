#include<bits/stdc++.h>
using namespace std;

struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;
    Node(char character, int frequency) : ch(character), freq(frequency), left(nullptr), right(nullptr) {}
};

// Comparison object for priority queue (min-heap)
struct Compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

// Function to rebuild the Huffman Tree from the frequency map
Node* rebuildHuffmanTree(const map<char, int>& frequencyMap) {
    priority_queue<Node*, vector<Node*>, Compare> pq;
    
    // Insert all characters and their frequencies into the priority queue
    for (auto pair : frequencyMap) {
        pq.push(new Node(pair.first, pair.second));
    }

    // Combine nodes to form the Huffman Tree
    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        Node* newNode = new Node('\0', left->freq + right->freq);  // Internal node
        newNode->left = left;
        newNode->right = right;
        pq.push(newNode);
    }

    // The final node in the priority queue is the root of the Huffman Tree
    return pq.top();
}

// Function to decode the bitstream using the Huffman Tree
void decodeHuffman(ifstream& inFile, Node* root, size_t bitStreamLength, int paddingBits, ofstream& outFile) {
    Node* currentNode = root;
    char byte;
    size_t bytesProcessed = 0;

    for (size_t i = 0; i < bitStreamLength; ++i) {
        inFile.read(&byte, sizeof(char));  // Read one byte (8 bits)
        bitset<8> bits(byte);  // Convert the byte to bits
        ++bytesProcessed;

        // Traverse each bit in the bitset
        for (int j = 7; j >= 0; --j) {  // Start from MSB to LSB
            // Check if this is the last byte, and skip the padding bits
            if (bytesProcessed == bitStreamLength && j < paddingBits) {
                break;  // Skip the padding bits in the last byte
            }

            if (bits[j] == 0) {
                currentNode = currentNode->left;
            } else {
                currentNode = currentNode->right;
            }

            // If we reach a leaf node, output the character
            if (!currentNode->left && !currentNode->right) {
                outFile << currentNode->ch;  // Write the character to the output file
                currentNode = root;  // Reset to root for the next character
            }
        }
    }
}


// void printHuffmanCodes(Node* root, string code) {
//     if (!root) return;

//     // If it's a leaf node, print the character and its code
//     if (root->left == nullptr && root->right == nullptr && root->ch != '\0') {
//         cout << root->ch << " : " << code << endl;
//     }

//     // Traverse the left subtree with '0' appended to the code
//     printHuffmanCodes(root->left, code + "0");

//     // Traverse the right subtree with '1' appended to the code
//     printHuffmanCodes(root->right, code + "1");
// }

void decodeFile(const string& inputFilename, const string& outputFilename) {
    ifstream inFile(inputFilename, ios::binary);
    ofstream outFile(outputFilename);

    if (!inFile.is_open() || !outFile.is_open()) {
        cerr << "Error opening file!" << endl;
        return;
    }

    // Step 1: Read the frequency map from the binary file
    size_t mapSize;
    inFile.read((char*)&mapSize, sizeof(mapSize));  // Read the size of the frequency map

    map<char, int> frequencyMap;
    for (size_t i = 0; i < mapSize; ++i) {
        char ch;
        int freq;
        inFile.read((char*)&ch, sizeof(ch));  // Read the character
        inFile.read((char*)&freq, sizeof(freq));  // Read the frequency
        frequencyMap[ch] = freq;
    }

    // Step 2: Read the number of padding bits from the file
    int paddingBits;
    inFile.read((char*)&paddingBits, sizeof(paddingBits));  // Read the number of padding bits

    // Step 3: Rebuild the Huffman Tree from the frequency map
    Node* root = rebuildHuffmanTree(frequencyMap);

    // Step 4: Read the compressed bitstream and decode it
    inFile.seekg(0, ios::end);  // Move to the end to get the file size
    size_t fileSize = inFile.tellg();  // Get the total file size
    size_t bitStreamLength = fileSize - sizeof(mapSize) - (mapSize * (sizeof(char) + sizeof(int))) - sizeof(paddingBits);  // Calculate length of the bitstream
    inFile.seekg(sizeof(mapSize) + (mapSize * (sizeof(char) + sizeof(int))) + sizeof(paddingBits), ios::beg);  // Move to the start of bitstream

    decodeHuffman(inFile, root, bitStreamLength, paddingBits, outFile);  // Decode the bitstream with padding info

    // Step 5: Close the files
    inFile.close();
    outFile.close();
}

string getDirectory(const string& filePath) {
    size_t found = filePath.find_last_of("/\\");  // Find the last '/' or '\' character
    return filePath.substr(0, found + 1);         // Extract the directory path
}

int main()
{
    string inputFilename;
    cout << "Please enter the file path: ";
    getline(cin, inputFilename);  // Use getline to capture the full path including spaces
    string directoryPath = getDirectory(inputFilename);

    // Step 2: Create the output file path by appending "output.txt" to the directory path
    string outputFilename = directoryPath + "output.txt";


    // Decode the file and write the recovered data to the output text file
    decodeFile(inputFilename, outputFilename);

    cout << "Decoding complete! Check the output file: " << outputFilename << endl;
    return 0;
}