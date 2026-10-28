#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BASELINE_PRODUCTIVITY_SCALE 100.0f
#define WEIGHT_PRODUCTIVITY 0.40f
#define WEIGHT_QUALITY 0.35f
#define WEIGHT_RELIABILITY 0.25f

typedef struct {
    int taskId;
    char description[100];
    float hoursSpent;
    float qualityScore;
    int isCompleted;
    int isError;
} TaskLog;

typedef struct {
    int id;
    char name[60];
    char department[40];
    TaskLog *logs;
    int logCount;
    int logCapacity;
    float productivity;
    float avgQuality;
    float reliability;
    float compositeScore;
} Employee;

static int caseInsensitiveCompare(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        int diff = tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
        if (diff != 0) return diff;
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

static char *caseInsensitiveSubstring(const char *haystack, const char *needle) {
    if (!*needle) return (char *)haystack;
    for (; *haystack; haystack++) {
        const char *h = haystack, *n = needle;
        while (*h && *n && tolower((unsigned char)*h) == tolower((unsigned char)*n)) {
            h++;
            n++;
        }
        if (!*n) return (char *)haystack;
    }
    return NULL;
}

static int readSafeInt(const char *prompt, int *val) {
    printf("%s", prompt);
    if (scanf("%d", val) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return 0;
    }
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    return 1;
}

static int readSafeFloat(const char *prompt, float *val) {
    printf("%s", prompt);
    if (scanf("%f", val) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return 0;
    }
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    return 1;
}

static void readSafeString(const char *prompt, char *buffer, int maxLen) {
    printf("%s", prompt);
    if (fgets(buffer, maxLen, stdin)) {
        buffer[strcspn(buffer, "\r\n")] = '\0';
    }
}

static void employeeInit(Employee *emp, int id, const char *name, const char *dept) {
    emp->id = id;
    strncpy(emp->name, name, sizeof(emp->name) - 1);
    emp->name[sizeof(emp->name) - 1] = '\0';
    strncpy(emp->department, dept, sizeof(emp->department) - 1);
    emp->department[sizeof(emp->department) - 1] = '\0';
    emp->logCount = 0;
    emp->logCapacity = 4;
    emp->logs = (TaskLog *)malloc((size_t)emp->logCapacity * sizeof(TaskLog));
    emp->productivity = 0.0f;
    emp->avgQuality = 0.0f;
    emp->reliability = 0.0f;
    emp->compositeScore = 0.0f;
}

static void employeeFree(Employee *emp) {
    if (emp && emp->logs) {
        free(emp->logs);
        emp->logs = NULL;
        emp->logCount = 0;
        emp->logCapacity = 0;
    }
}

static void employeeRecalculateKPIs(Employee *emp) {
    if (!emp || emp->logCount == 0) {
        emp->productivity = 0.0f;
        emp->avgQuality = 0.0f;
        emp->reliability = 0.0f;
        emp->compositeScore = 0.0f;
        return;
    }
    float totalHours = 0.0f, totalQuality = 0.0f;
    int completed = 0, errors = 0;
    for (int i = 0; i < emp->logCount; i++) {
        totalHours += emp->logs[i].hoursSpent;
        totalQuality += emp->logs[i].qualityScore;
        if (emp->logs[i].isCompleted) completed++;
        if (emp->logs[i].isError) errors++;
    }
    float rawProd = (totalHours > 0.0f) ? ((float)completed / totalHours) * BASELINE_PRODUCTIVITY_SCALE : 0.0f;
    emp->productivity = (rawProd > 100.0f) ? 100.0f : rawProd;
    emp->avgQuality = totalQuality / (float)emp->logCount;
    float errRate = (float)errors / (float)emp->logCount;
    emp->reliability = (1.0f - errRate) * emp->avgQuality;
    emp->compositeScore = (WEIGHT_PRODUCTIVITY * emp->productivity) +
                          (WEIGHT_QUALITY * emp->avgQuality) +
                          (WEIGHT_RELIABILITY * emp->reliability);
}

static int employeeAddLog(Employee *emp, TaskLog log) {
    if (!emp->logs) {
        emp->logCapacity = 4;
        emp->logs = (TaskLog *)malloc((size_t)emp->logCapacity * sizeof(TaskLog));
        if (!emp->logs) return 0;
    }
    if (emp->logCount >= emp->logCapacity) {
        int newCap = emp->logCapacity * 2;
        TaskLog *newLogs = (TaskLog *)realloc(emp->logs, (size_t)newCap * sizeof(TaskLog));
        if (!newLogs) return 0;
        emp->logs = newLogs;
        emp->logCapacity = newCap;
    }
    emp->logs[emp->logCount++] = log;
    employeeRecalculateKPIs(emp);
    return 1;
}

static int binarySearchEmployeeById(const Employee *employees, int count, int targetId) {
    int low = 0, high = count - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (employees[mid].id == targetId) return mid;
        else if (employees[mid].id < targetId) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

static void swapEmp(Employee *a, Employee *b) {
    Employee tmp = *a;
    *a = *b;
    *b = tmp;
}

static void sortEmployeesById(Employee *arr, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (arr[j].id > arr[j + 1].id) swapEmp(&arr[j], &arr[j + 1]);
        }
    }
}

