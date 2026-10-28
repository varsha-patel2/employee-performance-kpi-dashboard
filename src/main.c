#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EMPLOYEES 100
#define MAX_TASKS 500
#define MAX_NAME_LEN 50
#define MAX_DEPT_LEN 30
#define MAX_TITLE_LEN 80

typedef struct {
    int task_id;#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_NAME_LEN 50
#define MAX_DEPT_LEN 50
#define INITIAL_CAPACITY 4

typedef struct {
    int task_id;
    char title[100];
    double hours;
    double quality;
    int completed;
    int errors;
} TaskLog;

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char department[MAX_DEPT_LEN];
    TaskLog *logs;
    int log_count;
    int log_capacity;
    
    double productivity;
    double avg_quality;
    double reliability;
    double composite_score;
} Employee;

typedef struct {
    Employee *data;
    int count;
    int capacity;
} EmployeeArray;

void get_string(const char *prompt, char *buffer, size_t size) {
    if (prompt) printf("%s", prompt);
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        } else {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
    }
}

int get_int(const char *prompt) {
    char buf[100];
    int val;
    while (1) {
        get_string(prompt, buf, sizeof(buf));
        if (sscanf(buf, "%d", &val) == 1) return val;
        printf("Invalid integer input. Try again.\n");
    }
}

double get_double(const char *prompt) {
    char buf[100];
    double val;
    while (1) {
        get_string(prompt, buf, sizeof(buf));
        if (sscanf(buf, "%lf", &val) == 1) return val;
        printf("Invalid numeric input. Try again.\n");
    }
}

void recalculate_kpis(Employee *emp) {
    if (emp->log_count == 0) {
        emp->productivity = 0.0;
        emp->avg_quality = 0.0;
        emp->reliability = 0.0;
        emp->composite_score = 0.0;
        return;
    }

    int completed_tasks = 0;
    double total_hours = 0.0;
    double sum_quality = 0.0;
    int total_errors = 0;
    int total_tasks = emp->log_count;

    for (int i = 0; i < emp->log_count; i++) {
        if (emp->logs[i].completed) completed_tasks++;
        total_hours += emp->logs[i].hours;
        sum_quality += emp->logs[i].quality;
        total_errors += emp->logs[i].errors;
    }

    double prod = 0.0;
    if (total_hours > 0.0) {
        prod = ((double)completed_tasks / total_hours) * 100.0;
    }
    if (prod > 100.0) prod = 100.0;
    if (prod < 0.0) prod = 0.0;
    emp->productivity = prod;

    emp->avg_quality = sum_quality / total_tasks;

    double rel_factor = 1.0 - ((double)total_errors / total_tasks);
    if (rel_factor < 0.0) rel_factor = 0.0;
    emp->reliability = rel_factor * emp->avg_quality;

    emp->composite_score = 0.40 * emp->productivity + 0.35 * emp->avg_quality + 0.25 * emp->reliability;
}

void init_employee_array(EmployeeArray *arr) {
    arr->capacity = INITIAL_CAPACITY;
    arr->count = 0;
    arr->data = (Employee *)malloc(arr->capacity * sizeof(Employee));
    if (!arr->data) {
        perror("Allocation failed");
        exit(EXIT_FAILURE);
    }
}

void ensure_employee_capacity(EmployeeArray *arr) {
    if (arr->count >= arr->capacity) {
        int new_cap = arr->capacity * 2;
        Employee *tmp = (Employee *)realloc(arr->data, new_cap * sizeof(Employee));
        if (!tmp) {
            perror("Reallocation failed");
            return;
        }
        arr->data = tmp;
        arr->capacity = new_cap;
    }
}

void ensure_task_capacity(Employee *emp) {
    if (emp->log_capacity == 0) {
        emp->log_capacity = INITIAL_CAPACITY;
        emp->log_count = 0;
        emp->logs = (TaskLog *)malloc(emp->log_capacity * sizeof(TaskLog));
        if (!emp->logs) {
            perror("Task allocation failed");
            exit(EXIT_FAILURE);
        }
    } else if (emp->log_count >= emp->log_capacity) {
        int new_cap = emp->log_capacity * 2;
        TaskLog *tmp = (TaskLog *)realloc(emp->logs, new_cap * sizeof(TaskLog));
        if (!tmp) {
            perror("Task reallocation failed");
            return;
        }
        emp->logs = tmp;
        emp->log_capacity = new_cap;
    }
}

