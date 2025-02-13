#include <iostream>//Handles input/output 
#include <fstream>//Reads and writes files
#include <vector>//easy storage
#include <string>//Handles text processing
#include <sstream>//Processes CSV files with string stream
#include <stdexcept>//Handles errors and exceptions

using namespace std;

int generateMemberID() {
    static int idCounter = 1000;//static variable that retains its value between function calls
    return ++idCounter;//Increments and returns the next unique ID

}
//abstract class that will be used by books and members
class LibraryEntity {
public:
    virtual void addEntity() = 0;
    virtual void displayDetails() = 0;
};
//manages books
class Book : public LibraryEntity {
public:
    string title, author, isbn;
    int year, stock;
//consructor to initialize book attributes
    Book(string t, string a, string i, int y, int s) : title(t), author(a), isbn(i), year(y), stock(s) {}
//it is responsible for adding book details to a file books.csv
    void addEntity() override {
        ofstream file("books.csv", ios::app);//write to a file books in append mode
        //check if the file has successfully opened
        if (file.is_open()) {
            //write book details to books file
            file << title << "," << author << "," << isbn << "," << year << "," << stock << "\n";
            file.close();
        } else {
            //handles file operation errors
            cerr << "Error opening books.csv for writing.\n";
        }
    }
//Displays book details on the console in a formatted manner
    void displayDetails() override {
        cout << "Title: " << title << " | Author: " << author << " | ISBN: " << isbn
             << " | Year: " << year << " | Stock: " << stock << endl;
    }
};
//A base class representing a person in the library system 
class Person {
public:
    string name, id, contact;
//Constructor to initialize a Person object
    Person(string n, string i, string c) : name(n), id(i), contact(c) {}
};
//inherited class from Person and Library entity
class Member : public Person, public LibraryEntity {
public:
    string password;
    vector<string> borrowedBooks;
//costructor to initialize Member attributes
    Member(string n, string c, string p) : Person(n, to_string(generateMemberID()), c), password(p) {}
// void function to create member.csv file
    void addEntity() override {
        ofstream file("members.csv", ios::app);// ensures new members are appended
         //check if the file has successfully opened
        if (file.is_open()) {
            //write member details to member file
            file << name << "," << id << "," << contact << "," << password << "\n";
            file.close();
        } else {
            // handles errors
            cerr << "Error opening members.csv for writing.\n";
        }
    }
//Displays member information in a readable format
    void displayDetails() override {
        cout << "Member: " << name << " | ID: " << id << " | Contact: " << contact << endl;
    }
//Opens members.csv for reading to verify login credentials
    static bool authenticate(string memberID, string password) {
        ifstream file("members.csv");
        //Reads the file line by line to check each record
        string line, n, i, c, p;
        while (getline(file, line)) {
            stringstream ss(line);
            getline(ss, n, ',');//extract name
            getline(ss, i, ',');//extracts id
            getline(ss, c, ',');//contact
            getline(ss, p, ',');//password
            //check if password match
            if (i == memberID && p == password) {
                file.close();
                return true;
            }
        }
        file.close();
        return false;
    }

    void borrowBook(string bookTitle) {
        ifstream file("books.csv");
        vector<Book> books;
        string line, t, a, i;
        int y, s;
        bool found = false;

        while (getline(file, line)) {
            stringstream ss(line);
            getline(ss, t, ',');
            getline(ss, a, ',');
            getline(ss, i, ',');
            ss >> y;
            ss.ignore();
            ss >> s;
            if (t == bookTitle && s > 0) {
                found = true;
                books.emplace_back(t, a, i, y, s - 1);
                borrowedBooks.push_back(t);
            } else {
                books.emplace_back(t, a, i, y, s);
            }
        }
        file.close();

        if (found) {
            ofstream outFile("books.csv");
            for (auto &book : books) {
                outFile << book.title << "," << book.author << "," << book.isbn << "," << book.year << "," << book.stock << "\n";
            }
            outFile.close();
            cout << "Book borrowed successfully. You have 2 weeks to return it.\n";
        } else {
            cout << "Book not available or out of stock.\n";
        }
    }

    void returnBook(string bookTitle) {
        ifstream file("books.csv");
        vector<Book> books;
        string line, t, a, i;
        int y, s;
        bool found = false;

        while (getline(file, line)) {
            stringstream ss(line);
            getline(ss, t, ',');
            getline(ss, a, ',');
            getline(ss, i, ',');
            ss >> y;
            ss.ignore();
            ss >> s;
            if (t == bookTitle) {
                found = true;
                books.emplace_back(t, a, i, y, s + 1);
            } else {
                books.emplace_back(t, a, i, y, s);
            }
        }
        file.close();

        if (found) {
            ofstream outFile("books.csv");
            for (auto &book : books) {
                outFile << book.title << "," << book.author << "," << book.isbn << "," << book.year << "," << book.stock << "\n";
            }
            outFile.close();
            cout << "Book returned successfully.\n";
        } else {
            cout << "Book not found in the library system.\n";
        }
    }

