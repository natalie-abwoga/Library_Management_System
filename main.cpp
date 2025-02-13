#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
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
    int year;
    bool isAvailable;

    Book(string t, string a, string i, int y, bool avail = true)
        : title(t), author(a), isbn(i), year(y), isAvailable(avail) {}

    void addEntity() override {
        ofstream file("books.csv", ios::app);
        file << title << "," << author << "," << isbn << "," << year << "," << isAvailable << "\n";
        file.close();
    }

    void displayDetails() override {
        cout << "Title: " << title << " | Author: " << author << " | ISBN: " << isbn
             << " | Year: " << year << " | Available: " << (isAvailable ? "Yes" : "No") << endl;
    }
};

// Base Person Class
class Person {
public:
    string name, id, contact;
    Person(string n, string i, string c) : name(n), id(i), contact(c) {}
};

// Member Class (Inheritance & Aggregation)
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
            int year;
            cout << "Enter Book Title: "; getline(cin, title);
            cout << "Enter Author: "; getline(cin, author);
            cout << "Enter ISBN: "; getline(cin, isbn);
            cout << "Enter Year: "; cin >> year;
            cin.ignore();
            Book newBook(title, author, isbn, year);
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
                bool success = false;
                cout << "Enter Library ID Number: ";
                cin >> memberID;

                while (attempts < 3) {
                    cout << "Enter Password: ";
                    cin >> password;

                    if (Member::authenticate(memberID, password)) {
                        cout << "Login successful!\n";
                        success = true;
                        return;
                    } else {
                        attempts++;
                        cout << "Invalid password. Attempts remaining: " << (3 - attempts) << "\n";
                    }
                }
                if (!success) {
                    cout << "Too many failed attempts. Please contact the administrator.\n";
                }
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
            cout << "Enter Librarian ID: "; cin >> librarianID;
            cout << "Enter Password: "; cin >> password;

            if (Librarian::authenticate(librarianID, password)) {
                cout << "Login successful!\n";
                librarianActions();
            } else {
                cout << "Invalid credentials!\n";
            }
        } else {
            cout << "Invalid choice! Try again.\n";
        }
    }
}

int main() {
    signIn();
    return 0;
}