void free_employee(Employee *emp) {
    if (emp->logs) {
        free(emp->logs);
        emp->logs = NULL;
    }
    emp->log_count = 0;
    emp->log_capacity = 0;
}

void free_employee_array(EmployeeArray *arr) {
    for (int i = 0; i < arr->count; i++) {
        free_employee(&arr->data[i]);
    }
    free(arr->data);
    arr->data = NULL;
    arr->count = 0;
    arr->capacity = 0;
}

int find_employee_index_by_id_linear(const EmployeeArray *arr, int id) {
    for (int i = 0; i < arr->count; i++) {
        if (arr->data[i].id == id) return i;
    }
    return -1;
}

int compare_by_id(const void *a, const void *b) {
    const Employee *e1 = (const Employee *)a;
    const Employee *e2 = (const Employee *)b;
    return (e1->id - e2->id);
}

int binary_search_employee_by_id(EmployeeArray *arr, int id) {
    qsort(arr->data, arr->count, sizeof(Employee), compare_by_id);
    int low = 0, high = arr->count - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (arr->data[mid].id == id) return mid;
        if (arr->data[mid].id < id) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

void quicksort_employees(Employee *arr, int low, int high) {
    if (low >= high) return;
    Employee pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        int swap_needed = 0;
        if (arr[j].composite_score > pivot.composite_score) {
            swap_needed = 1;
        } else if (fabs(arr[j].composite_score - pivot.composite_score) < 1e-6) {
            if (arr[j].id < pivot.id) swap_needed = 1;
        }
        if (swap_needed) {
            i++;
            Employee temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    Employee temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    int p = i + 1;

    quicksort_employees(arr, low, p - 1);
    quicksort_employees(arr, p + 1, high);
}

void sort_employees_by_rank(EmployeeArray *arr) {
    if (arr->count > 1) {
        quicksort_employees(arr->data, 0, arr->count - 1);
    }
}

void add_employee(EmployeeArray *arr) {
    int id = get_int("Enter Employee ID: ");
    if (find_employee_index_by_id_linear(arr, id) != -1) {
        printf("Error: Employee ID %d already exists.\n", id);
        return;
    }
    ensure_employee_capacity(arr);
    Employee *emp = &arr->data[arr->count];
    emp->id = id;
    get_string("Enter Name: ", emp->name, sizeof(emp->name));
    get_string("Enter Department: ", emp->department, sizeof(emp->department));
    emp->logs = NULL;
    emp->log_count = 0;
    emp->log_capacity = 0;
    recalculate_kpis(emp);
    arr->count++;
    printf("Employee added successfully.\n");
}

void print_employee_summary(const Employee *emp) {
    printf("ID: %-5d | Name: %-20s | Dept: %-15s | Score: %6.2f | Prod: %6.2f | Qual: %6.2f | Rel: %6.2f | Tasks: %d\n",
           emp->id, emp->name, emp->department, emp->composite_score, emp->productivity, emp->avg_quality, emp->reliability, emp->log_count);
}

void view_employees(const EmployeeArray *arr) {
    if (arr->count == 0) {
        printf("No employees found.\n");
        return;
    }
    printf("\n--- Employee List ---\n");
    for (int i = 0; i < arr->count; i++) {
        print_employee_summary(&arr->data[i]);
    }
}

void search_employee_ui(EmployeeArray *arr) {
    printf("\n--- Search Employee ---\n");
    printf("1. Search by ID (Binary Search)\n");
    printf("2. Search by Name (Linear Search)\n");
    printf("3. Search by Department (Linear Search)\n");
    int choice = get_int("Choice: ");

    if (choice == 1) {
        int id = get_int("Enter ID to search: ");
        int idx = binary_search_employee_by_id(arr, id);
        if (idx != -1) {
            printf("Found:\n");
            print_employee_summary(&arr->data[idx]);
        } else {
            printf("Employee ID %d not found.\n", id);
        }
    } else if (choice == 2) {
        char query[MAX_NAME_LEN];
        get_string("Enter Name/Substring to search: ", query, sizeof(query));
        int found = 0;
        for (int i = 0; i < arr->count; i++) {
            if (strstr(arr->data[i].name, query) != NULL) {
                print_employee_summary(&arr->data[i]);
                found = 1;
            }
        }
        if (!found) printf("No matching employees found.\n");
    } else if (choice == 3) {
        char query[MAX_DEPT_LEN];
        get_string("Enter Department to search: ", query, sizeof(query));
        int found = 0;
        for (int i = 0; i < arr->count; i++) {
            if (strstr(arr->data[i].department, query) != NULL) {
                print_employee_summary(&arr->data[i]);
                found = 1;
            }
        }
        if (!found) printf("No matching employees found.\n");
    } else {
        printf("Invalid choice.\n");
    }
}

void update_employee(EmployeeArray *arr) {
    int id = get_int("Enter Employee ID to update: ");
    int idx = find_employee_index_by_id_linear(arr, id);
    if (idx == -1) {
        printf("Employee not found.\n");
        return;
    }
    Employee *emp = &arr->data[idx];
    printf("Updating Employee ID %d (%s, %s)\n", emp->id, emp->name, emp->department);
    get_string("Enter New Name (leave blank to keep current): ", emp->name, sizeof(emp->name));
    get_string("Enter New Department (leave blank to keep current): ", emp->department, sizeof(emp->department));
    printf("Employee details updated.\n");
}

void delete_employee(EmployeeArray *arr) {
    int id = get_int("Enter Employee ID to delete: ");
    int idx = find_employee_index_by_id_linear(arr, id);
    if (idx == -1) {
        printf("Employee not found.\n");
        return;
    }
    free_employee(&arr->data[idx]);
    for (int i = idx; i < arr->count - 1; i++) {
        arr->data[i] = arr->data[i + 1];
    }
    arr->count--;
    printf("Employee ID %d deleted.\n", id);
}

void task_management_submenu(EmployeeArray *arr) {
    int id = get_int("Enter Employee ID for Task Operations: ");
    int idx = find_employee_index_by_id_linear(arr, id);
    if (idx == -1) {
        printf("Employee not found.\n");
        return;
    }
    Employee *emp = &arr->data[idx];

    while (1) {
        printf("\n--- Task Management for %s (ID: %d) ---\n", emp->name, emp->id);
        printf("1. Add Task Log\n");
        printf("2. View Task Logs\n");
        printf("3. Update Task Log\n");
        printf("4. Delete Task Log\n");
        printf("5. Return to Main Menu\n");
        int choice = get_int("Choice: ");

        if (choice == 1) {
            ensure_task_capacity(emp);
            TaskLog *t = &emp->logs[emp->log_count];
            t->task_id = get_int("Enter Task ID: ");
            get_string("Enter Task Title: ", t->title, sizeof(t->title));
            t->hours = get_double("Enter Hours Spent: ");
            t->quality = get_double("Enter Quality Score (0-100): ");
            t->completed = get_int("Completed? (1 = Yes, 0 = No): ");
            t->errors = get_int("Number of Errors: ");
            emp->log_count++;
            recalculate_kpis(emp);
            printf("Task log added.\n");
        } else if (choice == 2) {
            if (emp->log_count == 0) {
                printf("No task logs for this employee.\n");
            } else {
                printf("\n--- Task Logs ---\n");
                for (int i = 0; i < emp->log_count; i++) {
                    TaskLog *t = &emp->logs[i];
                    printf("Task ID: %d | Title: %s | Hours: %.1f | Quality: %.1f | Completed: %s | Errors: %d\n",
                           t->task_id, t->title, t->hours, t->quality, t->completed ? "Yes" : "No", t->errors);
                }
            }
        } else if (choice == 3) {
            int tid = get_int("Enter Task ID to update: ");
            int tidx = -1;
            for (int i = 0; i < emp->log_count; i++) {
                if (emp->logs[i].task_id == tid) { tidx = i; break; }
            }
            if (tidx == -1) {
                printf("Task ID not found.\n");
            } else {
                TaskLog *t = &emp->logs[tidx];
                get_string("Enter New Title: ", t->title, sizeof(t->title));
                t->hours = get_double("Enter New Hours Spent: ");
                t->quality = get_double("Enter New Quality Score (0-100): ");
                t->completed = get_int("Completed? (1 = Yes, 0 = No): ");
                t->errors = get_int("Number of Errors: ");
                recalculate_kpis(emp);
                printf("Task log updated.\n");
            }
        } else if (choice == 4) {
            int tid = get_int("Enter Task ID to delete: ");
            int tidx = -1;
            for (int i = 0; i < emp->log_count; i++) {
                if (emp->logs[i].task_id == tid) { tidx = i; break; }
            }
            if (tidx == -1) {
                printf("Task ID not found.\n");
            } else {
                for (int i = tidx; i < emp->log_count - 1; i++) {
                    emp->logs[i] = emp->logs[i + 1];
                }
                emp->log_count--;
                recalculate_kpis(emp);
                printf("Task log deleted.\n");
            }
        } else if (choice == 5) {
            break;
        } else {
            printf("Invalid choice.\n");
        }
    }
}

void analyze_trends(EmployeeArray *arr) {
    int id = get_int("Enter Employee ID for Trend Analysis: ");
    int idx = find_employee_index_by_id_linear(arr, id);
    if (idx == -1) {
        printf("Employee not found.\n");
        return;
    }
    Employee *emp = &arr->data[idx];
    int n = get_int("Enter N (number of recent tasks to analyze): ");

    if (emp->log_count < 2 || n < 2) {
        printf("Trend Analysis: Insufficient Data (Need at least 2 tasks).\n");
        return;
    }

    int start = emp->log_count - n;
    if (start < 0) start = 0;
    int actual_n = emp->log_count - start;

    if (actual_n < 2) {
        printf("Trend Analysis: Insufficient Data.\n");
        return;
    }

    int half = actual_n / 2;
    double q_first_sum = 0, q_second_sum = 0;
    double h_first_sum = 0, h_second_sum = 0;
    double c_first_sum = 0, c_second_sum = 0;

    for (int i = 0; i < half; i++) {
        TaskLog *t = &emp->logs[start + i];
        q_first_sum += t->quality;
        h_first_sum += t->hours;
        c_first_sum += t->completed;
    }
    for (int i = actual_n - half; i < actual_n; i++) {
        TaskLog *t = &emp->logs[start + i];
        q_second_sum += t->quality;
        h_second_sum += t->hours;
        c_second_sum += t->completed;
    }

    double q_first_avg = q_first_sum / half;
    double q_second_avg = q_second_sum / half;

    double h_first_avg = h_first_sum / half;
    double h_second_avg = h_second_sum / half;

    double c_first_avg = c_first_sum / half;
    double c_second_avg = c_second_sum / half;

    printf("\n--- Trend Analysis for %s (Last %d tasks) ---\n", emp->name, actual_n);
    
    if (q_second_avg > q_first_avg + 1.0) printf("Quality Trend: Improving\n");
    else if (q_second_avg < q_first_avg - 1.0) printf("Quality Trend: Declining\n");
    else printf("Quality Trend: Consistent\n");

    if (h_second_avg < h_first_avg - 0.5) printf("Completion Time Trend: Improving (Faster)\n");
    else if (h_second_avg > h_first_avg + 0.5) printf("Completion Time Trend: Declining (Slower)\n");
    else printf("Completion Time Trend: Consistent\n");

    if (c_second_avg > c_first_avg + 0.05) printf("Success Rate Trend: Improving\n");
    else if (c_second_avg < c_first_avg - 0.05) printf("Success Rate Trend: Declining\n");
    else printf("Success Rate Trend: Consistent\n");
}

void show_statistics(EmployeeArray *arr) {
    if (arr->count == 0) {
        printf("No data available for statistics.\n");
        return;
    }

    sort_employees_by_rank(arr);

    double total_score = 0;
    for (int i = 0; i < arr->count; i++) {
        total_score += arr->data[i].composite_score;
    }
    double mean = total_score / arr->count;

    double median = 0;
    if (arr->count % 2 == 1) {
        median = arr->data[arr->count / 2].composite_score;
    } else {
        median = (arr->data[arr->count / 2 - 1].composite_score + arr->data[arr->count / 2].composite_score) / 2.0;
    }

    printf("\n================ KPI STATISTICS ================\n");
    printf("Total Employees: %d\n", arr->count);
    printf("Mean Composite Score: %.2f\n", mean);
    printf("Median Composite Score: %.2f\n", median);
    printf("Highest Performer: %s (ID: %d, Score: %.2f)\n", arr->data[0].name, arr->data[0].id, arr->data[0].composite_score);
    printf("Lowest Performer: %s (ID: %d, Score: %.2f)\n", arr->data[arr->count - 1].name, arr->data[arr->count - 1].id, arr->data[arr->count - 1].composite_score);

    double threshold = get_double("Enter Underperformance Threshold Score (e.g. 50.0): ");
    int underperforming = 0;
    for (int i = 0; i < arr->count; i++) {
        if (arr->data[i].composite_score < threshold) underperforming++;
    }
    printf("Underperforming Employees (< %.2f): %d (%.2f%%)\n",
           threshold, underperforming, (double)underperforming / arr->count * 100.0);

    printf("\n--- Department Summary ---\n");
    for (int i = 0; i < arr->count; i++) {
        int already = 0;
        for (int j = 0; j < i; j++) {
            if (strcmp(arr->data[i].department, arr->data[j].department) == 0) {
                already = 1;
                break;
            }
        }
        if (already) continue;

        int dept_count = 0;
        double dept_sum = 0;
        for (int k = 0; k < arr->count; k++) {
            if (strcmp(arr->data[i].department, arr->data[k].department) == 0) {
                dept_count++;
                dept_sum += arr->data[k].composite_score;
            }
        }
        printf("Dept: %-15s | Count: %-3d | Avg Score: %.2f\n",
               arr->data[i].department, dept_count, dept_sum / dept_count);
    }
}

void save_to_file(const EmployeeArray *arr, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        perror("Failed to open file for writing");
        return;
    }
    fprintf(f, "%d\n", arr->count);
    for (int i = 0; i < arr->count; i++) {
        Employee *e = &arr->data[i];
        fprintf(f, "%d\n%s\n%s\n%d\n", e->id, e->name, e->department, e->log_count);
        for (int j = 0; j < e->log_count; j++) {
            TaskLog *t = &e->logs[j];
            fprintf(f, "%d\n%s\n%.2f\n%.2f\n%d\n%d\n",
                    t->task_id, t->title, t->hours, t->quality, t->completed, t->errors);
        }
    }
    fclose(f);
    printf("Data saved successfully to %s\n", filename);
}

void load_from_file(EmployeeArray *arr, const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("No existing data file (%s) found. Starting fresh.\n", filename);
        return;
    }
    char buf[256];
    if (!fgets(buf, sizeof(buf), f)) { fclose(f); return; }
    int num_employees = atoi(buf);

    for (int i = 0; i < num_employees; i++) {
        ensure_employee_capacity(arr);
        Employee *e = &arr->data[arr->count];
        
        if (!fgets(buf, sizeof(buf), f)) break;
        e->id = atoi(buf);
        
        if (!fgets(e->name, sizeof(e->name), f)) break;
        size_t len = strlen(e->name);
        if (len > 0 && e->name[len-1] == '\n') e->name[len-1] = '\0';

        if (!fgets(e->department, sizeof(e->department), f)) break;
        len = strlen(e->department);
        if (len > 0 && e->department[len-1] == '\n') e->department[len-1] = '\0';

        if (!fgets(buf, sizeof(buf), f)) break;
        int num_logs = atoi(buf);

        e->logs = NULL;
        e->log_count = 0;
        e->log_capacity = 0;

        for (int j = 0; j < num_logs; j++) {
            ensure_task_capacity(e);
            TaskLog *t = &e->logs[e->log_count];

            if (!fgets(buf, sizeof(buf), f)) break;
            t->task_id = atoi(buf);

            if (!fgets(t->title, sizeof(t->title), f)) break;
            len = strlen(t->title);
            if (len > 0 && t->title[len-1] == '\n') t->title[len-1] = '\0';

            if (!fgets(buf, sizeof(buf), f)) break;
            t->hours = atof(buf);

            if (!fgets(buf, sizeof(buf), f)) break;
            t->quality = atof(buf);

            if (!fgets(buf, sizeof(buf), f)) break;
            t->completed = atoi(buf);

            if (!fgets(buf, sizeof(buf), f)) break;
            t->errors = atoi(buf);

            e->log_count++;
        }
        recalculate_kpis(e);
        arr->count++;
    }
    fclose(f);
    printf("Loaded %d employees from %s\n", arr->count, filename);
}

