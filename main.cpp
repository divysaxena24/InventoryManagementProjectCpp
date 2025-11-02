#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>

using namespace std;


class Product {
    public:
        int id;
        string name;
        double price;
        int quantity;

        Product(int _id = 0, string _name = "", double _price = 0, int _quantity = 0)
            : id(_id), name(_name), price(_price), quantity(_quantity) {}

        string toCSV() const {
            stringstream ss;
            ss << id << "," << name << "," << price << "," << quantity;
            return ss.str();
        }

        void displayRow() const {
            cout << "| " << left << setw(3) << id
                << " | " << setw(20) << name
                << " | " << setw(20) << fixed << setprecision(2) << price
                << " | " << setw(20) << quantity << " |\n";
            cout << "+-----+----------------------+----------------------+----------------------+\n";
        }
};




class Sale {
    public:
        string username;
        string mobile;
        string email;
        int productID;
        int quantity;
        double totalAmount;
        string timeStamp;

        Sale(string _username, string _mobile, string _email, int _productID, int _quantity, double _totalAmount)
            : username(_username), mobile(_mobile), email(_email),
            productID(_productID), quantity(_quantity), totalAmount(_totalAmount) {

            time_t now = time(0);
            timeStamp = ctime(&now);
            if (!timeStamp.empty() && timeStamp.back() == '\n')
                timeStamp.pop_back();
        }

        void recordSale() {
            fstream fout("sales.txt", ios::in | ios::out | ios::app);

            if (!fout.is_open()) {
                cerr << "Error: Unable to write to sales.txt\n";
                return;
            }

            fout.seekg(0, ios::end);
            bool isEmpty = fout.tellg() == 0;
            fout.seekp(0, ios::end);

            int sNo = 1;
            if (!isEmpty) {
                ifstream fin("sales.txt");
                string line;
                while (getline(fin, line)) {
                    if (line.rfind("| ", 0) == 0 && line.find("SNo") == string::npos) {
                        sNo++;
                    }
                }
                fin.close();
            }

            if (isEmpty) {
                fout << "+-----+-----------------+---------------+---------------------------+-----------+-----------+---------------+---------------------------+\n";
                fout << "| SNo | Customer Name   | Mobile        | Email                     | Prod ID   | Quantity  | Total (Rs)    | Date & Time               |\n";
                fout << "+-----+-----------------+---------------+---------------------------+-----------+-----------+---------------+---------------------------+\n";
            }

            fout << "| " << setw(3) << left << sNo
                << " | " << setw(15) << left << username
                << " | " << setw(13) << left << mobile
                << " | " << setw(25) << left << email
                << " | " << setw(9) << left << productID
                << " | " << setw(9) << left << quantity
                << " | " << setw(13) << fixed << setprecision(2) << left << totalAmount
                << " | " << setw(25) << left << timeStamp << " |\n";

            fout << "+-----+-----------------+---------------+---------------------------+-----------+-----------+---------------+---------------------------+\n";

            fout.close();
        }
};




class Inventory {
    private:
        vector<Product> products;

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
                if (line.empty()) continue;

                stringstream ss(line);
                string idStr, name, priceStr, qtyStr;
                getline(ss, idStr, ',');
                getline(ss, name, ',');
                getline(ss, priceStr, ',');
                getline(ss, qtyStr, ',');

                try {
                    if (idStr.empty() || priceStr.empty() || qtyStr.empty())
                        throw invalid_argument("Missing data");

                    int id = stoi(idStr);
                    double price = stod(priceStr);
                    int qty = stoi(qtyStr);

                    products.emplace_back(id, name, price, qty);
                } catch (const exception &e) {
                    cerr << " Skipping invalid line in " << filename << ": " << line << endl;
                }
            }

            fin.close();
        } 

        
        void displayAll() const {
            cout << "+------+---------------------------+------------------+------------+\n";
            cout << "| ID   | Product Name              | Price (Rs)       | Quantity   |\n";
            cout << "+------+---------------------------+------------------+------------+\n";

            for (const auto &p : products)
                p.displayRow();

            cout << "\n";
        }


        Product* findProductById(int id) {
            for (auto &p : products) {
                if (p.id == id)
                    return &p;
            }
            return nullptr;
        }

        void saveToFile(const string &filename) {
            ofstream fout(filename);
            if (!fout.is_open()) {
                cerr << "Error: Unable to write to " << filename << endl;
                return;
            }

            for (size_t i = 0; i < products.size(); ++i) {
                fout << products[i].toCSV();
                if (i != products.size() - 1)
                    fout << "\n";
            }
            fout.close();
        }
};




int main() {
    Inventory inventory;
    inventory.loadFromFile("Inventory.txt");
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
        cout << "Bill Amount      : Rs" << bill << endl;
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
            cout << "Bill Amount      : Rs" << bill << endl;
            cout << "----------------------------\n";

            Sale sale(username, mobile, email, p->id, p->quantity, bill);
            sale.recordSale();

            p->quantity = 0;
        } else {
            cout << "Purchase Cancelled.\n";
        }
    }

    inventory.saveToFile("Inventory.txt");

    cout << "\nInventory updated successfully!\n";

    return 0;
}