#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <ctime>
using namespace std;

int generateMemberID() {
    static int idCounter = 1000;
    return idCounter++;
}

// Abstract class enforcing common methods
class LibraryEntity {
public:
    virtual void addEntity() = 0;
    virtual void displayDetails() = 0;
};

// Book Class
class Book : public LibraryEntity {
public:
    string title, author, isbn;
    int year, stock;

    Book(string t, string a, string i, int y, int s)
        : title(t), author(a), isbn(i), year(y), stock(s) {}

    void addEntity() override {
        ofstream file("books.csv", ios::app);
        file << title << "," << author << "," << isbn << "," << year << "," << stock << "\n";
        file.close();
    }

    void displayDetails() override {
        cout << "Title: " << title << " | Author: " << author << " | ISBN: " << isbn
             << " | Year: " << year << " | Stock: " << stock << endl;
    }
};

// Base Person Class
class Person {
public:
    string name, id, contact;
    Person(string n, string i, string c) : name(n), id(i), contact(c) {}
};

// Member Class
class Member : public Person, public LibraryEntity {
public:
    string password;
    vector<string> borrowedBooks;

    Member(string n, string c, string p) : Person(n, to_string(generateMemberID()), c), password(p) {}

    void addEntity() override {
        ofstream file("members.csv", ios::app);
        file << name << "," << id << "," << contact << "," << password << "\n";
        file.close();
    }

    void displayDetails() override {
        cout << "Member: " << name << " | ID: " << id << " | Contact: " << contact << endl;
    }

    static bool authenticate(string memberID, string password) {
        ifstream file("members.csv");
        string line, n, i, c, p;
        while (getline(file, line)) {
            stringstream ss(line);
            getline(ss, n, ',');
            getline(ss, i, ',');
            getline(ss, c, ',');
            getline(ss, p, ',');
            if (i == memberID && p == password) {
                file.close();
                return true;
            }
        }
        file.close();
        return false;
    }
};

// Librarian Class
class Librarian : public Person {
public:
    string password;
    Librarian(string n, string i, string c, string p) : Person(n, i, c), password(p) {}

    void addBook(Book book) { book.addEntity(); }
    void addMember(Member member) { member.addEntity(); }

    static bool authenticate(string librarianID, string password) {
        ifstream file("librarians.txt");
        string line, n, i, c, p;
        while (getline(file, line)) {
            stringstream ss(line);
            getline(ss, n, ',');
            getline(ss, i, ',');
            getline(ss, c, ',');
            getline(ss, p, ',');
            if (i == librarianID && p == password) {
                file.close();
                return true;
            }
        }
        file.close();
        return false;
    }
};

// Function for librarian actions
void librarianActions() {
    int choice;
    while (true) {
        cout << "\nLibrarian Menu:\n1. Add Book\n2. Add Member\n3. Logout\nEnter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1: {
            string title, author, isbn;
            int year, stock;
            cout << "Enter Book Title: "; getline(cin, title);
            cout << "Enter Author: "; getline(cin, author);
            cout << "Enter ISBN: "; getline(cin, isbn);
            cout << "Enter Year: "; cin >> year;
            cout << "Enter Stock: "; cin >> stock;
            cin.ignore();
            Book newBook(title, author, isbn, year, stock);
            newBook.addEntity();
            cout << "Book added successfully!\n";
            break;
        }
        case 2: {
            string name, contact, password;
            cout << "Enter Member Name: "; getline(cin, name);
            cout << "Enter Contact: "; getline(cin, contact);
            cout << "Enter Password: "; getline(cin, password);
            Member newMember(name, contact, password);
            newMember.addEntity();
            cout << "Member added successfully! Library ID: " << newMember.id << "\n";
            break;
        }
        case 3:
            cout << "Logging out...\n";
            return;
        default:
            cout << "Invalid choice! Try again.\n";
        }
    }
}