int main(void) {
    EmployeeArray arr;
    init_employee_array(&arr);

    const char *data_file = "employees.txt";
    load_from_file(&arr, data_file);

    while (1) {
        printf("\n================ EMPLOYEE KPI DASHBOARD ================\n");
        printf("1. Add Employee\n");
        printf("2. View All Employees (Sorted by Rank)\n");
        printf("3. Search Employee (ID / Name / Department)\n");
        printf("4. Update Employee Details\n");
        printf("5. Delete Employee\n");
        printf("6. Task Management Submenu\n");
        printf("7. View Statistics & Department Summary\n");
        printf("8. Perform Trend Analysis\n");
        printf("9. Save Data to File\n");
        printf("10. Exit\n");
        int choice = get_int("Enter choice: ");

        switch (choice) {
            case 1:
                add_employee(&arr);
                break;
            case 2:
                sort_employees_by_rank(&arr);
                view_employees(&arr);
                break;
            case 3:
                search_employee_ui(&arr);
                break;
            case 4:
                update_employee(&arr);
                break;
            case 5:
                delete_employee(&arr);
                break;
            case 6:
                task_management_submenu(&arr);
                break;
            case 7:
                show_statistics(&arr);
                break;
            case 8:
                analyze_trends(&arr);
                break;
            case 9:
                save_to_file(&arr, data_file);
                break;
            case 10:
                save_to_file(&arr, data_file);
                free_employee_array(&arr);
                printf("Exiting program. Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice. Please enter 1-10.\n");
        }
    }

    return 0;
}
    int employee_id;
    char title[MAX_TITLE_LEN];
    int score;
    int completed;
    int on_time;
    int month;
} Task;

