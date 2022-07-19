#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
using namespace std;

class Roter
{
public:
    vector<int> order;
    vector<int> orderOrig;
    Roter()
    {
        order.resize(26);
        orderOrig.resize(26);
        for (int i = 0; i < order.size(); i++)
        {
            order[i] = i;
        }
    }
    void shuffle()
    {
        for (int i = 0; i < order.size(); i++)
        {
            int j = i + rand() % (order.size() - i);
            swap(order[i], order[j]);
        }
        for (int i = 0; i < order.size(); i++)
        {
            orderOrig[i] = order[i];
        }
    }
    void reset()
    {
        for (int i = 0; i < order.size(); i++)
        {
            order[i] = orderOrig[i];
        }
    }
    int find(int index){
        for(int i = 0; i < order.size(); i++){
            if(order[i] == index)return i;
        }
        return -1;
    }
};

class RoterList
{
public:
    vector<Roter> roterarr;

    RoterList()
    {
    }
    void append(Roter r)
    {
        roterarr.push_back(r);
    }
    void print()
    {
        cout << "Roter list:" << endl;
        for (int i = 0; i < roterarr.size(); i++)
        {
            cout << "[ ";
            for (int j = 0; j < roterarr[i].orderOrig.size(); j++)
            {
                cout << roterarr[i].orderOrig[j] << " ";
            }
            cout << " ]" << endl;
        }
    }
    void reset()
    {
        for (int i = 0; i < roterarr.size(); i++)
        {
            roterarr[i].reset();
        }
    }
};

class Enigma
{
    RoterList rlist;
    string inputtext;
    string outputtext;
    unordered_map<char, char> reflector;
    unordered_map<char, char> plugboard;
public:
    Enigma(RoterList &rl)
    {
        rlist = rl;
    }
    void input(string &str)
    {
        inputtext = str;
    }
    void setReflector(string path)
    {
        vector<char> arr(26);
        for (int i = 0; i < arr.size(); i++)
        {
            arr[i] = 'A' + i;
        }
        for (int i = 0; i < arr.size(); i++)
        {
            int j = i + rand() % (arr.size() - i);
            swap(arr[i], arr[j]);
        }
        for (int i = 0; i < 13; i++)
        {
            reflector[arr[i]] = arr[i + 13];
            reflector[arr[i + 13]] = arr[i];
        }
        ofstream fout;
        fout.open(path, ios::out);
        for (int i = 0; i < 26; i++)
        {
            fout << char(i + 'A') << ": " << reflector[i + 'A'] << endl;
        }
        fout.close();
    }
    void setPlugboard(string path)
    {
        vector<char> arr(26);
        for (int i = 0; i < arr.size(); i++)
        {
            arr[i] = 'A' + i;
        }
        for (int i = 0; i < arr.size(); i++)
        {
            int j = i + rand() % (arr.size() - i);
            swap(arr[i], arr[j]);
        }
        for (int i = 0; i < 13; i++)
        {
            plugboard[arr[i]] = arr[i + 13];
            plugboard[arr[i + 13]] = arr[i];
        }
        ofstream fout;
        fout.open(path, ios::out);
        for (int i = 0; i < 26; i++)
        {
            fout << char(i + 'A') << ": " << reflector[i + 'A'] << endl;
        }
        fout.close();
    }
    void step(int sign, char &c)
    {
        if(sign == 1){
            int index = int(c - 'A');
            for (auto roter : rlist.roterarr)
            {
                index = roter.order[index];
            }
            c = char(index + 'A');
        }else{
            int index = int(c - 'A');
            for (int i = rlist.roterarr.size()-1; i >= 0 ; i--)
            {
                index = rlist.roterarr[i].find(index);
            }
            c = char(index + 'A');
        }
        
    }
    void encipherAndDecipher()
    {
        for (auto c : inputtext)
        {
            c = plugboard[c];
            step(1, c);
            c = reflector[c];
            step(-1, c);
            c = plugboard[c];
            outputtext.push_back(c);
            linkAndMoveRotors(0);
        }
    }
    void print_inputtext()
    {
        cout << "inputtext: " << inputtext << endl;
    }
    void print_outputtext()
    {
        cout << "outputtext: " << outputtext << endl;
    }
    void reset()
    {
        rlist.reset();
        inputtext.resize(0);
        outputtext.resize(0);
    }
    void move(Roter &r)
    {
        int first = r.order[0];
        for (int i = 1; i < 26; i++)
        {
            r.order[i - 1] = r.order[i];
        }
        r.order[25] = first;
    }
    void linkAndMoveRotors(int counter)
    {
        if (counter >= 3)
            return;
        move(rlist.roterarr[counter]);
        if (rlist.roterarr[counter].order[0] == rlist.roterarr[counter].orderOrig[0])
        {
            linkAndMoveRotors(counter + 1);
        }
    }
    string get_outputtext()
    {
        return outputtext;
    }
};

void getPlainText(string path, string &plaintext)
{
    ifstream fin;
    fin.open(path, ios::in);
    fin >> plaintext;
    for (char &c : plaintext)
    {
        if (c >= 'a' && c <= 'z')
        {
            c = c - 'a' + 'A';
        }
    }
    fin.close();
}

void saveCipherTextToFlie(string path, string &ciphertext)
{
    ofstream fout;
    fout.open(path, ios::out);
    fout << ciphertext;
    fout.close();
}

int main()
{
    Roter r1;
    r1.shuffle();
    Roter r2;
    r2.shuffle();
    Roter r3;
    r3.shuffle();
    RoterList rlist;
    rlist.append(r1);
    rlist.append(r2);
    rlist.append(r3);
    rlist.print();

    Enigma enigma(rlist);

    string plaintext;
    getPlainText("./plainText.txt", plaintext);
    //加密
    enigma.input(plaintext);
    enigma.print_inputtext();
    enigma.setReflector("./reflector.txt");
    enigma.setPlugboard("./plugboard.txt");
    enigma.encipherAndDecipher();
    enigma.print_outputtext();
    string ciphertext = enigma.get_outputtext();
    saveCipherTextToFlie("./cipherText.txt", ciphertext);
    
    //解密
    enigma.reset();
    enigma.input(ciphertext);
    enigma.print_inputtext();
    enigma.encipherAndDecipher();
    enigma.print_outputtext();
}