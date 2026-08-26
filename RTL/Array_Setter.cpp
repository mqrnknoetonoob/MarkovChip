#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main() {
    string data[] = {
        "1FF", "8FF", "D7F", "47F", "EBF", "AFF", "3FF", "7BF", 
        "F7F", "23F", "3DF", "67F", "F5F", "5BF", "ADF", "B3F", 
        "CBF", "5FF", "65F", "CFF", "03F", "07F", "1DF", "7FF", 
        "93F", "97F", "EFF", "3BF", "8DF", "ABF", "B7F", "D5F"
    };

    // Pattern
    int pattern[] = {7, 8, 9, 6, 5, 10, 11, 4, 3, 12, 13, 2, 1, 14, 15, 0};

    vector<string> row_seeds, col_seeds;


    for (int i = 0; i < 32; ++i) {

        int offset = (i < 16) ? 0 : 16;
        int idx = pattern[i % 16] + offset;
        
        string val = "12'h" + data[idx];

        if (i % 2 == 0) {
            row_seeds.push_back(val);
        } else {
            col_seeds.push_back(val);
        }
    }

    
    cout << "localparam[11:0] ROW_SEED[15:0] = '{";
    for (int i = 0; i < 16; ++i) cout << row_seeds[i] << (i == 15 ? "" : ", ");
    cout << "};" << endl;

    cout << "localparam[11:0] COL_SEED[15:0] = '{";
    for (int i = 0; i < 16; ++i) cout << col_seeds[i] << (i == 15 ? "" : ", ");
    cout << "};" << endl;

    return 0;
}