typedef struct {
    int employee_id;
    char name[MAX_NAME_LEN];
    char department[MAX_DEPT_LEN];
    int total_assigned_tasks;
    int total_completed_tasks;
    int total_on_time_tasks;
    double completion_rate;
    double promptness_rate;
    double avg_score;
    double kpi_score;
} Employee;

Employee employees[MAX_EMPLOYEES];
int employee_count = 0;

Task tasks[MAX_TASKS];
int task_count = 0;

void calculate_kpis() {
    for (int i = 0; i < employee_count; i++) {
        int assigned = 0, completed = 0, on_time = 0, sum_score = 0;
        for (int j = 0; j < task_count; j++) {
            if (tasks[j].employee_id == employees[i].employee_id) {
                assigned++;
                if (tasks[j].completed) {
                    completed++;
                    sum_score += tasks[j].score;
                    if (tasks[j].on_time) on_time++;
                }
            }
        }
        employees[i].total_assigned_tasks = assigned;
        employees[i].total_completed_tasks = completed;
        employees[i].total_on_time_tasks = on_time;
        employees[i].completion_rate = (assigned > 0) ? (((double)completed / assigned) * 100.0) : 0.0;
        employees[i].promptness_rate = (completed > 0) ? (((double)on_time / completed) * 100.0) : 0.0;
        employees[i].avg_score = (completed > 0) ? ((double)sum_score / completed) : 0.0;
        employees[i].kpi_score = (0.40 * employees[i].avg_score) + (0.35 * employees[i].completion_rate) + (0.25 * employees[i].promptness_rate);
    }
}