static int partition(Employee *arr, int low, int high) {
    float pivotScore = arr[high].compositeScore;
    int pivotId = arr[high].id;
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j].compositeScore > pivotScore ||
           (arr[j].compositeScore == pivotScore && arr[j].id < pivotId)) {
            i++;
            swapEmp(&arr[i], &arr[j]);
        }
    }
    swapEmp(&arr[i + 1], &arr[high]);
    return i + 1;
}

static void quickSortEmployeesByScore(Employee *arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSortEmployeesByScore(arr, low, pi - 1);
        quickSortEmployeesByScore(arr, pi + 1, high);
    }
}

static void generateSummaryReport(Employee *employees, int count, float threshold) {
    if (count <= 0) {
        printf("[!] No employees to evaluate.\n");
        return;
    }
    float *validScores = (float *)malloc((size_t)count * sizeof(float));
    if (!validScores) return;
    int validCount = 0, belowCount = 0;
    float sum = 0.0f;
    for (int i = 0; i < count; i++) {
        if (employees[i].logCount > 0) {
            validScores[validCount++] = employees[i].compositeScore;
            sum += employees[i].compositeScore;
            if (employees[i].compositeScore < threshold) belowCount++;
        }
    }
    if (validCount == 0) {
        printf("[!] No active employees with task records.\n");
        free(validScores);
        return;
    }
    for (int i = 0; i < validCount - 1; i++) {
        for (int j = 0; j < validCount - i - 1; j++) {
            if (validScores[j] > validScores[j + 1]) {
                float t = validScores[j];
                validScores[j] = validScores[j + 1];
                validScores[j + 1] = t;
            }
        }
    }
    float mean = sum / (float)validCount;
    float median = (validCount % 2 == 0) ? (validScores[validCount / 2 - 1] + validScores[validCount / 2]) / 2.0f : validScores[validCount / 2];
    printf("\n================ ORGANIZATIONAL STATISTICS ================\n");
    printf(" Total Evaluated           : %d\n", validCount);
    printf(" Mean Composite Score      : %.2f\n", mean);
    printf(" Median Composite Score    : %.2f\n", median);
    printf(" Below Threshold (<%.1f)    : %d (%.1f%%)\n", threshold, belowCount, ((float)belowCount / (float)validCount) * 100.0f);
    free(validScores);
}

