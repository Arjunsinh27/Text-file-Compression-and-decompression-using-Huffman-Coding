#include<bits/stdc++.h>
using namespace std;

struct Node {
    char ch;          // Character
    int freq;         // Frequency of the character
    Node* left;       // Left child
    Node* right;      // Right child

    // Constructor
    Node(char character, int frequency) : ch(character), freq(frequency), left(nullptr), right(nullptr) {}
};

// Comparison object for the priority queue
struct Compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;  // Min-heap: higher frequency nodes have lower priority
    }
};

// Function to create a Huffman Tree and return the root node
Node* createHuffmanTree(map<char, int>& frequencyMap) {
    // Priority queue to store live nodes of the Huffman tree
    priority_queue<Node*, vector<Node*>, Compare> pq;

    // Step 1: Create a leaf node for each character and add it to the priority queue
    for (auto pair : frequencyMap) {
        pq.push(new Node(pair.first, pair.second));
    }

    // Step 2: Build the tree until only one node remains in the queue
    while (pq.size() > 1) {
        // Extract the two nodes with the lowest frequency
        Node* left = pq.top();
        pq.pop();
        Node* right = pq.top();
        pq.pop();

        // Create a new internal node with a frequency equal to the sum of the two nodes
        Node* newNode = new Node('\0', left->freq + right->freq);  // '\0' for internal nodes
        newNode->left = left;
        newNode->right = right;

        // Add the new node to the priority queue
        pq.push(newNode);
    }

    // Step 3: The remaining node is the root of the Huffman Tree
    return pq.top();
}

// Function to store Huffman Codes in a map
void storeHuffmanCodes(Node* root, string code, map<char, string>& huffmanCodeMap) {
    if (!root) return;

    // If it's a leaf node, store the character and its code
    if (root->left == nullptr && root->right == nullptr && root->ch != '\0') {
        huffmanCodeMap[root->ch] = code;
    }

    // Traverse the left subtree with '0' appended to the code
    storeHuffmanCodes(root->left, code + "0", huffmanCodeMap);

    // Traverse the right subtree with '1' appended to the code
    storeHuffmanCodes(root->right, code + "1", huffmanCodeMap);
}

// Function to write the frequency table and compressed data into a file
void writeCompressedFile(const string& inputFilename, const map<char, string>& huffmanCodeMap, const map<char, int>& frequencyMap, const string& outputFilename) {
    // Open the input file to read the data
    ifstream inFile(inputFilename);
    if (!inFile) {
        cerr << "Error opening input file!" << endl;
        return;
    }

    // Step 1: Read the input file content
    string inputText;
    char ch;
    while (inFile.get(ch)) {  // Read character by character
        inputText += ch;  // Append each character to the inputText string
    }
    inFile.close();  // Close the input file

    // Open the output file in binary mode
    ofstream outFile(outputFilename, ios::binary);
    if (!outFile) {
        cerr << "Error opening output file!" << endl;
        return;
    }

    // Step 2: Write the frequency map to the file (for decoding)
    size_t mapSize = frequencyMap.size();
    outFile.write((char*)&mapSize, sizeof(mapSize));  // Write the size of the frequency map
    for (auto pair : frequencyMap) {
        outFile.write((char*)&pair.first, sizeof(pair.first));  // Write the character
        outFile.write((char*)&pair.second, sizeof(pair.second));  // Write the frequency
    }

    // Step 3: Write the compressed data (Huffman codes for the characters)
    string bitStream = "";
    for (char ch : inputText) {
        bitStream += huffmanCodeMap.at(ch);  // Append the Huffman code of the character
    }

    // Step 4: Calculate the number of padding bits
    int paddingBits = (8 - (bitStream.size() % 8)) % 8;  // Calculate how many bits are needed to pad to a multiple of 8
    bitStream.append(paddingBits, '0');  // Add the padding bits (0's) to make the bitstream divisible by 8

    // Step 5: Write the number of padding bits to the file
    outFile.write((char*)&paddingBits, sizeof(paddingBits));  // Write the number of padding bits

    // Step 6: Convert the bitstream into bytes and write to the file
    for (size_t i = 0; i < bitStream.size(); i += 8) {
        bitset<8> byte(bitStream.substr(i, 8));
        unsigned char byteChar = static_cast<unsigned char>(byte.to_ulong());
        outFile.write((char*)&byteChar, sizeof(byteChar));
    }

    outFile.close();  // Close the output file
}


string getDirectory(const string& filePath) {
    size_t found = filePath.find_last_of(".");  
    return filePath.substr(0, found);      
}

int main()
{
    //1.Opening the file

    string inputFilename;
    cout << "Please enter the file path: ";
    getline(cin, inputFilename);  // Use getline to capture the full path including spaces
    
    ifstream inputFile;//creating an input file stream object
    inputFile.open(inputFilename);
    if(!inputFile.is_open())
    {
        cout<<"Unable to read the file"<<endl;
        return 1;
    }
    //2.Calculating the frequency of each character
    map<char,int>frequencyMap;
    char ch;
    while(inputFile.get(ch))
    {
        frequencyMap[ch]++;
    }

    //3.creating min-heap
    Node * root = createHuffmanTree(frequencyMap);

    //4.Storing huffman codes
    map<char, string> huffmanCodeMap;
    storeHuffmanCodes(root, "", huffmanCodeMap);


    //5.Writing the data in compressed file
    string directoryPath = getDirectory(inputFilename);
    string outputFilename = directoryPath + "Compressed.bin";
    writeCompressedFile(inputFilename, huffmanCodeMap, frequencyMap, outputFilename);
    cout << "File compressed successfully"<< endl;
    return 0;
}