int partition_kpi(int low, int high) {
    double pivot = employees[high].kpi_score;
    int i = (low - 1);
    for (int j = low; j < high; j++) {
        if (employees[j].kpi_score >= pivot) {
            i++;
            Employee temp = employees[i];
            employees[i] = employees[j];
            employees[j] = temp;
        }
    }
    Employee temp = employees[i + 1];
    employees[i + 1] = employees[high];
    employees[high] = temp;
    return (i + 1);
}

void quicksort_employees_by_kpi(int low, int high) {
    if (low < high) {
        int pi = partition_kpi(low, high);
        quicksort_employees_by_kpi(low, pi - 1);
        quicksort_employees_by_kpi(pi + 1, high);
    }
}

int partition_id(int low, int high) {
    int pivot = employees[high].employee_id;
    int i = (low - 1);
    for (int j = low; j < high; j++) {
        if (employees[j].employee_id <= pivot) {
            i++;
            Employee temp = employees[i];
            employees[i] = employees[j];
            employees[j] = temp;
        }
    }
    Employee temp = employees[i + 1];
    employees[i + 1] = employees[high];
    employees[high] = temp;
    return (i + 1);
}

void quicksort_employees_by_id(int low, int high) {
    if (low < high) {
        int pi = partition_id(low, high);
        quicksort_employees_by_id(low, pi - 1);
        quicksort_employees_by_id(pi + 1, high);
    }
}

