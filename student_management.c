#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <direct.h>  // For _mkdir on Windows

#define FOLDER_NAME "students_project"
#define FILE_NAME "students.csv"

// Student Structure
struct Student {
    int id;
    char name[50];
    char phone[15];
    char email[50];
    char address[100];
    char course[50];
    int currentAttendance;
    int previousAttendance;
    struct Student* next;
};

// Function to create a folder if it doesn't exist
void createFolder(const char* folderName) {
    struct stat st = {0};
    if (stat(folderName, &st) == -1) {
        // Folder doesn't exist, create it
        if (_mkdir(folderName) == 0) {  // Use _mkdir for Windows
            printf("Folder '%s' created successfully.\n", folderName);
        } else {
            printf("Failed to create folder '%s'.\n", folderName);
        }
    }
}

// Function to save student records to a CSV file
void saveToCSV(struct Student* head, const char* folderName) {
    createFolder(folderName);  // Ensure folder exists

    // Create the full file path for the CSV file
    char filePath[100];
    snprintf(filePath, sizeof(filePath), "%s/%s", folderName, FILE_NAME);

    FILE *file = fopen(filePath, "w");
    if (!file) {
        printf("Error opening file for writing!\n");
        return;
    }

    // Write column headers
    fprintf(file, "ID,Name,Phone,Email,Address,Course,CurrentAttendance,PreviousAttendance\n");

    // Write student records
    struct Student* temp = head;
    while (temp) {
        fprintf(file, "%d,%s,%s,%s,%s,%s,%d,%d\n", temp->id, temp->name, temp->phone,
                temp->email, temp->address, temp->course, temp->currentAttendance, temp->previousAttendance);
        temp = temp->next;
    }

    fclose(file);
    printf("Records saved successfully to '%s/%s'\n", folderName, FILE_NAME);
}

// Function to load student records from a CSV file
struct Student* loadFromCSV(const char* folderName) {
    // Create the full file path for the CSV file
    char filePath[100];
    snprintf(filePath, sizeof(filePath), "%s/%s", folderName, FILE_NAME);

    FILE* file = fopen(filePath, "r");
    if (!file) {
        printf("No previous records found in '%s'.\n", folderName);
        return NULL;
    }

    char line[200];
    struct Student* head = NULL;
    struct Student* temp = NULL;

    // Skip the header line
    fgets(line, sizeof(line), file);

    // Read student records from file
    while (fgets(line, sizeof(line), file)) {
        struct Student* newStudent = (struct Student*)malloc(sizeof(struct Student));

        // Read the student information from CSV line
        sscanf(line, "%d,%49[^,],%14[^,],%49[^,],%99[^,],%49[^,],%d,%d",
               &newStudent->id, newStudent->name, newStudent->phone, newStudent->email,
               newStudent->address, newStudent->course, &newStudent->currentAttendance, &newStudent->previousAttendance);

        // Insert the new student at the beginning of the list
        newStudent->next = head;
        head = newStudent;
    }

    fclose(file);
    return head;
}

// Function to add a student to the list
void addStudent(struct Student** head, int id, const char* name, const char* phone, const char* email, 
                const char* address, const char* course) {
    struct Student* newStudent = (struct Student*)malloc(sizeof(struct Student));
    newStudent->id = id;
    strcpy(newStudent->name, name);
    strcpy(newStudent->phone, phone);
    strcpy(newStudent->email, email);
    strcpy(newStudent->address, address);
    strcpy(newStudent->course, course);
    newStudent->currentAttendance = 0;
    newStudent->previousAttendance = 0;
    newStudent->next = *head;
    *head = newStudent;
}

// Function to display student records
void displayStudents(struct Student* head) {
    if (head == NULL) {
        printf("No student records available.\n");
        return;
    }

    printf("ID\tName\t\tPhone\t\tEmail\t\tAddress\t\tCourse\t\tAttendance\n");
    printf("---------------------------------------------------------------------\n");
    struct Student* temp = head;
    while (temp) {
        printf("%d\t%s\t%s\t%s\t%s\t%s\t%d\n", temp->id, temp->name, temp->phone, temp->email, temp->address, temp->course, temp->currentAttendance);
        temp = temp->next;
    }
}

// Function to mark attendance
void markAttendance(struct Student* head, int id, int present) {
    struct Student* temp = head;
    while (temp) {
        if (temp->id == id) {
            temp->currentAttendance += present ? 1 : 0;
            printf("Attendance updated.\n");
            return;
        }
        temp = temp->next;
    }
    printf("Student with ID %d not found.\n", id);
}

// Main function to demonstrate the system
int main() {
    struct Student* students = loadFromCSV(FOLDER_NAME);  // Load student records from CSV file

    int choice, id, present;
    char name[50], phone[15], email[50], address[100], course[50];

    while (1) {
        printf("\nStudent Record Management System\n");
        printf("1. Add Student\n");
        printf("2. Display Students\n");
        printf("3. Mark Attendance\n");
        printf("4. Save & Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter ID, Name, Phone, Email, Address, Course: ");
                scanf("%d %s %s %s %s %[^\n]", &id, name, phone, email, address, course);
                addStudent(&students, id, name, phone, email, address, course);
                break;
            case 2:
                displayStudents(students);
                break;
            case 3:
                printf("Enter Student ID and 1 for Present / 0 for Absent: ");
                scanf("%d %d", &id, &present);
                markAttendance(students, id, present);
                break;
            case 4:
                saveToCSV(students, FOLDER_NAME);  // Save records to CSV file before exiting
                exit(0);  // Properly exit the program
            default:
                printf("Invalid choice! Try again.\n");
        }
    }

    return 0;
}