// Sign In Function
void signIn() {
    while (true) {
        int choice;
        cout << "Sign in as:\n1. Member\n2. Librarian\nEnter choice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            int registered;
            cout << "Are you registered in the library? (1 for Yes, 0 for No): ";
            cin >> registered;
            cin.ignore();

            if (registered) {
                string memberID, password;
                int attempts = 0;
                cout << "Enter Library ID Number: ";
                cin >> memberID;

                while (attempts < 3) {
                    cout << "Enter Password: ";
                    cin >> password;

                    if (Member::authenticate(memberID, password)) {
                        cout << "Login successful!\n";
                        return;
                    } else {
                        attempts++;
                        cout << "Invalid password. Attempts remaining: " << (3 - attempts) << "\n";
                    }
                }
                cout << "Too many failed attempts. Please contact the administrator.\n";
            } else {
                cout << "Please sign up.\n";
                string name, contact, newPassword;
                cout << "Enter Name: "; cin.ignore(); getline(cin, name);
                cout << "Enter Contact: "; getline(cin, contact);
                cout << "Enter New Password: "; getline(cin, newPassword);
                Member newMember(name, contact, newPassword);
                newMember.addEntity();
                cout << "Sign-up successful! Your Library ID Number is: " << newMember.id << endl;
            }
        } else if (choice == 2) {
            string librarianID, password;
            cout << "Enter Librarian ID: ";
            cin >> librarianID;
            cout << "Enter Password: ";
            cin >> password;

            if (Librarian::authenticate(librarianID, password)) {
                cout << "Librarian login successful!\n";
                librarianActions();
            } else {
                cout << "Invalid credentials!\n";
            }
        }
    }
}

const string file_name = "transaction.csv"; // Corrected file name

class TransactionLogs : public LibraryEntity {
public:
    string action;
    void menu(); // display function for the menu

    void logTransaction(const string& id, const string& ISBN, const string& action);
    void displayTransactions();
    void findTransactions(const string& id);
};

string get_time() {
    time_t now = time(0);
    tm* localTime = localtime(&now);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H-%M", localTime);
    return string(buffer);
}

void TransactionLogs::logTransaction(const string& id, const string& ISBN, const string& action) {
    ofstream file;
    file.open(file_name, ios::app);
    if (!file) {
        cerr << "File cannot open!" << endl;
        return;
    }
    string current_time = get_time();
    string status = (action == "borrow") ? "borrowed" : "Returned";

    if (file.tellp() == 0) {
        file << "Transaction ID, Borrower ID, Book ID, Date, Status\n";
    }
    long long transactionID = static_cast<long long>(time(0));
    file << transactionID << ", " << id << ", " << ISBN << ", " << current_time << ", " << status << endl;
    file.close();
    cout << "Transaction successfully made: " << status << " - " << id << " --> " << ISBN << endl;
}

void TransactionLogs::displayTransactions() {
    ifstream file(file_name);
    if (!file) {
        cerr << "No transactions found!" << endl;
        return;
    }
    string line;
    while (getline(file, line)) {
        cout << line << endl;
    }
    file.close();
}

void TransactionLogs::findTransactions(const string& id) {
    ifstream file(file_name);
    if (!file) {
        cerr << "No transactions found" << endl;
        return;
    }
    string line;
    bool found = false;

    cout << "Transactions for borrower ID: " << id << endl;
    while (getline(file, line)) {
        if (line.find(id) != string::npos) {
            cout << line << endl;
            found = true;
        }
    }
    if (!found) {
        cout << "No transactions found for borrower ID: " << id << endl;
    }
    file.close();
}

void transactionLogMenu() {
    TransactionLogs logs;
    int choice;
    while (true) {
        cout << "Transaction Log Menu: " << endl;
        cout << "1. View all transactions " << endl;
        cout << "2. Find Transactions by borrower ID " << endl;
        cout << "3. Log a new Transaction " << endl;
        cout << "4. Back to main menu " << endl;
        cout << "Enter your choice: " << endl;
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            logs.displayTransactions();
            break;
        case 2: {
            string borrowerID;
            cout << "Enter Borrower ID: " << endl;
            getline(cin, borrowerID);
            logs.findTransactions(borrowerID);
            break;
        }
        case 3: {
            string id, ISBN, action;
            cout << "Enter Borrower's ID:" << endl;
            getline(cin, id);
            cout << "Enter book ISBN: " << endl;
            getline(cin, ISBN);
            cout << "Enter action (borrow/return):" << endl;
            getline(cin, action);
            logs.logTransaction(id, ISBN, action);
            break;
        }
        case 4:
            cout << "Returning to main menu...\n";
            return;
        default:
            cout << "Invalid choice! Please try again. \n";
        }
    }
}

int main() {
    int choice;
    while (true) {
        cout << "Main Menu:\n1. Sign In\n2. Transaction Log Menu\n3. Exit\nEnter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            signIn();
            break;
        case 2:
            transactionLogMenu();
            break;
        case 3:
            cout << "Exiting...\n";
            return 0;
        default:
            cout << "Invalid choice! Try again.\n";
        }
    }
}
