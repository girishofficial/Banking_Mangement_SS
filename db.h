//
// Created by deepanshpandey on 10/8/24.
//

// db.h
// db.h

#ifndef DB_H
#define DB_H

// Define a struct for Customer
typedef struct {
    int id;
    char name[50];
    char email[50];
    char phone[15];
    char password[50];
    double balance;
    int account_active;
} Customer;

// Define a struct for Employee
typedef struct {
    int id;
    char name[50];
    char email[50];
    char password[50];
    int is_manager; // 1 for manager, 0 for non-manager
} Employee;

// Define a struct for Manager
typedef struct {
    int id;
    char name[50];
    char email[50];
    char password[50];
} Manager;

// Define a struct for Admin
typedef struct {
    char name[50];
    char email[50];
    char password[50];
} Admin;

// Define a struct for Account
typedef struct {
    int account_id;
    int customer_id;
    double balance;
} Account;

// Define a struct for Transaction
typedef struct {
    int id;
    int customer_id;
    double amount;
    char date[20]; // Assuming date is stored as a string in the format "YYYY-MM-DD"
} Transaction;

// Define a struct for LoanApplication
typedef struct {
    int loan_id;
    int customer_id;
    double amount;
    char status[20]; // "pending", "approved", "rejected"
    int assigned_employee_id;
} LoanApplication;

// Define a struct for Feedback
typedef struct {
    int feedback_id;
    int customer_id;
    char feedback[255];
    char date[20];
} Feedback;

#endif // DB_H