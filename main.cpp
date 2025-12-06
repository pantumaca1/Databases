//COMPILING: g++ main.cpp -I"C:\Program Files\MariaDB\MariaDB-Connector-C-64-bit\include" -L"C:\Program Files\MariaDB\MariaDB-Connector-C-64-bit\lib" -lmariadb -o app

#include <iostream>
#include <mysql.h>

using namespace std;

// connect to database
MYSQL* connectDB() {
    MYSQL* conn = mysql_init(NULL);
    if (!conn) {
        cerr << "mysql_init() failed\n";
        exit(1);
    }

    // host/user/password/db
    if (!mysql_real_connect(conn, "localhost", "root", "54_6ahg/e@_fdh(j_.)",
                            "bookstore", 3306, NULL, 0)) {
        cerr << "Connection failed: " << mysql_error(conn) << endl;
        exit(1);
    }
    return conn;
}

void resetDatabase(MYSQL* conn) {
    mysql_query(conn, "DELETE FROM sales");
    mysql_query(conn, "DELETE FROM books");
    mysql_query(conn, "DELETE FROM customers");
    mysql_query(conn, "DELETE FROM authors");
}

//--------------------------------------
// CRUD OPERATIONS
//--------------------------------------

// AUTHORS
void createAuthor(MYSQL* conn, const string& name) {
    string q = "INSERT IGNORE INTO authors (name) VALUES('" + name + "')";
    mysql_query(conn, q.c_str());
}

void readAuthors(MYSQL* conn) {
    if (mysql_query(conn, "SELECT id, name FROM authors"))
        cerr << mysql_error(conn) << endl;

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;

    cout << "\nAuthors:\n";
    while ((row = mysql_fetch_row(res)))
        cout << row[0] << " | " << row[1] << endl;

    mysql_free_result(res);
}

void updateAuthor(MYSQL* conn, int id, const string& name) {
    string q = "UPDATE authors SET name='" + name + "' WHERE id=" + to_string(id);
    if (mysql_query(conn, q.c_str()))
        cerr << "Update Author Error: " << mysql_error(conn) << endl;
}

void deleteAuthorSafe(MYSQL* conn, int id) {
    // delete books referencing this author
    string qb = "DELETE FROM books WHERE author_id=" + to_string(id);
    mysql_query(conn, qb.c_str());

    // now delete author
    string qa = "DELETE FROM authors WHERE id=" + to_string(id);
    if (mysql_query(conn, qa.c_str()))
        cerr << "Delete Author Error: " << mysql_error(conn) << endl;
}

// BOOKS
void createBook(MYSQL* conn, const string& title, int author_id, double price) {
    string q = "INSERT INTO books (title, author_id, price) VALUES('" +
               title + "', " + to_string(author_id) + ", " + to_string(price) + ")";
    mysql_query(conn, q.c_str());
}

void readBooks(MYSQL* conn) {
    if (mysql_query(conn,
        "SELECT books.id, books.title, authors.name, books.price "
        "FROM books LEFT JOIN authors ON books.author_id = authors.id"))
        cerr << mysql_error(conn) << endl;

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;

    cout << "\nBooks:\n";
    while ((row = mysql_fetch_row(res)))
        cout << row[0] << " | " << row[1] << " | " << row[2] << " | $" << row[3] << endl;

    mysql_free_result(res);
}

void updateBookPrice(MYSQL* conn, int id, double price) {
    string q = "UPDATE books SET price=" + to_string(price) + " WHERE id=" + to_string(id);
    if (mysql_query(conn, q.c_str()))
        cerr << "Update Book Error: " << mysql_error(conn) << endl;
}

void deleteBook(MYSQL* conn, int id) {
    string q = "DELETE FROM books WHERE id=" + to_string(id);
    if (mysql_query(conn, q.c_str()))
        cerr << "Delete Book Error: " << mysql_error(conn) << endl;
}

// CUSTOMERS
void createCustomer(MYSQL* conn, const string& name, const string& email) {
    string q = "INSERT IGNORE INTO customers (name, email) VALUES('" 
                + name + "', '" + email + "')";
    mysql_query(conn, q.c_str());
}

void readCustomers(MYSQL* conn) {
    if (mysql_query(conn, "SELECT id, name, email FROM customers"))
        cerr << mysql_error(conn) << endl;

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;

    cout << "\nCustomers:\n";
    while ((row = mysql_fetch_row(res)))
        cout << row[0] << " | " << row[1] << " | " << row[2] << endl;

    mysql_free_result(res);
}


int main() {
    MYSQL* conn = connectDB();

    resetDatabase(conn);

    cout << "** Creating Records **\n";
    createAuthor(conn, "Isaac Asimov");
    createAuthor(conn, "Frank Herbert");

    createBook(conn, "Foundation", 1, 9.99);
    createBook(conn, "Dune", 2, 14.95);

    createCustomer(conn, "Alice Smith", "alice@example.com");
    createCustomer(conn, "Bob Jones", "bob@example.com");

    cout << "\n** Reading Data **\n";
    readAuthors(conn);
    readBooks(conn);
    readCustomers(conn);

    cout << "\n** Updating Data **\n";
    updateAuthor(conn, 1, "I. Asimov");
    updateBookPrice(conn, 2, 12.50);
    readAuthors(conn);
    readBooks(conn);

    cout << "\n** Deleting Data **\n";
    deleteAuthorSafe(conn, 2);
    readBooks(conn);
    readAuthors(conn);

    mysql_close(conn);
}