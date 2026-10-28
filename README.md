# Employee Performance KPI Dashboard

A C-based application for monitoring, calculating, and analyzing employee Key Performance Indicators (KPIs) efficiently.

---

## Academic Mapping

### Course & Subject Mapping
* **Subject:** Data Structures & Algorithms / Software Engineering / System Programming in C
* **Core Concepts Applied:**
  * **Data Structures:** Structs, Arrays, Pointers, Linked Lists/Dynamic Memory Allocation for employee record storage.
  * **Algorithms:** Linear Search, Sorting (QuickSort/MergeSort for ranking KPIs), File I/O operations for data persistence.
  * **System Engineering:** Modular C Programming, Memory Management (`malloc`/`free`), Error Handling, Makefile Automation.

---

## Detailed KPI Formulas

The dashboard calculates key operational and strategic performance metrics using standard mathematical models:

1. **Overall Performance Score (OPS):**
   $$\text{OPS} = (w_1 \cdot \text{Task Completion Rate}) + (w_2 \cdot \text{Code Quality Score}) + (w_3 \cdot \text{Punctuality Score})$$
   *Where weights sum to 1.0 ($w_1 + w_2 + w_3 = 1.0$).*

2. **Task Completion Rate (TCR):**
   $$\text{TCR} = \left( \frac{\text{Tasks Completed}}{\text{Tasks Assigned}} \right) \times 100\%$$

3. **Code Quality Index (CQI):**
   $$\text{CQI} = 100 - (\text{Bugs Found} \times 5 + \text{Code Review Deficiencies} \times 3)$$

4. **Efficiency Score (ES):**
   $$\text{ES} = \left( \frac{\text{Estimated Hours}}{\text{Actual Hours Spent}} \right) \times 100\%$$

---

## Algorithmic Complexity

| Operation / Module | Data Structure / Algorithm | Time Complexity (Best) | Time Complexity (Average) | Time Complexity (Worst) | Space Complexity |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Record Loading / CSV Parsing** | Sequential File Parsing | O(N) | O(N) | O(N) | O(N) |
| **Employee Lookup** | Linear Search by ID / Name | O(1) | O(N) | O(N) | O(1) |
| **KPI Ranking / Sorting** | QuickSort / MergeSort | O(N log N) | O(N log N) | O(N^2) | O(log N) |
| **Aggregate Summary Computation** | Iterative Accumulation | O(N) | O(N) | O(N) | O(1) |

---

## Sample Execution Walk-Through

### 1. Build and Run
```bash
make
./bin/kpi_dashboard
```

### 2. Output Walk-Through
```text
==================================================
        EMPLOYEE KPI DASHBOARD - MAIN MENU
==================================================
1. Load Employee Data
2. Display All Records & KPIs
3. Rank Employees by KPI Score
4. Generate Summary Report
5. Exit
--------------------------------------------------
Enter choice: 1
[SUCCESS] Loaded 50 employee records from data/employees.csv.

Enter choice: 2
ID     | Name             | TCR (%) | CQI   | Efficiency (%) | Overall Score
--------------------------------------------------------------------------------
E101   | Alice Smith      | 95.0%   | 92.0  | 105.2%         | 94.1
E102   | Bob Jones        | 88.0%   | 85.0  | 98.0%          | 87.2
E103   | Charlie Brown    | 92.0%   | 90.0  | 100.0%         | 91.0

Enter choice: 3
[RANKING RESULT] Top Performer: Alice Smith (Score: 94.1)
```

---

## Viva Preparation Questions & Answers

### Q1: Why use C for implementing a KPI Dashboard system instead of high-level languages like Python or JavaScript?
**Answer:** C provides direct memory control, deterministic performance, low CPU overhead, and minimal runtime dependencies. In systems where KPI processing is part of an embedded logging framework or require real-time processing on low-power devices, C provides maximum computational efficiency.

### Q2: How does dynamic memory allocation prevent memory leaks when managing employee records?
**Answer:** Dynamic memory allocated using `malloc()` or `calloc()` must be explicitly tracked with pointer management and freed using `free()` before program exit or when reallocating array sizes. Defensive programming includes setting pointers to `NULL` post-deallocation and checking returns from allocation routines.

### Q3: What is the benefit of organizing the codebase with a Makefile?
**Answer:** A Makefile automates compilation, manages dependencies between headers and implementation files (`.c` / `.h`), speeds up incremental builds by only recompiling modified modules, and simplifies build workflows (`make clean`, `make all`).

### Q4: How are edge cases handled during KPI formula evaluation (e.g., division by zero when Tasks Assigned = 0)?
**Answer:** Inputs are validated prior to computation. If `Tasks Assigned == 0`, `TCR` defaults safely to `0.0` or a predefined default value, preventing runtime floating-point or integer division-by-zero exceptions.

---

## Requirements & Building

### Requirements
* GCC Compiler (C11 standard support)
* GNU Make

### Building the Project
```bash
make
```

### Cleaning Artifacts
```bash
make clean
```