static void printDepartmentWiseAverages(const Employee *employees, int count) {
    if (count <= 0) return;
    char depts[count][40];
    int dCounts[count];
    float dScores[count];
    int uniqueDepts = 0;
    for (int i = 0; i < count; i++) {
        if (employees[i].logCount == 0) continue;
        int found = -1;
        for (int d = 0; d < uniqueDepts; d++) {
            if (caseInsensitiveCompare(depts[d], employees[i].department) == 0) {
                found = d;
                break;
            }
        }
        if (found != -1) {
            dCounts[found]++;
            dScores[found] += employees[i].compositeScore;
        } else {
            strncpy(depts[uniqueDepts], employees[i].department, 39);
            depts[uniqueDepts][39] = '\0';
            dCounts[uniqueDepts] = 1;
            dScores[uniqueDepts] = employees[i].compositeScore;
            uniqueDepts++;
        }
    }
    printf("\nDepartment Aggregates:\n");
    printf("%-20s %-15s %s\n", "Department", "Active Emps", "Avg Composite Score");
    printf("----------------------------------------------------------\n");
    for (int d = 0; d < uniqueDepts; d++) {
        printf("%-20s %-15d %.2f\n", depts[d], dCounts[d], dScores[d] / (float)dCounts[d]);
    }
}

static void detectTrend(const Employee *emp, int n) {
    if (!emp || emp->logCount < 2 || n < 2) {
        printf("[!] At least 2 tasks are required for trend detection.\n");
        return;
    }
    int recs = (n > emp->logCount) ? emp->logCount : n;
    int start = emp->logCount - recs;
    int half = recs / 2;
    float q1 = 0.0f, q2 = 0.0f, h1 = 0.0f, h2 = 0.0f;
    for (int i = 0; i < half; i++) {
        q1 += emp->logs[start + i].qualityScore;
        h1 += emp->logs[start + i].hoursSpent;
    }
    for (int i = half; i < recs; i++) {
        q2 += emp->logs[start + i].qualityScore;
        h2 += emp->logs[start + i].hoursSpent;
    }
    float avgQ1 = q1 / (float)half, avgQ2 = q2 / (float)(recs - half);
    float avgH1 = h1 / (float)half, avgH2 = h2 / (float)(recs - half);
    printf("\nTrend for %s across last %d tasks:\n", emp->name, recs);
    printf("- Quality: Earlier %.1f -> Recent %.1f (%s)\n",
           avgQ1, avgQ2, (avgQ2 - avgQ1 >= 3.0f) ? "IMPROVING" : (avgQ1 - avgQ2 >= 3.0f) ? "DECLINING" : "CONSISTENT");
    printf("- Speed  : Earlier %.1f hrs -> Recent %.1f hrs (%s)\n",
           avgH1, avgH2, (avgH1 - avgH2 >= 0.5f) ? "IMPROVING" : (avgH2 - avgH1 >= 0.5f) ? "DECLINING" : "CONSISTENT");
}

static int saveToFile(const char *filename, const Employee *employees, int count) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return 0;
    fprintf(fp, "%d\n", count);
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%d|%s|%s|%d\n", employees[i].id, employees[i].name, employees[i].department, employees[i].logCount);
        for (int j = 0; j < employees[i].logCount; j++) {
            fprintf(fp, "%d|%s|%.2f|%.2f|%d|%d\n",
                    employees[i].logs[j].taskId, employees[i].logs[j].description,
                    employees[i].logs[j].hoursSpent, employees[i].logs[j].qualityScore,
                    employees[i].logs[j].isCompleted, employees[i].logs[j].isError);
        }
    }
    fclose(fp);
    return 1;
}

