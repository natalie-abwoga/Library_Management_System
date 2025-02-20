#include <iostream>// Provides input and output stream functionality (e.g., cin, cout)
#include <fstream>//Allows file input and output operations using file streams
#include <vector>//storage library
#include <string>//Provides the  class for handling strings
#include <sstream>//Supports string stream operations for parsing and formatting strings
#include <stdexcept>//Contains standard exception classes 
#include <algorithm>// Offers a collection of algorithm
#include <iomanip>//Provides facilities to format input/output 
#include <ctime>// Contains functions and types for manipulating date and time.
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

    // Member borrows a book (in-memory record update)
    void borrowBook(const string &bookTitle) {
        borrowedBooks.push_back(bookTitle);
        cout << "Book borrowed: " << bookTitle << endl;
    }

    // Function to get current time
    string get_time() {
        time_t now = time(0);
        tm* localTime = localtime(&now);
        char buffer[20];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", localTime);
        cout << "You borrowed the book at: " << buffer << endl;
        return string(buffer);
    }

    // Function to display return deadline (14 days later)
    void display_end_time(time_t start) {
        time_t end = start + (14 * 24 * 60 * 60);
        tm* endTime = localtime(&end);
        cout << "You have to return the book on: " << put_time(endTime, "%Y-%m-%d %H:%M") << endl;
    }

    // Function to return a book. If the book is returned late, a fine of 500 is added.
    void returnBook(const string &bookTitle, bool returnedLate) {
           // Search for the book title in the borrowedBooks vector
        auto it = find(borrowedBooks.begin(), borrowedBooks.end(), bookTitle);
            // Check if the book was found among the borrowed books
        if (it != borrowedBooks.end()) {
             // Remove the book from the list as it is being returned
            borrowedBooks.erase(it);
            cout << "Book returned: " << bookTitle << endl;
                 // If the book was returned after the due date, apply a fine
            if (returnedLate) {
                fineAmount += 500; // Add a fine of 500 to the current fine amount
                cout << "Book returned late. A fine of 500 has been added." << endl;
            }
        } else {
            // If the book isn't found in the borrowed list, inform the user
            cout << "You haven't borrowed the book: " << bookTitle << endl;
        }
    }

    // Member reports a lost book; a fine of 500 is applied
    void reportLostBook(const string &bookTitle) {
            // Search for the book title in the borrowedBooks vector
        auto it = find(borrowedBooks.begin(), borrowedBooks.end(), bookTitle);
        if (it != borrowedBooks.end()) {
            borrowedBooks.erase(it);
            fineAmount += 500;
            cout << "Book reported as lost. A fine of 500 has been added." << endl;
        } else {
            cout << "You haven't borrowed the book: " << bookTitle << endl;
        }
    }

    // Function that allows the member to pay off their accumulated fine
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
    cin.ignore();
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
    cout << setw(30) << left << "Title" << setw(20) << "Author" << setw(15) << "ISBN" 
         << setw(10) << "Year" << setw(10) << "Stock" << endl;
    cout << "----------------------------------------------------------------------------\n";

    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, title, ',');
        getline(ss, author, ',');
        getline(ss, isbn, ',');
        getline(ss, year, ',');
        getline(ss, stock, ',');

        cout << setw(30) << left << title << setw(20) << author << setw(15) << isbn 
             << setw(10) << year << setw(10) << stock << endl;
    }

    file.close();
    cout << "----------------------------------------------------------------------------\n";
}

// Function to update the book stock record when a book is borrowed
bool updateBookStock(const string &bookTitle) {
    ifstream infile("books.csv");
    if (!infile) {
        cout << "Error: Could not open books.csv for reading." << endl;
        return false;
    }
    vector<string> lines;
    bool found = false;
    string line;
    while (getline(infile, line)) {
        stringstream ss(line);
        string title, author, isbn, year, stock;
        getline(ss, title, ',');
        getline(ss, author, ',');
        getline(ss, isbn, ',');
        getline(ss, year, ',');
        getline(ss, stock, ',');
        
        if (title == bookTitle) {
            found = true;
            int currentStock = stoi(stock);
            if (currentStock > 0) {
                currentStock--;  // Decrement the stock
                ostringstream oss;
                oss << title << "," << author << "," << isbn << "," << year << "," << currentStock;
                lines.push_back(oss.str());
            } else {
                cout << "No stock available for book: " << bookTitle << endl;
                lines.push_back(line); // Keep the original record
            }
        } else {
            lines.push_back(line);
        }
    }
    infile.close();

    if (!found) {
        cout << "Book not found: " << bookTitle << endl;
        return false;
    }

    ofstream outfile("books.csv");
    if (!outfile) {
        cout << "Error: Could not open books.csv for writing." << endl;
        return false;
    }
    for (const auto &updatedLine : lines) {
        outfile << updatedLine << "\n";
    }
    outfile.close();
    return true;
}

// Member action menu including borrowing, returning, and fine management
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
                // Update the book record by reducing the stock before borrowing
                if (updateBookStock(bookTitle)) {
                    member.borrowBook(bookTitle);
                    time_t start_time = time(0);
                    member.get_time();
                    member.display_end_time(start_time);
                }
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
                viewAllBooks();
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
    cin.ignore();
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
        ss >> fineAmount;

        if (name == searchQuery || id == searchQuery) {
            cout << "Member Found!\n";
            cout << "Name: " << name << "\nID: " << id << "\nContact: " << contact 
                 << "\nFine Due: " << fineAmount << endl;
            found = true;
            break;
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
