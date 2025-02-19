#include <iostream>      // used for input and output (I/O) operations
#include <fstream>       // provides file stream classes for reading/writing files
#include <vector>        // for using the vector container
#include <string>        // for using the string class
#include <sstream>       // for string stream operations
#include <stdexcept>     // for exception classes
#include <algorithm>     // for std::find
#include <iomanip>       // for put_time and formatting
#include <ctime>         // for time-related functions
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
        cout << "Title: " << title << " | Author: " << author
             << " | ISBN: " << isbn << " | Year: " << year
             << " | Stock: " << stock << endl;
    }
};

// Base Person Class
class Person {
public:
    string name, id, contact;
    Person(string n, string i, string c) : name(n), id(i), contact(c) {}
};

// Member Class with fine feature
class Member : public Person, public LibraryEntity {
public:
    string password;
    vector<string> borrowedBooks;
    double fineAmount; // Accumulated fine amount

    Member(string n, string c, string p)
        : Person(n, to_string(generateMemberID()), c), password(p), fineAmount(0) {}

    void addEntity() override {
        // Save member details along with current fine amount
        ofstream file("members.csv", ios::app);
        file << name << "," << id << "," << contact << "," << password << "," << fineAmount << "\n";
        file.close();
    }

    void displayDetails() override {
        cout << "Member: " << name << " | ID: " << id
             << " | Contact: " << contact << " | Fine Due: " << fineAmount << endl;
    }

    static bool authenticate(string memberID, string password) {
        ifstream file("members.csv");
        string line, n, i, c, p;
        // Note: This simple authentication ignores the fine field
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

    // Member borrows a book (simulated by adding to an in-memory vector)
    void borrowBook(const string &bookTitle) {
        borrowedBooks.push_back(bookTitle);
        cout << "Book borrowed: " << bookTitle << endl;
    }

    // Function to get current time
    string get_time() {
        time_t now = time(0); // Getting current time
        tm* localTime = localtime(&now); // Converts the time_t value to a tm structure representing local time
        char buffer[20]; // Declare an array with 20 elements
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", localTime); // Changing time format to human-readable form
        cout << "You borrowed the book at: " << buffer << endl;
        return string(buffer); // Changing char array to string
    }

    // Function to display when the member is supposed to return the book
    void display_end_time(time_t start) {
        time_t end = start + (14 * 24 * 60 * 60); // 14 days in seconds
        tm* endTime = localtime(&end); // Convert end time to local time structure
        cout << "You have to return the book on: " << put_time(endTime, "%Y-%m-%d %H:%M") << endl;
    }

    // Member returns a book; if returned late, a fine of 500 is applied
    void returnBook(const string &bookTitle, bool returnedLate) {
        auto it = find(borrowedBooks.begin(), borrowedBooks.end(), bookTitle);
        if (it != borrowedBooks.end()) {
            borrowedBooks.erase(it);
            cout << "Book returned: " << bookTitle << endl;
            if (returnedLate) {
                fineAmount += 500;
                cout << "Book returned late. A fine of 500 has been added." << endl;
            }
        } else {
            cout << "You haven't borrowed the book: " << bookTitle << endl;
        }
    }

    // Member reports a lost book; a fine of 500 is applied
    void reportLostBook(const string &bookTitle) {
        auto it = find(borrowedBooks.begin(), borrowedBooks.end(), bookTitle);
        if (it != borrowedBooks.end()) {
            borrowedBooks.erase(it);
            fineAmount += 500;
            cout << "Book reported as lost. A fine of 500 has been added." << endl;
        } else {
            cout << "You haven't borrowed the book: " << bookTitle << endl;
        }
    }

    // Member can pay the accumulated fine
    void payFine() {
        cout << "Your current fine is: " << fineAmount << "\n";
        double amount;
        cout << "Enter amount to pay: ";
        cin >> amount;
        if (amount >= fineAmount) {
            cout << "Fine cleared!" << endl;
            fineAmount = 0;
        } else {
            fineAmount -= amount;
            cout << "Remaining fine: " << fineAmount << endl;
        }
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

// For the user to search for books
void searchBooks() {
    string searchForBooks;
    cout << "Enter title or author to search for: ";
    cin.ignore(); // Clear input buffer
    getline(cin, searchForBooks);

    ifstream file("books.csv");
    string line, title, author, isbn, year, stock;
    bool found = false;

    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, title, ',');
        getline(ss, author, ',');
        getline(ss, isbn, ',');
        getline(ss, year, ',');
        getline(ss, stock, ',');

        if (title.find(searchForBooks) != string::npos || author.find(searchForBooks) != string::npos) {
            cout << "Found: " << title << " by " << author
                 << " | ISBN: " << isbn << " | Year: " << year
                 << " | Stock: " << stock << endl;
            found = true;
        }
    }
    file.close();

    if (!found) {
        cout << "No books found matching the search criteria." << endl;
    }
}

// Function to view all books
void viewAllBooks() {
    ifstream file("books.csv");
    if (!file) {
        cout << "Error: Could not open file.\n";
        return;
    }

    string line, title, author, isbn, year, stock;
    cout << "\nAll Books in the Library:\n";
    cout << "----------------------------------------------------------------------------\n";
    cout << setw(30) << left << "Title" << setw(20) << "Author" << setw(15) << "ISBN" << setw(10) << "Year" << setw(10) << "Stock" << endl;
    cout << "----------------------------------------------------------------------------\n";

    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, title, ',');
        getline(ss, author, ',');
        getline(ss, isbn, ',');
        getline(ss, year, ',');
        getline(ss, stock, ',');