static int loadFromFile(const char *filename, Employee **employees, int *count, int *capacity) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return 0;
    int total = 0;
    if (fscanf(fp, "%d\n", &total) != 1) {
        fclose(fp);
        return 0;
    }
    for (int i = 0; i < *count; i++) employeeFree(&((*employees)[i]));
    free(*employees);
    *capacity = (total > 4) ? total : 4;
    *employees = (Employee *)malloc((size_t)(*capacity) * sizeof(Employee));
    *count = 0;

    for (int i = 0; i < total; i++) {
        int id, lCount;
        char name[60], dept[40], line[256];
        if (!fgets(line, sizeof(line), fp)) break;
        line[strcspn(line, "\r\n")] = '\0';
        char *tok = strtok(line, "|");
        if (!tok) break;
        id = atoi(tok);
        tok = strtok(NULL, "|");
        if (!tok) break;
        strncpy(name, tok, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
        tok = strtok(NULL, "|");
        if (!tok) break;
        strncpy(dept, tok, sizeof(dept) - 1);
        dept[sizeof(dept) - 1] = '\0';
        tok = strtok(NULL, "|");
        if (!tok) break;
        lCount = atoi(tok);

        employeeInit(&((*employees)[i]), id, name, dept);
        for (int j = 0; j < lCount; j++) {
            TaskLog t;
            if (!fgets(line, sizeof(line), fp)) break;
            line[strcspn(line, "\r\n")] = '\0';
            tok = strtok(line, "|");
            if (tok) t.taskId = atoi(tok);
            tok = strtok(NULL, "|");
            if (tok) { strncpy(t.description, tok, sizeof(t.description) - 1); t.description[sizeof(t.description) - 1] = '\0'; }
            tok = strtok(NULL, "|");
            if (tok) t.hoursSpent = (float)atof(tok);
            tok = strtok(NULL, "|");
            if (tok) t.qualityScore = (float)atof(tok);
            tok = strtok(NULL, "|");
            if (tok) t.isCompleted = atoi(tok);
            tok = strtok(NULL, "|");
            if (tok) t.isError = atoi(tok);
            employeeAddLog(&((*employees)[i]), t);
        }
        (*count)++;
    }
    fclose(fp);
    return 1;
}

