#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>

using namespace std;

class Product {
public:
    int id;
    string name;
    double price;
    int quantity;

    Product(int _id = 0, string _name = "", double _price = 0, int _quantity = 0)
        : id(_id), name(_name), price(_price), quantity(_quantity) {}
};

class Sale {
public:
    string username, mobile, email, timeStamp;
    int productID, quantity;
    double totalAmount;

    Sale(string _username, string _mobile, string _email, int _productID, int _quantity, double _totalAmount)
        : username(_username), mobile(_mobile), email(_email),
          productID(_productID), quantity(_quantity), totalAmount(_totalAmount) {
        time_t now = time(0);
        timeStamp = ctime(&now);
        if (!timeStamp.empty() && timeStamp.back() == '\n')
            timeStamp.pop_back();
    }

    void recordSale() {
        const string filename = "sales.txt";

        
        bool fileExists = false;
        ifstream fin(filename);
        if (fin.good()) fileExists = true;

        
        int sNo = 0;
        string line;
        while (getline(fin, line)) {
            if (line.find('|') != string::npos && 
                line.find("SNo") == string::npos && 
                line.find('+') == string::npos)
                sNo++;
        }
        fin.close();

        ofstream fout(filename, ios::app);
        if (!fout.is_open()) {
            cerr << "Error: Unable to open " << filename << endl;
            return;
        }

        
        if (!fileExists || sNo == 0) {
            fout << "+-----+-----------------+---------------+---------------------------+-----------+-----------+---------------+---------------------------+\n";
            fout << "| SNo | Customer Name   | Mobile        | Email                     | Prod ID   | Quantity  | Total (Rs)    | Date & Time               |\n";
            fout << "+-----+-----------------+---------------+---------------------------+-----------+-----------+---------------+---------------------------+\n";
        }

        fout << "| " << setw(3) << left << sNo + 1
            << " | " << setw(15) << left << username.substr(0, 15)
            << " | " << setw(13) << left << mobile.substr(0, 13)
            << " | " << setw(25) << left << email.substr(0, 25)
            << " | " << setw(9) << right << productID
            << " | " << setw(9) << right << quantity
            << " | " << setw(13) << right << fixed << setprecision(2) << totalAmount
            << " | " << setw(25) << left << timeStamp << " |\n";
        fout << "+-----+-----------------+---------------+---------------------------+-----------+-----------+---------------+---------------------------+\n";

        fout.close();
    }

};

class Inventory {
private:
    vector<Product> products;

    string padRight(string text, int width) const {
        if ((int)text.size() < width) text.append(width - text.size(), ' ');
        else if ((int)text.size() > width) text = text.substr(0, width);
        return text;
    }

public:
    void loadFromFile(const string &filename) {
        ifstream fin(filename);
        if (!fin.is_open()) {
            cerr << "Error: Unable to open " << filename << endl;
            return;
        }

        products.clear();
        string line;
        
        while (getline(fin, line)) {
            if (line.empty() || line.find('+') != string::npos || line.find("ID") != string::npos)
                continue;

            vector<string> cols;
            stringstream ss(line);
            string col;

            while (getline(ss, col, '|')) {
                size_t start = col.find_first_not_of(" \t");
                size_t end = col.find_last_not_of(" \t");
                if (start != string::npos && end != string::npos)
                    col = col.substr(start, end - start + 1);
                else
                    col = "";
                if (!col.empty())
                    cols.push_back(col);
            }

            if (cols.size() == 4) {
                try {
                    int id = stoi(cols[0]);
                    string name = cols[1];
                    double price = stod(cols[2]);
                    int quantity = stoi(cols[3]);
                    products.emplace_back(id, name, price, quantity);
                } catch (...) {
                    continue;
                }
            }
        }
        fin.close();
    }

    void displayAll() const {
        cout << "+-----+--------------------------+-----------+-----------+\n";
        cout << "| ID  | Product Name             | Price(Rs) | Quantity  |\n";
        cout << "+-----+--------------------------+-----------+-----------+\n";
        for (const auto &p : products) {
            cout << "| " << setw(3) << left << p.id
                 << " | " << setw(24) << left << p.name
                 << " | " << setw(9) << left << (int)p.price
                 << " | " << setw(9) << left << p.quantity << " |\n";
            cout << "+-----+--------------------------+-----------+-----------+\n";
        }
    }

    Product* findProductById(int id) {
        for (auto &p : products)
            if (p.id == id)
                return &p;
        return nullptr;
    }

    void saveToFile(const string &filename) {
        ofstream fout(filename);
        if (!fout.is_open()) {
            cerr << "Error: Unable to write to " << filename << endl;
            return;
        }

        fout << "+-----+--------------------------+-----------+-----------+\n";
        fout << "| ID  | Product Name             | Price(Rs) | Quantity  |\n";
        fout << "+-----+--------------------------+-----------+-----------+\n";

        for (const auto &p : products) {
            fout << "| " << setw(3) << left << p.id
                 << " | " << setw(24) << left << p.name
                 << " | " << setw(9) << left << (int)p.price
                 << " | " << setw(9) << left << p.quantity << " |\n";
            fout << "+-----+--------------------------+-----------+-----------+\n";
        }

        fout.close();
    }
};

int main() {
    Inventory inventory;
    inventory.loadFromFile("inventory.txt");
    inventory.displayAll();

    string username, mobile, email;
    int productId, qty;

    cout << "\nEnter Username: ";
    getline(cin, username);
    cout << "Enter Mobile Number: ";
    getline(cin, mobile);
    cout << "Enter Email: ";
    getline(cin, email);

    cout << "Enter Product ID: ";
    cin >> productId;
    cout << "Enter Product Quantity: ";
    cin >> qty;

    Product *p = inventory.findProductById(productId);

    if (!p) {
        cout << "Product not found!\n";
        return 0;
    }

    if (qty <= p->quantity) {
        double bill = qty * p->price;
        cout << "\n----------------------------\n";
        cout << "Product Name     : " << p->name << endl;
        cout << "Product Price    : " << p->price << endl;
        cout << "Product Quantity : " << qty << endl;
        cout << "----------------------------\n";
        cout << "Bill Amount      : Rs " << bill << endl;
        cout << "----------------------------\n";

        Sale sale(username, mobile, email, p->id, qty, bill);
        sale.recordSale();

        p->quantity -= qty;
    } else {
        cout << "Sorry, we only have " << p->quantity << " in stock.\n";
        cout << "Would you like to purchase the available quantity? (Y/y): ";
        char choice;
        cin >> choice;
        if (tolower(choice) == 'y') {
            double bill = p->quantity * p->price;
            cout << "\n----------------------------\n";
            cout << "Product Name     : " << p->name << endl;
            cout << "Product Price    : " << p->price << endl;
            cout << "Product Quantity : " << p->quantity << endl;
            cout << "----------------------------\n";
            cout << "Bill Amount      : Rs " << bill << endl;
            cout << "----------------------------\n";

            Sale sale(username, mobile, email, p->id, p->quantity, bill);
            sale.recordSale();

            p->quantity = 0;
        } else {
            cout << "Purchase Cancelled.\n";
        }
    }

    inventory.saveToFile("inventory.txt");
    cout << "\nInventory updated successfully!\n";
    return 0;
}