        cout << setw(30) << left << title << setw(20) << author << setw(15) << isbn << setw(10) << year << setw(10) << stock << endl;
    }

    file.close();
    cout << "----------------------------------------------------------------------------\n";
}

// Member action menu for borrowing, returning, lost-book reporting, paying fines, and viewing all books
void memberActions(Member &member) {
    int choice;
    do {
        cout << "\nMember Menu:\n"
             << "1. Search Books\n"
             << "2. Borrow Book\n"
             << "3. Return Book\n"
             << "4. Report Lost Book\n"
             << "5. Pay Fine\n"
             << "6. View All Books\n"
             << "7. Logout\n"
             << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                searchBooks();
                break;
            case 2: {
                string bookTitle;
                cout << "Enter book title to borrow: ";
                getline(cin, bookTitle);
                member.borrowBook(bookTitle);
                time_t start_time = time(0); // Getting current time
                member.get_time(); // Display current time
                member.display_end_time(start_time); // Display end time
                break;
            }
            case 3: {
                string bookTitle;
                char late;
                cout << "Enter book title to return: ";
                getline(cin, bookTitle);
                cout << "Was the book returned late? (y/n): ";
                cin >> late;
                cin.ignore();
                bool isLate = (late == 'y' || late == 'Y');
                member.returnBook(bookTitle, isLate);
                break;
            }
            case 4: {
                string bookTitle;
                cout << "Enter book title to report lost: ";
                getline(cin, bookTitle);
                member.reportLostBook(bookTitle);
                break;
            }
            case 5:
                member.payFine();
                break;
            case 6:
                viewAllBooks(); // View all books
                break;
            case 7:
                cout << "Logging out...\n";
                break;
            default:
                cout << "Invalid choice! Try again." << endl;
        }
    } while (choice != 7);
}

void searchMember() {
    ifstream file("members.csv");
    if (!file) {
        cout << "Error: Could not open file.\n";
        return;
    }

    string searchQuery;
    cout << "Enter Member Name or ID: ";
    cin.ignore(); // Ignore any leftover newline characters
    getline(cin, searchQuery);

    string line;
    bool found = false;

    while (getline(file, line)) {
        string name, id, contact, password;
        double fineAmount;
        stringstream ss(line);
        getline(ss, name, ',');
        getline(ss, id, ',');
        getline(ss, contact, ',');
        getline(ss, password, ',');
        ss >> fineAmount; // Read fine amount

        if (name == searchQuery || id == searchQuery) {
            cout << "Member Found!\n";
            cout << "Name: " << name << "\nID: " << id << "\nContact: " << contact << "\nFine Due: " << fineAmount << endl;
            found = true;
            break; // Stop searching after the first match
        }
    }

    if (!found) {
        cout << "No member found with that name or ID.\n";
    }

    file.close();
}

void librarianActions() {
    int choice;
    while (true) {
        cout << "\nLibrarian Menu:\n"
             << "1. Add Book\n"
             << "2. Add Member\n"
             << "3. Search Books\n"
             << "4. Search for a member\n"
             << "5. Logout\n"
             << "Enter your choice: ";
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
                cout << "Book added successfully!" << endl;
                break;
            }
            case 2: {
                string name, contact, password;
                cout << "Enter Member Name: "; getline(cin, name);
                cout << "Enter Contact: "; getline(cin, contact);
                cout << "Enter Password: "; getline(cin, password);
                Member newMember(name, contact, password);
                newMember.addEntity();
                cout << "Member added successfully! Library ID: " << newMember.id << endl;
                break;
            }
            case 3:
                searchBooks();
                break;
            case 4:
                searchMember();
                break;
            case 5:
                cout << "Logging out...\n";
                return;
            default:
                cout << "Invalid choice! Try again." << endl;
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
                    cin.ignore();
                    if (Member::authenticate(memberID, password)) {
                        cout << "Login successful!\n";
                        // In a complete system, member details would be loaded from storage.
                        // Here we create a temporary Member instance for simulation.
                        Member currentMember("User", "", password);
                        currentMember.id = memberID;
                        memberActions(currentMember);
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
                cout << "Enter Name: "; getline(cin, name);
                cout << "Enter Contact: "; getline(cin, contact);
                cout << "Enter New Password: "; getline(cin, newPassword);
                Member newMember(name, contact, newPassword);
                newMember.addEntity();
                cout << "Sign-up successful! Your Library ID Number is: " << newMember.id << endl;
                memberActions(newMember);
                return;
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

int main() {
    signIn();
    return 0;
}