int main(void) {
    int count = 0;
    int capacity = 4;
    Employee *employees = (Employee *)malloc((size_t)capacity * sizeof(Employee));
    if (!employees) {
        fprintf(stderr, "Fatal: Initial allocation failed.\n");
        return 1;
    }

    loadFromFile("data/sample_data.txt", &employees, &count, &capacity);

    int choice;
    while (1) {
        printf("\n========================================================\n");
        printf("         EMPLOYEE PERFORMANCE & KPI DASHBOARD           \n");
        printf("========================================================\n");
        printf(" 1. Add Employee\n");
        printf(" 2. Manage Employee Tasks (Add / View / Delete)\n");
        printf(" 3. View Full Organization KPI Dashboard\n");
        printf(" 4. Search Employees (ID / Name / Department)\n");
        printf(" 5. Rank Employees by Composite Score (QuickSort)\n");
        printf(" 6. Statistical Analysis (Mean, Median, Threshold)\n");
        printf(" 7. Performance Trend Analysis (Last N Tasks)\n");
        printf(" 8. Department Performance Reports\n");
        printf(" 9. Save Data to File\n");
        printf("10. Load Data from File\n");
        printf("11. Exit Dashboard\n");
        printf("--------------------------------------------------------\n");

        if (!readSafeInt("Enter choice (1-11): ", &choice)) {
            printf("[!] Invalid input. Please enter a valid number.\n");
            continue;
        }

        switch (choice) {
            case 1: {
                int id;
                char name[60], dept[40];
                if (!readSafeInt("Enter Unique Employee ID: ", &id) || id <= 0) {
                    printf("[!] Invalid ID.\n");
                    break;
                }
                sortEmployeesById(employees, count);
                if (binarySearchEmployeeById(employees, count, id) != -1) {
                    printf("[!] Error: Employee ID %d already exists.\n", id);
                    break;
                }
                readSafeString("Enter Employee Name: ", name, sizeof(name));
                readSafeString("Enter Department: ", dept, sizeof(dept));

                if (count >= capacity) {
                    capacity *= 2;
                    Employee *newEmp = (Employee *)realloc(employees, (size_t)capacity * sizeof(Employee));
                    if (!newEmp) {
                        printf("[!] Memory reallocation failure.\n");
                        break;
                    }
                    employees = newEmp;
                }
                employeeInit(&employees[count], id, name, dept);
                count++;
                sortEmployeesById(employees, count);
                printf("[+] Employee %s (ID %d) added successfully.\n", name, id);
                break;
            }
            case 2: {
                int id;
                if (!readSafeInt("Enter Employee ID: ", &id)) break;
                sortEmployeesById(employees, count);
                int idx = binarySearchEmployeeById(employees, count, id);
                if (idx == -1) {
                    printf("[!] Employee not found.\n");
                    break;
                }
                int tChoice;
                printf("\n--- Task Management for %s ---\n", employees[idx].name);
                printf("1. Add Task Log\n");
                printf("2. View All Tasks\n");
                printf("3. Delete Last Task Log\n");
                if (!readSafeInt("Select action (1-3): ", &tChoice)) break;

                if (tChoice == 1) {
                    TaskLog log;
                    if (!readSafeInt("Enter Task ID: ", &log.taskId)) break;
                    readSafeString("Enter Task Description: ", log.description, sizeof(log.description));
                    if (!readSafeFloat("Enter Hours Spent: ", &log.hoursSpent) || log.hoursSpent < 0.0f) {
                        printf("[!] Hours cannot be negative.\n");
                        break;
                    }
                    if (!readSafeFloat("Enter Quality Score (0-100): ", &log.qualityScore) ||
                        log.qualityScore < 0.0f || log.qualityScore > 100.0f) {
                        printf("[!] Quality score must be between 0 and 100.\n");
                        break;
                    }
                    if (!readSafeInt("Task Completed? (1 = Yes, 0 = No): ", &log.isCompleted)) break;
                    if (!readSafeInt("Error Flagged? (1 = Yes, 0 = No): ", &log.isError)) break;

                    if (employeeAddLog(&employees[idx], log)) {
                        printf("[+] Task logged and KPIs updated.\n");
                    } else {
                        printf("[!] Failed to allocate memory for task.\n");
                    }
                } else if (tChoice == 2) {
                    printf("\nTasks for %s:\n", employees[idx].name);
                    for (int j = 0; j < employees[idx].logCount; j++) {
                        printf("[%d] %s | %.1f hrs | Quality: %.1f | Completed: %d | Error: %d\n",
                               employees[idx].logs[j].taskId, employees[idx].logs[j].description,
                               employees[idx].logs[j].hoursSpent, employees[idx].logs[j].qualityScore,
                               employees[idx].logs[j].isCompleted, employees[idx].logs[j].isError);
                    }
                } else if (tChoice == 3) {
                    if (employees[idx].logCount > 0) {
                        employees[idx].logCount--;
                        employeeRecalculateKPIs(&employees[idx]);
                        printf("[+] Last task deleted. KPIs updated.\n");
                    } else {
                        printf("[!] No tasks to delete.\n");
                    }
                }
                break;
            }
            case 3: {
                if (count == 0) {
                    printf("[!] No employees to display.\n");
                    break;
                }
                printf("\n%-6s %-18s %-12s %-6s %-8s %-8s %-8s %-8s %s\n",
                       "ID", "Name", "Department", "Tasks", "Hours", "Prod", "Quality", "Rel", "Composite");
                printf("----------------------------------------------------------------------------------------\n");
                for (int i = 0; i < count; i++) {
                    float totalHours = 0.0f;
                    for (int j = 0; j < employees[i].logCount; j++) totalHours += employees[i].logs[j].hoursSpent;
                    printf("%-6d %-18.18s %-12.12s %-6d %-8.1f %-8.1f %-8.1f %-8.1f %.2f\n",
                           employees[i].id, employees[i].name, employees[i].department,
                           employees[i].logCount, totalHours,
                           employees[i].productivity, employees[i].avgQuality,
                           employees[i].reliability, employees[i].compositeScore);
                }
                break;
            }
            case 4: {
                int sChoice;
                printf("\nSearch By: 1. ID (Binary) | 2. Name | 3. Department: ");
                if (!readSafeInt("", &sChoice)) break;
                if (sChoice == 1) {
                    int id;
                    if (!readSafeInt("Enter ID: ", &id)) break;
                    sortEmployeesById(employees, count);
                    int idx = binarySearchEmployeeById(employees, count, id);
                    if (idx != -1) {
                        printf("Found: %s (ID %d) | Dept: %s | Score: %.2f\n",
                               employees[idx].name, employees[idx].id, employees[idx].department, employees[idx].compositeScore);
                    } else printf("[!] Employee not found.\n");
                } else if (sChoice == 2) {
                    char term[60];
                    readSafeString("Enter Name: ", term, sizeof(term));
                    int found = 0;
                    for (int i = 0; i < count; i++) {
                        if (caseInsensitiveSubstring(employees[i].name, term)) {
                            printf("- %s (ID %d) | Score: %.2f\n", employees[i].name, employees[i].id, employees[i].compositeScore);
                            found++;
                        }
                    }
                    if (!found) printf("[!] No matching employees found.\n");
                } else if (sChoice == 3) {
                    char dept[40];
                    readSafeString("Enter Department: ", dept, sizeof(dept));
                    int found = 0;
                    for (int i = 0; i < count; i++) {
                        if (caseInsensitiveCompare(employees[i].department, dept) == 0) {
                            printf("- %s (ID %d) | Score: %.2f\n", employees[i].name, employees[i].id, employees[i].compositeScore);
                            found++;
                        }
                    }
                    if (!found) printf("[!] No employees found in department '%s'.\n", dept);
                }
                break;
            }
            case 5: {
                if (count == 0) {
                    printf("[!] No employees to rank.\n");
                    break;
                }
                quickSortEmployeesByScore(employees, 0, count - 1);
                printf("\n================ EMPLOYEES RANKED BY COMPOSITE SCORE ================\n");
                printf("%-6s %-6s %-18s %-14s %s\n", "Rank", "ID", "Name", "Department", "Composite Score");
                printf("--------------------------------------------------------------------\n");
                for (int i = 0; i < count; i++) {
                    printf("%-6d %-6d %-18.18s %-14.14s %.2f\n",
                           i + 1, employees[i].id, employees[i].name,
                           employees[i].department, employees[i].compositeScore);
                }
                break;
            }
            case 6: {
                float thresh = 50.0f;
                readSafeFloat("Enter threshold score (default 50.0): ", &thresh);
                generateSummaryReport(employees, count, thresh);
                break;
            }
            case 7: {
                int id, n;
                if (!readSafeInt("Enter Employee ID: ", &id)) break;
                sortEmployeesById(employees, count);
                int idx = binarySearchEmployeeById(employees, count, id);
                if (idx == -1) {
                    printf("[!] Employee not found.\n");
                    break;
                }
                if (!readSafeInt("Enter number of recent tasks (N >= 2): ", &n)) break;
                detectTrend(&employees[idx], n);
                break;
            }
            case 8:
                printDepartmentWiseAverages(employees, count);
                break;
            case 9:
                if (saveToFile("data/sample_data.txt", employees, count)) {
                    printf("[+] Successfully saved %d employees to data/sample_data.txt\n", count);
                } else {
                    printf("[!] Failed to save records.\n");
                }
                break;
            case 10:
                if (loadFromFile("data/sample_data.txt", &employees, &count, &capacity)) {
                    printf("[+] Loaded %d records from data/sample_data.txt\n", count);
                } else {
                    printf("[!] Failed to load file data/sample_data.txt.\n");
                }
                break;
            case 11:
                for (int i = 0; i < count; i++) employeeFree(&employees[i]);
                free(employees);
                printf("[+] Memory cleared. Exiting dashboard.\n");
                return 0;
            default:
                printf("[!] Invalid selection.\n");
        }
    }
}