    void searchBook(string bookTitle) {
        ifstream file("books.csv");
        string line, t, a, i;
        int y, s;
        bool found = false;

        while (getline(file, line)) {
            stringstream ss(line);
            getline(ss, t, ',');
            getline(ss, a, ',');
            getline(ss, i, ',');
            ss >> y;
            ss.ignore();
            ss >> s;
            if (t == bookTitle) {
                found = true;
                cout << "Book found:\n";
                cout << "Title: " << t << " | Author: " << a << " | ISBN: " << i
                     << " | Year: " << y << " | Stock: " << s << endl;
                break;
            }
        }
        file.close();

        if (!found) {
            cout << "Book not found.\n";
        }
    }

    void viewBorrowedBooks() {
        if (borrowedBooks.empty()) {
            cout << "You have not borrowed any books.\n";
        } else {
            cout << "Books you have borrowed:\n";
            for (const string &book : borrowedBooks) {
                cout << "- " << book << endl;
            }
        }
    }
};

class Librarian : public Person {
public:
    string password;
    Librarian(string n, string i, string c, string p) : Person(n, i, c), password(p) {}

    void addBook() {
        string title, author, isbn;
        int year, stock;
        cout << "Enter book title: "; getline(cin, title);
        cout << "Enter author: "; getline(cin, author);
        cout << "Enter ISBN: "; getline(cin, isbn);
        cout << "Enter publication year: "; cin >> year;
        cout << "Enter stock quantity: "; cin >> stock;
        cin.ignore();
        Book newBook(title, author, isbn, year, stock);
        newBook.addEntity();
        cout << "Book added successfully!\n";
    }

    void addMember() {
        string name, contact, password;
        cout << "Enter member name: "; getline(cin, name);
        cout << "Enter contact: "; getline(cin, contact);
        cout << "Enter password: "; getline(cin, password);
        Member newMember(name, contact, password);
        newMember.addEntity();
        cout << "Member added successfully! Library ID: " << newMember.id << "\n";
    }

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

// Forward declarations
void librarianActions();
void memberActions(string memberID);

void signIn() {
    while (true) {
        int choice;
        cout << "Sign in as:\n1. Member\n2. Librarian\nEnter choice: ";
        cin >> choice;
        cin.ignore();
        if (choice == 1) {
            int memberChoice;
            cout << "Are you a registered member?\n1. Yes\n2. No\nEnter choice: ";
            cin >> memberChoice;
            cin.ignore();
            if (memberChoice == 1) {
                string memberID, password;
                cout << "Enter Library ID Number: ";
                cin >> memberID;
                cout << "Enter Password: ";
                cin >> password;
                cin.ignore();
                if (Member::authenticate(memberID, password)) {
                    cout << "Login successful!\n";
                    memberActions(memberID);
                } else {
                    cout << "Invalid credentials!\n";
                }
            } else if (memberChoice == 2) {
                string name, contact, password;
                cout << "Enter your name: "; getline(cin, name);
                cout << "Enter your contact: "; getline(cin, contact);
                cout << "Enter a password: "; getline(cin, password);
                Member newMember(name, contact, password);
                newMember.addEntity();
                cout << "Account created successfully! Your Library ID is: " << newMember.id << "\n";
                memberActions(newMember.id);
            } else {
                cout << "Invalid choice. Try again.\n";
            }
        } else if (choice == 2) {
            string librarianID, password;
            cout << "Enter Librarian ID: ";
            cin >> librarianID;
            cout << "Enter Password: ";
            cin >> password;
            cin.ignore();
            if (Librarian::authenticate(librarianID, password)) {
                cout << "Librarian login successful!\n";
                librarianActions();
            } else {
                cout << "Invalid credentials!\n";
            }
        }
    }
}

void memberActions(string memberID) {
    Member member("", "", "");
    int choice;
    while (true) {
        cout << "\nMember Menu:\n1. Borrow Book\n2. Return Book\n3. Search Book\n4. View Borrowed Books\n5. Logout\nEnter choice: ";
        cin >> choice;
        cin.ignore();
        switch (choice) {
            case 1: {
                string bookTitle;
                cout << "Enter the title of the book you want to borrow: ";
                getline(cin, bookTitle);
                member.borrowBook(bookTitle);
                break;
            }
            case 2: {
                string bookTitle;
                cout << "Enter the title of the book you want to return: ";
                getline(cin, bookTitle);
                member.returnBook(bookTitle);
                break;
            }
            case 3: {
                string bookTitle;
                cout << "Enter the title of the book you want to search: ";
                getline(cin, bookTitle);
                member.searchBook(bookTitle);
                break;
            }
            case 4:
                member.viewBorrowedBooks();
                break;
            case 5:
                cout << "Logging out...\n";
                return;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    }
}

void librarianActions() {
    Librarian librarian("", "", "", "");
    int choice;
    while (true) {
        cout << "\nLibrarian Menu:\n1. Add Book\n2. Add Member\n3. Logout\nEnter choice: ";
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
                cout << "Logging out...\n";
                return;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    }
}

int main() {
    signIn();
    return 0;
}
