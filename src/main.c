#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EMPLOYEES 100
#define MAX_TASKS 500
#define MAX_NAME_LEN 50
#define MAX_DEPT_LEN 30
#define MAX_TITLE_LEN 80

typedef struct {
    int task_id;
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