int binary_search_employee_by_id(int id) {
    quicksort_employees_by_id(0, employee_count - 1);
    int low = 0, high = employee_count - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (employees[mid].employee_id == id) return mid;
        if (employees[mid].employee_id < id) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

void display_all_employees() {
    printf("\n====================================================================================================\n");
    printf("%-5s | %-20s | %-15s | %-6s | %-6s | %-8s | %-8s | %-8s\n", "ID", "Name", "Department", "Comp%", "OnTime%", "AvgScore", "KPI Score", "Rating");
    printf("----------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < employee_count; i++) {
        char rating[15];
        if (employees[i].kpi_score >= 85.0) strcpy(rating, "Excellent");
        else if (employees[i].kpi_score >= 70.0) strcpy(rating, "Good");
        else if (employees[i].kpi_score >= 55.0) strcpy(rating, "Average");
        else strcpy(rating, "Needs Impv");

        printf("%-5d | %-20s | %-15s | %-6.1f | %-7.1f | %-8.1f | %-9.2f | %-10s\n",
               employees[i].employee_id, employees[i].name, employees[i].department,
               employees[i].completion_rate, employees[i].promptness_rate,
               employees[i].avg_score, employees[i].kpi_score, rating);
    }
    printf("====================================================================================================\n");
}

void display_department_filter() {
    char dept[MAX_DEPT_LEN];
    printf("\nEnter Department Name to Filter: ");
    if (scanf("%29s", dept) != 1) return;

    printf("\n--- Department Filter: %s ---\n", dept);
    printf("%-5s | %-20s | %-8s | %-8s\n", "ID", "Name", "AvgScore", "KPI Score");
    printf("--------------------------------------------------------\n");
    int found = 0;
    for (int i = 0; i < employee_count; i++) {
        if (strcasecmp(employees[i].department, dept) == 0) {
            printf("%-5d | %-20s | %-8.1f | %-9.2f\n", employees[i].employee_id, employees[i].name, employees[i].avg_score, employees[i].kpi_score);
            found++;
        }
    }
    if (!found) printf("No employees found in department '%s'.\n", dept);
}

void display_statistical_reports() {
    if (employee_count == 0) return;
    double total_kpi = 0.0, max_kpi = -1.0, min_kpi = 101.0;
    int top_emp_idx = -1, low_emp_idx = -1;

    for (int i = 0; i < employee_count; i++) {
        total_kpi += employees[i].kpi_score;
        if (employees[i].kpi_score > max_kpi) { max_kpi = employees[i].kpi_score; top_emp_idx = i; }
        if (employees[i].kpi_score < min_kpi) { min_kpi = employees[i].kpi_score; low_emp_idx = i; }
    }

    printf("\n=================== STATISTICAL SUMMARY REPORT ===================\n");
    printf("Total Employees Tracked: %d\n", employee_count);
    printf("Total Tasks Recorded:   %d\n", task_count);
    printf("Average KPI Score:       %.2f\n", total_kpi / employee_count);
    if (top_emp_idx != -1) printf("Top Performer:           %s (%s) - KPI: %.2f\n", employees[top_emp_idx].name, employees[top_emp_idx].department, employees[top_emp_idx].kpi_score);
    if (low_emp_idx != -1) printf("Lowest Performer:        %s (%s) - KPI: %.2f\n", employees[low_emp_idx].name, employees[low_emp_idx].department, employees[low_emp_idx].kpi_score);
    printf("==================================================================\n");
}

void display_trend_analysis() {
    printf("\n================ MONTHLY PERFORMANCE TREND ANALYSIS ================\n");
    printf("%-8s | %-12s | %-15s | %-12s\n", "Month", "Total Tasks", "Completed Tasks", "Avg Score");
    printf("--------------------------------------------------------------------\n");
    for (int m = 1; m <= 12; m++) {
        int m_tasks = 0, m_completed = 0, m_score_sum = 0;
        for (int i = 0; i < task_count; i++) {
            if (tasks[i].month == m) {
                m_tasks++;
                if (tasks[i].completed) { m_completed++; m_score_sum += tasks[i].score; }
            }
        }
        if (m_tasks > 0) {
            double avg = (m_completed > 0) ? ((double)m_score_sum / m_completed) : 0.0;
            printf("Month %-2d | %-12d | %-15d | %-12.1f\n", m, m_tasks, m_completed, avg);
        }
    }
    printf("====================================================================\n");
}

void load_sample_data(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Loading built-in fallback data...\n");
        employee_count = 5;
        employees[0] = (Employee){101, "Alice Smith", "Engineering", 0, 0, 0, 0, 0, 0, 0};
        employees[1] = (Employee){102, "Bob Jones", "Marketing", 0, 0, 0, 0, 0, 0, 0};
        employees[2] = (Employee){103, "Charlie Brown", "Engineering", 0, 0, 0, 0, 0, 0, 0};
        employees[3] = (Employee){104, "Diana Prince", "Sales", 0, 0, 0, 0, 0, 0, 0};
        employees[4] = (Employee){105, "Evan Davis", "Finance", 0, 0, 0, 0, 0, 0, 0};

        task_count = 10;
        tasks[0] = (Task){1, 101, "Feature Development", 90, 1, 1, 1};
        tasks[1] = (Task){2, 101, "Code Review", 85, 1, 1, 1};
        tasks[2] = (Task){3, 102, "Ad Campaign", 78, 1, 0, 1};
        tasks[3] = (Task){4, 103, "Bug Fixing", 92, 1, 1, 2};
        tasks[4] = (Task){5, 104, "Client Meeting", 88, 1, 1, 2};
        tasks[5] = (Task){6, 105, "Audit Report", 95, 1, 1, 2};
        tasks[6] = (Task){7, 101, "API Integration", 88, 1, 1, 3};
        tasks[7] = (Task){8, 102, "Market Research", 70, 1, 1, 3};
        tasks[8] = (Task){9, 103, "Refactoring Engine", 80, 1, 0, 3};
        tasks[9] = (Task){10, 104, "Q1 Sales Targets", 94, 1, 1, 3};

        calculate_kpis();
        return;
    }

    char line[256], mode[32] = "";
    employee_count = 0; task_count = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;
        if (strncmp(line, "[EMPLOYEES]", 11) == 0) { strcpy(mode, "EMPLOYEES"); continue; }
        if (strncmp(line, "[TASKS]", 7) == 0) { strcpy(mode, "TASKS"); continue; }

        if (strcmp(mode, "EMPLOYEES") == 0) {
            int id; char name[MAX_NAME_LEN], dept[MAX_DEPT_LEN];
            if (sscanf(line, "%d,%49[^,],%29[^\r\n]", &id, name, dept) == 3 && employee_count < MAX_EMPLOYEES) {
                employees[employee_count].employee_id = id;
                strncpy(employees[employee_count].name, name, MAX_NAME_LEN - 1);
                strncpy(employees[employee_count].department, dept, MAX_DEPT_LEN - 1);
                employee_count++;
            }
        } else if (strcmp(mode, "TASKS") == 0) {
            int tid, eid, score, comp, ontime, month; char title[MAX_TITLE_LEN];
            if (sscanf(line, "%d,%d,%79[^,],%d,%d,%d,%d", &tid, &eid, title, &score, &comp, &ontime, &month) == 7 && task_count < MAX_TASKS) {
                tasks[task_count].task_id = tid; tasks[task_count].employee_id = eid;
                strncpy(tasks[task_count].title, title, MAX_TITLE_LEN - 1);
                tasks[task_count].score = score; tasks[task_count].completed = comp;
                tasks[task_count].on_time = ontime; tasks[task_count].month = month;
                task_count++;
            }
        }
    }
    fclose(fp);
    printf("Loaded data from '%s'.\n", filename);
    calculate_kpis();
}

