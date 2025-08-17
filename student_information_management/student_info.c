#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//定義學生資料結構
struct Student{
    char name[50];
    int id;
    float score;
    struct Student *next;

} Student;

//函數原型
struct Student* createStudentNode(const char *name, int id, float score){
    struct Student *newStudent = (struct Student*)malloc(sizeof(struct Student));
    if(newStudent == NULL){
        printf("Memory allocation failed!\n");
        exit(1);
    }
    strcpy(newStudent->name, name);
    newStudent->id = id;
    newStudent->score = score;
    newStudent->next = NULL;
    return newStudent;
}

//加入學生資訊
void addStudent(struct Student **head, struct Student *newStudent){
    if(*head == NULL){
        *head = newStudent; //空list直接當第一個
    }else{
        //接到最後一個節點
        struct Student *current = *head;
        while(current->next){
            current = current->next;
        }
        current->next = newStudent; //將新節點接到最後
    }
    printf("Student %s added successfully!\n", newStudent->name);
}

//顯示所有學生資訊
void displayStudents(struct Student *head){
    if(head == NULL){
        printf("No students found.\n");
        return;
    }
    struct Student *current = head;
    printf("Student Information:\n");
    while(current){
        printf("Name: %s, ID: %d, Score: %.2f\n",current->name, current->id, current->score);
        current = current->next;
    }
}

//搜尋學生資訊
void searchStudentByName(struct Student *head, const char *name){
    struct Student *current = head;
    while(current){
        if(strcmp(current->name, name) == 0){
            printf("Student Found: Name: %s, ID: %d, Score: %.2f\n", current->name, current->id, current->score);
            break;
        }else{
            current = current->next;
        }
    }
    if(current == NULL){
        printf("Student with name %s not found.\n", name);
    }
    
}

// ===================== Merge Sort for Linked List =====================
struct Student* sortedMerge(struct Student *a, struct Student *b, bool lowToHigh) {
    //合併排序過的
    if(a == NULL) return b;
    if(b == NULL) return a;
    if(lowToHigh) {
        if(a->score <= b->score) {
            a->next = sortedMerge(a->next, b, lowToHigh);
            return a;
        }else{
            b->next = sortedMerge(a, b->next, lowToHigh);
            return b;
        }
    }else {
        if(a->score >= b->score){
            a->next = sortedMerge(a->next, b, lowToHigh);
            return a;
        }else{
            b->next = sortedMerge(a, b->next, lowToHigh);
            return b;
        }
    }
}

void split(struct Student *source, struct Student **frontRef, struct Student **backRef) {
    struct Student *fast = source->next;
    struct Student *slow = source;

    //利用快慢指針分割list
    while(fast != NULL){
        fast = fast->next;
        if(fast != NULL){
            slow = slow->next;
            fast = fast->next;
        }
    }
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL; //將前半部的最後一個節點的next設為NULL
}

void mergesort(struct Student **headRef, bool lowToHigh) {
    struct Student *head = *headRef;
    struct Student *a;
    struct Student *b;
    if(head == NULL || head->next == NULL){
        return;
    }
    //分割list
    split(head, &a, &b);

    mergesort(&a, lowToHigh);
    mergesort(&b, lowToHigh);

    *headRef = sortedMerge(a,b, lowToHigh);
}


//sort students by score (high to low)
void sortStudentsByScoreHighToLow(struct Student **head){
    bool lowToHigh = false; //高排到低
    //高排到低
    mergesort(head, lowToHigh);
    printf("Students sorted by score (high to low):\n");
    displayStudents(*head);
}

//sort students by score (low to high)
void sortStudentsByScoreLowToHigh(struct Student **head){
    bool lowToHigh = true; //低排到高
    mergesort(head, lowToHigh);
    printf("Students sorted by score (low to high):\n");
    displayStudents(*head);
}


void saveDataToFile(struct Student *head, FILE *file) {
    struct Student *current = head;
    while(current){
        fprintf(file, "%s %d %.2f\n", current->name, current->id, current->score);
        current = current->next;
    }
}

void loadDataFromFile(struct Student **head, FILE *file) {
    char name[50];
    int id;
    float score;
    while(fscanf(file, "%s %d %f", name, &id, &score) == 3) {
        struct Student *newStudent = createStudentNode(name, id, score);
        addStudent(head, newStudent);
    }

}

int main(){
    bool running = true;
    struct Student *head = NULL; //學生資料的頭指標
    while(running){

        printf("Welcome to the Student Information Management System!\n");
        printf("=== Menu ===\n");
        printf("1. Add Student Information\n");
        printf("2. Display All Students\n");
        printf("3. Search Student by Name\n");
        printf("4. Sort Students by Score (High to Low)\n");
        printf("5. Sort Students by Score (Low to High)\n");
        printf("6. Save Data to File\n");
        printf("7. Load Data from File\n");
        printf("8. Exit\n");
        int choice;
        printf("Please enter your choice: ");
        scanf("%d", &choice);
        switch(choice) {
            case 1:
                printf("Adding student information...\n");
                printf("Enter student name, ID, and score (format: name id score): ");
                char name[50];
                int id;
                float score;
                scanf("%s %d %f", name, &id, &score);
                addStudent(&head, createStudentNode(name, id, score));
                printf("Student added successfully!\n");

                break;
            case 2:
                printf("Displaying all students...\n");
                displayStudents(head);
                break;
            case 3:
                printf("Searching for student by name...\n");
                printf("Enter student name to search: ");
                char searchName[50];
                scanf("%s",searchName);
                searchStudentByName(head, searchName);
                break;
            case 4:
                printf("Sorting students by score (high to low)...\n");
                sortStudentsByScoreHighToLow(&head);
                break;
            case 5:
                printf("Sorting students by score (low to high)...\n");
                sortStudentsByScoreLowToHigh(&head);
                break;
            case 6:
                printf("Saving data to file...\n");
                FILE *savefile = fopen("./storefile/students.txt", "w");
                if(savefile == NULL){
                    printf("Error opening file for writing!\n");
                }else{
                    saveDataToFile(head, savefile);
                    fclose(savefile);
                    printf("Data saved successfully!\n");
                }
                break;
            case 7:
                printf("Loading data from file...\n");
                FILE *loadfile = fopen("./storefile/students.txt", "r");
                if(loadfile == NULL){
                    printf("Error opening file for reading!\n");
                }else{
                    loadDataFromFile(&head, loadfile);
                    fclose(loadfile);
                    printf("Data loaded successfully!\n");
                }
                break;
            case 8:
                printf("Exiting the system...\n");
                running = false;
                break;
            default:
                printf("Invalid choice! Please try again.\n");
                continue;
        }
    }
    return 0;
}