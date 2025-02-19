#include <iostream> // Handles input/output
#include <fstream>  // Reads and writes files
#include <vector>   // Easy storage
#include <string>   // Handles text processing
#include <sstream>  // Processes CSV files with string stream
#include <stdexcept> // Handles errors and exceptions

using namespace std;

int generateMemberID() {
    static int idCounter = 1000; // Static variable that retains its value between function calls
    return ++idCounter;          // Increments and returns the next unique ID
}

// Abstract class that will be used by books and members
class LibraryEntity {
public:
    virtual void addEntity() = 0;
    virtual void displayDetails() = 0;
};

// Manages books
class Book : public LibraryEntity {
public:
    string title, author, isbn;
    int year, stock;

    // Constructor to initialize book attributes
    Book(string t, string a, string i, int y, int s) : title(t), author(a), isbn(i), year(y), stock(s) {}

    // It is responsible for adding book details to a file books.csv
    void addEntity() override {
        ofstream file("books.csv", ios::app); // Write to a file books in append mode
        if (file.is_open()) {
            // Write book details to books file
            file << title << "," << author << "," << isbn << "," << year << "," << stock << "\n";
            file.close();
        } else {
            // Handles file operation errors
            cerr << "Error opening books.csv for writing.\n";
        }
    }

    // Displays book details on the console in a formatted manner
    void displayDetails() override {
        cout << "Title: " << title << " | Author: " << author << " | ISBN: " << isbn
             << " | Year: " << year << " | Stock: " << stock << endl;
    }
};

// Librarian class modification to operate independently and manage books and members
class Librarian {
public:
    string name, id, contact, password;

    Librarian(string n, string i, string c, string p) : name(n), id(i), contact(c), password(p) {}

    void addBook() {
        string title, author, isbn;
        int year, stock;
        cout << "Enter book title: ";
        getline(cin, title);
        cout << "Enter author: ";
        getline(cin, author);
        cout << "Enter ISBN: ";
        getline(cin, isbn);
        cout << "Enter year: ";
        cin >> year;
        cout << "Enter stock: ";
        cin >> stock;
        cin.ignore();
        Book book(title, author, isbn, year, stock);
        book.addEntity();
        cout << "Book added successfully!\n";
    }

    void addMember() {
        string name, id, contact;
        cout << "Enter member name: ";
        getline(cin, name);
        id = to_string(generateMemberID());
        cout << "Enter contact details: ";
        getline(cin, contact);
        ofstream file("members.csv", ios::app);
        if (file.is_open()) {
            file << name << "," << id << "," << contact << "\n";
            file.close();
            cout << "Member added successfully! ID: " << id << "\n";
        } else {
            cerr << "Error opening members.csv for writing.\n";
        }
    }

    void viewBorrowedBooks() {
        ifstream file("borrowed_books.csv");
        if (!file.is_open()) {
            cout << "No borrowed books record found.\n";
            return;
        }
        string line;
        cout << "Borrowed Books List:\n";
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    }
};

void librarianActions() {
    Librarian librarian("", "", "", "");
    int choice;
    while (true) {
        cout << "\nLibrarian Menu:\n1. Add Book\n2. Add Member\n3. View Borrowed Books\n4. Logout\nEnter choice: ";
        cin >> choice;
        cin.ignore();
        switch (choice) {
            case 1:
                librarian.addBook();
                break;
            case 2:
                librarian.addMember();
                break;
            case 3:
                librarian.viewBorrowedBooks();
                break;
            case 4:
                cout << "Logging out...\n";
                return;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    }
}