void cli_menu() {
    int choice;
    do {
        printf("\n========= EMPLOYEE PERFORMANCE & KPI DASHBOARD =========\n");
        printf("1. View All Employees & KPI Rankings (QuickSort by KPI)\n");
        printf("2. Search Employee by ID (Binary Search)\n");
        printf("3. Filter Employees by Department\n");
        printf("4. Statistical Summary Report\n");
        printf("5. Monthly Trend Analysis\n");
        printf("6. Reload Data from File\n");
        printf("0. Exit\n");
        printf("========================================================\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) break;

        switch (choice) {
            case 1:
                calculate_kpis();
                quicksort_employees_by_kpi(0, employee_count - 1);
                display_all_employees();
                break;
            case 2: {
                int search_id;
                printf("Enter Employee ID to search: ");
                if (scanf("%d", &search_id) == 1) {
                    int idx = binary_search_employee_by_id(search_id);
                    if (idx != -1) {
                        printf("\n--- Employee Found ---\n");
                        printf("ID: %d | Name: %s | Dept: %s | KPI: %.2f\n", employees[idx].employee_id, employees[idx].name, employees[idx].department, employees[idx].kpi_score);
                    } else printf("Employee ID %d not found.\n", search_id);
                }
                break;
            }
            case 3: display_department_filter(); break;
            case 4: display_statistical_reports(); break;
            case 5: display_trend_analysis(); break;
            case 6: load_sample_data("data/sample_data.txt"); break;
            case 0: printf("Exiting Dashboard. Goodbye!\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

int main(int argc, char *argv[]) {
    const char *data_file = (argc > 1) ? argv[1] : "data/sample_data.txt";
    printf("Initializing Employee Performance & KPI Dashboard...\n");
    load_sample_data(data_file);
    cli_menu();
    return 0;
}
