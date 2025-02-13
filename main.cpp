#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

using namespace std;

int generateMemberID() {
    static int idCounter = 1000;
    return ++idCounter;
}

class LibraryEntity {
public:
    virtual void addEntity() = 0;
    virtual void displayDetails() = 0;
};

class Book : public LibraryEntity {
public:
    string title, author, isbn;
    int year, stock;

    Book(string t, string a, string i, int y, int s) : title(t), author(a), isbn(i), year(y), stock(s) {}

    void addEntity() override {
        ofstream file("books.csv", ios::app);
        if (file.is_open()) {
            file << title << "," << author << "," << isbn << "," << year << "," << stock << "\n";
            file.close();
        } else {
            cerr << "Error opening books.csv for writing.\n";
        }
    }

    void displayDetails() override {
        cout << "Title: " << title << " | Author: " << author << " | ISBN: " << isbn
             << " | Year: " << year << " | Stock: " << stock << endl;
    }
};

class Person {
public:
    string name, id, contact;
    Person(string n, string i, string c) : name(n), id(i), contact(c) {}
};

class Member : public Person, public LibraryEntity {
public:
    string password;
    vector<string> borrowedBooks;

    Member(string n, string c, string p) : Person(n, to_string(generateMemberID()), c), password(p) {}

    void addEntity() override {
        ofstream file("members.csv", ios::app);
        if (file.is_open()) {
            file << name << "," << id << "," << contact << "," << password << "\n";
            file.close();
        } else {
            cerr << "Error opening members.csv for writing.\n";
        }
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
